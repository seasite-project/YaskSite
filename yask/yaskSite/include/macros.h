#ifndef YASKSITE_MACRO
#define YASKSITE_MACRO

#define SYNC_WITH_DIM(z,y,x, name)\
    if(dim==1)\
{\
    if(x!=1)\
    {\
        WARNING_PRINT("Resetting %s_x from %d to 1, since dim = %d", name, x, dim);\
        x=1;\
    }\
    if(y!=1)\
    {\
        WARNING_PRINT("Resetting %s_y from %d to 1, since dim = %d", name, x, dim);\
        y=1;\
    }\
}\
if(dim==2)\
{\
    if(x!=1)\
    {\
        WARNING_PRINT("Resetting %s_x from %d to 1, since dim = %d", name, x, dim);\
        x=1;\
    }\
}

#if yaskSite_VERBOSITY<2
#define SYSTEM(...)\
{\
    char *__cmd__, *__pipedCmd__;\
    asprintf(&__cmd__, __VA_ARGS__);\
    char* nullPipe = " &> /dev/null 2>&1";\
    asprintf(&__pipedCmd__, "%s%s", __cmd__, nullPipe);\
    systemCall(__pipedCmd__);\
    free( __cmd__);\
    free(__pipedCmd__);\
}\

#else

#define SYSTEM(...)\
{\
    char* __cmd__;\
    asprintf(&__cmd__, __VA_ARGS__);\
    systemCall(__cmd__);\
    delete[] __cmd__;\
}\

#endif

#define STRINGIFY(__str__, ...)\
{\
    asprintf(&__str__, __VA_ARGS__);\
}

#define UNUSED(x) (void)(x)

#define EXTRACT_IDX(grid_idx)\
    int x_start = grid_idx[0];\
    int x_end   = grid_idx[1];\
    int y_start = grid_idx[2];\
    int y_end   = grid_idx[3];\
    int z_start = grid_idx[4];\
    int z_end   = grid_idx[5];\


#endif
