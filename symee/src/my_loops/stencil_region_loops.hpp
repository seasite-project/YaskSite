/*
 * 4-D loop code.
 * Generated automatically from the following pseudo-code:
 *
 * omp grouped loop(rw,rx,ry,rz) {  calc(block(rt)); }
 *
 */

 // ** Begin loop in rw, rx, ry, rz dimension(s). **

 // Computation loop.
_Pragma("nounroll_and_jam") _Pragma("nofusion")
 // Distribute iterations among OpenMP threads.
_Pragma("omp parallel for schedule(dynamic,1) proc_bind(spread)")
for(idx_t start_rw = begin_rw; start_rw<=(end_rw-step_rw); start_rw += step_rw) {
    idx_t stop_rw = std::min(start_rw+step_rw, end_rw);
    for(idx_t start_rx = begin_rx; start_rx<=(end_rx-step_rx); start_rx += step_rx) {
        idx_t stop_rx = std::min(start_rx+step_rx, end_rx);
        for(idx_t start_ry = begin_ry; start_ry<=(end_ry-step_ry); start_ry += step_ry) {
            idx_t stop_ry = std::min(start_ry+step_ry, end_ry);
            for(idx_t start_rz = begin_rz; start_rz<=(end_rz-step_rz); start_rz += step_rz) {
                idx_t stop_rz = std::min(start_rz+step_rz, end_rz);
                // Computation.
                eg->calc_block(rt, start_rw, start_rx, start_ry, start_rz, stop_rw, stop_rx, stop_ry, stop_rz);
            }
        }
    }
 }
// End of generated code.
