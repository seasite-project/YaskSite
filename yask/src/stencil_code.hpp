// Automatically generated code; do not edit.

////// YASK implementation of the 'heat3d_irk_3' stencil //////

/*
Identified stencil equation-groups:
 Equation group 'stencil_0' updates grid 'data' with dim = 4.
 Equation group 'stencil_0' reads grid 'F_3' with dim = 4.
 Equation group 'stencil_0' reads grid 'F_2' with dim = 4.
 Equation group 'stencil_0' reads grid 'F_1' with dim = 4.
 Equation group 'stencil_0' reads grid 'data' with dim = 4.
 Equation group 'stencil_0' reads grid 'h' with dim = 0.
 Equation group 'stencil_0' reads grid 'h_inv' with dim = 0.
 Equation group 'stencil_0' reads grid 'F_0' with dim = 4.
*/

namespace yask {

 ////// Stencil-specific data //////
struct StencilContext_heat3d_irk_3_data : public StencilContext {

 ///// Grid(s).

 // The 4D 'data' grid, which is updated by one or more equations.
 static const idx_t data_alloc_t = 1; // total allocation required in 't' dimension.
 Grid_TXYZ<data_alloc_t>* data;
 const idx_t data_halo_x = 1; // halo allocation required in 'x' dimension.
 const idx_t data_halo_y = 1; // halo allocation required in 'y' dimension.
 const idx_t data_halo_z = 1; // halo allocation required in 'z' dimension.

 // The 4D 'F_0' grid, which is not updated by any equation (read-only).
 static const idx_t F_0_alloc_t = 1; // total allocation required in 't' dimension.
 Grid_TXYZ<F_0_alloc_t>* F_0;
 const idx_t F_0_halo_x = 1; // halo allocation required in 'x' dimension.
 const idx_t F_0_halo_y = 1; // halo allocation required in 'y' dimension.
 const idx_t F_0_halo_z = 1; // halo allocation required in 'z' dimension.

 // The 4D 'F_1' grid, which is not updated by any equation (read-only).
 static const idx_t F_1_alloc_t = 1; // total allocation required in 't' dimension.
 Grid_TXYZ<F_1_alloc_t>* F_1;
 const idx_t F_1_halo_x = 1; // halo allocation required in 'x' dimension.
 const idx_t F_1_halo_y = 1; // halo allocation required in 'y' dimension.
 const idx_t F_1_halo_z = 1; // halo allocation required in 'z' dimension.

 // The 4D 'F_2' grid, which is not updated by any equation (read-only).
 static const idx_t F_2_alloc_t = 1; // total allocation required in 't' dimension.
 Grid_TXYZ<F_2_alloc_t>* F_2;
 const idx_t F_2_halo_x = 1; // halo allocation required in 'x' dimension.
 const idx_t F_2_halo_y = 1; // halo allocation required in 'y' dimension.
 const idx_t F_2_halo_z = 1; // halo allocation required in 'z' dimension.

 // The 4D 'F_3' grid, which is not updated by any equation (read-only).
 static const idx_t F_3_alloc_t = 1; // total allocation required in 't' dimension.
 Grid_TXYZ<F_3_alloc_t>* F_3;
 const idx_t F_3_halo_x = 1; // halo allocation required in 'x' dimension.
 const idx_t F_3_halo_y = 1; // halo allocation required in 'y' dimension.
 const idx_t F_3_halo_z = 1; // halo allocation required in 'z' dimension.

 // Max halos across all grids.
 const idx_t max_halo_x = 1;
 const idx_t max_halo_y = 1;
 const idx_t max_halo_z = 1;

 ///// Parameter(s).

 // The 0D 'h_inv' parameter.
 GenericGrid0d<real_t>* h_inv;

 // The 0D 'h' parameter.
 GenericGrid0d<real_t>* h;

 // Constructor.
 StencilContext_heat3d_irk_3_data(StencilSettings& settings) : StencilContext(settings) {
  name = "heat3d_irk_3";

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

  // Init grid 'F_0'.
 F_0 = new Grid_TXYZ<F_0_alloc_t>("F_0");
 gridPtrs.push_back(F_0);
 gridMap["F_0"] = F_0;
 F_0->set_halo_x(F_0_halo_x);
 F_0->set_halo_y(F_0_halo_y);
 F_0->set_halo_z(F_0_halo_z);

  // Init grid 'F_1'.
 F_1 = new Grid_TXYZ<F_1_alloc_t>("F_1");
 gridPtrs.push_back(F_1);
 gridMap["F_1"] = F_1;
 F_1->set_halo_x(F_1_halo_x);
 F_1->set_halo_y(F_1_halo_y);
 F_1->set_halo_z(F_1_halo_z);

  // Init grid 'F_2'.
 F_2 = new Grid_TXYZ<F_2_alloc_t>("F_2");
 gridPtrs.push_back(F_2);
 gridMap["F_2"] = F_2;
 F_2->set_halo_x(F_2_halo_x);
 F_2->set_halo_y(F_2_halo_y);
 F_2->set_halo_z(F_2_halo_z);

  // Init grid 'F_3'.
 F_3 = new Grid_TXYZ<F_3_alloc_t>("F_3");
 gridPtrs.push_back(F_3);
 gridMap["F_3"] = F_3;
 F_3->set_halo_x(F_3_halo_x);
 F_3->set_halo_y(F_3_halo_y);
 F_3->set_halo_z(F_3_halo_z);

  // Init parameter 'h_inv'.
 h_inv = new GenericGrid0d<real_t>();
 paramPtrs.push_back(h_inv);
 paramMap["h_inv"] = h_inv;

  // Init parameter 'h'.
 h = new GenericGrid0d<real_t>();
 paramPtrs.push_back(h);
 paramMap["h"] = h;

  // Rounded halo sizes.
  hx = ROUND_UP(max_halo_x, VLEN_X);
  hy = ROUND_UP(max_halo_y, VLEN_Y);
  hz = ROUND_UP(max_halo_z, VLEN_Z);
 }
}; // StencilContext_heat3d_irk_3_data

