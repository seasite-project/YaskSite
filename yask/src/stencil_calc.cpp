/*****************************************************************************

YASK: Yet Another Stencil Kernel
Copyright (c) 2014-2017, Intel Corporation

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

* The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

*****************************************************************************/

// Stencil types.
#include "stencil.hpp"

// Base classes for stencil code.
#include "stencil_calc.hpp"

using namespace std;

namespace yask {

    ///// StencilContext functions:

    // Init MPI, OMP, etc.
    void StencilContext::initEnv(int* argc, char*** argv)
    {
        // Stop collecting VTune data.
        // Even better to use -start-paused option.
        VTUNE_PAUSE;

        // MPI init.
        my_rank = 0;
        num_ranks = 1;
#ifdef USE_MPI
        int provided = 0;
        MPI_Init_thread(argc, argv, MPI_THREAD_SERIALIZED, &provided);
        if (provided < MPI_THREAD_SERIALIZED) {
            cerr << "error: MPI_THREAD_SERIALIZED not provided.\n";
            exit_yask(1);
        }
        comm = MPI_COMM_WORLD;
        MPI_Comm_rank(comm, &my_rank);
        MPI_Comm_size(comm, &num_ranks);
#else
        comm = 0;
#endif

        // Enable the default output stream on the msg-rank only.
        set_ostr();

        // There is no specific call to init OMP, but we make a gratuitous
        // OMP call to trigger any debug output.
        omp_get_num_procs();

        // Make sure any MPI/OMP debug data is dumped before continuing.
        global_barrier();
    }

    // Copy env settings from another context.
    void StencilContext::copyEnv(const StencilContext& src) {
        comm = src.comm;
        my_rank = src.my_rank;
        num_ranks = src.num_ranks;
        _ostr = src._ostr;
    }

    // Set ostr to given stream if provided.
    // If not provided, set to cout if my_rank == msg_rank
    // or a null stream otherwise.
    ostream& StencilContext::set_ostr(std::ostream* os) {
        if (os)
            _ostr = os;
        else if (my_rank == _opts->msg_rank)
            _ostr = &cout;
        else
            _ostr = new ofstream;    // null stream (unopened ofstream).
        assert(_ostr);
        return *_ostr;
    }


    ///// Top-level methods for evaluating reference and optimized stencils.

    // Eval stencil equation group(s) over grid(s) using scalar code.
    void StencilContext::calc_rank_ref()
    {
        idx_t begin_dt = ofs_t;
        idx_t end_dt = begin_dt + _opts->dt;
        TRACE_MSG("calc_rank_ref(t=" << begin_dt << ".." << (end_dt-1) << ")");

        // Time steps.
        // TODO: check that scalar version actually does CPTS_T time steps.
        // (At this point, CPTS_T == 1 for all existing stencil examples.)
        for(idx_t t = begin_dt; t < end_dt; t += CPTS_T) {

            // Loop thru eq-groups.
            for (auto* eg : eqGroups) {

                // Halo exchange(s) needed for this eq-group.
                exchange_halos(t, t + CPTS_T, *eg);

                // Loop through 4D space within the bounding-box of this
                // equation set.
#pragma omp parallel for collapse(4)
                for (idx_t w = eg->begin_bbw; w < eg->end_bbw; w++)
                    for (idx_t x = eg->begin_bbx; x < eg->end_bbx; x++)
                        for (idx_t y = eg->begin_bby; y < eg->end_bby; y++)
                            for (idx_t z = eg->begin_bbz; z < eg->end_bbz; z++) {

                                // Update only if point is in sub-domain for this eq group.
                                if (eg->is_in_valid_domain(t, ARG_W(w) x, y, z)) {

                                    // Evaluate the reference scalar code.
                                    eg->calc_scalar(t, ARG_W(w) x, y, z);
                                }
                            }

                // Remember grids that have been written to by this eq-group.
                mark_grids_dirty(*eg);

            } // eq-groups.
        } // iterations.
    }

    // Eval equation group(s) over grid(s) using optimized code.
    void StencilContext::calc_rank_opt()
    {
        idx_t begin_dt = ofs_t;
        idx_t end_dt = begin_dt + _opts->dt;
        idx_t step_dt = _opts->rt;
        TRACE_MSG("calc_rank_opt(t=" << begin_dt << ".." << (end_dt-1) <<
                " by " << step_dt << ")");
        ostream& os = get_ostr();

#ifdef MODEL_CACHE
        if (context.my_rank != context.msg_rank)
            cache_model.disable();
        if (cache_model.isEnabled())
            os << "Modeling cache...\n";
#endif

        // Domain begin points.
        idx_t begin_dw = begin_bbw;
        idx_t begin_dx = begin_bbx;
        idx_t begin_dy = begin_bby;
        idx_t begin_dz = begin_bbz;

        // Domain end-points.
        idx_t end_dw = end_bbw;
        idx_t end_dx = end_bbx;
        idx_t end_dy = end_bby;
        idx_t end_dz = end_bbz;

        // Steps are based on region sizes.
        idx_t step_dw = _opts->rw;
        idx_t step_dx = _opts->rx;
        idx_t step_dy = _opts->ry;
        idx_t step_dz = _opts->rz;

        // Groups in rank loops are set to smallest size.
        const idx_t group_size_dw = 1;
        const idx_t group_size_dx = 1;
        const idx_t group_size_dy = 1;
        const idx_t group_size_dz = 1;

        // Extend end points for overlapping regions due to wavefront angle.
        // For each subsequent time step in a region, the spatial location
        // of each block evaluation is shifted by the angle for each
        // eq-group. So, the total shift in a region is the angle * num
        // stencils * num timesteps. Thus, the number of overlapping regions
        // is ceil(total shift / region size). This assumes all eq-groups
        // are inter-dependent to find minimum extension. Actual required
        // extension may be less, but this will just result in some calls to
        // calc_region() that do nothing.
        //
        // Conceptually (4 regions in t and x dims):
        // -----------------------------  t = rt
        //  \    |\     \     \ |   \
        //   \   | \     \     \|    \
        //    \  |  \     \     |     \
        //     \ |   \     \    |\     \
        //      \|    \     \   | \     \
        //  ----------------------------- t = 0
        // x = begin_dx       end_dx   end_dx
        //                    (orig)   (after extension)
        //
        idx_t nshifts = (idx_t(eqGroups.size()) * _opts->rt) - 1;
        end_dw += angle_w * nshifts;
        end_dx += angle_x * nshifts;
        end_dy += angle_y * nshifts;
        end_dz += angle_z * nshifts;
        TRACE_MSG("extended domain after wave-front adjustment: " <<
                  "t=" << begin_dt << ".." << (end_dt-1) <<
                  ", w=" << begin_dw << ".." << (end_dw-1) <<
                  ", x=" << begin_dx << ".." << (end_dx-1) <<
                  ", y=" << begin_dy << ".." << (end_dy-1) <<
                  ", z=" << begin_dz << ".." << (end_dz-1) <<
                  ")");

        // Set number of threads for a region.
        set_region_threads();

        // Number of iterations to get from begin_dt to (but not including) end_dt,
        // stepping by step_dt.
        const idx_t num_dt = ((end_dt - begin_dt) + (step_dt - 1)) / step_dt;
        for (idx_t index_dt = 0; index_dt < num_dt; index_dt++)
        {
            // This value of index_dt covers dt from start_dt to stop_dt-1.
            const idx_t start_dt = begin_dt + (index_dt * step_dt);
            const idx_t stop_dt = min(start_dt + step_dt, end_dt);

            // If doing only one time step in a region (default), loop
            // through equations here, and do only one equation group at a
            // time in calc_region(). This is similar to loop in
            // calc_rank_ref().
            if (step_dt == 1) {

                for (auto* eg : eqGroups) {

                    // Halo exchange(s) needed for this eq-group.
                    exchange_halos(start_dt, stop_dt, *eg);

                    // Eval this eq-group in calc_region().
                    EqGroupSet eqGroup_set;
                    eqGroup_set.insert(eg);
                    EqGroupSet* eqGroup_ptr = &eqGroup_set;

                    // Include automatically-generated loop code that calls
                    // calc_region() for each region.
#include "stencil_rank_loops.hpp"

                    // Remember grids that have been written to by this eq-group.
                    mark_grids_dirty(*eg);
                }
            }

            // If doing more than one time step in a region (temporal wave-front),
            // must loop through all eq-groups in calc_region().
            else {

                // TODO: enable halo exchange for wave-fronts.
                if (num_ranks > 1) {
                    cerr << "Error: halo exchange with wave-fronts not yet supported.\n";
                    exit_yask(1);
                }
                
                // Eval all equation groups.
                EqGroupSet* eqGroup_ptr = NULL;
                
                // Include automatically-generated loop code that calls calc_region() for each region.
#include "stencil_rank_loops.hpp"
            }

        }

        // Reset threads back to max.
        set_all_threads();

#ifdef MODEL_CACHE
        // Print cache stats, then disable.
        // Thus, cache is only modeled for first call.
        if (cache_model.isEnabled()) {
            os << "Done modeling cache...\n";
            cache_model.dumpStats();
            cache_model.disable();
        }
#endif

    }

