// Base classes for stencil code.
//
// This is just a template each builds should generate
// functions of this kind; which will be linked on the fly

#include "yaskFork.h"
#include "print.h"
#include "config.h"
#include "macros.h"

#ifdef GENERATED
#include "yask_kernel_api.hpp"
#include "yask_stencil.hpp"
#ifdef yaskSite_HAVE_MPI
#include <mpi.h>
#endif

#ifdef LIKWID_PERFMON
#include <likwid.h>
#endif

using namespace std;
using namespace yask;

extern "C" {
    //TODO:split alloc and init
    int YASKinit(yaskSite* opt, bool noAlloc)
    {

        //Right now new allocation is done only if
        //dimension is changed; Need to think aboout
        //when nthreads changes and the associated
        //NUMA problems that can arise.
        bool needAlloc = opt->dimUpdated;

        // The factory from which all other kernel objects are made.
        yk_factory *kfac;
        yk_solution_ptr *soln;


        if(needAlloc)
        {
            //delete all settings and context if dimension
            //is updated
            if(opt->stencilSoln)
            {
                //delete old setting if it exists
                yk_solution_ptr* solnOld = (yk_solution_ptr*) opt->stencilSoln;
                delete solnOld;
            }
            if(opt->stencilFactory)
            {
                //delete old context if it exists
                yk_factory* factorytOld = (yk_factory*) opt->stencilFactory;
                delete factorytOld;
            }


            kfac = new yk_factory;
            auto env = make_shared<KernelEnv>();
            //set mpi
            if(!(opt->mpi_man))
            {
                ERROR_PRINT("You should have initialized MPI at the beginning");
            }
            else
            {
                env->comm = opt->mpi_man->comm;
                env->num_ranks = opt->mpi_man->nRanks;
                env->group = opt->mpi_man->group;
                env->my_rank = opt->mpi_man->myRank;

                env->shm_comm = opt->mpi_man->shm_comm;
                env->num_shm_ranks = opt->mpi_man->shm_nRanks;
                env->shm_group = opt->mpi_man->shm_group;
                env->my_shm_rank = opt->mpi_man->shm_myRank;

                env->max_threads = opt->nthreads;
                //mpi_man->maxThreads;
            }

            //define soln
            soln = new yk_solution_ptr;
            (*soln) = kfac->new_solution(env);

            ostream* osp = &cout;
#if yaskSite_VERBOSITY < 2
            yask_output_factory ofac;
            auto null_out = ofac.new_null_output();
            (*soln)->set_debug_output(null_out);
            osp = &null_out->get_ostream();
#else
            int rank_num = env->get_rank_index();
            if (rank_num != opt->mpi_man->printRank) {
                yask_output_factory ofac;
                auto null_out = ofac.new_null_output();
                (*soln)->set_debug_output(null_out);
                osp = &null_out->get_ostream();
                cout << "Suppressing output on rank " << rank_num << ".\n";
            }
#endif
        }
        else
        {
            soln = (yk_solution_ptr*) opt->stencilSoln;
            kfac = (yk_factory *) opt->stencilFactory;
        }


        //(*soln)->set_rank_domain_size("t", opt->dt);
        (*soln)->set_rank_domain_size("x", opt->dx);
        (*soln)->set_rank_domain_size("y", opt->dy);
        (*soln)->set_rank_domain_size("z", opt->dz);

        (*soln)->set_region_size("t", opt->rt);
        (*soln)->set_region_size("x", opt->rx);
        (*soln)->set_region_size("y", opt->ry);
        (*soln)->set_region_size("z", opt->rz);

        (*soln)->set_block_size("x", opt->bx);
        (*soln)->set_block_size("y", opt->by);
        (*soln)->set_block_size("z", opt->bz);

        (*soln)->set_step_wrap(true); // so I can access any step size, API will take care of wrapping

        char* cmd;
        STRINGIFY(cmd, "-block_threads %d -mbx %d -mby %d -mbz %d -sbx %d -sby %d -sbz %d", opt->threadPerBlock, opt->bx, opt->by, opt->bz, opt->sbx, opt->sby, opt->sbz);

        (*soln)->apply_command_line_options(cmd);
        (*soln)->set_default_numa_preferred(yask_numa_local);
        // Alloc memory, create lists of grids, etc. Also reflects changes in
        // settings
        (*soln)->prepare_solution();
        if(needAlloc)
        {
            auto context = dynamic_pointer_cast<StencilContext>(*soln);

            if(!noAlloc)
            {
                // Alloc memory, create lists of grids, etc.
                //(*soln)->prepare_solution();
                context->initData();
            }

            //now copy parameter values
            opt->paramName.clear();
            opt->paramList.clear();
            //reallocate opt->grids
            opt->gridName.clear();
            opt->gridIdx.clear();
            for (auto var : (*soln)->get_vars())
            {
                if(var->get_dim_names().size() == 0)
                {
                    opt->paramName.push_back(var->get_name().c_str());
                    void* param = (void*)(var->get_raw_storage_buffer());
                    opt->paramList.push_back(param);
                }
                else
                {
                    opt->gridName.push_back(var->get_name().c_str());
                    std::vector<int> currIdx(6);
                    int ctr=0;
                    std::vector<const char *> dname_list = {"x", "y", "z"};
                    for (auto it=dname_list.begin(); it != dname_list.end(); it++)
                    {
                        currIdx[2*ctr]=var->get_first_rank_domain_index(*it);
                        currIdx[2*ctr+1]=var->get_last_rank_domain_index(*it);
                        ctr++;
                    }
                    opt->gridIdx.push_back(currIdx);
                }
            }

            for (auto& sp : context->stPacks)
            {
                for (auto sb : *sp)
                {
                    std::string eqGroup_name = sb->get_name();
                    opt->eqGroups[opt->eqGroupMap[eqGroup_name]].num_points = sb->getBB().bb_num_points;
                }
            }
            if (!noAlloc && (context->tot_domain_pts < 1)) {
                ERROR_PRINT("Exiting because there are zero points to evaluate.");
                exit_yask(1);
            }
        }
        opt->stencilSoln = ((void*) soln);
        opt->stencilFactory =  ((void*) kfac);

        //context->global_barrier();
        return 0;
    }

    void YASKautoTuner(yaskSite* opt, bool enable)
    {
        yk_solution_ptr* soln = (yk_solution_ptr*) opt->stencilSoln;
        (*soln)->reset_auto_tuner(enable, true);
    }

    int YASKfinalize(yaskSite* opt)
    {
        yk_solution_ptr* soln = (yk_solution_ptr*) opt->stencilSoln;
        yk_factory* kFac = (yk_factory*) opt->stencilFactory;

        //context->global_barrier();
        (*soln)->end_solution();

        delete soln;
        delete kFac;
        return 0;
    }

    int YASKstencil( yaskSite* opt, int timeOffset)
    {
        yk_solution_ptr* soln = (yk_solution_ptr*) opt->stencilSoln;
        (*soln)->run_solution(timeOffset, timeOffset+opt->dt-1);

        //context->global_barrier();
        return 0;
    }

    double YASKgetElement(yaskSite* opt, const char* data, int t, int x, int y, int z)
    {
        yk_solution_ptr* soln = (yk_solution_ptr*) opt->stencilSoln;
        std::vector<int> rawIndices = {t,x,y,z};
        std::vector<idx_t> indices;
        for(int i=0; i<4; ++i)
        {
            //404 is reserved, so no folding above 400 :)
            if(rawIndices[i] != -404)
            {
                indices.push_back(rawIndices[i]);
            }
        }
        return (*soln)->get_var(data)->get_element(indices);
    }

    void YASKsetElement(yaskSite* opt, const char* data, int t, int x, int y, int z, double val)
    {
        yk_solution_ptr* soln = (yk_solution_ptr*) opt->stencilSoln;
        std::vector<int> rawIndices = {t,x,y,z};
        std::vector<idx_t> indices;
        for(int i=0; i<4; ++i)
        {
            //404 is reserved, so no folding above 400 :)
            if(rawIndices[i] != -404)
            {
                indices.push_back(rawIndices[i]);
            }
        }

        (*soln)->get_var(data)->set_element(val, indices);
    }

    void YASKsetAll(yaskSite* opt, const char* data, double val)
    {
        yk_solution_ptr* soln = (yk_solution_ptr*) opt->stencilSoln;
        (*soln)->get_var(data)->set_all_elements_same(val);
    }

    void YASKfuseGrid(yaskSite *to_opt, const char *to_grid_name, yaskSite *from_opt, const char *from_grid_name)
    {
        yk_solution_ptr* to_soln = (yk_solution_ptr*) to_opt->stencilSoln;
        yk_solution_ptr* from_soln = (yk_solution_ptr*) from_opt->stencilSoln;
        (*to_soln)->get_var(to_grid_name)->fuse_vars((*from_soln)->get_var(from_grid_name));
    }

    void* YASKgetGridPtr(yaskSite *opt, const char *grid_name)
    {
        yk_solution_ptr* soln = (yk_solution_ptr*) opt->stencilSoln;
        return (void*)((*soln)->get_var(grid_name)->get_raw_storage_buffer());
    }

    int YASKgetHalo(yaskSite *opt, const char *grid_name, int dim)
    {
        yk_solution_ptr* soln = (yk_solution_ptr*) opt->stencilSoln;
        char *dim_name;
        if(dim==0)
            dim_name="x";
        else if(dim==1)
            dim_name="y";
        else
            dim_name="z";
        idx_t left_halo = (*soln)->get_var(grid_name)->get_left_halo_size(dim_name);
        idx_t right_halo = (*soln)->get_var(grid_name)->get_right_halo_size(dim_name);

        if(left_halo != right_halo)
        {
            ERROR_PRINT("left halo != right_halo for dim = %s, YaskSite currently only supports homogeneous radius", dim_name);
        }

        return right_halo;
    }

}

