// Automatically generated code; do not edit.

// Stencil:
#define YASK_STENCIL_NAME "heat3d"
#define YASK_STENCIL_IS_HEAT3D (1)
#define YASK_STENCIL_CONTEXT StencilContext_heat3d

// Dimensions:
#define USING_DIM_T (1)
#define USING_DIM_X (1)
#define USING_DIM_Y (1)
#define USING_DIM_Z (1)

// One vector fold: x=4 * y=1 * z=1
#define VLEN_X (4)
#define VLEN_Y (1)
#define VLEN_Z (1)
#define VLEN (4)
#define VLEN_FIRST_DIM_IS_UNIT_STRIDE (1)
#define USING_UNALIGNED_LOADS (0)

// Cluster multipliers of vector folds: x=1 * y=1 * z=1
#define CLEN_X (1)
#define CLEN_Y (1)
#define CLEN_Z (1)
#define CLEN (1)

// Settings from YASK Makefile
#define USE_INTRIN256
#define DEF_ARGS "-thread_divisor 1 -block_threads 2 -d 128 -b 64 -p 0 "
#define REAL_BYTES 8
#define LAYOUT_XYZ Layout_123
#define LAYOUT_TXYZ Layout_1234
#define LAYOUT_WXYZ Layout_1234
#define LAYOUT_TWXYZ Layout_23415
#define PFDL1 1
#define PFDL2 2
#define ARCH_IVB
#define USE_MPI
#define NO_STORE_INTRINSICS
