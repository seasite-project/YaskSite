/*
 * 4-D loop code.
 * Generated automatically from the following pseudo-code:
 *
 * omp grouped loop(rw,rx,ry,rz) {  calc(block(rt)); }
 *
 */

 // ** Begin loop in rw, rx, ry, rz dimension(s). **

 // Number of iterations to get from begin_rw to (but not including) end_rw, stepping by step_rw. This value is rounded up because the last iteration may cover fewer than step_rw steps.
 const idx_t num_iters_rw = ((end_rw - begin_rw) + (step_rw - 1)) / step_rw;

 // Number of iterations in one full group in rw dimension. This value is rounded up, effectively increasing the group size if needed to a multiple of step_rw.
 const idx_t num_iters_in_full_group_rw = std::min((group_size_rw + (step_rw - 1)) / step_rw, num_iters_rw);

 // Number of *full* groups in rw dimension.
 const idx_t num_full_groups_rw = num_iters_rw / num_iters_in_full_group_rw;

 // Number of iterations to get from begin_rx to (but not including) end_rx, stepping by step_rx. This value is rounded up because the last iteration may cover fewer than step_rx steps.
 const idx_t num_iters_rx = ((end_rx - begin_rx) + (step_rx - 1)) / step_rx;

 // Number of iterations in one full group in rx dimension. This value is rounded up, effectively increasing the group size if needed to a multiple of step_rx.
 const idx_t num_iters_in_full_group_rx = std::min((group_size_rx + (step_rx - 1)) / step_rx, num_iters_rx);

 // Number of *full* groups in rx dimension.
 const idx_t num_full_groups_rx = num_iters_rx / num_iters_in_full_group_rx;

 // Number of iterations to get from begin_ry to (but not including) end_ry, stepping by step_ry. This value is rounded up because the last iteration may cover fewer than step_ry steps.
 const idx_t num_iters_ry = ((end_ry - begin_ry) + (step_ry - 1)) / step_ry;

 // Number of iterations in one full group in ry dimension. This value is rounded up, effectively increasing the group size if needed to a multiple of step_ry.
 const idx_t num_iters_in_full_group_ry = std::min((group_size_ry + (step_ry - 1)) / step_ry, num_iters_ry);

 // Number of *full* groups in ry dimension.
 const idx_t num_full_groups_ry = num_iters_ry / num_iters_in_full_group_ry;

 // Number of iterations to get from begin_rz to (but not including) end_rz, stepping by step_rz. This value is rounded up because the last iteration may cover fewer than step_rz steps.
 const idx_t num_iters_rz = ((end_rz - begin_rz) + (step_rz - 1)) / step_rz;

 // Number of iterations in one full group in rz dimension. This value is rounded up, effectively increasing the group size if needed to a multiple of step_rz.
 const idx_t num_iters_in_full_group_rz = std::min((group_size_rz + (step_rz - 1)) / step_rz, num_iters_rz);

 // Number of *full* groups in rz dimension.
 const idx_t num_full_groups_rz = num_iters_rz / num_iters_in_full_group_rz;

 // Number of iterations in rw, rx, ry, rz dimensions
 const idx_t num_iters_rw_rx_ry_rz = num_iters_rw * num_iters_rx * num_iters_ry * num_iters_rz;

 // Number of iterations in rx, ry, rz dimensions
 const idx_t num_iters_rx_ry_rz = num_iters_rx * num_iters_ry * num_iters_rz;

 // Number of iterations in ry, rz dimensions
 const idx_t num_iters_ry_rz = num_iters_ry * num_iters_rz;

 // Computation loop.
_Pragma("nounroll_and_jam") _Pragma("nofusion")

 // Distribute iterations among OpenMP threads.
