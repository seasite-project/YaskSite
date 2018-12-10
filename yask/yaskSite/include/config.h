#ifndef yaskSite_CONFIG_H
#define yaskSite_CONFIG_H

#define yaskSite_VERBOSITY 1
#define YASK_PATH "/home/hpc/unrz/unrz002h/yask"
#define TARGET_ARCH "ivb"
#define TOOL_DIR "/home/hpc/unrz/unrz002h/yask/yaskSite/src/tools"
#define SRC_DIR "/home/hpc/unrz/unrz002h/yask/yaskSite/src"
#define INC_DIR "/home/hpc/unrz/unrz002h/yask/yaskSite/include"
#define yaskSite_PRINT_RANK 0
#define L1_cache "32768; private"
#define L2_cache "262144; private"
#define L3_cache "26214400; shared"

/* #undef LOG_SYS_CALL */
#define yaskSite_HAVE_MPI
#define yaskSite_HAVE_OpenMP
/* #undef yaskSite_PRINT_ONLYFIRST */
#define CACHE_BUILD
/* #undef ENABLE_LOOPADAPT */

#endif