 ////// Stencil equation-group 'stencil_0' w/no condition //////

 class EqGroup_stencil_0 : public EqGroupBase {
 protected:
 StencilContext_heat3d_irk_3_data* _context = 0;
 public:

 // 65 FP operation(s) per point:
 // data(t+1, x, y, z) EQUALS (data(t, x, y, z) + (h * ((1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))) * h_inv) + (2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))) * h_inv) + (3 * ((1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z))) * h_inv) + (4 * ((1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)) + (-6 * F_3(t, x, y, z))) * h_inv)))).
 EqGroup_stencil_0(StencilContext_heat3d_irk_3_data* context) :
 EqGroupBase(context),
 _context(context) {
 _name = "stencil_0";
 _scalar_fp_ops = 65;
 _scalar_points_read = 29;
 _scalar_points_written = 1;

 // The following grids are written by EqGroup_stencil_0
  outputGridPtrs.push_back(_context->data);

 // The following grids are read by EqGroup_stencil_0
  inputGridPtrs.push_back(_context->F_3);
  inputGridPtrs.push_back(_context->F_2);
  inputGridPtrs.push_back(_context->F_1);
  inputGridPtrs.push_back(_context->data);
  inputGridPtrs.push_back(_context->F_0);
 } // Ctor.

 // Determine whether EqGroup_stencil_0 is valid at the given indices. Return true if indices are within the valid sub-domain or false otherwise.
 virtual bool is_in_valid_domain(idx_t t, idx_t x, idx_t y, idx_t z) {
 return (x>=FIRST_INDEX(x)) && (x<=LAST_INDEX(x)) && (y>=FIRST_INDEX(y)) && (y<=LAST_INDEX(y)) && (z>=FIRST_INDEX(z)) && (z<=LAST_INDEX(z)); // full domain.
 }

