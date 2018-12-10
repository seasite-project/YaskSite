// Automatically generated code; do not edit.

////// YASK implementation of the 'heat3d' stencil //////

/*
Identified stencil equation-groups:
 Equation group 'stencil_0' updates grid 'data' with dim = 4.
 Equation group 'stencil_0' reads grid 'h_inv' with dim = 0.
 Equation group 'stencil_0' reads grid 'data' with dim = 4.
*/

namespace yask {

 ////// Stencil-specific data //////
struct StencilContext_heat3d_data : public StencilContext {

 ///// Grid(s).

 // The 4D 'data' grid, which is updated by one or more equations.
 static const idx_t data_alloc_t = 2; // total allocation required in 't' dimension.
 Grid_TXYZ<data_alloc_t>* data;
 const idx_t data_halo_x = 1; // halo allocation required in 'x' dimension.
 const idx_t data_halo_y = 1; // halo allocation required in 'y' dimension.
 const idx_t data_halo_z = 1; // halo allocation required in 'z' dimension.

 // Max halos across all grids.
 const idx_t max_halo_x = 1;
 const idx_t max_halo_y = 1;
 const idx_t max_halo_z = 1;

 ///// Parameter(s).

 // The 0D 'h_inv' parameter.
 GenericGrid0d<real_t>* h_inv;

 // Constructor.
 StencilContext_heat3d_data(StencilSettings& settings) : StencilContext(settings) {
  name = "heat3d";

 // Create grids and parameters.

  // Init grid 'data'.
 data = new Grid_TXYZ<data_alloc_t>("data");
 gridPtrs.push_back(data);
 gridMap["data"] = data;
 outputGridPtrs.push_back(data);
 outputGridMap["data"] = data;
 data->set_halo_x(data_halo_x);
 data->set_halo_y(data_halo_y);
 data->set_halo_z(data_halo_z);

  // Init parameter 'h_inv'.
 h_inv = new GenericGrid0d<real_t>();
 paramPtrs.push_back(h_inv);
 paramMap["h_inv"] = h_inv;

  // Rounded halo sizes.
  hx = ROUND_UP(max_halo_x, VLEN_X);
  hy = ROUND_UP(max_halo_y, VLEN_Y);
  hz = ROUND_UP(max_halo_z, VLEN_Z);
 }
}; // StencilContext_heat3d_data

 ////// Stencil equation-group 'stencil_0' w/no condition //////

 class EqGroup_stencil_0 : public EqGroupBase {
 protected:
 StencilContext_heat3d_data* _context = 0;
 public:

 // 14 FP operation(s) per point:
 // data(t+1, x, y, z) EQUALS (((1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)) + (-6 * data(t, x, y, z))) * h_inv).
 EqGroup_stencil_0(StencilContext_heat3d_data* context) :
 EqGroupBase(context),
 _context(context) {
 _name = "stencil_0";
 _scalar_fp_ops = 14;
 _scalar_points_read = 7;
 _scalar_points_written = 1;

 // The following grids are written by EqGroup_stencil_0
  outputGridPtrs.push_back(_context->data);

 // The following grids are read by EqGroup_stencil_0
  inputGridPtrs.push_back(_context->data);
 } // Ctor.

 // Determine whether EqGroup_stencil_0 is valid at the given indices. Return true if indices are within the valid sub-domain or false otherwise.
 virtual bool is_in_valid_domain(idx_t t, idx_t x, idx_t y, idx_t z) {
 return (x>=FIRST_INDEX(x)) && (x<=LAST_INDEX(x)) && (y>=FIRST_INDEX(y)) && (y<=LAST_INDEX(y)) && (z>=FIRST_INDEX(z)) && (z<=LAST_INDEX(z)); // full domain.
 }

 // Calculate one scalar result relative to indices t, x, y, z.
 virtual void calc_scalar(idx_t t, idx_t x, idx_t y, idx_t z) {

 // temp1 = 1 * data(t, x+1, y, z).
 real_t temp1 = 1.00000000000000000e+00 * _context->data->readElem(t, x+1, y, z, __LINE__);

 // temp2 = 1 * data(t, x-1, y, z).
 real_t temp2 = 1.00000000000000000e+00 * _context->data->readElem(t, x-1, y, z, __LINE__);

 // temp3 = (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)).
 real_t temp3 = temp1 + temp2;

 // temp4 = 1 * data(t, x, y+1, z).
 real_t temp4 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y+1, z, __LINE__);

 // temp5 = (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+1, z)).
 real_t temp5 = temp3 + temp4;

 // temp6 = 1 * data(t, x, y-1, z).
 real_t temp6 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y-1, z, __LINE__);

