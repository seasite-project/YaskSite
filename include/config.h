#ifndef yaskSite_CONFIG_H
#define yaskSite_CONFIG_H

#define yaskSite_VERBOSITY 1
#define YASK_PATH "/home/hpc/unrz/unrz002h/yasksite/yask"
#define TARGET_ARCH "ivb"
#define IACA_PATH "/home/hpc/unrz/unrz002h/installkit_rem_bound_pre_lat/tools/yaskSite/tools/IACA/iaca-lin64/bin"
#define TOOL_DIR "/home/hpc/unrz/unrz002h/installkit_rem_bound_pre_lat/tools/yaskSite/tools"
#define SRC_DIR "/home/hpc/unrz/unrz002h/yasksite/src"
#define STENCIL_DIR "/home/hpc/unrz/unrz002h/tmp_YaskSite_rem_bound_pre_lat/stencils"
#define INC_DIR "/home/hpc/unrz/unrz002h/yasksite/include"
#define TEMP_DIR "/home/hpc/unrz/unrz002h/tmp_YaskSite_rem_bound_pre_lat"
#define yaskSite_PRINT_RANK 0
#define LD_PORTS "2:D;3:D"
#define L1_cache "32768; private"
#define L2_cache "262144; private"
#define L3_cache "26214400; shared"
#define MEM "67348721664; shared"
#define TH_P_SOCK 10
#define CL_SIZE 8

#define MODEL_BOUNDARY_EFFECT
#define MODEL_PREFETCH_EFFECT
#define MODEL_LATENCY_EFFECT
/* #undef MODEL_ASSOCIATIVITY_EFFECT */
#define MODEL_REMAINDER_EFFECT

/* #undef LOG_SYS_CALL */
#define yaskSite_HAVE_MPI
#define yaskSite_HAVE_OpenMP
/* #undef yaskSite_PRINT_ONLYFIRST */
#define CACHE_BUILD
/* #undef ENABLE_LOOPADAPT */

#endif
