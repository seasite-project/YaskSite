/*
 * 4-D loop code.
 * Generated automatically from the following pseudo-code:
 *
 *  loop(dw,dx,dy,dz) {  calc(region(start_dt, stop_dt, eqGroup_ptr)); }
 *
 */

 // ** Begin loop in dw, dx, dy, dz dimension(s). **

 // Number of iterations to get from begin_dw to (but not including) end_dw, stepping by step_dw. This value is rounded up because the last iteration may cover fewer than step_dw steps.
 const idx_t num_iters_dw = ((end_dw - begin_dw) + (step_dw - 1)) / step_dw;

 // Number of iterations to get from begin_dx to (but not including) end_dx, stepping by step_dx. This value is rounded up because the last iteration may cover fewer than step_dx steps.
 const idx_t num_iters_dx = ((end_dx - begin_dx) + (step_dx - 1)) / step_dx;

 // Number of iterations to get from begin_dy to (but not including) end_dy, stepping by step_dy. This value is rounded up because the last iteration may cover fewer than step_dy steps.
 const idx_t num_iters_dy = ((end_dy - begin_dy) + (step_dy - 1)) / step_dy;

 // Number of iterations to get from begin_dz to (but not including) end_dz, stepping by step_dz. This value is rounded up because the last iteration may cover fewer than step_dz steps.
 const idx_t num_iters_dz = ((end_dz - begin_dz) + (step_dz - 1)) / step_dz;

 // Number of iterations in dw, dx, dy, dz dimensions
 const idx_t num_iters_dw_dx_dy_dz = num_iters_dw * num_iters_dx * num_iters_dy * num_iters_dz;

 // Number of iterations in dx, dy, dz dimensions
 const idx_t num_iters_dx_dy_dz = num_iters_dx * num_iters_dy * num_iters_dz;

 // Number of iterations in dy, dz dimensions
 const idx_t num_iters_dy_dz = num_iters_dy * num_iters_dz;

 // Computation loop.
_Pragma("nounroll_and_jam") _Pragma("nofusion")
 for (idx_t loop_index_dw_dx_dy_dz = 0; loop_index_dw_dx_dy_dz < num_iters_dw_dx_dy_dz; loop_index_dw_dx_dy_dz++) {

 // Zero-based, unit-stride index for dw.
 idx_t index_dw = loop_index_dw_dx_dy_dz / num_iters_dx_dy_dz;

 // Zero-based, unit-stride index for dx.
 idx_t index_dx = (loop_index_dw_dx_dy_dz / num_iters_dy_dz) % num_iters_dx;

 // Zero-based, unit-stride index for dy.
 idx_t index_dy = (loop_index_dw_dx_dy_dz / num_iters_dz) % num_iters_dy;

 // Zero-based, unit-stride index for dz.
 idx_t index_dz = (loop_index_dw_dx_dy_dz) % num_iters_dz;

 // This value of index_dw covers dw from start_dw to stop_dw-1.
 const idx_t start_dw = begin_dw + (index_dw * step_dw);
 const idx_t stop_dw = std::min(start_dw + step_dw, end_dw);

 // This value of index_dx covers dx from start_dx to stop_dx-1.
 const idx_t start_dx = begin_dx + (index_dx * step_dx);
 const idx_t stop_dx = std::min(start_dx + step_dx, end_dx);

 // This value of index_dy covers dy from start_dy to stop_dy-1.
 const idx_t start_dy = begin_dy + (index_dy * step_dy);
 const idx_t stop_dy = std::min(start_dy + step_dy, end_dy);

 // This value of index_dz covers dz from start_dz to stop_dz-1.
 const idx_t start_dz = begin_dz + (index_dz * step_dz);
 const idx_t stop_dz = std::min(start_dz + step_dz, end_dz);

 // Computation.
  calc_region(start_dt, stop_dt, eqGroup_ptr, start_dw, start_dx, start_dy, start_dz, stop_dw, stop_dx, stop_dy, stop_dz);
 }
// End of generated code.