    // Calculate results within a region.
    // Each region is typically computed in a separate OpenMP 'for' region.
    // In it, we loop over the time steps and the stencil
    // equations and evaluate the blocks in the region.
    // Boundaries are named start_d* and stop_d* because region loops are nested
    // inside the rank-domain loops.
    void StencilContext::calc_region(idx_t start_dt, idx_t stop_dt,
                                     EqGroupSet* eqGroup_set,
                                     idx_t start_dw, idx_t start_dx, idx_t start_dy, idx_t start_dz,
                                     idx_t stop_dw, idx_t stop_dx, idx_t stop_dy, idx_t stop_dz)
    {
        TRACE_MSG("calc_region(t=" << start_dt << ".." << (stop_dt-1) <<
                  ", w=" << start_dw << ".." << (stop_dw-1) <<
                  ", x=" << start_dx << ".." << (stop_dx-1) <<
                  ", y=" << start_dy << ".." << (stop_dy-1) <<
                  ", z=" << start_dz << ".." << (stop_dz-1) <<
                  ")");

        // Steps within a region are based on block sizes.
        const idx_t step_rt = _opts->bt;
        const idx_t step_rw = _opts->bw;
        const idx_t step_rx = _opts->bx;
        const idx_t step_ry = _opts->by;
        const idx_t step_rz = _opts->bz;

        // Groups in region loops are based on block-group sizes.
        const idx_t group_size_rw = _opts->bgw;
        const idx_t group_size_rx = _opts->bgx;
        const idx_t group_size_ry = _opts->bgy;
        const idx_t group_size_rz = _opts->bgz;

        // Not yet supporting temporal blocking.
        if (step_rt != 1) {
            cerr << "Error: temporal blocking not yet supported." << endl;
            exit_yask(1);
        }

        // Number of iterations to get from start_dt to (but not including) stop_dt,
        // stepping by step_rt.
        const idx_t num_rt = ((stop_dt - start_dt) + (step_rt - 1)) / step_rt;
    
        // Step through time steps in this region. This is the temporal size
        // of a wave-front tile.
        for (idx_t index_rt = 0; index_rt < num_rt; index_rt++) {
        
            // This value of index_rt covers rt from start_rt to stop_rt-1.
            const idx_t start_rt = start_dt + (index_rt * step_rt);
            const idx_t stop_rt = min (start_rt + step_rt, stop_dt);

            // TODO: remove this when temporal blocking is implemented.
            assert(stop_rt == start_rt + 1);
            const idx_t rt = start_rt; // only one time value needed for block.

            // equations to evaluate at this time step.
            for (auto* eg : eqGroups) {
                if (!eqGroup_set || eqGroup_set->count(eg)) {
                    TRACE_MSG("calc_region: eq-group '" << eg->get_name() << "'");

                    // For wavefront adjustments, see conceptual diagram in
                    // calc_rank_opt().  In this function, 1 of the 4
                    // parallelogram-shaped regions is being evaluated.  At
                    // each time-step, the parallelogram may be trimmed
                    // based on the BB.
                    
                    // Actual region boundaries must stay within BB for this eq group.
                    idx_t begin_rw = max<idx_t>(start_dw, eg->begin_bbw);
                    idx_t end_rw = min<idx_t>(stop_dw, eg->end_bbw);
                    idx_t begin_rx = max<idx_t>(start_dx, eg->begin_bbx);
                    idx_t end_rx = min<idx_t>(stop_dx, eg->end_bbx);
                    idx_t begin_ry = max<idx_t>(start_dy, eg->begin_bby);
                    idx_t end_ry = min<idx_t>(stop_dy, eg->end_bby);
                    idx_t begin_rz = max<idx_t>(start_dz, eg->begin_bbz);
                    idx_t end_rz = min<idx_t>(stop_dz, eg->end_bbz);

                    // Only need to loop through the spatial extent of the
                    // region if any of its blocks are at least partly
                    // inside the domain. For overlapping regions, they may
                    // start outside the domain but enter the domain as time
                    // progresses and their boundaries shift. So, we don't
                    // want to return if this condition isn't met.
                    if (end_rw > begin_rw &&
                        end_rx > begin_rx &&
                        end_ry > begin_ry &&
                        end_rz > begin_rz) {

                        // Include automatically-generated loop code that
                        // calls calc_block() for each block in this region.
                        // Loops through w from begin_rw to end_rw-1;
                        // similar for x, y, and z.  This code typically
                        // contains the outer OpenMP loop(s).
#include "stencil_region_loops.hpp"

                    }
            
                    // Shift spatial region boundaries for next iteration to
                    // implement temporal wavefront.  We only shift
                    // backward, so region loops must increment. They may do
                    // so in any order.  TODO: shift only what is needed by
                    // this eq-group, not the global max.
                    start_dw -= angle_w;
                    stop_dw -= angle_w;
                    start_dx -= angle_x;
                    stop_dx -= angle_x;
                    start_dy -= angle_y;
                    stop_dy -= angle_y;
                    start_dz -= angle_z;
                    stop_dz -= angle_z;

                }            
            } // stencil equations.
        } // time.
    }

    // Calculate results within a block.
    void EqGroupBase::calc_block(idx_t bt,
                                 idx_t begin_bw, idx_t begin_bx, idx_t begin_by, idx_t begin_bz,
                                 idx_t end_bw, idx_t end_bx, idx_t end_by, idx_t end_bz)
    {
        TRACE_MSG2(get_name() << ".calc_block(t=" << bt <<
                  ", w=" << begin_bw << ".." << (end_bw-1) <<
                  ", x=" << begin_bx << ".." << (end_bx-1) <<
                  ", y=" << begin_by << ".." << (end_by-1) <<
                  ", z=" << begin_bz << ".." << (end_bz-1) <<
                  ").");
        StencilSettings& opts = _generic_context->get_settings();

        // Steps within a block are based on sub-block sizes.
        const idx_t step_bw = opts.sbw;
        const idx_t step_bx = opts.sbx;
        const idx_t step_by = opts.sby;
        const idx_t step_bz = opts.sbz;

        // Groups in block loops are based on sub-block-group sizes.
        const idx_t group_size_bw = opts.sbgw;
        const idx_t group_size_bx = opts.sbgx;
        const idx_t group_size_by = opts.sbgy;
        const idx_t group_size_bz = opts.sbgz;

        // Set number of threads for a block.
        // This should be nested within a top-level OpenMP task.
        _generic_context->set_block_threads();

        // Include automatically-generated loop code that calls
        // calc_sub_block() for each sub-block in this block.  Loops through
        // w from begin_bw to end_bw-1; similar for x, y, and z.  This
        // code typically contains the nested OpenMP loop(s).
#include "stencil_block_loops.hpp"
    }
    
    // Calculate results for one sub-block.
    // Each block is typically computed in a separate OpenMP thread.
    // The begin/end_sb* vars are the start/stop_b* vars from the block loops.
    void EqGroupBase::calc_sub_block(idx_t sbt,
                                     idx_t begin_sbw, idx_t begin_sbx, idx_t begin_sby, idx_t begin_sbz,
                                     idx_t end_sbw, idx_t end_sbx, idx_t end_sby, idx_t end_sbz)
    {
        TRACE_MSG2(get_name() << ".calc_sub_block(t=" << sbt <<
                   ", w=" << begin_sbw << ".." << (end_sbw-1) <<
                   ", x=" << begin_sbx << ".." << (end_sbx-1) <<
                   ", y=" << begin_sby << ".." << (end_sby-1) <<
                   ", z=" << begin_sbz << ".." << (end_sbz-1) <<
                   ").");
        
        // If not a 'simple' domain, must use scalar code.  TODO: this
        // is very inefficient--need to vectorize as much as possible.
        if (!bb_simple) {
            
            TRACE_MSG2("...using scalar code.");
            for (idx_t w = begin_sbw; w < end_sbw; w++)
                for (idx_t x = begin_sbx; x < end_sbx; x++)
                    for (idx_t y = begin_sby; y < end_sby; y++) {

                        // Are there holes in the BB?
                        if (bb_num_points != bb_size) {
                            for (idx_t z = begin_sbz; z < end_sbz; z++) {

                                // Update only if point is in sub-domain for this eq group.
                                if (is_in_valid_domain(sbt, ARG_W(w) x, y, z))
                                    calc_scalar(sbt, ARG_W(w) x, y, z);
                            }
                        }

                        // If no holes, don't need to check domain.
                        else {
                            for (idx_t z = begin_sbz; z < end_sbz; z++) {
                                calc_scalar(sbt, ARG_W(w) x, y, z);
                            }
                        }
                    }
        }

        // Full rectangular polytope: use optimized code.
        else {

            // Divide indices by vector lengths.  Use idiv_flr() instead of '/'
            // because begin/end vars may be negative (if in halo).
            const idx_t begin_sbtv = sbt;
            const idx_t begin_sbwv = idiv_flr<idx_t>(begin_sbw, VLEN_W);
            const idx_t begin_sbxv = idiv_flr<idx_t>(begin_sbx, VLEN_X);
            const idx_t begin_sbyv = idiv_flr<idx_t>(begin_sby, VLEN_Y);
            const idx_t begin_sbzv = idiv_flr<idx_t>(begin_sbz, VLEN_Z);
            const idx_t end_sbtv = sbt + CLEN_T;
            const idx_t end_sbwv = idiv_flr<idx_t>(end_sbw, VLEN_W);
            const idx_t end_sbxv = idiv_flr<idx_t>(end_sbx, VLEN_X);
            const idx_t end_sbyv = idiv_flr<idx_t>(end_sby, VLEN_Y);
            const idx_t end_sbzv = idiv_flr<idx_t>(end_sbz, VLEN_Z);

            // Evaluate sub-block of clusters.
            calc_sub_block_of_clusters(begin_sbtv, ARG_W(begin_sbwv)
                                       begin_sbxv, begin_sbyv, begin_sbzv,
                                       end_sbtv, ARG_W(end_sbwv) end_sbxv, end_sbyv, end_sbzv);
        }
        
        // Make sure stores are visible for later loads.
        make_stores_visible();
    }

