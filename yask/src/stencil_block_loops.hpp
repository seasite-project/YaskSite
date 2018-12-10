/*
 * 4-D loop code.
 * Generated automatically from the following pseudo-code:
 *
 * omp grouped loop(bw,bx,by,bz) {  calc(sub_block(bt)); }
 *
 */

 // ** Begin loop in bw, bx, by, bz dimension(s). **

 // Number of iterations to get from begin_bw to (but not including) end_bw, stepping by step_bw. This value is rounded up because the last iteration may cover fewer than step_bw steps.
 const idx_t num_iters_bw = ((end_bw - begin_bw) + (step_bw - 1)) / step_bw;

 // Number of iterations in one full group in bw dimension. This value is rounded up, effectively increasing the group size if needed to a multiple of step_bw.
 const idx_t num_iters_in_full_group_bw = std::min((group_size_bw + (step_bw - 1)) / step_bw, num_iters_bw);

 // Number of *full* groups in bw dimension.
 const idx_t num_full_groups_bw = num_iters_bw / num_iters_in_full_group_bw;

 // Number of iterations to get from begin_bx to (but not including) end_bx, stepping by step_bx. This value is rounded up because the last iteration may cover fewer than step_bx steps.
 const idx_t num_iters_bx = ((end_bx - begin_bx) + (step_bx - 1)) / step_bx;

 // Number of iterations in one full group in bx dimension. This value is rounded up, effectively increasing the group size if needed to a multiple of step_bx.
 const idx_t num_iters_in_full_group_bx = std::min((group_size_bx + (step_bx - 1)) / step_bx, num_iters_bx);

 // Number of *full* groups in bx dimension.
 const idx_t num_full_groups_bx = num_iters_bx / num_iters_in_full_group_bx;

 // Number of iterations to get from begin_by to (but not including) end_by, stepping by step_by. This value is rounded up because the last iteration may cover fewer than step_by steps.
 const idx_t num_iters_by = ((end_by - begin_by) + (step_by - 1)) / step_by;

 // Number of iterations in one full group in by dimension. This value is rounded up, effectively increasing the group size if needed to a multiple of step_by.
 const idx_t num_iters_in_full_group_by = std::min((group_size_by + (step_by - 1)) / step_by, num_iters_by);

 // Number of *full* groups in by dimension.
 const idx_t num_full_groups_by = num_iters_by / num_iters_in_full_group_by;

 // Number of iterations to get from begin_bz to (but not including) end_bz, stepping by step_bz. This value is rounded up because the last iteration may cover fewer than step_bz steps.
 const idx_t num_iters_bz = ((end_bz - begin_bz) + (step_bz - 1)) / step_bz;

 // Number of iterations in one full group in bz dimension. This value is rounded up, effectively increasing the group size if needed to a multiple of step_bz.
 const idx_t num_iters_in_full_group_bz = std::min((group_size_bz + (step_bz - 1)) / step_bz, num_iters_bz);

 // Number of *full* groups in bz dimension.
 const idx_t num_full_groups_bz = num_iters_bz / num_iters_in_full_group_bz;

 // Number of iterations in bw, bx, by, bz dimensions
 const idx_t num_iters_bw_bx_by_bz = num_iters_bw * num_iters_bx * num_iters_by * num_iters_bz;

 // Number of iterations in bx, by, bz dimensions
 const idx_t num_iters_bx_by_bz = num_iters_bx * num_iters_by * num_iters_bz;

 // Number of iterations in by, bz dimensions
 const idx_t num_iters_by_bz = num_iters_by * num_iters_bz;

 // Computation loop.
_Pragma("nounroll_and_jam") _Pragma("nofusion")

 // Distribute iterations among OpenMP threads.