 // Calculate one scalar result relative to indices t, x, y, z.
 virtual void calc_scalar(idx_t t, idx_t x, idx_t y, idx_t z) {

 // temp1 = 1 * F_0(t, x+1, y, z).
 real_t temp1 = 1.00000000000000000e+00 * _context->F_0->readElem(t, x+1, y, z, __LINE__);

 // temp2 = 1 * F_0(t, x-1, y, z).
 real_t temp2 = 1.00000000000000000e+00 * _context->F_0->readElem(t, x-1, y, z, __LINE__);

 // temp3 = (1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)).
 real_t temp3 = temp1 + temp2;

 // temp4 = 1 * F_0(t, x, y+1, z).
 real_t temp4 = 1.00000000000000000e+00 * _context->F_0->readElem(t, x, y+1, z, __LINE__);

 // temp5 = (1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)).
 real_t temp5 = temp3 + temp4;

 // temp6 = 1 * F_0(t, x, y-1, z).
 real_t temp6 = 1.00000000000000000e+00 * _context->F_0->readElem(t, x, y-1, z, __LINE__);

 // temp7 = (1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)).
 real_t temp7 = temp5 + temp6;

 // temp8 = 1 * F_0(t, x, y, z+1).
 real_t temp8 = 1.00000000000000000e+00 * _context->F_0->readElem(t, x, y, z+1, __LINE__);

 // temp9 = (1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)).
 real_t temp9 = temp7 + temp8;

 // temp10 = 1 * F_0(t, x, y, z-1).
 real_t temp10 = 1.00000000000000000e+00 * _context->F_0->readElem(t, x, y, z-1, __LINE__);

 // temp11 = (1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)).
 real_t temp11 = temp9 + temp10;

 // temp12 = -6 * F_0(t, x, y, z).
 real_t temp12 = -6.00000000000000000e+00 * _context->F_0->readElem(t, x, y, z, __LINE__);

 // temp13 = (1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z)).
 real_t temp13 = temp11 + temp12;

 // temp14 = 1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))).
 real_t temp14 = 1.00000000000000000e+00 * temp13;

 // temp15 = 1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))) * h_inv().
 real_t temp15 = temp14 * (*_context->h_inv)();

 // temp16 = 1 * F_1(t, x+1, y, z).
 real_t temp16 = 1.00000000000000000e+00 * _context->F_1->readElem(t, x+1, y, z, __LINE__);

 // temp17 = 1 * F_1(t, x-1, y, z).
 real_t temp17 = 1.00000000000000000e+00 * _context->F_1->readElem(t, x-1, y, z, __LINE__);

 // temp18 = (1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)).
 real_t temp18 = temp16 + temp17;

 // temp19 = 1 * F_1(t, x, y+1, z).
 real_t temp19 = 1.00000000000000000e+00 * _context->F_1->readElem(t, x, y+1, z, __LINE__);

 // temp20 = (1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)).
 real_t temp20 = temp18 + temp19;

 // temp21 = 1 * F_1(t, x, y-1, z).
 real_t temp21 = 1.00000000000000000e+00 * _context->F_1->readElem(t, x, y-1, z, __LINE__);

 // temp22 = (1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)).
 real_t temp22 = temp20 + temp21;

 // temp23 = 1 * F_1(t, x, y, z+1).
 real_t temp23 = 1.00000000000000000e+00 * _context->F_1->readElem(t, x, y, z+1, __LINE__);

 // temp24 = (1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)).
 real_t temp24 = temp22 + temp23;

 // temp25 = 1 * F_1(t, x, y, z-1).
 real_t temp25 = 1.00000000000000000e+00 * _context->F_1->readElem(t, x, y, z-1, __LINE__);

 // temp26 = (1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)).
 real_t temp26 = temp24 + temp25;

 // temp27 = -6 * F_1(t, x, y, z).
 real_t temp27 = -6.00000000000000000e+00 * _context->F_1->readElem(t, x, y, z, __LINE__);

 // temp28 = (1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z)).
 real_t temp28 = temp26 + temp27;

 // temp29 = 2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))).
 real_t temp29 = 2.00000000000000000e+00 * temp28;

 // temp30 = 2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))) * h_inv().
 real_t temp30 = temp29 * (*_context->h_inv)();

 // temp31 = (1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))) * h_inv) + (2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))) * h_inv).
 real_t temp31 = temp15 + temp30;

 // temp32 = 1 * F_2(t, x+1, y, z).
 real_t temp32 = 1.00000000000000000e+00 * _context->F_2->readElem(t, x+1, y, z, __LINE__);

 // temp33 = 1 * F_2(t, x-1, y, z).
 real_t temp33 = 1.00000000000000000e+00 * _context->F_2->readElem(t, x-1, y, z, __LINE__);

 // temp34 = (1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)).
 real_t temp34 = temp32 + temp33;

 // temp35 = 1 * F_2(t, x, y+1, z).
 real_t temp35 = 1.00000000000000000e+00 * _context->F_2->readElem(t, x, y+1, z, __LINE__);

 // temp36 = (1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)).
 real_t temp36 = temp34 + temp35;

 // temp37 = 1 * F_2(t, x, y-1, z).
 real_t temp37 = 1.00000000000000000e+00 * _context->F_2->readElem(t, x, y-1, z, __LINE__);

 // temp38 = (1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)).
 real_t temp38 = temp36 + temp37;

 // temp39 = 1 * F_2(t, x, y, z+1).
 real_t temp39 = 1.00000000000000000e+00 * _context->F_2->readElem(t, x, y, z+1, __LINE__);

 // temp40 = (1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)).
 real_t temp40 = temp38 + temp39;

 // temp41 = 1 * F_2(t, x, y, z-1).
 real_t temp41 = 1.00000000000000000e+00 * _context->F_2->readElem(t, x, y, z-1, __LINE__);

 // temp42 = (1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)).
 real_t temp42 = temp40 + temp41;

 // temp43 = -6 * F_2(t, x, y, z).
 real_t temp43 = -6.00000000000000000e+00 * _context->F_2->readElem(t, x, y, z, __LINE__);

 // temp44 = (1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z)).
 real_t temp44 = temp42 + temp43;

 // temp45 = 3 * ((1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z))).
 real_t temp45 = 3.00000000000000000e+00 * temp44;

 // temp46 = 3 * ((1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z))) * h_inv().
 real_t temp46 = temp45 * (*_context->h_inv)();

 // temp47 = (1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))) * h_inv) + (2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))) * h_inv) + (3 * ((1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z))) * h_inv).
 real_t temp47 = temp31 + temp46;

 // temp48 = 1 * F_3(t, x+1, y, z).
 real_t temp48 = 1.00000000000000000e+00 * _context->F_3->readElem(t, x+1, y, z, __LINE__);

 // temp49 = 1 * F_3(t, x-1, y, z).
 real_t temp49 = 1.00000000000000000e+00 * _context->F_3->readElem(t, x-1, y, z, __LINE__);

 // temp50 = (1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)).
 real_t temp50 = temp48 + temp49;

 // temp51 = 1 * F_3(t, x, y+1, z).
 real_t temp51 = 1.00000000000000000e+00 * _context->F_3->readElem(t, x, y+1, z, __LINE__);

 // temp52 = (1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)).
 real_t temp52 = temp50 + temp51;

 // temp53 = 1 * F_3(t, x, y-1, z).
 real_t temp53 = 1.00000000000000000e+00 * _context->F_3->readElem(t, x, y-1, z, __LINE__);

 // temp54 = (1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)).
 real_t temp54 = temp52 + temp53;

 // temp55 = 1 * F_3(t, x, y, z+1).
 real_t temp55 = 1.00000000000000000e+00 * _context->F_3->readElem(t, x, y, z+1, __LINE__);

 // temp56 = (1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)).
 real_t temp56 = temp54 + temp55;

 // temp57 = 1 * F_3(t, x, y, z-1).
 real_t temp57 = 1.00000000000000000e+00 * _context->F_3->readElem(t, x, y, z-1, __LINE__);

 // temp58 = (1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)).
 real_t temp58 = temp56 + temp57;

 // temp59 = -6 * F_3(t, x, y, z).
 real_t temp59 = -6.00000000000000000e+00 * _context->F_3->readElem(t, x, y, z, __LINE__);

 // temp60 = (1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)) + (-6 * F_3(t, x, y, z)).
 real_t temp60 = temp58 + temp59;

 // temp61 = 4 * ((1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)) + (-6 * F_3(t, x, y, z))).
 real_t temp61 = 4.00000000000000000e+00 * temp60;

 // temp62 = 4 * ((1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)) + (-6 * F_3(t, x, y, z))) * h_inv().
 real_t temp62 = temp61 * (*_context->h_inv)();

 // temp63 = (1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))) * h_inv) + (2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))) * h_inv) + (3 * ((1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z))) * h_inv) + (4 * ((1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)) + (-6 * F_3(t, x, y, z))) * h_inv).
 real_t temp63 = temp47 + temp62;

 // temp64 = h() * ((1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))) * h_inv) + (2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))) * h_inv) + (3 * ((1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z))) * h_inv) + (4 * ((1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)) + (-6 * F_3(t, x, y, z))) * h_inv)).
 real_t temp64 = (*_context->h)() * temp63;

 // temp65 = data(t, x, y, z) + (h * ((1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))) * h_inv) + (2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))) * h_inv) + (3 * ((1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z))) * h_inv) + (4 * ((1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)) + (-6 * F_3(t, x, y, z))) * h_inv))).
 real_t temp65 = _context->data->readElem(t, x, y, z, __LINE__) + temp64;

 // temp66 = (data(t, x, y, z) + (h * ((1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))) * h_inv) + (2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))) * h_inv) + (3 * ((1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z))) * h_inv) + (4 * ((1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)) + (-6 * F_3(t, x, y, z))) * h_inv)))).
 real_t temp66 = temp65;

 // Define value at data(t+1, x, y, z).
 _context->data->writeElem(temp66, t+1, x, y, z, __LINE__);
} // calc_scalar.