    // Init MPI-related vars and other vars related to my rank's place in
    // the global problem: rank index, offset, etc.  Need to call this even
    // if not using MPI to properly init these vars.  Called from
    // allocAll(), so it doesn't normally need to be called from user code.
    void StencilContext::setupRank() {
        ostream& os = get_ostr();

        // Report ranks.
        os << "Num ranks: " << num_ranks << endl;
        os << "This rank index: " << my_rank << endl;

        // Check ranks.
        idx_t req_ranks = _opts->nrw * _opts->nrx * _opts->nry * _opts->nrz;
        if (req_ranks != num_ranks) {
            cerr << "error: " << req_ranks << " rank(s) requested, but " <<
                num_ranks << " rank(s) are active." << endl;
            exit_yask(1);
        }
        assertEqualityOverRanks(_opts->dt, comm, "time-step");

        // Determine my coordinates if not provided already.
        // TODO: do this more intelligently based on proximity.
        if (_opts->find_loc) {
            Layout_4321 rank_layout(_opts->nrw, _opts->nrx, _opts->nry, _opts->nrz);
            rank_layout.unlayout((idx_t)my_rank,
                                 _opts->riw, _opts->rix, _opts->riy, _opts->riz);
        }
        os << "Logical coordinates of this rank: " <<
            _opts->riw << ", " <<
            _opts->rix << ", " <<
            _opts->riy << ", " <<
            _opts->riz << endl;

        // A table of rank-coordinates for everyone.
        const int num_dims = 4;
        idx_t coords[num_ranks][num_dims];

        // Init coords for this rank.
        coords[my_rank][0] = _opts->riw;
        coords[my_rank][1] = _opts->rix;
        coords[my_rank][2] = _opts->riy;
        coords[my_rank][3] = _opts->riz;

        // A table of rank-sizes for everyone.
        idx_t rsizes[num_ranks][num_dims];

        // Init sizes for this rank.
        rsizes[my_rank][0] = _opts->dw;
        rsizes[my_rank][1] = _opts->dx;
        rsizes[my_rank][2] = _opts->dy;
        rsizes[my_rank][3] = _opts->dz;

#ifdef USE_MPI
        // Exchange coord and size info between all ranks.
        for (int rn = 0; rn < num_ranks; rn++) {
            MPI_Bcast(&coords[rn][0], num_dims, MPI_INTEGER8,
                      rn, comm);
            MPI_Bcast(&rsizes[rn][0], num_dims, MPI_INTEGER8,
                      rn, comm);
        }
#endif
        
        ofs_w = ofs_x = ofs_y = ofs_z = 0;
        tot_w = tot_x = tot_y = tot_z = 0;
        int num_neighbors = 0, num_exchanges = 0;
        for (int rn = 0; rn < num_ranks; rn++) {

            // Get coordinates of rn.
            idx_t rnw = coords[rn][0];
            idx_t rnx = coords[rn][1];
            idx_t rny = coords[rn][2];
            idx_t rnz = coords[rn][3];

            // Coord offset of rn from me: prev => negative, self => 0, next => positive.
            idx_t rdw = rnw - _opts->riw;
            idx_t rdx = rnx - _opts->rix;
            idx_t rdy = rny - _opts->riy;
            idx_t rdz = rnz - _opts->riz;

            // Get sizes of rn;
            idx_t rsw = rsizes[rn][0];
            idx_t rsx = rsizes[rn][1];
            idx_t rsy = rsizes[rn][2];
            idx_t rsz = rsizes[rn][3];
        
            // Accumulate total problem size in each dim for ranks that
            // intersect with this rank, including myself.
            // Adjust my offset in the global problem by adding all domain
            // sizes from prev ranks only.
            if (rdx == 0 && rdy == 0 && rdz == 0) {
                tot_w += rsw;
                if (rdw < 0)
                    ofs_w += rsw;
            }
            if (rdw == 0 && rdy == 0 && rdz == 0) {
                tot_x += rsx;
                if (rdx < 0)
                    ofs_x += rsx;
            }
            if (rdw == 0 && rdx == 0 && rdz == 0) {
                tot_y += rsy;
                if (rdy < 0)
                    ofs_y += rsy;
            }
            if (rdw == 0 && rdx == 0 && rdy == 0) {
                tot_z += rsz;
                if (rdz < 0)
                    ofs_z += rsz;
            }
            
            // Manhattan distance.
            int mdist = abs(rdw) + abs(rdx) + abs(rdy) + abs(rdz);
            
            // Myself.
            if (rn == my_rank) {
                if (mdist != 0) {
                    cerr << "internal error: distance to own rank == " << mdist << endl;
                    exit_yask(1);
                }
                continue; // nothing else to do for self.
            }

            // Someone else.
            else {
                if (mdist == 0) {
                    cerr << "error: ranks " << my_rank <<
                        " and " << rn << " at same coordinates." << endl;
                    exit_yask(1);
                }
            }
            
            // Rank rn is my immediate neighbor if its distance <= 1 in
            // every dim.  Assume we do not need to exchange halos except
            // with immediate neighbor. TODO: validate domain size is larger
            // than halo.
            if (abs(rdw) > 1 || abs(rdx) > 1 || abs(rdy) > 1 || abs(rdz) > 1)
                continue;

            // Save rank of this neighbor.
            // Add one to -1..+1 dist to get 0..2 range for my_neighbors indices.
            my_neighbors[rdw+1][rdx+1][rdy+1][rdz+1] = rn;
            num_neighbors++;
            os << "Neighbor #" << num_neighbors << " at " <<
                rnw << ", " << rnx << ", " << rny << ", " << rnz <<
                " is rank " << rn << endl;
                    
            // Check against max dist needed.  TODO: determine max dist
            // automatically from stencil equations; may not be same for all
            // grids.
#ifndef MAX_EXCH_DIST
#define MAX_EXCH_DIST 4
#endif

            // Is buffer needed?
            // TODO: calculate and use exch dist for each grid.
            if (mdist > MAX_EXCH_DIST) {
                os << " No halo exchange with rank " << rn << '.' << endl;
                continue;
            }

            // Determine size of MPI buffers between rn and my rank.
            // Need send and receive for each updated grid.
            for (auto* gp : gridPtrs) {
                auto& gname = gp->get_name();
                
                // Size of buffer in each direction: if dist to neighbor is
                // zero in given direction (i.e., is perpendicular to this
                // rank), use domain size; otherwise, use halo size.
                idx_t bsw = ROUND_UP((rdw == 0) ? _opts->dw : gp->get_halo_w(), VLEN_W);
                idx_t bsx = ROUND_UP((rdx == 0) ? _opts->dx : gp->get_halo_x(), VLEN_X);
                idx_t bsy = ROUND_UP((rdy == 0) ? _opts->dy : gp->get_halo_y(), VLEN_Y);
                idx_t bsz = ROUND_UP((rdz == 0) ? _opts->dz : gp->get_halo_z(), VLEN_Z);

                if (bsw * bsx * bsy * bsz == 0) {
                    os << " No halo exchange needed for grid '" << gname <<
                        "' with rank " << rn << '.' << endl;
                }
                else {

                    // Make a buffer in each direction (send & receive).
                    size_t num_bytes = 0;
                    for (int bd = 0; bd < MPIBufs::nBufDirs; bd++) {
                        ostringstream oss;
                        oss << gname;
                        if (bd == MPIBufs::bufSend)
                            oss << "_send_halo_from_" << my_rank << "_to_" << rn;
                        else
                            oss << "_get_halo_to_" << my_rank << "_from_" << rn;
                        
                        Grid_WXYZ* bp = mpiBufs[gname].makeBuf(bd,
                                                               rdw+1, rdx+1, rdy+1, rdz+1,
                                                               bsw, bsx, bsy, bsz,
                                                               oss.str());
                        num_bytes += bp->get_num_bytes();
                        num_exchanges++;
                    }

                    os << " Halo exchange of " << printWithPow2Multiplier(num_bytes) <<
                        "B enabled for grid '" << gname << "' with rank " << rn << '.' << endl;
                }
            }
        }
        os << "Number of halo exchanges from this rank: " << num_exchanges << endl;
        os << "Problem-domain offsets of this rank: " <<
            ofs_w << ", " << ofs_x << ", " << ofs_y << ", " << ofs_z << endl;
    }

    // Allocate memory for grids, params, and MPI bufs.
    // TODO: allow different types of memory for different grids, MPI bufs, etc.
    void StencilContext::allocData() {
        ostream& os = get_ostr();

        // if '_data_buf' is null, allocate memory and call recursively to distribute.
        // If '_data_buf' is not null, distribute already-allocated memory.
        
        // Determine how many bytes are needed.
        size_t nbytes = 0, gbytes = 0, pbytes = 0, bbytes = 0;
        
        // Grids.
        for (auto gp : gridPtrs) {

            // set grid sizes from settings.
            gp->set_dw(_opts->dw);
            gp->set_dx(_opts->dx);
            gp->set_dy(_opts->dy);
            gp->set_dz(_opts->dz);
            gp->set_pad_w(_opts->pw);
            gp->set_pad_x(_opts->px);
            gp->set_pad_y(_opts->py);
            gp->set_pad_z(_opts->pz);
            gp->set_ofs_w(ofs_w);
            gp->set_ofs_x(ofs_x);
            gp->set_ofs_y(ofs_y);
            gp->set_ofs_z(ofs_z);

            // set storage if requested.
            if (_data_buf) {
                gp->set_storage(_data_buf, nbytes);
                gp->print_info(os);
            }

            // determine size used (also offset to next location).
            gbytes += gp->get_num_bytes();
            nbytes += ROUND_UP(gp->get_num_bytes() + _data_buf_pad,
                               CACHELINE_BYTES);
            TRACE_MSG("grid '" << gp->get_name() << "' needs " <<
                      gp->get_num_bytes() << " bytes");
        }

        // Params.
        for (auto pp : paramPtrs) {

            // set storage if requested.
            if (_data_buf)
                pp->set_storage(_data_buf, nbytes);

            // determine size used (also offset to next location).
            pbytes += pp->get_num_bytes();
            nbytes += ROUND_UP(pp->get_num_bytes() + _data_buf_pad,
                               CACHELINE_BYTES);
            TRACE_MSG("param needs " <<
                      pp->get_num_bytes() << " bytes");
        }

        // MPI buffers.
        for (auto* gp : gridPtrs) {
            auto& gname = gp->get_name();
            if (mpiBufs.count(gname) == 0)
                continue;

            // Visit buffers for each neighbor for this grid.
            mpiBufs[gname].visitNeighbors
                (*this, false,
                 [&](idx_t nw, idx_t nx, idx_t ny, idx_t nz,
                     int rank,
                     Grid_WXYZ* sendBuf,
                     Grid_WXYZ* rcvBuf)
                 {
                     // Send.
                     if (_data_buf)
                         sendBuf->set_storage(_data_buf, nbytes);
                     bbytes += sendBuf->get_num_bytes();
                     nbytes += ROUND_UP(sendBuf->get_num_bytes() + _data_buf_pad,
                                        CACHELINE_BYTES);
                     TRACE_MSG("send buf '" << sendBuf->get_name() << "' needs " <<
                               sendBuf->get_num_bytes() << " bytes");

                     // Recv.
                     if (_data_buf)
                         rcvBuf->set_storage(_data_buf, nbytes);
                     bbytes += rcvBuf->get_num_bytes();
                     nbytes += ROUND_UP(rcvBuf->get_num_bytes() + _data_buf_pad,
                                        CACHELINE_BYTES);
                     TRACE_MSG("rcv buf '" << rcvBuf->get_name() << "' needs " <<
                               rcvBuf->get_num_bytes() << " bytes");
                 } );
        }

        // Don't need pad after last one.
        if (nbytes)
            nbytes -= _data_buf_pad;

        // Allocate and distribute data.
        if (!_data_buf) {
            os << "Allocating " << printWithPow2Multiplier(nbytes) <<
                "B for all grids, parameters, and other buffers with a " <<
                printWithPow2Multiplier(_data_buf_alignment) << "B alignment...\n" << flush;
            int ret = posix_memalign(&_data_buf, _data_buf_alignment, nbytes);
            if (ret || !_data_buf) {
                cerr << "Error: unable to allocate memory.\n";
                exit_yask(1);
            }
            _data_buf_size = nbytes;

            os << "  " << printWithPow2Multiplier(gbytes) << "B for grid(s).\n" <<
                "  " << printWithPow2Multiplier(pbytes) << "B for parameters(s).\n" <<
                "  " << printWithPow2Multiplier(bbytes) << "B for MPI buffers(s).\n" <<
                "  " << printWithPow2Multiplier(nbytes - gbytes - pbytes - bbytes) <<
                "B for inter-data padding.\n";
                
            // Distribute this allocation w/a recursive call.
            allocData();
        }
    }