_Pragma("omp parallel for schedule(static,1) proc_bind(close)")
 for (idx_t loop_index_bw_bx_by_bz = 0; loop_index_bw_bx_by_bz < num_iters_bw_bx_by_bz; loop_index_bw_bx_by_bz++) {

 // Working vars for iterations in groups. These are initialized to full-group counts and then reduced if we are in a partial group.
 idx_t num_iters_in_group_bw = num_iters_in_full_group_bw;
 idx_t num_iters_in_group_bx = num_iters_in_full_group_bx;
 idx_t num_iters_in_group_by = num_iters_in_full_group_by;
 idx_t num_iters_in_group_bz = num_iters_in_full_group_bz;

 // Number of iterations in the set of groups across bx, by, bz dimensions.
 idx_t num_iters_in_group_set_bx_by_bz = num_iters_in_group_bw * num_iters_bx * num_iters_by * num_iters_bz;

 // Index of this group in bw dimension.
 idx_t index_of_group_bw = loop_index_bw_bx_by_bz / num_iters_in_group_set_bx_by_bz;

 // Linear offset within the set of groups across bx, by, bz dimensions.
 idx_t index_offset_within_group_set_bx_by_bz = loop_index_bw_bx_by_bz % num_iters_in_group_set_bx_by_bz;

 // Adjust number of iterations in this group in bw dimension.
 if (index_of_group_bw >= num_full_groups_bw)  num_iters_in_group_bw = num_iters_bw - (num_full_groups_bw * num_iters_in_full_group_bw);

 // Number of iterations in the set of groups across by, bz dimensions.
 idx_t num_iters_in_group_set_by_bz = num_iters_in_group_bw * num_iters_in_group_bx * num_iters_by * num_iters_bz;

 // Index of this group in bx dimension.
 idx_t index_of_group_bx = index_offset_within_group_set_bx_by_bz / num_iters_in_group_set_by_bz;

 // Linear offset within the set of groups across by, bz dimensions.
 idx_t index_offset_within_group_set_by_bz = index_offset_within_group_set_bx_by_bz % num_iters_in_group_set_by_bz;

 // Adjust number of iterations in this group in bx dimension.
 if (index_of_group_bx >= num_full_groups_bx)  num_iters_in_group_bx = num_iters_bx - (num_full_groups_bx * num_iters_in_full_group_bx);

 // Number of iterations in the set of groups across bz dimension.
 idx_t num_iters_in_group_set_bz = num_iters_in_group_bw * num_iters_in_group_bx * num_iters_in_group_by * num_iters_bz;

 // Index of this group in by dimension.
 idx_t index_of_group_by = index_offset_within_group_set_by_bz / num_iters_in_group_set_bz;

 // Linear offset within the set of groups across bz dimension.
 idx_t index_offset_within_group_set_bz = index_offset_within_group_set_by_bz % num_iters_in_group_set_bz;

 // Adjust number of iterations in this group in by dimension.
 if (index_of_group_by >= num_full_groups_by)  num_iters_in_group_by = num_iters_by - (num_full_groups_by * num_iters_in_full_group_by);

 // Number of iterations in this group.
 idx_t num_iters_in_full_group = num_iters_in_group_bw * num_iters_in_group_bx * num_iters_in_group_by * num_iters_in_group_bz;

 // Index of this group in bz dimension.
 idx_t index_of_group_bz = index_offset_within_group_set_bz / num_iters_in_full_group;

 // Linear offset within this group.
 idx_t index_offset_within_this_group = index_offset_within_group_set_bz % num_iters_in_full_group;

 // Adjust number of iterations in this group in bz dimension.
 if (index_of_group_bz >= num_full_groups_bz)  num_iters_in_group_bz = num_iters_bz - (num_full_groups_bz * num_iters_in_full_group_bz);

 // Offset within this group in bw dimension.
 idx_t index_offset_within_this_group_bw = index_offset_within_this_group / (num_iters_in_group_bx * num_iters_in_group_by * num_iters_in_group_bz);

 // Zero-based, unit-stride index for bw.
 idx_t index_bw = num_iters_in_full_group_bw * index_of_group_bw + index_offset_within_this_group_bw;

 // Offset within this group in bx dimension.
 idx_t index_offset_within_this_group_bx = (index_offset_within_this_group / (num_iters_in_group_by * num_iters_in_group_bz)) % num_iters_in_group_bx;

 // Zero-based, unit-stride index for bx.
 idx_t index_bx = num_iters_in_full_group_bx * index_of_group_bx + index_offset_within_this_group_bx;

 // Offset within this group in by dimension.
 idx_t index_offset_within_this_group_by = (index_offset_within_this_group / (num_iters_in_group_bz)) % num_iters_in_group_by;

 // Zero-based, unit-stride index for by.
 idx_t index_by = num_iters_in_full_group_by * index_of_group_by + index_offset_within_this_group_by;

 // Offset within this group in bz dimension.
 idx_t index_offset_within_this_group_bz = (index_offset_within_this_group) % num_iters_in_group_bz;

 // Zero-based, unit-stride index for bz.
 idx_t index_bz = num_iters_in_full_group_bz * index_of_group_bz + index_offset_within_this_group_bz;

 // This value of index_bw covers bw from start_bw to stop_bw-1.
 const idx_t start_bw = begin_bw + (index_bw * step_bw);
 const idx_t stop_bw = std::min(start_bw + step_bw, end_bw);

 // This value of index_bx covers bx from start_bx to stop_bx-1.
 const idx_t start_bx = begin_bx + (index_bx * step_bx);
 const idx_t stop_bx = std::min(start_bx + step_bx, end_bx);

 // This value of index_by covers by from start_by to stop_by-1.
 const idx_t start_by = begin_by + (index_by * step_by);
 const idx_t stop_by = std::min(start_by + step_by, end_by);

 // This value of index_bz covers bz from start_bz to stop_bz-1.
 const idx_t start_bz = begin_bz + (index_bz * step_bz);
 const idx_t stop_bz = std::min(start_bz + step_bz, end_bz);

 // Computation.
  calc_sub_block(bt, start_bw, start_bx, start_by, start_bz, stop_bw, stop_bx, stop_by, stop_bz);
 }
// End of generated code.
