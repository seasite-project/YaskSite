/*
 * 4-D loop code.
 * Generated automatically from the following pseudo-code:
 *
 *  loop(sbwv,sbxv,sbyv) {  loop(sbzv) { calc(cluster(begin_sbtv)); } }
 *
 */

 // ** Begin loop in sbwv, sbxv, sbyv dimension(s). **

 // Number of iterations to get from begin_sbwv to (but not including) end_sbwv, stepping by step_sbwv. This value is rounded up because the last iteration may cover fewer than step_sbwv steps.
 const idx_t num_iters_sbwv = ((end_sbwv - begin_sbwv) + (step_sbwv - 1)) / step_sbwv;

 // Number of iterations to get from begin_sbxv to (but not including) end_sbxv, stepping by step_sbxv. This value is rounded up because the last iteration may cover fewer than step_sbxv steps.
 const idx_t num_iters_sbxv = ((end_sbxv - begin_sbxv) + (step_sbxv - 1)) / step_sbxv;

 // Number of iterations to get from begin_sbyv to (but not including) end_sbyv, stepping by step_sbyv. This value is rounded up because the last iteration may cover fewer than step_sbyv steps.
 const idx_t num_iters_sbyv = ((end_sbyv - begin_sbyv) + (step_sbyv - 1)) / step_sbyv;

 // Number of iterations in sbwv, sbxv, sbyv dimensions
 const idx_t num_iters_sbwv_sbxv_sbyv = num_iters_sbwv * num_iters_sbxv * num_iters_sbyv;

 // Number of iterations in sbxv, sbyv dimensions
 const idx_t num_iters_sbxv_sbyv = num_iters_sbxv * num_iters_sbyv;
 for (idx_t loop_index_sbwv_sbxv_sbyv = 0; loop_index_sbwv_sbxv_sbyv < num_iters_sbwv_sbxv_sbyv; loop_index_sbwv_sbxv_sbyv++) {

 // Zero-based, unit-stride index for sbwv.
 idx_t index_sbwv = loop_index_sbwv_sbxv_sbyv / num_iters_sbxv_sbyv;

 // Zero-based, unit-stride index for sbxv.
 idx_t index_sbxv = (loop_index_sbwv_sbxv_sbyv / num_iters_sbyv) % num_iters_sbxv;

 // Zero-based, unit-stride index for sbyv.
 idx_t index_sbyv = (loop_index_sbwv_sbxv_sbyv) % num_iters_sbyv;

 // This value of index_sbwv covers sbwv from start_sbwv to stop_sbwv-1.
 const idx_t start_sbwv = begin_sbwv + (index_sbwv * step_sbwv);
 const idx_t stop_sbwv = std::min(start_sbwv + step_sbwv, end_sbwv);

 // This value of index_sbxv covers sbxv from start_sbxv to stop_sbxv-1.
 const idx_t start_sbxv = begin_sbxv + (index_sbxv * step_sbxv);
 const idx_t stop_sbxv = std::min(start_sbxv + step_sbxv, end_sbxv);

 // This value of index_sbyv covers sbyv from start_sbyv to stop_sbyv-1.
 const idx_t start_sbyv = begin_sbyv + (index_sbyv * step_sbyv);
 const idx_t stop_sbyv = std::min(start_sbyv + step_sbyv, end_sbyv);

 // ** Begin loop in sbzv dimension(s). **

 // Number of iterations to get from begin_sbzv to (but not including) end_sbzv, stepping by step_sbzv. This value is rounded up because the last iteration may cover fewer than step_sbzv steps.
 const idx_t num_iters_sbzv = ((end_sbzv - begin_sbzv) + (step_sbzv - 1)) / step_sbzv;

 // Computation loop.
_Pragma("nounroll_and_jam") _Pragma("nofusion")
 for (idx_t loop_index_sbzv = 0; loop_index_sbzv < num_iters_sbzv; loop_index_sbzv++) {

 // Zero-based, unit-stride index for sbzv.
 idx_t index_sbzv = loop_index_sbzv;

 // This value of index_sbzv covers sbzv from start_sbzv to stop_sbzv-1.
 const idx_t start_sbzv = begin_sbzv + (index_sbzv * step_sbzv);
 const idx_t stop_sbzv = std::min(start_sbzv + step_sbzv, end_sbzv);

 // Computation.
  calc_cluster(begin_sbtv, start_sbwv, start_sbxv, start_sbyv, start_sbzv, stop_sbwv, stop_sbxv, stop_sbyv, stop_sbzv);
 }
 }
// End of generated code.