    // Allocate grids, params, and MPI bufs.
    // Initialize some data structures.
    void StencilContext::allocAll()
    {
        // Don't continue until all ranks are this far.
        global_barrier();

        ostream& os = get_ostr();
#ifdef DEBUG
        os << "*** WARNING: YASK compiled with DEBUG; ignore performance results.\n";
#endif
#if defined(NO_INTRINSICS) && (VLEN > 1)
        os << "*** WARNING: YASK compiled with NO_INTRINSICS; ignore performance results.\n";
#endif
#ifdef MODEL_CACHE
        os << "*** WARNING: YASK compiled with MODEL_CACHE; ignore performance results.\n";
#endif
#ifdef TRACE_MEM
        os << "*** WARNING: YASK compiled with TRACE_MEM; ignore performance results.\n";
#endif
#ifdef TRACE_INTRINSICS
        os << "*** WARNING: YASK compiled with TRACE_INTRINSICS; ignore performance results.\n";
#endif
        
        // Adjust all settings before setting MPI buffers or sizing grids.
        _opts->finalizeSettings(os);
        
        // report threads.
        os << endl;
        os << "Num OpenMP procs: " << omp_get_num_procs() << endl;
        set_all_threads();
        os << "Num OpenMP threads: " << omp_get_max_threads() << endl;
        set_region_threads(); // Temporary; just for reporting.
        os << "  Num threads per region: " << omp_get_max_threads() << endl;
        set_block_threads(); // Temporary; just for reporting.
        os << "  Num threads per block: " << omp_get_max_threads() << endl;
        set_all_threads(); // Back to normal.

        // TODO: enable multi-rank wave-front tiling.
        if (_opts->rt > 1 && num_ranks > 1) {
            cerr << "MPI communication is not currently enabled with wave-front tiling." << endl;
            exit_yask(1);
        }

        os << endl;
        os << "Num grids: " << gridPtrs.size() << endl;
        os << "Num grids to be updated: " << outputGridPtrs.size() << endl;
        os << "Num stencil equation-groups: " << eqGroups.size() << endl;
        
        // Set up MPI data.  Must do this before sizing grids so that
        // global offsets are calculated properly.
        if (num_ranks > 1)
            os << "Creating MPI buffers..." << endl;
        setupRank();

        // Determine bounding-boxes for all eq-groups.
        find_bounding_boxes();

        // Alloc grids, params, and MPI bufs.
        allocData();

        // Report some stats.
        idx_t dt = _opts->dt;
        os << "\nSizes in points per grid (t*w*x*y*z):\n"
            " vector-size:          " <<
            VLEN_T << '*' << VLEN_W << '*' << VLEN_X << '*' << VLEN_Y << '*' << VLEN_Z << endl <<
            " cluster-size:         " <<
            CPTS_T << '*' << CPTS_W << '*' << CPTS_X << '*' << CPTS_Y << '*' << CPTS_Z << endl <<
            " sub-block-size:       " <<
            _opts->sbt << '*' << _opts->sbw << '*' << _opts->sbx << '*' << _opts->sby << '*' << _opts->sbz << endl <<
            " sub-block-group-size: 1*" <<
            _opts->sbgw << '*' << _opts->sbgx << '*' << _opts->sbgy << '*' << _opts->sbgz << endl <<
            " block-size:           " <<
            _opts->bt << '*' << _opts->bw << '*' << _opts->bx << '*' << _opts->by << '*' << _opts->bz << endl <<
            " block-group-size:     1*" <<
            _opts->bgw << '*' << _opts->bgx << '*' << _opts->bgy << '*' << _opts->bgz << endl <<
            " region-size:          " <<
            _opts->rt << '*' << _opts->rw << '*' << _opts->rx << '*' << _opts->ry << '*' << _opts->rz << endl <<
            " rank-domain-size:     " <<
            dt << '*' << _opts->dw << '*' << _opts->dx << '*' << _opts->dy << '*' << _opts->dz << endl <<
            " overall-problem-size: " <<
            dt << '*' << tot_w << '*' << tot_x << '*' << tot_y << '*' << tot_z << endl <<
            endl <<
            "Other settings:\n"
            " stencil-name: " YASK_STENCIL_NAME << endl << 
            " num-ranks: " <<
            _opts->nrw << '*' << _opts->nrx << '*' << _opts->nry << '*' << _opts->nrz << endl <<
            " vector-len: " << VLEN << endl <<
            " extra-padding: " <<
            _opts->pw << '+' << _opts->px << '+' << _opts->py << '+' << _opts->pz << endl <<
            " max-wave-front-angles: " <<
            angle_w << '+' << angle_x << '+' << angle_y << '+' << angle_z << endl <<
            " max-halos: " << hw << '+' << hx << '+' << hy << '+' << hz << endl <<
            " manual-L1-prefetch-distance: " << PFDL1 << endl <<
            " manual-L2-prefetch-distance: " << PFDL2 << endl <<
            endl;
        
        // sums across eqs for this rank.
        rank_numpts_1t = 0;
        rank_reads_1t = 0;
        rank_numFpOps_1t = 0;
        for (auto eg : eqGroups) {
            idx_t updates1 = eg->get_scalar_points_written();
            idx_t updates_domain = updates1 * eg->bb_num_points;
            rank_numpts_1t += updates_domain;
            idx_t reads1 = eg->get_scalar_points_read();
            idx_t reads_domain = reads1 * eg->bb_num_points;
            rank_reads_1t += reads_domain;
            idx_t fpops1 = eg->get_scalar_fp_ops();
            idx_t fpops_domain = fpops1 * eg->bb_num_points;
            rank_numFpOps_1t += fpops_domain;
            os << "Stats for equation-group '" << eg->get_name() << "':\n" <<
                " sub-domain size:            " <<
                eg->len_bbw << '*' << eg->len_bbx << '*' << eg->len_bby << '*' << eg->len_bbz << endl <<
                " valid points in sub domain: " << printWithPow10Multiplier(eg->bb_num_points) << endl <<
                " grid-updates per point:     " << updates1 << endl <<
                " grid-updates in sub-domain: " << printWithPow10Multiplier(updates_domain) << endl <<
                " grid-reads per point:       " << reads1 << endl <<
                " grid-reads in sub-domain:   " << printWithPow10Multiplier(reads_domain) << endl <<
                " est FP-ops per point:       " << fpops1 << endl <<
                " est FP-ops in sub-domain:   " << printWithPow10Multiplier(fpops_domain) << endl;
        }

        // Report total allocation.
        rank_nbytes = get_num_bytes();
        os << "Total allocation in this rank (bytes): " <<
            printWithPow2Multiplier(rank_nbytes) << endl;
        tot_nbytes = sumOverRanks(rank_nbytes, comm);
        os << "Total overall allocation in " << num_ranks << " rank(s) (bytes): " <<
            printWithPow2Multiplier(tot_nbytes) << endl;
    
        // Various metrics for amount of work.
        rank_numpts_dt = rank_numpts_1t * dt;
        tot_numpts_1t = sumOverRanks(rank_numpts_1t, comm);
        tot_numpts_dt = tot_numpts_1t * dt;

        rank_reads_dt = rank_reads_1t * dt;
        tot_reads_1t = sumOverRanks(rank_reads_1t, comm);
        tot_reads_dt = tot_reads_1t * dt;

        rank_numFpOps_dt = rank_numFpOps_1t * dt;
        tot_numFpOps_1t = sumOverRanks(rank_numFpOps_1t, comm);
        tot_numFpOps_dt = tot_numFpOps_1t * dt;

        rank_domain_1t = _opts->dw * _opts->dx * _opts->dy * _opts->dz;
        rank_domain_dt = rank_domain_1t * dt;
        tot_domain_1t = sumOverRanks(rank_domain_1t, comm);
        tot_domain_dt = tot_domain_1t * dt;
    
        // Print some more stats.
        os << endl <<
            "Amount-of-work stats:\n" <<
            " domain-size in this rank, for one time-step: " <<
            printWithPow10Multiplier(rank_domain_1t) << endl <<
            " overall-problem-size in all ranks, for one time-step: " <<
            printWithPow10Multiplier(tot_domain_1t) << endl <<
            " domain-size in this rank, for all time-steps: " <<
            printWithPow10Multiplier(rank_domain_dt) << endl <<
            " overall-problem-size in all ranks, for all time-steps: " <<
            printWithPow10Multiplier(tot_domain_dt) << endl <<
            endl <<
            " grid-point-updates in this rank, for one time-step: " <<
            printWithPow10Multiplier(rank_numpts_1t) << endl <<
            " grid-point-updates in all ranks, for one time-step: " <<
            printWithPow10Multiplier(tot_numpts_1t) << endl <<
            " grid-point-updates in this rank, for all time-steps: " <<
            printWithPow10Multiplier(rank_numpts_dt) << endl <<
            " grid-point-updates in all ranks, for all time-steps: " <<
            printWithPow10Multiplier(tot_numpts_dt) << endl <<
            endl <<
            " grid-point-reads in this rank, for one time-step: " <<
            printWithPow10Multiplier(rank_reads_1t) << endl <<
            " grid-point-reads in all ranks, for one time-step: " <<
            printWithPow10Multiplier(tot_reads_1t) << endl <<
            " grid-point-reads in this rank, for all time-steps: " <<
            printWithPow10Multiplier(rank_reads_dt) << endl <<
            " grid-point-reads in all ranks, for all time-steps: " <<
            printWithPow10Multiplier(tot_reads_dt) << endl <<
            endl <<
            " est-FP-ops in this rank, for one time-step: " <<
            printWithPow10Multiplier(rank_numFpOps_1t) << endl <<
            " est-FP-ops in all ranks, for one time-step: " <<
            printWithPow10Multiplier(tot_numFpOps_1t) << endl <<
            " est-FP-ops in this rank, for all time-steps: " <<
            printWithPow10Multiplier(rank_numFpOps_dt) << endl <<
            " est-FP-ops in all ranks, for all time-steps: " <<
            printWithPow10Multiplier(tot_numFpOps_dt) << endl <<
            endl << 
            "Notes:\n"
            " Domain-sizes and overall-problem-sizes are based on rank-domain sizes (dw * dx * dy * dz)\n"
            "  and number of ranks (nrw * nrx * nry * nrz) regardless of number of grids or sub-domains.\n"
            " Grid-point-updates are based on sum of grid-updates in sub-domain across equation-group(s).\n"
            " Grid-point-reads are based on sum of grid-reads in sub-domain across equation-group(s).\n"
            " Est-FP-ops are based on sum of est-FP-ops in sub-domain across equation-group(s).\n"
            "\n";
    }

