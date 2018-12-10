/*
 * 4-D loop code.
 * Generated automatically from the following pseudo-code:
 *
 *  loop(sbwv,sbxv,sbyv) { prefetch(L2) loop(sbzv) { calc(cluster(begin_sbtv)); } }
 *
 */

 // ** Begin loop in sbwv, sbxv, sbyv dimension(s). **
// Number of iterations in sbxv, sbyv dimensions
for (idx_t start_sbwv = begin_sbwv; start_sbwv <= (end_sbwv-step_sbwv); start_sbwv+=step_sbwv) {
    idx_t stop_sbwv = std::min(start_sbwv+step_sbwv, end_sbwv);
    for (idx_t start_sbxv = begin_sbxv; start_sbxv <= (end_sbxv-step_sbxv); start_sbxv+=step_sbxv) {
        idx_t stop_sbxv = std::min(start_sbxv+step_sbxv, end_sbxv);
        for (idx_t start_sbyv = begin_sbyv; start_sbyv <= (end_sbyv-step_sbyv); start_sbyv+=step_sbyv) {
            idx_t stop_sbyv = std::min(start_sbyv+step_sbyv, end_sbyv);
            for (idx_t start_sbzv = begin_sbzv; start_sbzv <= (end_sbzv-step_sbzv); start_sbzv+=step_sbzv) {
                // ** Begin loop in sbzv dimension(s). **
                // Computation.
                idx_t stop_sbzv = std::min(start_sbzv+step_sbzv, end_sbzv);
                calc_cluster(begin_sbtv, start_sbwv, start_sbxv, start_sbyv, start_sbzv, stop_sbwv, stop_sbxv, stop_sbyv, stop_sbzv);
            }
        }
    }
}
// End of generated code.