#else

extern "C"
{
    int YASKinit(yaskSite* opt, bool noAlloc)
    {
        UNUSED(opt);
        UNUSED(noAlloc);
        return -1;
    }

    int YASKfinalize(yaskSite* opt)
    {
        UNUSED(opt);
        return -1;
    }

    int YASKstencil(yaskSite* opt, int timeOffset)
    {
        UNUSED(timeOffset);
        UNUSED(opt);
        return -1;
    }

    void YASKautoTuner(yaskSite* opt, bool enable)
    {
        UNUSED(opt);
        UNUSED(enable);
    }

    double YASKgetElement(yaskSite*opt, const char* data, int t, int x, int y, int z)
    {
        UNUSED(opt);
        UNUSED(data);
        UNUSED(t);
        UNUSED(x);
        UNUSED(y);
        UNUSED(z);
        return -1;
    }

    void YASKsetElement(yaskSite*opt, const char* data, int t, int x, int y, int z, double val)
    {
        UNUSED(opt);
        UNUSED(data);
        UNUSED(t);
        UNUSED(x);
        UNUSED(y);
        UNUSED(z);
        UNUSED(val);
    }

    void YASKsetAll(yaskSite* opt, const char* data, double val)
    {
        UNUSED(opt);
        UNUSED(data);
        UNUSED(val);
    }

    void YASKfuseGrid(yaskSite *to_opt, const char *to_grid_name, yaskSite *from_opt, const char *from_grid_name)
    {
        UNUSED(to_opt);
        UNUSED(to_grid_name);
        UNUSED(from_opt);
        UNUSED(from_grid_name);
    }

    void* YASKgetGridPtr(yaskSite *opt, const char *grid)
    {
        UNUSED(opt);
        UNUSED(grid);
        return NULL;
    }

    int YASKgetHalo(yaskSite *opt, const char *grid_name, int dim)
    {
        UNUSED(opt);
        UNUSED(grid_name);
        UNUSED(dim);
        return -1;
    }

}
#endif