    // Init all grids & params by calling initFn.
    void StencilContext::initValues(function<void (RealVecGridBase* gp, 
                                                   real_t seed)> realVecInitFn,
                                    function<void (RealGrid* gp,
                                                   real_t seed)> realInitFn)
    {
        ostream& os = get_ostr();
        real_t v = 0.1;
        os << "Initializing grids..." << endl;
        for (auto gp : gridPtrs) {
            realVecInitFn(gp, v);
            v += 0.01;
        }
        if (paramPtrs.size()) {
            os << "Initializing parameters..." << endl;
            for (auto pp : paramPtrs) {
                realInitFn(pp, v);
                v += 0.01;
            }
        }
    }

    // Compare grids in contexts.
    // Return number of mis-compares.
    idx_t StencilContext::compareData(const StencilContext& ref) const {
        ostream& os = get_ostr();

        os << "Comparing grid(s) in '" << name << "' to '" << ref.name << "'..." << endl;
        if (gridPtrs.size() != ref.gridPtrs.size()) {
            cerr << "** number of grids not equal." << endl;
            return 1;
        }
        idx_t errs = 0;
        for (size_t gi = 0; gi < gridPtrs.size(); gi++) {
            os << "Grid '" << ref.gridPtrs[gi]->get_name() << "'..." << endl;
            errs += gridPtrs[gi]->compare(*ref.gridPtrs[gi]);
        }

        os << "Comparing parameter(s) in '" << name << "' to '" << ref.name << "'..." << endl;
        if (paramPtrs.size() != ref.paramPtrs.size()) {
            cerr << "** number of params not equal." << endl;
            return 1;
        }
        for (size_t pi = 0; pi < paramPtrs.size(); pi++) {
            errs += paramPtrs[pi]->compare(ref.paramPtrs[pi], EPSILON);
        }

        return errs;
    }
    
    
    // Set the bounding-box around all eq groups.
    void StencilContext::find_bounding_boxes()
    {
        if (bb_valid == true) return;

        // Init overall BB.
        // Init min vars w/max val and vice-versa.
        begin_bbw = idx_max; end_bbw = idx_min;
        begin_bbx = idx_max; end_bbx = idx_min;
        begin_bby = idx_max; end_bby = idx_min;
        begin_bbz = idx_max; end_bbz = idx_min;

        // Find BB for each eq group and update context.
        for (auto eg : eqGroups) {
            eg->find_bounding_box();

            begin_bbw = min(begin_bbw, eg->begin_bbw);
            begin_bbx = min(begin_bbx, eg->begin_bbx);
            begin_bby = min(begin_bby, eg->begin_bby);
            begin_bbz = min(begin_bbz, eg->begin_bbz);
            end_bbw = max(end_bbw, eg->end_bbw);
            end_bbx = max(end_bbx, eg->end_bbx);
            end_bby = max(end_bby, eg->end_bby);
            end_bbz = max(end_bbz, eg->end_bbz);
        }
        update_lengths();

        // These vars are N/A for the overall BB.
        bb_num_points = 0;
        bb_simple = false;
        
        // Adjust region size to be within BB.
        _opts->rw = min(_opts->rw, len_bbw);
        _opts->rx = min(_opts->rx, len_bbx);
        _opts->ry = min(_opts->ry, len_bby);
        _opts->rz = min(_opts->rz, len_bbz);

        // Adjust block size to be within region.
        _opts->bw = min(_opts->bw, _opts->rw);
        _opts->bx = min(_opts->bx, _opts->rx);
        _opts->by = min(_opts->by, _opts->ry);
        _opts->bz = min(_opts->bz, _opts->rz);

        // Determine the max spatial skewing angles for temporal wavefronts
        // based on the max halos.  This assumes the smallest granularity of
        // calculation is CPTS_* in each dim.  We only need non-zero angles
        // if the region size is less than the rank size, i.e., if the
        // region covers the whole rank in a given dimension, no wave-front
        // is needed in thar dim.
        angle_w = (_opts->rw < len_bbw) ? ROUND_UP(hw, CPTS_W) : 0;
        angle_x = (_opts->rx < len_bbx) ? ROUND_UP(hx, CPTS_X) : 0;
        angle_y = (_opts->ry < len_bby) ? ROUND_UP(hy, CPTS_Y) : 0;
        angle_z = (_opts->rz < len_bbz) ? ROUND_UP(hz, CPTS_Z) : 0;
    }

    // Set the bounding-box vars for this eq group in this rank.
    void EqGroupBase::find_bounding_box() {
        if (bb_valid) return;
        StencilContext& context = *_generic_context;
        StencilSettings& opts = context.get_settings();
        ostream& os = context.get_ostr();

        // Init min vars w/max val and vice-versa.
        idx_t minw = idx_max, maxw = idx_min;
        idx_t minx = idx_max, maxx = idx_min;
        idx_t miny = idx_max, maxy = idx_min;
        idx_t minz = idx_max, maxz = idx_min;
        idx_t npts = 0;
        
        // Assume bounding-box is same for all time steps.
        // TODO: consider adding time to domain.
        idx_t t = 0;

        // Loop through 4D space.
        // Find the min and max valid points in this space.
        // Looping also through halos since this would solve the problem
        // of unaligned writes
#pragma omp parallel for collapse(4)            \
        reduction(min:minw,minx,miny,minz)          \
        reduction(max:maxw,maxx,maxy,maxz)          \
        reduction(+:npts)
/*       for (idx_t w = context.ofs_w; w < context.ofs_w + opts.dw; w++)
            for(idx_t x = context.ofs_x; x < context.ofs_x + opts.dx; x++)
                for(idx_t y = context.ofs_y; y < context.ofs_y + opts.dy; y++)
                    for(idx_t z = context.ofs_z; z < context.ofs_z + opts.dz; z++) {
*/
      for (idx_t w = context.ofs_w-context.hw; w < context.ofs_w+opts.dw+context.hw; w++)
            for(idx_t x = context.ofs_x-context.hx; x < context.ofs_x+context.hx + opts.dx; x++)
                for(idx_t y = context.ofs_y-context.hy; y < context.ofs_y+context.hy + opts.dy; y++)
                    for(idx_t z = context.ofs_z-context.hz; z < context.ofs_z+context.hz + opts.dz; z++) {


                        // Update only if point in domain for this eq group.
                        if (is_in_valid_domain(t, ARG_W(w) x, y, z)) {
                            minw = min(minw, w);
                            maxw = max(maxw, w);
                            minx = min(minx, x);
                            maxx = max(maxx, x);
                            miny = min(miny, y);
                            maxy = max(maxy, y);
                            minz = min(minz, z);
                            maxz = max(maxz, z);
                            npts++;
                        }
                    }


        // Set begin vars to min indices and end vars to one beyond max indices.
        if (npts) {
            begin_bbw = minw;
            end_bbw = maxw + 1;
            begin_bbx = minx;
            end_bbx = maxx + 1;
            begin_bby = miny;
            end_bby = maxy + 1;
            begin_bbz = minz;
            end_bbz = maxz + 1;

            /*printf("check bbx : %d - %d\n", begin_bbx, end_bbx);
            printf("check bby : %d - %d\n", begin_bby, end_bby);
            printf("check bbz : %d - %d\n", begin_bbz, end_bbz);*/

       } else {
            begin_bbw = end_bbw = 0;
            begin_bbx = end_bbx = 0;
            begin_bby = end_bby = 0;
            begin_bbz = end_bbz = 0;
        }
        update_lengths();
        bb_num_points = npts;

        // Solid rectangle?
        if (bb_num_points != bb_size) {
            os << "Warning: domain for equation-group '" << get_name() << "' has only " <<
                printWithPow10Multiplier(bb_num_points) <<
                " valid point(s) inside its bounding-box of " <<
                printWithPow10Multiplier(bb_size) <<
                " point(s); slower scalar calculations will be used.\n";
            bb_simple = false;
        }

        // Lengths are cluster-length multiples?
        else if (len_bbw % CLEN_W ||
                 len_bbx % CLEN_X ||
                 len_bby % CLEN_Y ||
                 len_bbz % CLEN_Z) {
            os << "Warning: domain for equation-group '" << get_name() <<
                "' has one or more sizes that are not vector-cluster multiples;"
                " slower scalar calculations will be used.\n";
            bb_simple = false;
        }

        // Edges are cluster-length multiples?
        else if (begin_bbw % CLEN_W ||
                 begin_bbx % CLEN_X ||
                 begin_bby % CLEN_Y ||
                 begin_bbz % CLEN_Z) {
            os << "Warning: domain for equation-group '" << get_name() <<
                "' has one or more edges that do not start on vector-cluster boundaries;"
                " slower scalar calculations will be used.\n";
            bb_simple = false;
        }
    }
    