_Pragma("omp parallel for schedule(static,1) proc_bind(close)")
 for (idx_t loop_index_rw_rx_ry_rz = 0; loop_index_rw_rx_ry_rz < num_iters_rw_rx_ry_rz; loop_index_rw_rx_ry_rz++) {

 // Working vars for iterations in groups. These are initialized to full-group counts and then reduced if we are in a partial group.
 idx_t num_iters_in_group_rw = num_iters_in_full_group_rw;
 idx_t num_iters_in_group_rx = num_iters_in_full_group_rx;
 idx_t num_iters_in_group_ry = num_iters_in_full_group_ry;
 idx_t num_iters_in_group_rz = num_iters_in_full_group_rz;

 // Number of iterations in the set of groups across rx, ry, rz dimensions.
 idx_t num_iters_in_group_set_rx_ry_rz = num_iters_in_group_rw * num_iters_rx * num_iters_ry * num_iters_rz;

 // Index of this group in rw dimension.
 idx_t index_of_group_rw = loop_index_rw_rx_ry_rz / num_iters_in_group_set_rx_ry_rz;

 // Linear offset within the set of groups across rx, ry, rz dimensions.
 idx_t index_offset_within_group_set_rx_ry_rz = loop_index_rw_rx_ry_rz % num_iters_in_group_set_rx_ry_rz;

 // Adjust number of iterations in this group in rw dimension.
 if (index_of_group_rw >= num_full_groups_rw)  num_iters_in_group_rw = num_iters_rw - (num_full_groups_rw * num_iters_in_full_group_rw);

 // Number of iterations in the set of groups across ry, rz dimensions.
 idx_t num_iters_in_group_set_ry_rz = num_iters_in_group_rw * num_iters_in_group_rx * num_iters_ry * num_iters_rz;

 // Index of this group in rx dimension.
 idx_t index_of_group_rx = index_offset_within_group_set_rx_ry_rz / num_iters_in_group_set_ry_rz;

 // Linear offset within the set of groups across ry, rz dimensions.
 idx_t index_offset_within_group_set_ry_rz = index_offset_within_group_set_rx_ry_rz % num_iters_in_group_set_ry_rz;

 // Adjust number of iterations in this group in rx dimension.
 if (index_of_group_rx >= num_full_groups_rx)  num_iters_in_group_rx = num_iters_rx - (num_full_groups_rx * num_iters_in_full_group_rx);

 // Number of iterations in the set of groups across rz dimension.
 idx_t num_iters_in_group_set_rz = num_iters_in_group_rw * num_iters_in_group_rx * num_iters_in_group_ry * num_iters_rz;

 // Index of this group in ry dimension.
 idx_t index_of_group_ry = index_offset_within_group_set_ry_rz / num_iters_in_group_set_rz;

 // Linear offset within the set of groups across rz dimension.
 idx_t index_offset_within_group_set_rz = index_offset_within_group_set_ry_rz % num_iters_in_group_set_rz;

 // Adjust number of iterations in this group in ry dimension.
 if (index_of_group_ry >= num_full_groups_ry)  num_iters_in_group_ry = num_iters_ry - (num_full_groups_ry * num_iters_in_full_group_ry);

 // Number of iterations in this group.
 idx_t num_iters_in_full_group = num_iters_in_group_rw * num_iters_in_group_rx * num_iters_in_group_ry * num_iters_in_group_rz;

 // Index of this group in rz dimension.
 idx_t index_of_group_rz = index_offset_within_group_set_rz / num_iters_in_full_group;

 // Linear offset within this group.
 idx_t index_offset_within_this_group = index_offset_within_group_set_rz % num_iters_in_full_group;

 // Adjust number of iterations in this group in rz dimension.
 if (index_of_group_rz >= num_full_groups_rz)  num_iters_in_group_rz = num_iters_rz - (num_full_groups_rz * num_iters_in_full_group_rz);

 // Offset within this group in rw dimension.
 idx_t index_offset_within_this_group_rw = index_offset_within_this_group / (num_iters_in_group_rx * num_iters_in_group_ry * num_iters_in_group_rz);

 // Zero-based, unit-stride index for rw.
 idx_t index_rw = num_iters_in_full_group_rw * index_of_group_rw + index_offset_within_this_group_rw;

 // Offset within this group in rx dimension.
 idx_t index_offset_within_this_group_rx = (index_offset_within_this_group / (num_iters_in_group_ry * num_iters_in_group_rz)) % num_iters_in_group_rx;

 // Zero-based, unit-stride index for rx.
 idx_t index_rx = num_iters_in_full_group_rx * index_of_group_rx + index_offset_within_this_group_rx;

 // Offset within this group in ry dimension.
 idx_t index_offset_within_this_group_ry = (index_offset_within_this_group / (num_iters_in_group_rz)) % num_iters_in_group_ry;

 // Zero-based, unit-stride index for ry.
 idx_t index_ry = num_iters_in_full_group_ry * index_of_group_ry + index_offset_within_this_group_ry;

 // Offset within this group in rz dimension.
 idx_t index_offset_within_this_group_rz = (index_offset_within_this_group) % num_iters_in_group_rz;

 // Zero-based, unit-stride index for rz.
 idx_t index_rz = num_iters_in_full_group_rz * index_of_group_rz + index_offset_within_this_group_rz;

 // This value of index_rw covers rw from start_rw to stop_rw-1.
 const idx_t start_rw = begin_rw + (index_rw * step_rw);
 const idx_t stop_rw = std::min(start_rw + step_rw, end_rw);

 // This value of index_rx covers rx from start_rx to stop_rx-1.
 const idx_t start_rx = begin_rx + (index_rx * step_rx);
 const idx_t stop_rx = std::min(start_rx + step_rx, end_rx);

 // This value of index_ry covers ry from start_ry to stop_ry-1.
 const idx_t start_ry = begin_ry + (index_ry * step_ry);
 const idx_t stop_ry = std::min(start_ry + step_ry, end_ry);

 // This value of index_rz covers rz from start_rz to stop_rz-1.
 const idx_t start_rz = begin_rz + (index_rz * step_rz);
 const idx_t stop_rz = std::min(start_rz + step_rz, end_rz);

 // Computation.
  eg->calc_block(rt, start_rw, start_rx, start_ry, start_rz, stop_rw, stop_rx, stop_ry, stop_rz);
 }
// End of generated code.