 // temp7 = (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)).
 real_t temp7 = temp5 + temp6;

 // temp8 = 1 * data(t, x, y, z+1).
 real_t temp8 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y, z+1, __LINE__);

 // temp9 = (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+1)).
 real_t temp9 = temp7 + temp8;

 // temp10 = 1 * data(t, x, y, z-1).
 real_t temp10 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y, z-1, __LINE__);

 // temp11 = (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)).
 real_t temp11 = temp9 + temp10;

 // temp12 = (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)) + (-6 * data(t, x, y, z)).
 real_t temp12 = temp11 + (-6.00000000000000000e+00 * _context->data->readElem(t, x, y, z, __LINE__));

 // temp13 = ((1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)) + (-6 * data(t, x, y, z))) * h_inv().
 real_t temp13 = temp12 * (*_context->h_inv)();

 // temp14 = (((1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)) + (-6 * data(t, x, y, z))) * h_inv).
 real_t temp14 = temp13;

 // Define value at data(t+1, x, y, z).
 _context->data->writeElem(temp14, t+1, x, y, z, __LINE__);
} // calc_scalar.

 // Calculate 4 result(s) relative to indices t, x, y, z in a 'x=1 * y=1 * z=4' cluster containing 1 'x=1 * y=1 * z=4' vector(s).
 // Indices must be normalized, i.e., already divided by VLEN_*.
 // SIMD calculations use 7 vector block(s) created from 7 aligned vector-block(s).
 // There are approximately 56 FP operation(s) per invocation.
 inline void calc_cluster(idx_t tv, idx_t xv, idx_t yv, idx_t zv) {

 // Element (un-normalized) indices.
 idx_t t = tv;
 idx_t x = xv * VLEN_X;
 idx_t y = yv * VLEN_Y;
 idx_t z = zv * VLEN_Z;

 // Read aligned vector block from data at t, x+1, y, z.
 real_vec_t temp_vec1 =  _context->data->readVecNorm(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Read aligned vector block from data at t, x-1, y, z.
 real_vec_t temp_vec2 =  _context->data->readVecNorm(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Read aligned vector block from data at t, x, y+1, z.
 real_vec_t temp_vec3 =  _context->data->readVecNorm(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Read aligned vector block from data at t, x, y-1, z.
 real_vec_t temp_vec4 =  _context->data->readVecNorm(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Read aligned vector block from data at t, x, y, z.
 real_vec_t temp_vec5 =  _context->data->readVecNorm(tv, xv, yv, zv, __LINE__);

 // Read aligned vector block from data at t, x, y, z+4.
 real_vec_t temp_vec6 =  _context->data->readVecNorm(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Construct unaligned vector block from data at t, x, y, z+1.
 real_vec_t temp_vec7;
 // temp_vec7[0] = temp_vec5[1];  // for t, x, y, z+1;
 // temp_vec7[1] = temp_vec5[2];  // for t, x, y, z+2;
 // temp_vec7[2] = temp_vec5[3];  // for t, x, y, z+3;
 // temp_vec7[3] = temp_vec6[0];  // for t, x, y, z+4;
 // Get 1 element(s) from temp_vec6 and 3 from temp_vec5.
 real_vec_align<1>(temp_vec7, temp_vec6, temp_vec5);

 // Read aligned vector block from data at t, x, y, z-4.
 real_vec_t temp_vec8 =  _context->data->readVecNorm(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Construct unaligned vector block from data at t, x, y, z-1.
 real_vec_t temp_vec9;
 // temp_vec9[0] = temp_vec8[3];  // for t, x, y, z-1;
 // temp_vec9[1] = temp_vec5[0];  // for t, x, y, z;
 // temp_vec9[2] = temp_vec5[1];  // for t, x, y, z+1;
 // temp_vec9[3] = temp_vec5[2];  // for t, x, y, z+2;
 // Get 3 element(s) from temp_vec5 and 1 from temp_vec8.
 real_vec_align<3>(temp_vec9, temp_vec5, temp_vec8);

 // temp_vec10 = 1 * data(t, x+1, y, z).
 real_vec_t temp_vec10 = 1.00000000000000000e+00 * temp_vec1;

 // temp_vec11 = 1 * data(t, x-1, y, z).
 real_vec_t temp_vec11 = 1.00000000000000000e+00 * temp_vec2;

 // temp_vec12 = (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)).
 real_vec_t temp_vec12 = temp_vec10 + temp_vec11;

 // temp_vec13 = 1 * data(t, x, y+1, z).
 real_vec_t temp_vec13 = 1.00000000000000000e+00 * temp_vec3;

 // temp_vec14 = (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+1, z)).
 real_vec_t temp_vec14 = temp_vec12 + temp_vec13;

 // temp_vec15 = 1 * data(t, x, y-1, z).
 real_vec_t temp_vec15 = 1.00000000000000000e+00 * temp_vec4;

 // temp_vec16 = (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)).
 real_vec_t temp_vec16 = temp_vec14 + temp_vec15;

 // temp_vec17 = 1 * data(t, x, y, z+1).
 real_vec_t temp_vec17 = 1.00000000000000000e+00 * temp_vec7;

 // temp_vec18 = (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+1)).
 real_vec_t temp_vec18 = temp_vec16 + temp_vec17;

 // temp_vec19 = 1 * data(t, x, y, z-1).
 real_vec_t temp_vec19 = 1.00000000000000000e+00 * temp_vec9;

 // temp_vec20 = (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)).
 real_vec_t temp_vec20 = temp_vec18 + temp_vec19;

 // temp_vec21 = (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)) + (-6 * data(t, x, y, z)).
 real_vec_t temp_vec21 = temp_vec20 + (-6.00000000000000000e+00 * temp_vec5);

 // temp_vec22 = ((1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)) + (-6 * data(t, x, y, z))) * h_inv().
 real_vec_t temp_vec22 = temp_vec21 * (*_context->h_inv)();

 // temp_vec23 = (((1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)) + (-6 * data(t, x, y, z))) * h_inv).
 real_vec_t temp_vec23 = temp_vec22;

 // Define value at data(t+1, x, y, z).

 // Write aligned vector block to data at t+1, x, y, z.
 _context->data->writeVecNorm(temp_vec23, tv+1, xv, yv, zv, __LINE__) ;
} // calc_cluster.

 // Simple shim function to map sub-block start vars to simple vars (ignoring stop vars).
 inline void calc_cluster(idx_t start_sbtv, idx_t start_sbwv, idx_t start_sbxv, idx_t start_sbyv, idx_t start_sbzv, idx_t stop_sbwv, idx_t stop_sbxv, idx_t stop_sbyv, idx_t stop_sbzv) {
 idx_t tv = start_sbtv;
 idx_t xv = start_sbxv;
 idx_t yv = start_sbyv;
 idx_t zv = start_sbzv;
 calc_cluster(tv, xv, yv, zv);
} // calc_cluster shim.

 // Prefetches cache line(s) for entire stencil relative to indices t, x, y, z in a 'x=1 * y=1 * z=4' cluster containing 1 'x=1 * y=1 * z=4' vector(s).
 // Indices must be normalized, i.e., already divided by VLEN_*.
 template<int level> inline void prefetch_cluster(idx_t tv, idx_t xv, idx_t yv, idx_t zv) {

 // Aligned data at t, x-1, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned data at t, x, y-1, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y, z-4.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv, __LINE__);

 // Aligned data at t, x, y, z+4.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y+1, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x+1, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);
} // prefetch_cluster.

 // Simple shim function to map sub-block start vars to simple vars (ignoring stop vars).
 template <int level> inline void prefetch_cluster(idx_t start_sbtv, idx_t start_sbwv, idx_t start_sbxv, idx_t start_sbyv, idx_t start_sbzv, idx_t stop_sbwv, idx_t stop_sbxv, idx_t stop_sbyv, idx_t stop_sbzv) {
 idx_t tv = start_sbtv;
 idx_t xv = start_sbxv;
 idx_t yv = start_sbyv;
 idx_t zv = start_sbzv;
 prefetch_cluster<level>(tv, xv, yv, zv);
} // prefetch_cluster shim.

 // Prefetches cache line(s) for leading edge of stencil advancing by 1 vector(s) in '+x' direction relative to indices t, x, y, z in a 'x=1 * y=1 * z=4' cluster containing 1 'x=1 * y=1 * z=4' vector(s).
 // Indices must be normalized, i.e., already divided by VLEN_*.
 template<int level> inline void prefetch_cluster_x(idx_t tv, idx_t xv, idx_t yv, idx_t zv) {

 // Aligned data at t, x, y-1, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y, z-4.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z+4.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y+1, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x+1, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);
} // prefetch_cluster_x.

 // Simple shim function to map sub-block start vars to simple vars (ignoring stop vars).
 template <int level> inline void prefetch_cluster_sbxv(idx_t start_sbtv, idx_t start_sbwv, idx_t start_sbxv, idx_t start_sbyv, idx_t start_sbzv, idx_t stop_sbwv, idx_t stop_sbxv, idx_t stop_sbyv, idx_t stop_sbzv) {
 idx_t tv = start_sbtv;
 idx_t xv = start_sbxv;
 idx_t yv = start_sbyv;
 idx_t zv = start_sbzv;
 prefetch_cluster_x<level>(tv, xv, yv, zv);
} // prefetch_cluster shim.

 // Prefetches cache line(s) for leading edge of stencil advancing by 1 vector(s) in '+y' direction relative to indices t, x, y, z in a 'x=1 * y=1 * z=4' cluster containing 1 'x=1 * y=1 * z=4' vector(s).
 // Indices must be normalized, i.e., already divided by VLEN_*.
 template<int level> inline void prefetch_cluster_y(idx_t tv, idx_t xv, idx_t yv, idx_t zv) {

 // Aligned data at t, x-1, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned data at t, x, y, z-4.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z+4.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y+1, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x+1, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);
} // prefetch_cluster_y.

 // Simple shim function to map sub-block start vars to simple vars (ignoring stop vars).
 template <int level> inline void prefetch_cluster_sbyv(idx_t start_sbtv, idx_t start_sbwv, idx_t start_sbxv, idx_t start_sbyv, idx_t start_sbzv, idx_t stop_sbwv, idx_t stop_sbxv, idx_t stop_sbyv, idx_t stop_sbzv) {
 idx_t tv = start_sbtv;
 idx_t xv = start_sbxv;
 idx_t yv = start_sbyv;
 idx_t zv = start_sbzv;
 prefetch_cluster_y<level>(tv, xv, yv, zv);
} // prefetch_cluster shim.

 // Prefetches cache line(s) for leading edge of stencil advancing by 1 vector(s) in '+z' direction relative to indices t, x, y, z in a 'x=1 * y=1 * z=4' cluster containing 1 'x=1 * y=1 * z=4' vector(s).
 // Indices must be normalized, i.e., already divided by VLEN_*.
 template<int level> inline void prefetch_cluster_z(idx_t tv, idx_t xv, idx_t yv, idx_t zv) {

 // Aligned data at t, x-1, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned data at t, x, y-1, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y, z+4.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y+1, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x+1, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);
} // prefetch_cluster_z.

 // Simple shim function to map sub-block start vars to simple vars (ignoring stop vars).
 template <int level> inline void prefetch_cluster_sbzv(idx_t start_sbtv, idx_t start_sbwv, idx_t start_sbxv, idx_t start_sbyv, idx_t start_sbzv, idx_t stop_sbwv, idx_t stop_sbxv, idx_t stop_sbyv, idx_t stop_sbzv) {
 idx_t tv = start_sbtv;
 idx_t xv = start_sbxv;
 idx_t yv = start_sbyv;
 idx_t zv = start_sbzv;
 prefetch_cluster_z<level>(tv, xv, yv, zv);
} // prefetch_cluster shim.

 // Calculate one sub-block of whole clusters.
 virtual void calc_sub_block_of_clusters(idx_t begin_sbtv, idx_t begin_sbxv, idx_t begin_sbyv, idx_t begin_sbzv, idx_t end_sbtv, idx_t end_sbxv, idx_t end_sbyv, idx_t end_sbzv) {

 // Steps and group sizes are based on cluster lengths.
 const idx_t step_sbtv = CLEN_T;
 const idx_t group_size_sbtv = CLEN_T;
 const idx_t step_sbxv = CLEN_X;
 const idx_t group_size_sbxv = CLEN_X;
 const idx_t step_sbyv = CLEN_Y;
 const idx_t group_size_sbyv = CLEN_Y;
 const idx_t step_sbzv = CLEN_Z;
 const idx_t group_size_sbzv = CLEN_Z;
 const idx_t begin_sbwv = 0; // not used in this stencil.
 const idx_t end_sbwv = 1;
 const idx_t step_sbwv = CLEN_W;
 const idx_t group_size_sbwv = CLEN_W;
 #if !defined(DEBUG) && defined(__INTEL_COMPILER)
 #pragma forceinline recursive
 #endif
 {
  // Include automatically-generated loop code that calls calc_cluster()  and optionally, the prefetch function(s).
  #include "stencil_sub_block_loops.hpp"
 }
} // calc_sub_block_of_clusters
}; // EqGroup_stencil_0.

 ////// Overall stencil-specific context //////
struct StencilContext_heat3d : public StencilContext_heat3d_data {

 // Stencil equation-groups.
 EqGroup_stencil_0 eqGroup_stencil_0;

 // Constructor.
 StencilContext_heat3d(StencilSettings& settings) : StencilContext_heat3d_data(settings),
  eqGroup_stencil_0(this) {

 // Equation groups.
  eqGroups.push_back(&eqGroup_stencil_0);
 } // Ctor.
}; // StencilContext_heat3d
} // namespace yask.