    // Exchange halo data needed by eq-group 'eg' at the given time.
    // Data is needed for input grids that have not already been updated.
    // TODO: overlap halo exchange with computation.
    void StencilContext::exchange_halos(idx_t start_dt, idx_t stop_dt, EqGroupBase& eg)
    {
        StencilSettings& opts = get_settings();
        TRACE_MSG("exchange_halos(t=" << start_dt << ".." << (stop_dt-1) <<
                  ", needed for eq-group '" << eg.get_name() << "')");

#ifdef USE_MPI
        double start_time = getTimeInSecs();

        // These vars control blocking within halo packing.
        // Currently, only zv has a loop in the calc_halo macros below.
        // Thus, step_{w,x,y}v must be 1.
        const idx_t step_wv = 1;
        const idx_t step_xv = 1;
        const idx_t step_yv = 1;
#ifndef HALO_STEP_ZV
#define HALO_STEP_ZV (4)
#endif
        const idx_t step_zv = HALO_STEP_ZV;

        // Groups in halo loops are set to smallest size.
        const idx_t group_size_wv = 1;
        const idx_t group_size_xv = 1;
        const idx_t group_size_yv = 1;
        const idx_t group_size_zv = 1;

        // 1D array to store send request handles.
        // We use a 1D array so we can call MPI_Waitall().
        MPI_Request send_reqs[eg.inputGridPtrs.size() * MPIBufs::neighborhood_size];
        int num_send_reqs = 0;

        // 2D array for receive request handles.
        // We use a 2D array to simplify individual indexing.
        MPI_Request recv_reqs[eg.inputGridPtrs.size()][MPIBufs::neighborhood_size];

        // Sequence of things to do for each grid's neighbors.
        enum halo_steps { halo_irecv, halo_isend, halo_unpack, halo_nsteps };
        for (int hi = 0; hi < halo_nsteps; hi++) {

            if (hi == halo_irecv)
                TRACE_MSG("exchange_halos: requesting data...");
            else if (hi == halo_isend)
                TRACE_MSG("exchange_halos: sending data...");
            else if (hi == halo_unpack)
                TRACE_MSG("exchange_halos: unpacking data...");
            
            // Loop thru all grids.
            for (size_t gi = 0; gi < eg.inputGridPtrs.size(); gi++) {
                auto gp = eg.inputGridPtrs[gi];

                // Only need to swap grids whose halos are not up-to-date.
                if (gp->is_updated())
                    continue;

                // Only need to swap grids that have MPI buffers.
                auto& gname = gp->get_name();
                if (mpiBufs.count(gname) == 0)
                    continue;
            
                // Determine halo sizes to be exchanged for this grid.  Round up
                // to vector lengths because the halo exchange only works with
                // whole vectors. TODO: make this more efficient for halos that
                // are not vector-length multiples.
                idx_t ghw = ROUND_UP(gp->get_halo_w(), VLEN_W);
                idx_t ghx = ROUND_UP(gp->get_halo_x(), VLEN_X);
                idx_t ghy = ROUND_UP(gp->get_halo_y(), VLEN_Y);
                idx_t ghz = ROUND_UP(gp->get_halo_z(), VLEN_Z);

                // Visit all this rank's neighbors.
                int ni = 0;
                mpiBufs[gname].visitNeighbors
                    (*this, false,
                     [&](idx_t nw, idx_t nx, idx_t ny, idx_t nz,
                         int neighbor_rank,
                         Grid_WXYZ* sendBuf,
                         Grid_WXYZ* rcvBuf)
                     {
                         ni++;

                         // Submit request to receive data from neighbor.
                         if (hi == halo_irecv) {
                             TRACE_MSG("exchange_halos: requesting data from rank " <<
                                       neighbor_rank << " for grid '" << gname << "'...");
                             void* buf = (void*)(rcvBuf->get_storage());
                             MPI_Irecv(buf, rcvBuf->get_num_bytes(), MPI_BYTE,
                                       neighbor_rank, int(gi), comm, &recv_reqs[gi][ni]);
                         }

                         // Common code for pack (and send) and unpack.
                         if (hi == halo_isend || hi == halo_unpack) {

                             // Set begin/end vars to indicate what part
                             // of main grid to read from or write to.
                             // Init range to whole rank domain (inside halos).
                             idx_t begin_w = 0;
                             idx_t begin_x = 0;
                             idx_t begin_y = 0;
                             idx_t begin_z = 0;
                             idx_t end_w = opts.dw;
                             idx_t end_x = opts.dx;
                             idx_t end_y = opts.dy;
                             idx_t end_z = opts.dz;

                             // Region to read from, i.e., data to be put into receiver's halo.
                             if (hi == halo_isend) {

                                 // Modify begin and/or end based on direction.
                                 if (nw == idx_t(MPIBufs::rank_prev)) // neighbor is prev W.
                                     end_w = ghw; // read first halo-width only.
                                 if (nw == idx_t(MPIBufs::rank_next)) // neighbor is next W.
                                     begin_w = opts.dw - ghw; // read last halo-width only.
                                 if (nx == idx_t(MPIBufs::rank_prev)) // neighbor is on left.
                                     end_x = ghx;
                                 if (nx == idx_t(MPIBufs::rank_next)) // neighbor is on right.
                                     begin_x = opts.dx - ghx;
                                 if (ny == idx_t(MPIBufs::rank_prev)) // neighbor is in front.
                                     end_y = ghy;
                                 if (ny == idx_t(MPIBufs::rank_next)) // neighbor is in back.
                                     begin_y = opts.dy - ghy;
                                 if (nz == idx_t(MPIBufs::rank_prev)) // neighbor is above.
                                     end_z = ghz;
                                 if (nz == idx_t(MPIBufs::rank_next)) // neighbor is below.
                                     begin_z = opts.dz - ghz;
                             }

                             // Region to write to, i.e., this rank's halo.
                             else if (hi == halo_unpack) {

                                 // Modify begin and/or end based on direction.
                                 if (nw == idx_t(MPIBufs::rank_prev)) { // neighbor is prev W.
                                     begin_w = -ghw; // begin at outside of halo.
                                     end_w = 0;     // end at inside of halo.
                                 }
                                 if (nw == idx_t(MPIBufs::rank_next)) { // neighbor is next W.
                                     begin_w = opts.dw; // begin at inside of halo.
                                     end_w = opts.dw + ghw; // end of outside of halo.
                                 }
                                 if (nx == idx_t(MPIBufs::rank_prev)) { // neighbor is on left.
                                     begin_x = -ghx;
                                     end_x = 0;
                                 }
                                 if (nx == idx_t(MPIBufs::rank_next)) { // neighbor is on right.
                                     begin_x = opts.dx;
                                     end_x = opts.dx + ghx;
                                 }
                                 if (ny == idx_t(MPIBufs::rank_prev)) { // neighbor is in front.
                                     begin_y = -ghy;
                                     end_y = 0;
                                 }
                                 if (ny == idx_t(MPIBufs::rank_next)) { // neighbor is in back.
                                     begin_y = opts.dy;
                                     end_y = opts.dy + ghy;
                                 }
                                 if (nz == idx_t(MPIBufs::rank_prev)) { // neighbor is above.
                                     begin_z = -ghz;
                                     end_z = 0;
                                 }
                                 if (nz == idx_t(MPIBufs::rank_next)) { // neighbor is below.
                                     begin_z = opts.dz;
                                     end_z = opts.dz + ghz;
                                 }
                             }

                             // Add offsets and divide indices by vector
                             // lengths. Use idiv_flr() because indices may be neg (in halo).
                             idx_t begin_wv = idiv_flr<idx_t>(ofs_w + begin_w, VLEN_W);
                             idx_t begin_xv = idiv_flr<idx_t>(ofs_x + begin_x, VLEN_X);
                             idx_t begin_yv = idiv_flr<idx_t>(ofs_y + begin_y, VLEN_Y);
                             idx_t begin_zv = idiv_flr<idx_t>(ofs_z + begin_z, VLEN_Z);
                             idx_t end_wv = idiv_flr<idx_t>(ofs_w + end_w, VLEN_W);
                             idx_t end_xv = idiv_flr<idx_t>(ofs_x + end_x, VLEN_X);
                             idx_t end_yv = idiv_flr<idx_t>(ofs_y + end_y, VLEN_Y);
                             idx_t end_zv = idiv_flr<idx_t>(ofs_z + end_z, VLEN_Z);

                             // Assume only one time-step to exchange.
                             // TODO: fix this when MPI + wave-front is enabled.
                             assert(stop_dt = start_dt + 1);
                             idx_t t = start_dt;

                             // Force dummy time value for grids w/o time dim.
                             if (!gp->got_t())
                                 t = 0;

                             // Wait for data.
                             if (hi == halo_unpack) {
                                 TRACE_MSG("exchange_halos: waiting for data from rank " <<
                                           neighbor_rank << " for grid '" << gname << "'...");
                                 MPI_Wait(&recv_reqs[gi][ni], MPI_STATUS_IGNORE);
                                 TRACE_MSG("exchange_halos: done waiting for data.");
                             }

                             // Define calc_halo to copy data between main grid and MPI buffer.
                             // Add a short loop in z-dim to increase work done in halo loop.
                             // Use 'index_*' vars to access buffers because they are always 0-based.
#define calc_halo(t,                                                    \
                  start_wv, start_xv, start_yv, start_zv,               \
                  stop_wv, stop_xv, stop_yv, stop_zv)  do {             \
                                 idx_t wv = start_wv;                   \
                                 idx_t xv = start_xv;                   \
                                 idx_t yv = start_yv;                   \
                                 idx_t izv = index_zv * step_zv;        \
                                 if (hi == halo_isend) {                \
                                     for (idx_t zv = start_zv; zv < stop_zv; zv++) { \
                                         real_vec_t hval =              \
                                             gp->readVecNorm_TWXYZ(t, wv, xv, yv, zv, \
                                                                   __LINE__); \
                                         sendBuf->writeVecNorm(hval, index_wv, index_xv, index_yv, izv++, \
                                                               __LINE__); \
                                     }                                  \
                                 } else if (hi == halo_unpack) {        \
                                     for (idx_t zv = start_zv; zv < stop_zv; zv++) { \
                                         real_vec_t hval =              \
                                             rcvBuf->readVecNorm(index_wv, index_xv, index_yv, izv++, \
                                                                 __LINE__); \
                                         gp->writeVecNorm_TWXYZ(hval, t, wv, xv, yv, zv, \
                                                                __LINE__); \
                                     } } } while(0)

                             // Include auto-generated loops to invoke calc_halo() from
                             // begin_*v to end_*v by step_*v.
#include "stencil_halo_loops.hpp"
#undef calc_halo

                             // Send filled buffer to neighbor.
                             if (hi == halo_isend) {
                                 TRACE_MSG("exchange_halos: sending data to rank " <<
                                           neighbor_rank << " for grid '" << gname << "'...");
                                 const void* buf = (const void*)(sendBuf->get_storage());
                                 MPI_Isend(buf, sendBuf->get_num_bytes(), MPI_BYTE,
                                           neighbor_rank, int(gi), comm, &send_reqs[num_send_reqs++]);
                             }
                         } // not receive.
                     } ); // visit neighbors.
                
                // Mark this grid as up-to-date.
                if (hi == halo_unpack) {
                    gp->set_updated(true);
                    TRACE_MSG("exchange_halos: grid '" << gp->get_name() << "' is updated");
                }
                
            } // grids.

        } // halo sequence.

