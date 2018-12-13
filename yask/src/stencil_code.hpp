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
 const idx_t data_halo_x = 4; // halo allocation required in 'x' dimension.
 const idx_t data_halo_y = 4; // halo allocation required in 'y' dimension.
 const idx_t data_halo_z = 4; // halo allocation required in 'z' dimension.

 // Max halos across all grids.
 const idx_t max_halo_x = 4;
 const idx_t max_halo_y = 4;
 const idx_t max_halo_z = 4;

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

 // 50 FP operation(s) per point:
 // data(t+1, x, y, z) EQUALS (((1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)) + (1 * data(t, x, y, z+2)) + (1 * data(t, x, y, z-2)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)) + (-6 * data(t, x, y, z))) * h_inv).
 EqGroup_stencil_0(StencilContext_heat3d_data* context) :
 EqGroupBase(context),
 _context(context) {
 _name = "stencil_0";
 _scalar_fp_ops = 50;
 _scalar_points_read = 25;
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

 // temp1 = 1 * data(t, x+4, y, z).
 real_t temp1 = 1.00000000000000000e+00 * _context->data->readElem(t, x+4, y, z, __LINE__);

 // temp2 = 1 * data(t, x-4, y, z).
 real_t temp2 = 1.00000000000000000e+00 * _context->data->readElem(t, x-4, y, z, __LINE__);

 // temp3 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)).
 real_t temp3 = temp1 + temp2;

 // temp4 = 1 * data(t, x+3, y, z).
 real_t temp4 = 1.00000000000000000e+00 * _context->data->readElem(t, x+3, y, z, __LINE__);

 // temp5 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)).
 real_t temp5 = temp3 + temp4;

 // temp6 = 1 * data(t, x-3, y, z).
 real_t temp6 = 1.00000000000000000e+00 * _context->data->readElem(t, x-3, y, z, __LINE__);

 // temp7 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)).
 real_t temp7 = temp5 + temp6;

 // temp8 = 1 * data(t, x+2, y, z).
 real_t temp8 = 1.00000000000000000e+00 * _context->data->readElem(t, x+2, y, z, __LINE__);

 // temp9 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)).
 real_t temp9 = temp7 + temp8;

 // temp10 = 1 * data(t, x-2, y, z).
 real_t temp10 = 1.00000000000000000e+00 * _context->data->readElem(t, x-2, y, z, __LINE__);

 // temp11 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)).
 real_t temp11 = temp9 + temp10;

 // temp12 = 1 * data(t, x+1, y, z).
 real_t temp12 = 1.00000000000000000e+00 * _context->data->readElem(t, x+1, y, z, __LINE__);

 // temp13 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)).
 real_t temp13 = temp11 + temp12;

 // temp14 = 1 * data(t, x-1, y, z).
 real_t temp14 = 1.00000000000000000e+00 * _context->data->readElem(t, x-1, y, z, __LINE__);

 // temp15 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)).
 real_t temp15 = temp13 + temp14;

 // temp16 = 1 * data(t, x, y+4, z).
 real_t temp16 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y+4, z, __LINE__);

 // temp17 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)).
 real_t temp17 = temp15 + temp16;

 // temp18 = 1 * data(t, x, y-4, z).
 real_t temp18 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y-4, z, __LINE__);

 // temp19 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)).
 real_t temp19 = temp17 + temp18;

 // temp20 = 1 * data(t, x, y+3, z).
 real_t temp20 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y+3, z, __LINE__);

 // temp21 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)).
 real_t temp21 = temp19 + temp20;

 // temp22 = 1 * data(t, x, y-3, z).
 real_t temp22 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y-3, z, __LINE__);

 // temp23 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)).
 real_t temp23 = temp21 + temp22;

 // temp24 = 1 * data(t, x, y+2, z).
 real_t temp24 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y+2, z, __LINE__);

 // temp25 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)).
 real_t temp25 = temp23 + temp24;

 // temp26 = 1 * data(t, x, y-2, z).
 real_t temp26 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y-2, z, __LINE__);

 // temp27 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)).
 real_t temp27 = temp25 + temp26;

 // temp28 = 1 * data(t, x, y+1, z).
 real_t temp28 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y+1, z, __LINE__);

 // temp29 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)).
 real_t temp29 = temp27 + temp28;

 // temp30 = 1 * data(t, x, y-1, z).
 real_t temp30 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y-1, z, __LINE__);

 // temp31 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)).
 real_t temp31 = temp29 + temp30;

 // temp32 = 1 * data(t, x, y, z+4).
 real_t temp32 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y, z+4, __LINE__);

 // temp33 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)).
 real_t temp33 = temp31 + temp32;

 // temp34 = 1 * data(t, x, y, z-4).
 real_t temp34 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y, z-4, __LINE__);

 // temp35 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)).
 real_t temp35 = temp33 + temp34;

 // temp36 = 1 * data(t, x, y, z+3).
 real_t temp36 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y, z+3, __LINE__);

 // temp37 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)).
 real_t temp37 = temp35 + temp36;

 // temp38 = 1 * data(t, x, y, z-3).
 real_t temp38 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y, z-3, __LINE__);

 // temp39 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)).
 real_t temp39 = temp37 + temp38;

 // temp40 = 1 * data(t, x, y, z+2).
 real_t temp40 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y, z+2, __LINE__);

 // temp41 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)) + (1 * data(t, x, y, z+2)).
 real_t temp41 = temp39 + temp40;

 // temp42 = 1 * data(t, x, y, z-2).
 real_t temp42 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y, z-2, __LINE__);

 // temp43 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)) + (1 * data(t, x, y, z+2)) + (1 * data(t, x, y, z-2)).
 real_t temp43 = temp41 + temp42;

 // temp44 = 1 * data(t, x, y, z+1).
 real_t temp44 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y, z+1, __LINE__);

 // temp45 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)) + (1 * data(t, x, y, z+2)) + (1 * data(t, x, y, z-2)) + (1 * data(t, x, y, z+1)).
 real_t temp45 = temp43 + temp44;

 // temp46 = 1 * data(t, x, y, z-1).
 real_t temp46 = 1.00000000000000000e+00 * _context->data->readElem(t, x, y, z-1, __LINE__);

 // temp47 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)) + (1 * data(t, x, y, z+2)) + (1 * data(t, x, y, z-2)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)).
 real_t temp47 = temp45 + temp46;

 // temp48 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)) + (1 * data(t, x, y, z+2)) + (1 * data(t, x, y, z-2)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)) + (-6 * data(t, x, y, z)).
 real_t temp48 = temp47 + (-6.00000000000000000e+00 * _context->data->readElem(t, x, y, z, __LINE__));

 // temp49 = ((1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)) + (1 * data(t, x, y, z+2)) + (1 * data(t, x, y, z-2)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)) + (-6 * data(t, x, y, z))) * h_inv().
 real_t temp49 = temp48 * (*_context->h_inv)();

 // temp50 = (((1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)) + (1 * data(t, x, y, z+2)) + (1 * data(t, x, y, z-2)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)) + (-6 * data(t, x, y, z))) * h_inv).
 real_t temp50 = temp49;

 // Define value at data(t+1, x, y, z).
 _context->data->writeElem(temp50, t+1, x, y, z, __LINE__);
} // calc_scalar.

 // Calculate 4 result(s) relative to indices t, x, y, z in a 'x=4 * y=1 * z=1' cluster containing 1 'x=4 * y=1 * z=1' vector(s).
 // Indices must be normalized, i.e., already divided by VLEN_*.
 // SIMD calculations use 25 vector block(s) created from 19 aligned vector-block(s).
 // There are approximately 200 FP operation(s) per invocation.
 inline void calc_cluster(idx_t tv, idx_t xv, idx_t yv, idx_t zv) {

 // Element (un-normalized) indices.
 idx_t t = tv;
 idx_t x = xv * VLEN_X;
 idx_t y = yv * VLEN_Y;
 idx_t z = zv * VLEN_Z;

 // Read aligned vector block from data at t, x+4, y, z.
 real_vec_t temp_vec1 =  _context->data->readVecNorm(tv, xv + (4 / VLEN_X), yv, zv, __LINE__);

 // temp_vec2 = 1 * data(t, x+4, y, z).
 real_vec_t temp_vec2 = 1.00000000000000000e+00 * temp_vec1;

 // Read aligned vector block from data at t, x-4, y, z.
 real_vec_t temp_vec3 =  _context->data->readVecNorm(tv, xv - (4 / VLEN_X), yv, zv, __LINE__);

 // temp_vec4 = 1 * data(t, x-4, y, z).
 real_vec_t temp_vec4 = 1.00000000000000000e+00 * temp_vec3;

 // temp_vec5 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)).
 real_vec_t temp_vec5 = temp_vec2 + temp_vec4;

 // Read aligned vector block from data at t, x, y, z.
 real_vec_t temp_vec6 =  _context->data->readVecNorm(tv, xv, yv, zv, __LINE__);

 // Construct unaligned vector block from data at t, x+3, y, z.
 real_vec_t temp_vec7;
 // temp_vec7[0] = temp_vec6[3];  // for t, x+3, y, z;
 // temp_vec7[1] = temp_vec1[0];  // for t, x+4, y, z;
 // temp_vec7[2] = temp_vec1[1];  // for t, x+5, y, z;
 // temp_vec7[3] = temp_vec1[2];  // for t, x+6, y, z;
 // Get 3 element(s) from temp_vec1 and 1 from temp_vec6.
 real_vec_align<3>(temp_vec7, temp_vec1, temp_vec6);

 // temp_vec8 = 1 * data(t, x+3, y, z).
 real_vec_t temp_vec8 = 1.00000000000000000e+00 * temp_vec7;

 // temp_vec9 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)).
 real_vec_t temp_vec9 = temp_vec5 + temp_vec8;

 // Construct unaligned vector block from data at t, x-3, y, z.
 real_vec_t temp_vec10;
 // temp_vec10[0] = temp_vec3[1];  // for t, x-3, y, z;
 // temp_vec10[1] = temp_vec3[2];  // for t, x-2, y, z;
 // temp_vec10[2] = temp_vec3[3];  // for t, x-1, y, z;
 // temp_vec10[3] = temp_vec6[0];  // for t, x, y, z;
 // Get 1 element(s) from temp_vec6 and 3 from temp_vec3.
 real_vec_align<1>(temp_vec10, temp_vec6, temp_vec3);

 // temp_vec11 = 1 * data(t, x-3, y, z).
 real_vec_t temp_vec11 = 1.00000000000000000e+00 * temp_vec10;

 // temp_vec12 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)).
 real_vec_t temp_vec12 = temp_vec9 + temp_vec11;

 // Construct unaligned vector block from data at t, x+2, y, z.
 real_vec_t temp_vec13;
 // temp_vec13[0] = temp_vec6[2];  // for t, x+2, y, z;
 // temp_vec13[1] = temp_vec6[3];  // for t, x+3, y, z;
 // temp_vec13[2] = temp_vec1[0];  // for t, x+4, y, z;
 // temp_vec13[3] = temp_vec1[1];  // for t, x+5, y, z;
 // Get 2 element(s) from temp_vec1 and 2 from temp_vec6.
 real_vec_align<2>(temp_vec13, temp_vec1, temp_vec6);

 // temp_vec14 = 1 * data(t, x+2, y, z).
 real_vec_t temp_vec14 = 1.00000000000000000e+00 * temp_vec13;

 // temp_vec15 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)).
 real_vec_t temp_vec15 = temp_vec12 + temp_vec14;

 // Construct unaligned vector block from data at t, x-2, y, z.
 real_vec_t temp_vec16;
 // temp_vec16[0] = temp_vec3[2];  // for t, x-2, y, z;
 // temp_vec16[1] = temp_vec3[3];  // for t, x-1, y, z;
 // temp_vec16[2] = temp_vec6[0];  // for t, x, y, z;
 // temp_vec16[3] = temp_vec6[1];  // for t, x+1, y, z;
 // Get 2 element(s) from temp_vec6 and 2 from temp_vec3.
 real_vec_align<2>(temp_vec16, temp_vec6, temp_vec3);

 // temp_vec17 = 1 * data(t, x-2, y, z).
 real_vec_t temp_vec17 = 1.00000000000000000e+00 * temp_vec16;

 // temp_vec18 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)).
 real_vec_t temp_vec18 = temp_vec15 + temp_vec17;

 // Construct unaligned vector block from data at t, x+1, y, z.
 real_vec_t temp_vec19;
 // temp_vec19[0] = temp_vec6[1];  // for t, x+1, y, z;
 // temp_vec19[1] = temp_vec6[2];  // for t, x+2, y, z;
 // temp_vec19[2] = temp_vec6[3];  // for t, x+3, y, z;
 // temp_vec19[3] = temp_vec1[0];  // for t, x+4, y, z;
 // Get 1 element(s) from temp_vec1 and 3 from temp_vec6.
 real_vec_align<1>(temp_vec19, temp_vec1, temp_vec6);

 // temp_vec20 = 1 * data(t, x+1, y, z).
 real_vec_t temp_vec20 = 1.00000000000000000e+00 * temp_vec19;

 // temp_vec21 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)).
 real_vec_t temp_vec21 = temp_vec18 + temp_vec20;

 // Construct unaligned vector block from data at t, x-1, y, z.
 real_vec_t temp_vec22;
 // temp_vec22[0] = temp_vec3[3];  // for t, x-1, y, z;
 // temp_vec22[1] = temp_vec6[0];  // for t, x, y, z;
 // temp_vec22[2] = temp_vec6[1];  // for t, x+1, y, z;
 // temp_vec22[3] = temp_vec6[2];  // for t, x+2, y, z;
 // Get 3 element(s) from temp_vec6 and 1 from temp_vec3.
 real_vec_align<3>(temp_vec22, temp_vec6, temp_vec3);

 // temp_vec23 = 1 * data(t, x-1, y, z).
 real_vec_t temp_vec23 = 1.00000000000000000e+00 * temp_vec22;

 // temp_vec24 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)).
 real_vec_t temp_vec24 = temp_vec21 + temp_vec23;

 // Read aligned vector block from data at t, x, y+4, z.
 real_vec_t temp_vec25 =  _context->data->readVecNorm(tv, xv, yv + (4 / VLEN_Y), zv, __LINE__);

 // temp_vec26 = 1 * data(t, x, y+4, z).
 real_vec_t temp_vec26 = 1.00000000000000000e+00 * temp_vec25;

 // temp_vec27 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)).
 real_vec_t temp_vec27 = temp_vec24 + temp_vec26;

 // Read aligned vector block from data at t, x, y-4, z.
 real_vec_t temp_vec28 =  _context->data->readVecNorm(tv, xv, yv - (4 / VLEN_Y), zv, __LINE__);

 // temp_vec29 = 1 * data(t, x, y-4, z).
 real_vec_t temp_vec29 = 1.00000000000000000e+00 * temp_vec28;

 // temp_vec30 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)).
 real_vec_t temp_vec30 = temp_vec27 + temp_vec29;

 // Read aligned vector block from data at t, x, y+3, z.
 real_vec_t temp_vec31 =  _context->data->readVecNorm(tv, xv, yv + (3 / VLEN_Y), zv, __LINE__);

 // temp_vec32 = 1 * data(t, x, y+3, z).
 real_vec_t temp_vec32 = 1.00000000000000000e+00 * temp_vec31;

 // temp_vec33 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)).
 real_vec_t temp_vec33 = temp_vec30 + temp_vec32;

 // Read aligned vector block from data at t, x, y-3, z.
 real_vec_t temp_vec34 =  _context->data->readVecNorm(tv, xv, yv - (3 / VLEN_Y), zv, __LINE__);

 // temp_vec35 = 1 * data(t, x, y-3, z).
 real_vec_t temp_vec35 = 1.00000000000000000e+00 * temp_vec34;

 // temp_vec36 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)).
 real_vec_t temp_vec36 = temp_vec33 + temp_vec35;

 // Read aligned vector block from data at t, x, y+2, z.
 real_vec_t temp_vec37 =  _context->data->readVecNorm(tv, xv, yv + (2 / VLEN_Y), zv, __LINE__);

 // temp_vec38 = 1 * data(t, x, y+2, z).
 real_vec_t temp_vec38 = 1.00000000000000000e+00 * temp_vec37;

 // temp_vec39 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)).
 real_vec_t temp_vec39 = temp_vec36 + temp_vec38;

 // Read aligned vector block from data at t, x, y-2, z.
 real_vec_t temp_vec40 =  _context->data->readVecNorm(tv, xv, yv - (2 / VLEN_Y), zv, __LINE__);

 // temp_vec41 = 1 * data(t, x, y-2, z).
 real_vec_t temp_vec41 = 1.00000000000000000e+00 * temp_vec40;

 // temp_vec42 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)).
 real_vec_t temp_vec42 = temp_vec39 + temp_vec41;

 // Read aligned vector block from data at t, x, y+1, z.
 real_vec_t temp_vec43 =  _context->data->readVecNorm(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // temp_vec44 = 1 * data(t, x, y+1, z).
 real_vec_t temp_vec44 = 1.00000000000000000e+00 * temp_vec43;

 // temp_vec45 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)).
 real_vec_t temp_vec45 = temp_vec42 + temp_vec44;

 // Read aligned vector block from data at t, x, y-1, z.
 real_vec_t temp_vec46 =  _context->data->readVecNorm(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // temp_vec47 = 1 * data(t, x, y-1, z).
 real_vec_t temp_vec47 = 1.00000000000000000e+00 * temp_vec46;

 // temp_vec48 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)).
 real_vec_t temp_vec48 = temp_vec45 + temp_vec47;

 // Read aligned vector block from data at t, x, y, z+4.
 real_vec_t temp_vec49 =  _context->data->readVecNorm(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // temp_vec50 = 1 * data(t, x, y, z+4).
 real_vec_t temp_vec50 = 1.00000000000000000e+00 * temp_vec49;

 // temp_vec51 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)).
 real_vec_t temp_vec51 = temp_vec48 + temp_vec50;

 // Read aligned vector block from data at t, x, y, z-4.
 real_vec_t temp_vec52 =  _context->data->readVecNorm(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // temp_vec53 = 1 * data(t, x, y, z-4).
 real_vec_t temp_vec53 = 1.00000000000000000e+00 * temp_vec52;

 // temp_vec54 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)).
 real_vec_t temp_vec54 = temp_vec51 + temp_vec53;

 // Read aligned vector block from data at t, x, y, z+3.
 real_vec_t temp_vec55 =  _context->data->readVecNorm(tv, xv, yv, zv + (3 / VLEN_Z), __LINE__);

 // temp_vec56 = 1 * data(t, x, y, z+3).
 real_vec_t temp_vec56 = 1.00000000000000000e+00 * temp_vec55;

 // temp_vec57 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)).
 real_vec_t temp_vec57 = temp_vec54 + temp_vec56;

 // Read aligned vector block from data at t, x, y, z-3.
 real_vec_t temp_vec58 =  _context->data->readVecNorm(tv, xv, yv, zv - (3 / VLEN_Z), __LINE__);

 // temp_vec59 = 1 * data(t, x, y, z-3).
 real_vec_t temp_vec59 = 1.00000000000000000e+00 * temp_vec58;

 // temp_vec60 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)).
 real_vec_t temp_vec60 = temp_vec57 + temp_vec59;

 // Read aligned vector block from data at t, x, y, z+2.
 real_vec_t temp_vec61 =  _context->data->readVecNorm(tv, xv, yv, zv + (2 / VLEN_Z), __LINE__);

 // temp_vec62 = 1 * data(t, x, y, z+2).
 real_vec_t temp_vec62 = 1.00000000000000000e+00 * temp_vec61;

 // temp_vec63 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)) + (1 * data(t, x, y, z+2)).
 real_vec_t temp_vec63 = temp_vec60 + temp_vec62;

 // Read aligned vector block from data at t, x, y, z-2.
 real_vec_t temp_vec64 =  _context->data->readVecNorm(tv, xv, yv, zv - (2 / VLEN_Z), __LINE__);

 // temp_vec65 = 1 * data(t, x, y, z-2).
 real_vec_t temp_vec65 = 1.00000000000000000e+00 * temp_vec64;

 // temp_vec66 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)) + (1 * data(t, x, y, z+2)) + (1 * data(t, x, y, z-2)).
 real_vec_t temp_vec66 = temp_vec63 + temp_vec65;

 // Read aligned vector block from data at t, x, y, z+1.
 real_vec_t temp_vec67 =  _context->data->readVecNorm(tv, xv, yv, zv + (1 / VLEN_Z), __LINE__);

 // temp_vec68 = 1 * data(t, x, y, z+1).
 real_vec_t temp_vec68 = 1.00000000000000000e+00 * temp_vec67;

 // temp_vec69 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)) + (1 * data(t, x, y, z+2)) + (1 * data(t, x, y, z-2)) + (1 * data(t, x, y, z+1)).
 real_vec_t temp_vec69 = temp_vec66 + temp_vec68;

 // Read aligned vector block from data at t, x, y, z-1.
 real_vec_t temp_vec70 =  _context->data->readVecNorm(tv, xv, yv, zv - (1 / VLEN_Z), __LINE__);

 // temp_vec71 = 1 * data(t, x, y, z-1).
 real_vec_t temp_vec71 = 1.00000000000000000e+00 * temp_vec70;

 // temp_vec72 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)) + (1 * data(t, x, y, z+2)) + (1 * data(t, x, y, z-2)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)).
 real_vec_t temp_vec72 = temp_vec69 + temp_vec71;

 // temp_vec73 = (1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)) + (1 * data(t, x, y, z+2)) + (1 * data(t, x, y, z-2)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)) + (-6 * data(t, x, y, z)).
 real_vec_t temp_vec73 = temp_vec72 + (-6.00000000000000000e+00 * temp_vec6);

 // temp_vec74 = ((1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)) + (1 * data(t, x, y, z+2)) + (1 * data(t, x, y, z-2)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)) + (-6 * data(t, x, y, z))) * h_inv().
 real_vec_t temp_vec74 = temp_vec73 * (*_context->h_inv)();

 // temp_vec75 = (((1 * data(t, x+4, y, z)) + (1 * data(t, x-4, y, z)) + (1 * data(t, x+3, y, z)) + (1 * data(t, x-3, y, z)) + (1 * data(t, x+2, y, z)) + (1 * data(t, x-2, y, z)) + (1 * data(t, x+1, y, z)) + (1 * data(t, x-1, y, z)) + (1 * data(t, x, y+4, z)) + (1 * data(t, x, y-4, z)) + (1 * data(t, x, y+3, z)) + (1 * data(t, x, y-3, z)) + (1 * data(t, x, y+2, z)) + (1 * data(t, x, y-2, z)) + (1 * data(t, x, y+1, z)) + (1 * data(t, x, y-1, z)) + (1 * data(t, x, y, z+4)) + (1 * data(t, x, y, z-4)) + (1 * data(t, x, y, z+3)) + (1 * data(t, x, y, z-3)) + (1 * data(t, x, y, z+2)) + (1 * data(t, x, y, z-2)) + (1 * data(t, x, y, z+1)) + (1 * data(t, x, y, z-1)) + (-6 * data(t, x, y, z))) * h_inv).
 real_vec_t temp_vec75 = temp_vec74;

 // Define value at data(t+1, x, y, z).

 // Write aligned vector block to data at t+1, x, y, z.
 _context->data->writeVecNorm(temp_vec75, tv+1, xv, yv, zv, __LINE__) ;
} // calc_cluster.

 // Simple shim function to map sub-block start vars to simple vars (ignoring stop vars).
 inline void calc_cluster(idx_t start_sbtv, idx_t start_sbwv, idx_t start_sbxv, idx_t start_sbyv, idx_t start_sbzv, idx_t stop_sbwv, idx_t stop_sbxv, idx_t stop_sbyv, idx_t stop_sbzv) {
 idx_t tv = start_sbtv;
 idx_t xv = start_sbxv;
 idx_t yv = start_sbyv;
 idx_t zv = start_sbzv;
 calc_cluster(tv, xv, yv, zv);
} // calc_cluster shim.

 // Prefetches cache line(s) for entire stencil relative to indices t, x, y, z in a 'x=4 * y=1 * z=1' cluster containing 1 'x=4 * y=1 * z=1' vector(s).
 // Indices must be normalized, i.e., already divided by VLEN_*.
 template<int level> inline void prefetch_cluster(idx_t tv, idx_t xv, idx_t yv, idx_t zv) {

 // Aligned data at t, x-4, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv - (4 / VLEN_X), yv, zv, __LINE__);

 // Aligned data at t, x, y-4, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv - (4 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y-3, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv - (3 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y-2, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv - (2 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y-1, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y, z-4.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z-3.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv - (3 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z-2.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv - (2 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z-1.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv - (1 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv, __LINE__);

 // Aligned data at t, x, y, z+1.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (1 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z+2.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (2 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z+3.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (3 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z+4.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y+1, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y+2, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (2 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y+3, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (3 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y+4, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (4 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x+4, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv + (4 / VLEN_X), yv, zv, __LINE__);
} // prefetch_cluster.

 // Simple shim function to map sub-block start vars to simple vars (ignoring stop vars).
 template <int level> inline void prefetch_cluster(idx_t start_sbtv, idx_t start_sbwv, idx_t start_sbxv, idx_t start_sbyv, idx_t start_sbzv, idx_t stop_sbwv, idx_t stop_sbxv, idx_t stop_sbyv, idx_t stop_sbzv) {
 idx_t tv = start_sbtv;
 idx_t xv = start_sbxv;
 idx_t yv = start_sbyv;
 idx_t zv = start_sbzv;
 prefetch_cluster<level>(tv, xv, yv, zv);
} // prefetch_cluster shim.

 // Prefetches cache line(s) for leading edge of stencil advancing by 1 vector(s) in '+x' direction relative to indices t, x, y, z in a 'x=4 * y=1 * z=1' cluster containing 1 'x=4 * y=1 * z=1' vector(s).
 // Indices must be normalized, i.e., already divided by VLEN_*.
 template<int level> inline void prefetch_cluster_x(idx_t tv, idx_t xv, idx_t yv, idx_t zv) {

 // Aligned data at t, x, y-4, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv - (4 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y-3, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv - (3 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y-2, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv - (2 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y-1, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y, z-4.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z-3.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv - (3 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z-2.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv - (2 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z-1.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv - (1 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z+1.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (1 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z+2.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (2 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z+3.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (3 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z+4.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y+1, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y+2, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (2 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y+3, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (3 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y+4, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (4 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x+4, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv + (4 / VLEN_X), yv, zv, __LINE__);
} // prefetch_cluster_x.

 // Simple shim function to map sub-block start vars to simple vars (ignoring stop vars).
 template <int level> inline void prefetch_cluster_sbxv(idx_t start_sbtv, idx_t start_sbwv, idx_t start_sbxv, idx_t start_sbyv, idx_t start_sbzv, idx_t stop_sbwv, idx_t stop_sbxv, idx_t stop_sbyv, idx_t stop_sbzv) {
 idx_t tv = start_sbtv;
 idx_t xv = start_sbxv;
 idx_t yv = start_sbyv;
 idx_t zv = start_sbzv;
 prefetch_cluster_x<level>(tv, xv, yv, zv);
} // prefetch_cluster shim.

 // Prefetches cache line(s) for leading edge of stencil advancing by 1 vector(s) in '+y' direction relative to indices t, x, y, z in a 'x=4 * y=1 * z=1' cluster containing 1 'x=4 * y=1 * z=1' vector(s).
 // Indices must be normalized, i.e., already divided by VLEN_*.
 template<int level> inline void prefetch_cluster_y(idx_t tv, idx_t xv, idx_t yv, idx_t zv) {

 // Aligned data at t, x-4, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv - (4 / VLEN_X), yv, zv, __LINE__);

 // Aligned data at t, x, y, z-4.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z-3.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv - (3 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z-2.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv - (2 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z-1.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv - (1 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z+1.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (1 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z+2.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (2 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z+3.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (3 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y, z+4.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y+4, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (4 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x+4, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv + (4 / VLEN_X), yv, zv, __LINE__);
} // prefetch_cluster_y.

 // Simple shim function to map sub-block start vars to simple vars (ignoring stop vars).
 template <int level> inline void prefetch_cluster_sbyv(idx_t start_sbtv, idx_t start_sbwv, idx_t start_sbxv, idx_t start_sbyv, idx_t start_sbzv, idx_t stop_sbwv, idx_t stop_sbxv, idx_t stop_sbyv, idx_t stop_sbzv) {
 idx_t tv = start_sbtv;
 idx_t xv = start_sbxv;
 idx_t yv = start_sbyv;
 idx_t zv = start_sbzv;
 prefetch_cluster_y<level>(tv, xv, yv, zv);
} // prefetch_cluster shim.

 // Prefetches cache line(s) for leading edge of stencil advancing by 1 vector(s) in '+z' direction relative to indices t, x, y, z in a 'x=4 * y=1 * z=1' cluster containing 1 'x=4 * y=1 * z=1' vector(s).
 // Indices must be normalized, i.e., already divided by VLEN_*.
 template<int level> inline void prefetch_cluster_z(idx_t tv, idx_t xv, idx_t yv, idx_t zv) {

 // Aligned data at t, x-4, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv - (4 / VLEN_X), yv, zv, __LINE__);

 // Aligned data at t, x, y-4, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv - (4 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y-3, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv - (3 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y-2, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv - (2 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y-1, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y, z+4.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned data at t, x, y+1, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y+2, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (2 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y+3, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (3 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x, y+4, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv + (4 / VLEN_Y), zv, __LINE__);

 // Aligned data at t, x+4, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv + (4 / VLEN_X), yv, zv, __LINE__);
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