 // Calculate 4 result(s) relative to indices t, x, y, z in a 'x=1 * y=1 * z=4' cluster containing 1 'x=1 * y=1 * z=4' vector(s).
 // Indices must be normalized, i.e., already divided by VLEN_*.
 // SIMD calculations use 29 vector block(s) created from 29 aligned vector-block(s).
 // There are approximately 260 FP operation(s) per invocation.
 inline void calc_cluster(idx_t tv, idx_t xv, idx_t yv, idx_t zv) {

 // Element (un-normalized) indices.
 idx_t t = tv;
 idx_t x = xv * VLEN_X;
 idx_t y = yv * VLEN_Y;
 idx_t z = zv * VLEN_Z;

 // Read aligned vector block from data at t, x, y, z.
 real_vec_t temp_vec1 =  _context->data->readVecNorm(tv, xv, yv, zv, __LINE__);

 // Read aligned vector block from F_0 at t, x+1, y, z.
 real_vec_t temp_vec2 =  _context->F_0->readVecNorm(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Read aligned vector block from F_0 at t, x-1, y, z.
 real_vec_t temp_vec3 =  _context->F_0->readVecNorm(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Read aligned vector block from F_0 at t, x, y+1, z.
 real_vec_t temp_vec4 =  _context->F_0->readVecNorm(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Read aligned vector block from F_0 at t, x, y-1, z.
 real_vec_t temp_vec5 =  _context->F_0->readVecNorm(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Read aligned vector block from F_0 at t, x, y, z.
 real_vec_t temp_vec6 =  _context->F_0->readVecNorm(tv, xv, yv, zv, __LINE__);

 // Read aligned vector block from F_0 at t, x, y, z+4.
 real_vec_t temp_vec7 =  _context->F_0->readVecNorm(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Construct unaligned vector block from F_0 at t, x, y, z+1.
 real_vec_t temp_vec8;
 // temp_vec8[0] = temp_vec6[1];  // for t, x, y, z+1;
 // temp_vec8[1] = temp_vec6[2];  // for t, x, y, z+2;
 // temp_vec8[2] = temp_vec6[3];  // for t, x, y, z+3;
 // temp_vec8[3] = temp_vec7[0];  // for t, x, y, z+4;
 // Get 1 element(s) from temp_vec7 and 3 from temp_vec6.
 real_vec_align<1>(temp_vec8, temp_vec7, temp_vec6);

 // Read aligned vector block from F_0 at t, x, y, z-4.
 real_vec_t temp_vec9 =  _context->F_0->readVecNorm(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Construct unaligned vector block from F_0 at t, x, y, z-1.
 real_vec_t temp_vec10;
 // temp_vec10[0] = temp_vec9[3];  // for t, x, y, z-1;
 // temp_vec10[1] = temp_vec6[0];  // for t, x, y, z;
 // temp_vec10[2] = temp_vec6[1];  // for t, x, y, z+1;
 // temp_vec10[3] = temp_vec6[2];  // for t, x, y, z+2;
 // Get 3 element(s) from temp_vec6 and 1 from temp_vec9.
 real_vec_align<3>(temp_vec10, temp_vec6, temp_vec9);

 // temp_vec11 = 1 * F_0(t, x+1, y, z).
 real_vec_t temp_vec11 = 1.00000000000000000e+00 * temp_vec2;

 // temp_vec12 = 1 * F_0(t, x-1, y, z).
 real_vec_t temp_vec12 = 1.00000000000000000e+00 * temp_vec3;

 // temp_vec13 = (1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)).
 real_vec_t temp_vec13 = temp_vec11 + temp_vec12;

 // temp_vec14 = 1 * F_0(t, x, y+1, z).
 real_vec_t temp_vec14 = 1.00000000000000000e+00 * temp_vec4;

 // temp_vec15 = (1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)).
 real_vec_t temp_vec15 = temp_vec13 + temp_vec14;

 // temp_vec16 = 1 * F_0(t, x, y-1, z).
 real_vec_t temp_vec16 = 1.00000000000000000e+00 * temp_vec5;

 // temp_vec17 = (1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)).
 real_vec_t temp_vec17 = temp_vec15 + temp_vec16;

 // temp_vec18 = 1 * F_0(t, x, y, z+1).
 real_vec_t temp_vec18 = 1.00000000000000000e+00 * temp_vec8;

 // temp_vec19 = (1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)).
 real_vec_t temp_vec19 = temp_vec17 + temp_vec18;

 // temp_vec20 = 1 * F_0(t, x, y, z-1).
 real_vec_t temp_vec20 = 1.00000000000000000e+00 * temp_vec10;

 // temp_vec21 = (1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)).
 real_vec_t temp_vec21 = temp_vec19 + temp_vec20;

 // temp_vec22 = -6 * F_0(t, x, y, z).
 real_vec_t temp_vec22 = -6.00000000000000000e+00 * temp_vec6;

 // temp_vec23 = (1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z)).
 real_vec_t temp_vec23 = temp_vec21 + temp_vec22;

 // temp_vec24 = 1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))).
 real_vec_t temp_vec24 = 1.00000000000000000e+00 * temp_vec23;

 // temp_vec25 = 1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))) * h_inv().
 real_vec_t temp_vec25 = temp_vec24 * (*_context->h_inv)();

 // Read aligned vector block from F_1 at t, x+1, y, z.
 real_vec_t temp_vec26 =  _context->F_1->readVecNorm(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Read aligned vector block from F_1 at t, x-1, y, z.
 real_vec_t temp_vec27 =  _context->F_1->readVecNorm(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Read aligned vector block from F_1 at t, x, y+1, z.
 real_vec_t temp_vec28 =  _context->F_1->readVecNorm(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Read aligned vector block from F_1 at t, x, y-1, z.
 real_vec_t temp_vec29 =  _context->F_1->readVecNorm(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Read aligned vector block from F_1 at t, x, y, z.
 real_vec_t temp_vec30 =  _context->F_1->readVecNorm(tv, xv, yv, zv, __LINE__);

 // Read aligned vector block from F_1 at t, x, y, z+4.
 real_vec_t temp_vec31 =  _context->F_1->readVecNorm(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Construct unaligned vector block from F_1 at t, x, y, z+1.
 real_vec_t temp_vec32;
 // temp_vec32[0] = temp_vec30[1];  // for t, x, y, z+1;
 // temp_vec32[1] = temp_vec30[2];  // for t, x, y, z+2;
 // temp_vec32[2] = temp_vec30[3];  // for t, x, y, z+3;
 // temp_vec32[3] = temp_vec31[0];  // for t, x, y, z+4;
 // Get 1 element(s) from temp_vec31 and 3 from temp_vec30.
 real_vec_align<1>(temp_vec32, temp_vec31, temp_vec30);

 // Read aligned vector block from F_1 at t, x, y, z-4.
 real_vec_t temp_vec33 =  _context->F_1->readVecNorm(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Construct unaligned vector block from F_1 at t, x, y, z-1.
 real_vec_t temp_vec34;
 // temp_vec34[0] = temp_vec33[3];  // for t, x, y, z-1;
 // temp_vec34[1] = temp_vec30[0];  // for t, x, y, z;
 // temp_vec34[2] = temp_vec30[1];  // for t, x, y, z+1;
 // temp_vec34[3] = temp_vec30[2];  // for t, x, y, z+2;
 // Get 3 element(s) from temp_vec30 and 1 from temp_vec33.
 real_vec_align<3>(temp_vec34, temp_vec30, temp_vec33);

 // temp_vec35 = 1 * F_1(t, x+1, y, z).
 real_vec_t temp_vec35 = 1.00000000000000000e+00 * temp_vec26;

 // temp_vec36 = 1 * F_1(t, x-1, y, z).
 real_vec_t temp_vec36 = 1.00000000000000000e+00 * temp_vec27;

 // temp_vec37 = (1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)).
 real_vec_t temp_vec37 = temp_vec35 + temp_vec36;

 // temp_vec38 = 1 * F_1(t, x, y+1, z).
 real_vec_t temp_vec38 = 1.00000000000000000e+00 * temp_vec28;

 // temp_vec39 = (1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)).
 real_vec_t temp_vec39 = temp_vec37 + temp_vec38;

 // temp_vec40 = 1 * F_1(t, x, y-1, z).
 real_vec_t temp_vec40 = 1.00000000000000000e+00 * temp_vec29;

 // temp_vec41 = (1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)).
 real_vec_t temp_vec41 = temp_vec39 + temp_vec40;

 // temp_vec42 = 1 * F_1(t, x, y, z+1).
 real_vec_t temp_vec42 = 1.00000000000000000e+00 * temp_vec32;

 // temp_vec43 = (1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)).
 real_vec_t temp_vec43 = temp_vec41 + temp_vec42;

 // temp_vec44 = 1 * F_1(t, x, y, z-1).
 real_vec_t temp_vec44 = 1.00000000000000000e+00 * temp_vec34;

 // temp_vec45 = (1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)).
 real_vec_t temp_vec45 = temp_vec43 + temp_vec44;

 // temp_vec46 = -6 * F_1(t, x, y, z).
 real_vec_t temp_vec46 = -6.00000000000000000e+00 * temp_vec30;

 // temp_vec47 = (1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z)).
 real_vec_t temp_vec47 = temp_vec45 + temp_vec46;

 // temp_vec48 = 2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))).
 real_vec_t temp_vec48 = 2.00000000000000000e+00 * temp_vec47;

 // temp_vec49 = 2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))) * h_inv().
 real_vec_t temp_vec49 = temp_vec48 * (*_context->h_inv)();

 // temp_vec50 = (1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))) * h_inv) + (2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))) * h_inv).
 real_vec_t temp_vec50 = temp_vec25 + temp_vec49;

 // Read aligned vector block from F_2 at t, x+1, y, z.
 real_vec_t temp_vec51 =  _context->F_2->readVecNorm(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Read aligned vector block from F_2 at t, x-1, y, z.
 real_vec_t temp_vec52 =  _context->F_2->readVecNorm(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Read aligned vector block from F_2 at t, x, y+1, z.
 real_vec_t temp_vec53 =  _context->F_2->readVecNorm(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Read aligned vector block from F_2 at t, x, y-1, z.
 real_vec_t temp_vec54 =  _context->F_2->readVecNorm(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Read aligned vector block from F_2 at t, x, y, z.
 real_vec_t temp_vec55 =  _context->F_2->readVecNorm(tv, xv, yv, zv, __LINE__);

 // Read aligned vector block from F_2 at t, x, y, z+4.
 real_vec_t temp_vec56 =  _context->F_2->readVecNorm(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Construct unaligned vector block from F_2 at t, x, y, z+1.
 real_vec_t temp_vec57;
 // temp_vec57[0] = temp_vec55[1];  // for t, x, y, z+1;
 // temp_vec57[1] = temp_vec55[2];  // for t, x, y, z+2;
 // temp_vec57[2] = temp_vec55[3];  // for t, x, y, z+3;
 // temp_vec57[3] = temp_vec56[0];  // for t, x, y, z+4;
 // Get 1 element(s) from temp_vec56 and 3 from temp_vec55.
 real_vec_align<1>(temp_vec57, temp_vec56, temp_vec55);

 // Read aligned vector block from F_2 at t, x, y, z-4.
 real_vec_t temp_vec58 =  _context->F_2->readVecNorm(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Construct unaligned vector block from F_2 at t, x, y, z-1.
 real_vec_t temp_vec59;
 // temp_vec59[0] = temp_vec58[3];  // for t, x, y, z-1;
 // temp_vec59[1] = temp_vec55[0];  // for t, x, y, z;
 // temp_vec59[2] = temp_vec55[1];  // for t, x, y, z+1;
 // temp_vec59[3] = temp_vec55[2];  // for t, x, y, z+2;
 // Get 3 element(s) from temp_vec55 and 1 from temp_vec58.
 real_vec_align<3>(temp_vec59, temp_vec55, temp_vec58);

 // temp_vec60 = 1 * F_2(t, x+1, y, z).
 real_vec_t temp_vec60 = 1.00000000000000000e+00 * temp_vec51;

 // temp_vec61 = 1 * F_2(t, x-1, y, z).
 real_vec_t temp_vec61 = 1.00000000000000000e+00 * temp_vec52;

 // temp_vec62 = (1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)).
 real_vec_t temp_vec62 = temp_vec60 + temp_vec61;

 // temp_vec63 = 1 * F_2(t, x, y+1, z).
 real_vec_t temp_vec63 = 1.00000000000000000e+00 * temp_vec53;

 // temp_vec64 = (1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)).
 real_vec_t temp_vec64 = temp_vec62 + temp_vec63;

 // temp_vec65 = 1 * F_2(t, x, y-1, z).
 real_vec_t temp_vec65 = 1.00000000000000000e+00 * temp_vec54;

 // temp_vec66 = (1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)).
 real_vec_t temp_vec66 = temp_vec64 + temp_vec65;

 // temp_vec67 = 1 * F_2(t, x, y, z+1).
 real_vec_t temp_vec67 = 1.00000000000000000e+00 * temp_vec57;

 // temp_vec68 = (1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)).
 real_vec_t temp_vec68 = temp_vec66 + temp_vec67;

 // temp_vec69 = 1 * F_2(t, x, y, z-1).
 real_vec_t temp_vec69 = 1.00000000000000000e+00 * temp_vec59;

 // temp_vec70 = (1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)).
 real_vec_t temp_vec70 = temp_vec68 + temp_vec69;

 // temp_vec71 = -6 * F_2(t, x, y, z).
 real_vec_t temp_vec71 = -6.00000000000000000e+00 * temp_vec55;

 // temp_vec72 = (1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z)).
 real_vec_t temp_vec72 = temp_vec70 + temp_vec71;

 // temp_vec73 = 3 * ((1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z))).
 real_vec_t temp_vec73 = 3.00000000000000000e+00 * temp_vec72;

 // temp_vec74 = 3 * ((1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z))) * h_inv().
 real_vec_t temp_vec74 = temp_vec73 * (*_context->h_inv)();

 // temp_vec75 = (1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))) * h_inv) + (2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))) * h_inv) + (3 * ((1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z))) * h_inv).
 real_vec_t temp_vec75 = temp_vec50 + temp_vec74;

 // Read aligned vector block from F_3 at t, x+1, y, z.
 real_vec_t temp_vec76 =  _context->F_3->readVecNorm(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Read aligned vector block from F_3 at t, x-1, y, z.
 real_vec_t temp_vec77 =  _context->F_3->readVecNorm(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Read aligned vector block from F_3 at t, x, y+1, z.
 real_vec_t temp_vec78 =  _context->F_3->readVecNorm(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Read aligned vector block from F_3 at t, x, y-1, z.
 real_vec_t temp_vec79 =  _context->F_3->readVecNorm(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Read aligned vector block from F_3 at t, x, y, z.
 real_vec_t temp_vec80 =  _context->F_3->readVecNorm(tv, xv, yv, zv, __LINE__);

 // Read aligned vector block from F_3 at t, x, y, z+4.
 real_vec_t temp_vec81 =  _context->F_3->readVecNorm(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Construct unaligned vector block from F_3 at t, x, y, z+1.
 real_vec_t temp_vec82;
 // temp_vec82[0] = temp_vec80[1];  // for t, x, y, z+1;
 // temp_vec82[1] = temp_vec80[2];  // for t, x, y, z+2;
 // temp_vec82[2] = temp_vec80[3];  // for t, x, y, z+3;
 // temp_vec82[3] = temp_vec81[0];  // for t, x, y, z+4;
 // Get 1 element(s) from temp_vec81 and 3 from temp_vec80.
 real_vec_align<1>(temp_vec82, temp_vec81, temp_vec80);

 // Read aligned vector block from F_3 at t, x, y, z-4.
 real_vec_t temp_vec83 =  _context->F_3->readVecNorm(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Construct unaligned vector block from F_3 at t, x, y, z-1.
 real_vec_t temp_vec84;
 // temp_vec84[0] = temp_vec83[3];  // for t, x, y, z-1;
 // temp_vec84[1] = temp_vec80[0];  // for t, x, y, z;
 // temp_vec84[2] = temp_vec80[1];  // for t, x, y, z+1;
 // temp_vec84[3] = temp_vec80[2];  // for t, x, y, z+2;
 // Get 3 element(s) from temp_vec80 and 1 from temp_vec83.
 real_vec_align<3>(temp_vec84, temp_vec80, temp_vec83);

 // temp_vec85 = 1 * F_3(t, x+1, y, z).
 real_vec_t temp_vec85 = 1.00000000000000000e+00 * temp_vec76;

 // temp_vec86 = 1 * F_3(t, x-1, y, z).
 real_vec_t temp_vec86 = 1.00000000000000000e+00 * temp_vec77;

 // temp_vec87 = (1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)).
 real_vec_t temp_vec87 = temp_vec85 + temp_vec86;

 // temp_vec88 = 1 * F_3(t, x, y+1, z).
 real_vec_t temp_vec88 = 1.00000000000000000e+00 * temp_vec78;

 // temp_vec89 = (1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)).
 real_vec_t temp_vec89 = temp_vec87 + temp_vec88;

 // temp_vec90 = 1 * F_3(t, x, y-1, z).
 real_vec_t temp_vec90 = 1.00000000000000000e+00 * temp_vec79;

 // temp_vec91 = (1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)).
 real_vec_t temp_vec91 = temp_vec89 + temp_vec90;

 // temp_vec92 = 1 * F_3(t, x, y, z+1).
 real_vec_t temp_vec92 = 1.00000000000000000e+00 * temp_vec82;

 // temp_vec93 = (1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)).
 real_vec_t temp_vec93 = temp_vec91 + temp_vec92;

 // temp_vec94 = 1 * F_3(t, x, y, z-1).
 real_vec_t temp_vec94 = 1.00000000000000000e+00 * temp_vec84;

 // temp_vec95 = (1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)).
 real_vec_t temp_vec95 = temp_vec93 + temp_vec94;

 // temp_vec96 = -6 * F_3(t, x, y, z).
 real_vec_t temp_vec96 = -6.00000000000000000e+00 * temp_vec80;

 // temp_vec97 = (1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)) + (-6 * F_3(t, x, y, z)).
 real_vec_t temp_vec97 = temp_vec95 + temp_vec96;

 // temp_vec98 = 4 * ((1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)) + (-6 * F_3(t, x, y, z))).
 real_vec_t temp_vec98 = 4.00000000000000000e+00 * temp_vec97;

 // temp_vec99 = 4 * ((1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)) + (-6 * F_3(t, x, y, z))) * h_inv().
 real_vec_t temp_vec99 = temp_vec98 * (*_context->h_inv)();

 // temp_vec100 = (1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))) * h_inv) + (2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))) * h_inv) + (3 * ((1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z))) * h_inv) + (4 * ((1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)) + (-6 * F_3(t, x, y, z))) * h_inv).
 real_vec_t temp_vec100 = temp_vec75 + temp_vec99;

 // temp_vec101 = h() * ((1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))) * h_inv) + (2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))) * h_inv) + (3 * ((1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z))) * h_inv) + (4 * ((1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)) + (-6 * F_3(t, x, y, z))) * h_inv)).
 real_vec_t temp_vec101 = (*_context->h)() * temp_vec100;

 // temp_vec102 = data(t, x, y, z) + (h * ((1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))) * h_inv) + (2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))) * h_inv) + (3 * ((1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z))) * h_inv) + (4 * ((1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)) + (-6 * F_3(t, x, y, z))) * h_inv))).
 real_vec_t temp_vec102 = temp_vec1 + temp_vec101;

 // temp_vec103 = (data(t, x, y, z) + (h * ((1 * ((1 * F_0(t, x+1, y, z)) + (1 * F_0(t, x-1, y, z)) + (1 * F_0(t, x, y+1, z)) + (1 * F_0(t, x, y-1, z)) + (1 * F_0(t, x, y, z+1)) + (1 * F_0(t, x, y, z-1)) + (-6 * F_0(t, x, y, z))) * h_inv) + (2 * ((1 * F_1(t, x+1, y, z)) + (1 * F_1(t, x-1, y, z)) + (1 * F_1(t, x, y+1, z)) + (1 * F_1(t, x, y-1, z)) + (1 * F_1(t, x, y, z+1)) + (1 * F_1(t, x, y, z-1)) + (-6 * F_1(t, x, y, z))) * h_inv) + (3 * ((1 * F_2(t, x+1, y, z)) + (1 * F_2(t, x-1, y, z)) + (1 * F_2(t, x, y+1, z)) + (1 * F_2(t, x, y-1, z)) + (1 * F_2(t, x, y, z+1)) + (1 * F_2(t, x, y, z-1)) + (-6 * F_2(t, x, y, z))) * h_inv) + (4 * ((1 * F_3(t, x+1, y, z)) + (1 * F_3(t, x-1, y, z)) + (1 * F_3(t, x, y+1, z)) + (1 * F_3(t, x, y-1, z)) + (1 * F_3(t, x, y, z+1)) + (1 * F_3(t, x, y, z-1)) + (-6 * F_3(t, x, y, z))) * h_inv)))).
 real_vec_t temp_vec103 = temp_vec102;

 // Define value at data(t+1, x, y, z).

 // Write aligned vector block to data at t+1, x, y, z.
 _context->data->writeVecNorm(temp_vec103, tv+1, xv, yv, zv, __LINE__) ;
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

 // Aligned data at t, x, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv, __LINE__);

 // Aligned F_0 at t, x-1, y, z.
 _context->F_0->prefetchVecNorm<level>(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_0 at t, x, y-1, z.
 _context->F_0->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_0 at t, x, y, z-4.
 _context->F_0->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned F_0 at t, x, y, z.
 _context->F_0->prefetchVecNorm<level>(tv, xv, yv, zv, __LINE__);

 // Aligned F_0 at t, x, y, z+4.
 _context->F_0->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned F_0 at t, x, y+1, z.
 _context->F_0->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_0 at t, x+1, y, z.
 _context->F_0->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_1 at t, x-1, y, z.
 _context->F_1->prefetchVecNorm<level>(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_1 at t, x, y-1, z.
 _context->F_1->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_1 at t, x, y, z-4.
 _context->F_1->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned F_1 at t, x, y, z.
 _context->F_1->prefetchVecNorm<level>(tv, xv, yv, zv, __LINE__);

 // Aligned F_1 at t, x, y, z+4.
 _context->F_1->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned F_1 at t, x, y+1, z.
 _context->F_1->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_1 at t, x+1, y, z.
 _context->F_1->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_2 at t, x-1, y, z.
 _context->F_2->prefetchVecNorm<level>(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_2 at t, x, y-1, z.
 _context->F_2->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_2 at t, x, y, z-4.
 _context->F_2->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned F_2 at t, x, y, z.
 _context->F_2->prefetchVecNorm<level>(tv, xv, yv, zv, __LINE__);

 // Aligned F_2 at t, x, y, z+4.
 _context->F_2->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned F_2 at t, x, y+1, z.
 _context->F_2->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_2 at t, x+1, y, z.
 _context->F_2->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_3 at t, x-1, y, z.
 _context->F_3->prefetchVecNorm<level>(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_3 at t, x, y-1, z.
 _context->F_3->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_3 at t, x, y, z-4.
 _context->F_3->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned F_3 at t, x, y, z.
 _context->F_3->prefetchVecNorm<level>(tv, xv, yv, zv, __LINE__);

 // Aligned F_3 at t, x, y, z+4.
 _context->F_3->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned F_3 at t, x, y+1, z.
 _context->F_3->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_3 at t, x+1, y, z.
 _context->F_3->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);
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

 // Aligned data at t, x, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv, __LINE__);

 // Aligned F_0 at t, x, y-1, z.
 _context->F_0->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_0 at t, x, y, z-4.
 _context->F_0->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned F_0 at t, x, y, z+4.
 _context->F_0->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned F_0 at t, x, y+1, z.
 _context->F_0->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_0 at t, x+1, y, z.
 _context->F_0->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_1 at t, x, y-1, z.
 _context->F_1->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_1 at t, x, y, z-4.
 _context->F_1->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned F_1 at t, x, y, z+4.
 _context->F_1->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned F_1 at t, x, y+1, z.
 _context->F_1->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_1 at t, x+1, y, z.
 _context->F_1->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_2 at t, x, y-1, z.
 _context->F_2->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_2 at t, x, y, z-4.
 _context->F_2->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned F_2 at t, x, y, z+4.
 _context->F_2->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned F_2 at t, x, y+1, z.
 _context->F_2->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_2 at t, x+1, y, z.
 _context->F_2->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_3 at t, x, y-1, z.
 _context->F_3->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_3 at t, x, y, z-4.
 _context->F_3->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned F_3 at t, x, y, z+4.
 _context->F_3->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned F_3 at t, x, y+1, z.
 _context->F_3->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_3 at t, x+1, y, z.
 _context->F_3->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);
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

 // Aligned data at t, x, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv, __LINE__);

 // Aligned F_0 at t, x-1, y, z.
 _context->F_0->prefetchVecNorm<level>(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_0 at t, x, y, z-4.
 _context->F_0->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned F_0 at t, x, y, z+4.
 _context->F_0->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned F_0 at t, x, y+1, z.
 _context->F_0->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_0 at t, x+1, y, z.
 _context->F_0->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_1 at t, x-1, y, z.
 _context->F_1->prefetchVecNorm<level>(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_1 at t, x, y, z-4.
 _context->F_1->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned F_1 at t, x, y, z+4.
 _context->F_1->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned F_1 at t, x, y+1, z.
 _context->F_1->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_1 at t, x+1, y, z.
 _context->F_1->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_2 at t, x-1, y, z.
 _context->F_2->prefetchVecNorm<level>(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_2 at t, x, y, z-4.
 _context->F_2->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned F_2 at t, x, y, z+4.
 _context->F_2->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned F_2 at t, x, y+1, z.
 _context->F_2->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_2 at t, x+1, y, z.
 _context->F_2->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_3 at t, x-1, y, z.
 _context->F_3->prefetchVecNorm<level>(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_3 at t, x, y, z-4.
 _context->F_3->prefetchVecNorm<level>(tv, xv, yv, zv - (4 / VLEN_Z), __LINE__);

 // Aligned F_3 at t, x, y, z+4.
 _context->F_3->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned F_3 at t, x, y+1, z.
 _context->F_3->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_3 at t, x+1, y, z.
 _context->F_3->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);
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

 // Aligned data at t, x, y, z.
 _context->data->prefetchVecNorm<level>(tv, xv, yv, zv, __LINE__);

 // Aligned F_0 at t, x-1, y, z.
 _context->F_0->prefetchVecNorm<level>(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_0 at t, x, y-1, z.
 _context->F_0->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_0 at t, x, y, z+4.
 _context->F_0->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned F_0 at t, x, y+1, z.
 _context->F_0->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_0 at t, x+1, y, z.
 _context->F_0->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_1 at t, x-1, y, z.
 _context->F_1->prefetchVecNorm<level>(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_1 at t, x, y-1, z.
 _context->F_1->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_1 at t, x, y, z+4.
 _context->F_1->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned F_1 at t, x, y+1, z.
 _context->F_1->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_1 at t, x+1, y, z.
 _context->F_1->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_2 at t, x-1, y, z.
 _context->F_2->prefetchVecNorm<level>(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_2 at t, x, y-1, z.
 _context->F_2->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_2 at t, x, y, z+4.
 _context->F_2->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned F_2 at t, x, y+1, z.
 _context->F_2->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_2 at t, x+1, y, z.
 _context->F_2->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_3 at t, x-1, y, z.
 _context->F_3->prefetchVecNorm<level>(tv, xv - (1 / VLEN_X), yv, zv, __LINE__);

 // Aligned F_3 at t, x, y-1, z.
 _context->F_3->prefetchVecNorm<level>(tv, xv, yv - (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_3 at t, x, y, z+4.
 _context->F_3->prefetchVecNorm<level>(tv, xv, yv, zv + (4 / VLEN_Z), __LINE__);

 // Aligned F_3 at t, x, y+1, z.
 _context->F_3->prefetchVecNorm<level>(tv, xv, yv + (1 / VLEN_Y), zv, __LINE__);

 // Aligned F_3 at t, x+1, y, z.
 _context->F_3->prefetchVecNorm<level>(tv, xv + (1 / VLEN_X), yv, zv, __LINE__);
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
struct StencilContext_heat3d_irk_3 : public StencilContext_heat3d_irk_3_data {

 // Stencil equation-groups.
 EqGroup_stencil_0 eqGroup_stencil_0;

 // Constructor.
 StencilContext_heat3d_irk_3(StencilSettings& settings) : StencilContext_heat3d_irk_3_data(settings),
  eqGroup_stencil_0(this) {

 // Equation groups.
  eqGroups.push_back(&eqGroup_stencil_0);
 } // Ctor.
}; // StencilContext_heat3d_irk_3
} // namespace yask.