        // Wait for all send requests to complete.
        // TODO: delay this until next attempted halo exchange.
        TRACE_MSG("exchange_halos: waiting for " << num_send_reqs << " MPI send request(s)...");
        MPI_Waitall(num_send_reqs, send_reqs, MPI_STATUS_IGNORE);
        TRACE_MSG("exchange_halos: done waiting for MPI send request(s).");
        
        double end_time = getTimeInSecs();
        mpi_time += end_time - start_time;
#endif
    }

    // Mark grids that have been written to by eq-group 'eg'.
    // TODO: only mark grids that are written to in their halo-read area.
    void StencilContext::mark_grids_dirty(EqGroupBase& eg)
    {
        for (auto* gp : eg.outputGridPtrs) {
            gp->set_updated(false);
            TRACE_MSG("grid '" << gp->get_name() << "' is modified");
        }
    }
    
    // Apply a function to each neighbor rank.
    // Called visitor function will contain the rank index of the neighbor.
    // The send and receive buffer pointers may be null if 'null_ok' is true.
    void MPIBufs::visitNeighbors(StencilContext& context,
                                 bool null_ok,
                                 std::function<void (idx_t nw, idx_t nx, idx_t ny, idx_t nz,
                                                     int rank,
                                                     Grid_WXYZ* sendBuf,
                                                     Grid_WXYZ* rcvBuf)> visitor)
    {
        for (idx_t nw = 0; nw < num_neighbors; nw++)
            for (idx_t nx = 0; nx < num_neighbors; nx++)
                for (idx_t ny = 0; ny < num_neighbors; ny++)
                    for (idx_t nz = 0; nz < num_neighbors; nz++)
                        if (context.my_neighbors[nw][nx][ny][nz] != MPI_PROC_NULL) {
                            Grid_WXYZ* sendBuf = bufs[bufSend][nw][nx][ny][nz];
                            Grid_WXYZ* rcvBuf = bufs[bufRec][nw][nx][ny][nz];
                            if (null_ok || (sendBuf && rcvBuf)) {
                                visitor(nw, nx, ny, nz,
                                        context.my_neighbors[nw][nx][ny][nz],
                                        sendBuf, rcvBuf);
                            }
                        }
    }

    // Create new buffer in given direction and size.
    // Does not yet allocate space in it.
    Grid_WXYZ* MPIBufs::makeBuf(int bd,
                                idx_t nw, idx_t nx, idx_t ny, idx_t nz,
                                idx_t dw, idx_t dx, idx_t dy, idx_t dz,
                                const std::string& name)
    {
        TRACE_MSG0(cout, "making MPI buffer '" << name << "' at " <<
                   nw << ", " << nx << ", " << ny << ", " << nz << " with size " <<
                   dw << " * " << dx << " * " << dy << " * " << dz);
        auto** gp = getBuf(bd, nw, nx, ny, nz);
        *gp = new Grid_WXYZ(name);
        assert(*gp);
        (*gp)->set_dw(dw);
        (*gp)->set_dx(dx);
        (*gp)->set_dy(dy);
        (*gp)->set_dz(dz);
        TRACE_MSG0(cout, "MPI buffer '" << name << "' size: " <<
                   (*gp)->get_num_bytes());
        return *gp;
    }

    // TODO: get rid of all these macros after making a more general
    // mechanism for handling dimensions.
