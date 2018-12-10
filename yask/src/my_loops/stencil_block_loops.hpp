/*
 * 4-D loop code.
 * Generated automatically from the following pseudo-code:
 *
 * omp grouped loop(bw,bx,by,bz) {  calc(sub_block(bt)); }
 *
 */

 // ** Begin loop in bw, bx, by, bz dimension(s). **
 // Computation loop.
_Pragma("nounroll_and_jam") _Pragma("nofusion")

 // Distribute iterations among OpenMP threads.
_Pragma("omp parallel for schedule(static,1) proc_bind(close)")
for (idx_t start_bw = begin_bw; start_bw <=(end_bw-step_bw); start_bw+=step_bw) {
    idx_t stop_bw = std::min(start_bw+step_bw, end_bw);
    for (idx_t start_bx = begin_bx; start_bx <=(end_bx-step_bx); start_bx+=step_bx) {
        idx_t stop_bx = std::min(start_bx+step_bx, end_bx);
        for (idx_t start_by = begin_by; start_by <=(end_by-step_by); start_by+=step_by) {
            idx_t stop_by = std::min(start_by+step_by, end_by);
            for (idx_t start_bz = begin_bz; start_bz <=(end_bz-step_bz); start_bz+=step_bz) {
                idx_t stop_bz = std::min(start_bz+step_bz, end_bz);
                // Computation.
                calc_sub_block(bt, start_bw, start_bx, start_by, start_bz, stop_bw, stop_bx, stop_by, stop_bz);
            }
        }
    }
}
// End of generated code.
