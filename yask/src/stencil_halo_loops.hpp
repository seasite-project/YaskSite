/*
 * 4-D loop code.
 * Generated automatically from the following pseudo-code:
 *
 * omp loop(wv,xv,yv,zv)  { calc(halo(t)); }
 *
 */

 // ** Begin loop in wv, xv, yv, zv dimension(s). **

 // Number of iterations to get from begin_wv to (but not including) end_wv, stepping by step_wv. This value is rounded up because the last iteration may cover fewer than step_wv steps.
 const idx_t num_iters_wv = ((end_wv - begin_wv) + (step_wv - 1)) / step_wv;

 // Number of iterations to get from begin_xv to (but not including) end_xv, stepping by step_xv. This value is rounded up because the last iteration may cover fewer than step_xv steps.
 const idx_t num_iters_xv = ((end_xv - begin_xv) + (step_xv - 1)) / step_xv;

 // Number of iterations to get from begin_yv to (but not including) end_yv, stepping by step_yv. This value is rounded up because the last iteration may cover fewer than step_yv steps.
 const idx_t num_iters_yv = ((end_yv - begin_yv) + (step_yv - 1)) / step_yv;

 // Number of iterations to get from begin_zv to (but not including) end_zv, stepping by step_zv. This value is rounded up because the last iteration may cover fewer than step_zv steps.
 const idx_t num_iters_zv = ((end_zv - begin_zv) + (step_zv - 1)) / step_zv;

 // Number of iterations in wv, xv, yv, zv dimensions
 const idx_t num_iters_wv_xv_yv_zv = num_iters_wv * num_iters_xv * num_iters_yv * num_iters_zv;

 // Number of iterations in xv, yv, zv dimensions
 const idx_t num_iters_xv_yv_zv = num_iters_xv * num_iters_yv * num_iters_zv;

 // Number of iterations in yv, zv dimensions
 const idx_t num_iters_yv_zv = num_iters_yv * num_iters_zv;

 // Computation loop.
_Pragma("nounroll_and_jam") _Pragma("nofusion")

 // Distribute iterations among OpenMP threads.
_Pragma("omp parallel for schedule(static) proc_bind(spread)")
 for (idx_t loop_index_wv_xv_yv_zv = 0; loop_index_wv_xv_yv_zv < num_iters_wv_xv_yv_zv; loop_index_wv_xv_yv_zv++) {

 // Zero-based, unit-stride index for wv.
 idx_t index_wv = loop_index_wv_xv_yv_zv / num_iters_xv_yv_zv;

 // Zero-based, unit-stride index for xv.
 idx_t index_xv = (loop_index_wv_xv_yv_zv / num_iters_yv_zv) % num_iters_xv;

 // Zero-based, unit-stride index for yv.
 idx_t index_yv = (loop_index_wv_xv_yv_zv / num_iters_zv) % num_iters_yv;

 // Zero-based, unit-stride index for zv.
 idx_t index_zv = (loop_index_wv_xv_yv_zv) % num_iters_zv;

 // This value of index_wv covers wv from start_wv to stop_wv-1.
 const idx_t start_wv = begin_wv + (index_wv * step_wv);
 const idx_t stop_wv = std::min(start_wv + step_wv, end_wv);

 // This value of index_xv covers xv from start_xv to stop_xv-1.
 const idx_t start_xv = begin_xv + (index_xv * step_xv);
 const idx_t stop_xv = std::min(start_xv + step_xv, end_xv);

 // This value of index_yv covers yv from start_yv to stop_yv-1.
 const idx_t start_yv = begin_yv + (index_yv * step_yv);
 const idx_t stop_yv = std::min(start_yv + step_yv, end_yv);

 // This value of index_zv covers zv from start_zv to stop_zv-1.
 const idx_t start_zv = begin_zv + (index_zv * step_zv);
 const idx_t stop_zv = std::min(start_zv + step_zv, end_zv);

 // Computation.
  calc_halo(t, start_wv, start_xv, start_yv, start_zv, stop_wv, stop_xv, stop_yv, stop_zv);
 }
// End of generated code.