#define ADD_1_OPTION(name, help1, help2, var, dim)                      \
    parser.add_option(new CommandLineParser::IdxOption                  \
                      (name #dim,                                       \
                       help1 " in '" #dim "' dimension" help2 ".",      \
                       var ## dim))
#define ADD_XYZ_OPTION(name, help, var) \
    ADD_1_OPTION(name, help, "", var, x); \
    ADD_1_OPTION(name, help, "", var, y);                               \
    ADD_1_OPTION(name, help, "", var, z);                               \
    parser.add_option(new CommandLineParser::MultiIdxOption             \
                      (name,                                            \
                       "Shorthand for -" name "x <integer> -" name      \
                       "y <integer> -" name "z <integer>.",             \
                       var ## x, var ## y, var ## z))
#define ADD_TXYZ_OPTION(name, help, var)                         \
    ADD_XYZ_OPTION(name, help, var);                             \
    ADD_1_OPTION(name, help, " (number of time steps)", var, t)
#define ADD_WXYZ_OPTION(name, help, var)                         \
    ADD_XYZ_OPTION(name, help, var);                             \
    ADD_1_OPTION(name, help, "", var, w)
#define ADD_TWXYZ_OPTION(name, help, var) \
    ADD_TXYZ_OPTION(name, help, var); \
    ADD_1_OPTION(name, help, "", var, w)

#ifdef USING_DIM_W
#define ADD_T_DIM_OPTION(name, help, var) \
    ADD_TWXYZ_OPTION(name, help, var)
#define ADD_DIM_OPTION(name, help, var) \
    ADD_WXYZ_OPTION(name, help, var)
#else
#define ADD_T_DIM_OPTION(name, help, var) \
    ADD_TXYZ_OPTION(name, help, var)
#define ADD_DIM_OPTION(name, help, var) \
    ADD_XYZ_OPTION(name, help, var)
#endif
    
    // Add these settigns to a cmd-line parser.
    void StencilSettings::add_options(CommandLineParser& parser)
    {
        ADD_T_DIM_OPTION("d", "Rank-domain size", d);
        ADD_T_DIM_OPTION("r", "Region size", r);
        ADD_DIM_OPTION("b", "Block size", b);
        ADD_DIM_OPTION("bg", "Block-group size", bg);
        ADD_DIM_OPTION("sb", "Sub-block size", sb);
        ADD_DIM_OPTION("sbg", "Sub-block-group size", sbg);
        ADD_DIM_OPTION("p", "Extra memory-padding size", p);
#ifdef USE_MPI
        ADD_DIM_OPTION("nr", "Num ranks", nr);
        ADD_DIM_OPTION("ri", "This rank's logical index", ri);
        parser.add_option(new CommandLineParser::IntOption
                          ("msg_rank",
                           "Rank that will print informational messages.",
                           msg_rank));
#endif
        parser.add_option(new CommandLineParser::IntOption
                          ("max_threads",
                           "Max OpenMP threads to use.",
                           max_threads));
        parser.add_option(new CommandLineParser::IntOption
                          ("thread_divisor",
                           "Divide max OpenMP threads by <integer>.",
                           thread_divisor));
        parser.add_option(new CommandLineParser::IntOption
                          ("block_threads",
                           "Number of threads to use within each block.",
                           num_block_threads));
    }
    
    // Print usage message.
    void StencilSettings::print_usage(ostream& os,
                                      CommandLineParser& parser,
                                      const string& pgmName,
                                      const string& appNotes,
                                      const vector<string>& appExamples) const
    {
        os << "Usage: " << pgmName << " [options]\n"
            "Options:\n";
        parser.print_help(os);
        os << "Terms for the various levels of tiling:\n"
            " A 'point' is a single floating-point (FP) element.\n"
            "  This binary uses " << REAL_BYTES << "-byte FP elements.\n"
            " A 'vector' is composed of points.\n"
            "  A 'folded vector' contains points in more than one dimension.\n"
            "  The size of a vector is typically that of a SIMD register.\n"
            " A 'vector-cluster' is composed of vectors.\n"
            "  This is the unit of work done in each inner-most loop iteration.\n"
            " A 'sub-block' is composed of vector-clusters.\n"
            "  This is the unit of work for one OpenMP thread.\n"
            " A 'block' is composed of sub-blocks.\n"
            "  This is the unit of work for one OpenMP thread team.\n"
            " A 'region' is composed of blocks.\n"
            "  Regions are used to implement temporal wave-front tiling.\n"
            " A 'rank-domain' is composed of regions.\n"
            "  This is the unit of work for one MPI rank.\n"
            " The 'overall-problem' is composed of rank-domains.\n"
            "  This is the unit of work for all MPI ranks.\n"
#ifndef USE_MPI
            "  This binary has NOT been compiled with MPI support.\n"
#endif
            "Guidelines for setting tiling sizes:\n"
            " The vector and vector-cluster sizes are set at compile-time, so\n"
            "  there are no run-time options to set them.\n"
            " Set sub-block sizes to specify a unit of work done by each thread.\n"
            "  A sub-block size of 0 in dimensions 'w' or 'x' =>\n"
            "   sub-block size is set to vector-cluster size in that dimension.\n"
            "  A sub-block size of 0 in dimensions 'y' or 'z' =>\n"
            "   sub-block size is set to block size in that dimension.\n"
            "  Thus, the default sub-block is a 'y-z' slab.\n"
            "  Temporal tiling in sub-blocks is not yet supported, so effectively, sbt = 1.\n"
            " Set sub-block-group sizes to control the ordering of sub-blocks within a block.\n"
            "  All sub-blocks that intersect a given sub-block-group are evaluated\n"
            "   before sub-blocks in the next sub-block-group.\n"
            "  A sub-block-group size of 0 in a given dimension =>\n"
            "   sub-block-group size is set to sub-block size in that dimension.\n"
            " Set block sizes to specify a unit of work done by each thread team.\n"
            "  A block size of 0 in a given dimension =>\n"
            "   block size is set to region size in that dimension.\n"
            "  Temporal tiling in blocks is not yet supported, so effectively, bt = 1.\n"
            " Set block-group sizes to control the ordering of blocks within a region.\n"
            "  All blocks that intersect a given block-group are evaluated before blocks\n"
            "   in the next block-group.\n"
            "  A block-group size of 0 in a given dimension =>\n"
            "   block-group size is set to block size in that dimension.\n"
            " Set region sizes to control temporal wave-front tile sizes.\n"
            "  The temopral region size should be larger than one, and\n"
            "   the spatial region sizes should be less than the rank-domain sizes\n"
            "   in at least one dimension to enable temporal wave-front tiling.\n"
            "  The spatial region sizes should be greater than corresponding block sizes\n"
            "   to enable threading withing each wave-front tile.\n"
            "  Control the time-steps in each temporal wave-front with -rt.\n"
            "   Special cases:\n"
            "    Using '-rt 1' disables wave-front tiling.\n"
            "    Using '-rt 0' => all time-steps done in one wave-front.\n"
            "  A region size of 0 in a given dimension =>\n"
            "   region size is set to rank-domain size in that dimension.\n"
            " Set rank-domain sizes to specify the work done on this rank.\n"
            "  This and the number of grids affect the amount of memory used.\n"
            "Controlling OpenMP threading:\n"
            " Using '-max_threads 0' =>\n"
            "  max_threads is set to OpenMP's default number of threads.\n"
            " The -thread_divisor option is a convenience to control the number of\n"
            "  hyper-threads used without having to know the number of cores,\n"
            "  e.g., using '-thread_divisor 2' will halve the number of OpenMP threads.\n"
            " For stencil evaluation, threads are allocated using nested OpenMP:\n"
            "  Num threads per region = max_threads / thread_divisor / block_threads.\n"
            "  Num threads per block = block_threads.\n"
            "  Num threads per sub-block = 1.\n"
            "  Num threads used for halo exchange is same as num per region.\n" <<
#ifdef USE_MPI
            "Controlling MPI scaling:\n"
            "  To 'weak-scale' to a larger overall-problem size, use multiple MPI ranks\n"
            "   and keep the rank-domain sizes constant.\n"
            "  To 'strong-scale' a given overall-problem size, use multiple MPI ranks\n"
            "   and reduce the size of each rank-domain appropriately.\n" <<
#endif
            appNotes <<
            "Examples:\n" <<
            " " << pgmName << " -d 768 -dt 25\n" <<
            " " << pgmName << " -dx 512 -dy 256 -dz 128\n" <<
            " " << pgmName << " -d 2048 -dt 20 -r 512 -rt 10  # temporal tiling.\n" <<
            " " << pgmName << " -d 512 -nrx 2 -nry 1 -nrz 2   # multi-rank.\n";
        for (auto ae : appExamples)
            os << " " << pgmName << " " << ae << endl;
        os << flush;
    }
    
        // Make sure all user-provided settings are valid and finish setting up some
        // other vars before allocating memory.
        // Called from allocAll(), so it doesn't normally need to be called from user code.
        void StencilSettings::finalizeSettings(std::ostream& os) {

            // Round up domain size as needed.
            dt = roundUp(os, dt, CPTS_T, "rank domain size in t (time steps)");
            dw = roundUp(os, dw, CPTS_W, "rank domain size in w");
            dx = roundUp(os, dx, CPTS_X, "rank domain size in x");
            dy = roundUp(os, dy, CPTS_Y, "rank domain size in y");
            dz = roundUp(os, dz, CPTS_Z, "rank domain size in z");

            // Determine num regions.
            // Also fix up region sizes as needed.
            // Default region size (if 0) will be size of rank-domain.
            os << "\nRegions:" << endl;
            idx_t nrt = findNumRegionsInDomain(os, rt, dt, CPTS_T, "t");
            idx_t nrw = findNumRegionsInDomain(os, rw, dw, CPTS_W, "w");
            idx_t nrx = findNumRegionsInDomain(os, rx, dx, CPTS_X, "x");
            idx_t nry = findNumRegionsInDomain(os, ry, dy, CPTS_Y, "y");
            idx_t nrz = findNumRegionsInDomain(os, rz, dz, CPTS_Z, "z");
            idx_t nr = nrt * nrw * nrx * nry * nrz;
            os << " num-regions-per-rank-domain: " << nr << endl;
            os << " Since the temporal region size is " << rt <<
                ", temporal wave-front tiling is ";
            if (rt <= 1) os << "NOT ";
            os << "enabled.\n";

            // Determine num blocks.
            // Also fix up block sizes as needed.
            // Default block size (if 0) will be size of region.
            os << "\nBlocks:" << endl;
            idx_t nbt = findNumBlocksInRegion(os, bt, rt, CPTS_T, "t");
            idx_t nbw = findNumBlocksInRegion(os, bw, rw, CPTS_W, "w");
            idx_t nbx = findNumBlocksInRegion(os, bx, rx, CPTS_X, "x");
            idx_t nby = findNumBlocksInRegion(os, by, ry, CPTS_Y, "y");
            idx_t nbz = findNumBlocksInRegion(os, bz, rz, CPTS_Z, "z");
            idx_t nb = nbt * nbw * nbx * nby * nbz;
            os << " num-blocks-per-region: " << nb << endl;
            os << " num-blocks-per-rank-domain: " << (nb * nr) << endl;

            // Adjust defaults for sub-blocks to be y-z slab.
            // Otherwise, findNumSubBlocksInBlock() would set default
            // to entire block.
            if (!sbw) sbw = CPTS_W; // min size in 'w'.
            if (!sbx) sbx = CPTS_X; // min size in 'x'.
            if (!sby) sby = by;     // max size in 'y'.
            if (!sbz) sbz = bz;     // max size in 'z'.

            // Determine num sub-blocks.
            // Also fix up sub-block sizes as needed.
            os << "\nSub-blocks:" << endl;
            idx_t nsbt = findNumSubBlocksInBlock(os, sbt, bt, CPTS_T, "t");
            idx_t nsbw = findNumSubBlocksInBlock(os, sbw, bw, CPTS_W, "w");
            idx_t nsbx = findNumSubBlocksInBlock(os, sbx, bx, CPTS_X, "x");
            idx_t nsby = findNumSubBlocksInBlock(os, sby, by, CPTS_Y, "y");
            idx_t nsbz = findNumSubBlocksInBlock(os, sbz, bz, CPTS_Z, "z");
            idx_t nsb = nsbt * nsbw * nsbx * nsby * nsbz;
            os << " num-sub-blocks-per-block: " << nsb << endl;

            // Now, we adjust groups. These are done after all the above sizes
            // because group sizes are more like 'guidelines' and don't have
            // their own loops.
            os << "\nGroups:" << endl;
        
            // Adjust defaults for block-groups to be size of block.
            if (!bgw) bgw = bw;
            if (!bgx) bgx = bx;
            if (!bgy) bgy = by;
            if (!bgz) bgz = bz;

            // Determine num block-groups.
            // Also fix up block-group sizes as needed.
            // TODO: only print this if block-grouping is enabled.
            idx_t nbgw = findNumBlockGroupsInRegion(os, bgw, rw, bw, "w");
            idx_t nbgx = findNumBlockGroupsInRegion(os, bgx, rx, bx, "x");
            idx_t nbgy = findNumBlockGroupsInRegion(os, bgy, ry, by, "y");
            idx_t nbgz = findNumBlockGroupsInRegion(os, bgz, rz, bz, "z");
            idx_t nbg = nbt * nbgw * nbgx * nbgy * nbgz;
            os << " num-block-groups-per-region: " << nbg << endl;
            nbw = findNumBlocksInBlockGroup(os, bw, bgw, CPTS_W, "w");
            nbx = findNumBlocksInBlockGroup(os, bx, bgx, CPTS_X, "x");
            nby = findNumBlocksInBlockGroup(os, by, bgy, CPTS_Y, "y");
            nbz = findNumBlocksInBlockGroup(os, bz, bgz, CPTS_Z, "z");
            nb = nbw * nbx * nby * nbz;
            os << " num-blocks-per-block-group: " << nb << endl;

            // Adjust defaults for sub-block-groups to be size of sub-block.
            if (!sbgw) sbgw = sbw;
            if (!sbgx) sbgx = sbx;
            if (!sbgy) sbgy = sby;
            if (!sbgz) sbgz = sbz;

            // Determine num sub-block-groups.
            // Also fix up sub-block-group sizes as needed.
            // TODO: only print this if sub-block-grouping is enabled.
            idx_t nsbgw = findNumSubBlockGroupsInBlock(os, sbgw, bw, sbw, "w");
            idx_t nsbgx = findNumSubBlockGroupsInBlock(os, sbgx, bx, sbx, "x");
            idx_t nsbgy = findNumSubBlockGroupsInBlock(os, sbgy, by, sby, "y");
            idx_t nsbgz = findNumSubBlockGroupsInBlock(os, sbgz, bz, sbz, "z");
            idx_t nsbg = nsbgw * nsbgx * nsbgy * nsbgz;
            os << " num-sub-block-groups-per-block: " << nsbg << endl;
            nsbw = findNumSubBlocksInSubBlockGroup(os, sbw, sbgw, CPTS_W, "w");
            nsbx = findNumSubBlocksInSubBlockGroup(os, sbx, sbgx, CPTS_X, "x");
            nsby = findNumSubBlocksInSubBlockGroup(os, sby, sbgy, CPTS_Y, "y");
            nsbz = findNumSubBlocksInSubBlockGroup(os, sbz, sbgz, CPTS_Z, "z");
            nsb = nsbw * nsbx * nsby * nsbz;
            os << " num-sub-blocks-per-sub-block-group: " << nsb << endl;
        }

} // namespace yask.
