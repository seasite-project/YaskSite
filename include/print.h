#ifndef yaskSite_PRINT_H
#define yaskSite_PRINT_H

#ifdef __cplusplus
#include <cstdio>
#include <cstring>
#else
#include <stdio.h>
#include <string.h>
#endif
#include "config.h"

#ifdef yaskSite_HAVE_MPI
    #include <mpi.h>
#endif

#ifdef yasSite_HAVE_OpenMP
    #include <omp.h>
#endif

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#define TICK               "\xE2\x9C\x93"

#define FILE_BASENAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)


/* taken from http://stackoverflow.com/a/11172679 */
/* expands to the first argument */
#define FIRST(...) FIRST_HELPER(__VA_ARGS__, throwaway)
#define FIRST_HELPER(first, ...) first

/*
 * if there's only one argument, expands to nothing.  if there is more
 * than one argument, expands to a comma followed by everything but
 * the first argument.  only supports up to 9 arguments but can be
 * trivially expanded.
 */
#define REST(...) REST_HELPER(NUM(__VA_ARGS__), __VA_ARGS__)
#define REST_HELPER(qty, ...) REST_HELPER2(qty, __VA_ARGS__)
#define REST_HELPER2(qty, ...) REST_HELPER_##qty(__VA_ARGS__)
#define REST_HELPER_ONE(first)
#define REST_HELPER_TWOORMORE(first, ...) , __VA_ARGS__
#define NUM(...) \
    SELECT_20TH(__VA_ARGS__, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE,\
            TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, TWOORMORE, ONE, throwaway)
#define SELECT_20TH(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, ...) a20



#ifdef yaskSite_HAVE_MPI

#define PRINT(type,color,...) {\
    int logmacrome;\
    int logmacroerr = MPI_Comm_rank(MPI_COMM_WORLD,&logmacrome);\
    if (logmacroerr != MPI_SUCCESS) {\
        logmacrome = -1;\
    }\
    if (logmacrome == yaskSite_PRINT_RANK || -1 == yaskSite_PRINT_RANK) {\
        fprintf(stderr, color "[yaskSite] PE%d " #type " at %s() <%s:%d>: " FIRST(__VA_ARGS__) ANSI_COLOR_RESET , logmacrome, __func__, FILE_BASENAME, __LINE__ REST(__VA_ARGS__)); \
        fflush(stderr);\
    }\
}\

#else

#define PRINT(type,color,...) {\
    fprintf(stderr, color "[yaskSite] " #type " at %s() <%s:%d>: " FIRST(__VA_ARGS__) ANSI_COLOR_RESET , __func__, FILE_BASENAME, __LINE__ REST(__VA_ARGS__));\
}\

#endif

#define TIME_PRINT(...) {if (yaskSite_VERBOSITY) { PRINT(TIMING,ANSI_COLOR_GREEN,__VA_ARGS__); }}

#define NEWLINE {\
    fprintf(stderr,"\n");\
    fflush(stderr);\
}

#ifdef yaskSite_PRINT_ONLYFIRST
    #define INFO_PRINT(...) {static int __printed = 0; if(!__printed && yaskSite_VERBOSITY) { PRINT(INFO,ANSI_COLOR_BLUE,__VA_ARGS__); NEWLINE;  __printed=1; }}
   #define WARNING_PRINT(...) {static int __printed = 0; if(!__printed && yaskSite_VERBOSITY) { PRINT(WARNING,ANSI_COLOR_YELLOW,__VA_ARGS__); NEWLINE; __printed=1; }}
    #define PERFWARNING_PRINT(...) {static int __printed = 0; if(!__printed && yaskSite_VERBOSITY) { PRINT(PERFWARNING,ANSI_COLOR_MAGENTA,__VA_ARGS__); NEWLINE; __printed=1; }}
    #define ERROR_PRINT(...) {static int __printed = 0; if(!__printed && yaskSite_VERBOSITY) { PRINT(ERROR,ANSI_COLOR_RED,__VA_ARGS__); NEWLINE; __printed=1; }}
#else
    #define INFO_PRINT(...) {if (yaskSite_VERBOSITY) { PRINT(INFO,ANSI_COLOR_BLUE,__VA_ARGS__); NEWLINE}}
    #define WARNING_PRINT(...) {if (yaskSite_VERBOSITY) { PRINT(WARNING,ANSI_COLOR_YELLOW,__VA_ARGS__); NEWLINE}}
    #define PERFWARNING_PRINT(...) {if (yaskSite_VERBOSITY) { PRINT(PERFWARNING,ANSI_COLOR_MAGENTA,__VA_ARGS__); NEWLINE}}
    #define ERROR_PRINT(...) {if (yaskSite_VERBOSITY) { PRINT(ERROR,ANSI_COLOR_RED,__VA_ARGS__); NEWLINE}}
#endif

#define PRINT_LOG(...)\
    if(yaskSite_VERBOSITY > 1)\
    {\
        printf(__VA_ARGS__);\
    }\


#if ((yaskSite_VERBOSITY<2) && defined(yaskSite_HAVE_OpenMP))

#define LOAD_PRINT_START(...)\
    if(yaskSite_VERBOSITY)\
    {\
        bool stop_anim = false;\
        _Pragma("omp parallel num_threads(2) shared(stop_anim)")\
        {\
            int tid = omp_get_thread_num();\
            if(tid!=0)\
            {\
                PRINT(INFO,ANSI_COLOR_BLUE, __VA_ARGS__);\
                fprintf(stderr,ANSI_COLOR_GREEN "  ");\
                fprintf(stderr,"─");\
                while(!stop_anim) {\
                    sleep(1);\
                    fprintf(stderr,"\b\\");\
                    fflush(stderr);\
                    sleep(1);\
                    fprintf(stderr,"\b|");\
                    sleep(1);\
                    fprintf(stderr,"\b/");\
                    sleep(1);\
                    fprintf(stderr,"\b─");\
                }\
                fprintf(stderr, "\b" TICK ANSI_COLOR_RESET);\
                NEWLINE \
            }\
            else\
            {\

#define LOAD_PRINT_END()\
                stop_anim = true;\
            }\
        }\
    }

#else

#define LOAD_PRINT_START(...)\
    INFO_PRINT(__VA_ARGS__);\

#define LOAD_PRINT_END()


#endif

#endif
