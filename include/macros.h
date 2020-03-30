#ifndef YASKSITE_MACRO
#define YASKSITE_MACRO

#include "util.h"
#include <stdio.h>

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

#define POPEN(sysLogFile, file, ...)\
{\
    char* __cmd__;\
    asprintf(&__cmd__, __VA_ARGS__);\
    file = popenCallUtil(__cmd__, "r", sysLogFile);\
    delete[] __cmd__;\
}\

#define PCLOSE(file)\
{\
    pclose(file);\
}\


//#if yaskSite_VERBOSITY<2

/*This will make problems for command that need to pipe
 *
 */

/*#define SYSTEM(sysLogFile, ...)\
{\
    char *__cmd__, *__pipedCmd__;\
    asprintf(&__cmd__, __VA_ARGS__);\
    char* nullPipe = " &> /dev/null 2>&1";\
    asprintf(&__pipedCmd__, "%s%s", __cmd__, nullPipe);\
    systemCallUtil(__pipedCmd__, sysLogFile);\
    free( __cmd__);\
    free(__pipedCmd__);\
}\

#else
*/

#define SYSTEM(sysLogFile, ...)\
{\
    char* __cmd__;\
    asprintf(&__cmd__, __VA_ARGS__);\
    systemCallUtil(__cmd__, sysLogFile);\
    delete[] __cmd__;\
}\

//#endif

#define STRINGIFY(__str__, ...)\
{\
    asprintf(&__str__, __VA_ARGS__);\
}

#define STRINGIFY_ADD(__str__, ...)\
{\
    char *__str_cpy__, *new_str;\
    STRINGIFY(__str_cpy__, "%s", __str__);\
    free(__str__);\
    STRINGIFY(new_str, __VA_ARGS__);\
    STRINGIFY(__str__,"%s%s", __str_cpy__, new_str);\
    free(__str_cpy__);\
    free(new_str);\
}\


#define UNUSED(x) (void)(x)

#define EXTRACT_IDX(grid_idx)\
    int x_start = grid_idx[0];\
    int x_end   = grid_idx[1];\
    int y_start = grid_idx[2];\
    int y_end   = grid_idx[3];\
    int z_start = grid_idx[4];\
    int z_end   = grid_idx[5];\


#define getRadiusFolded(val, stencil_, dim_)\
{\
    double radius_ = stencil_->radius;\
    double fold_ = 0;\
    if(dim_ == x)\
    {\
        fold_ = stencil_->fold_x;\
    }\
    else if(dim_ == y)\
    {\
        fold_ = stencil_->fold_y;\
    }\
    else if(dim_ == z)\
    {\
        fold_ = stencil_->fold_z;\
    }\
    val = fold_*(static_cast<int>((radius_-1)/fold_)+1);\
}

#define getRadius(val, stencil_, dim_)\
{\
    double radius_ = stencil_->radius;\
    double fold_ = 0;\
    if(dim_ == x)\
    {\
        fold_ = stencil_->fold_x;\
    }\
    else if(dim_ == y)\
    {\
        fold_ = stencil_->fold_y;\
    }\
    else if(dim_ == z)\
    {\
        fold_ = stencil_->fold_z;\
    }\
    val = (static_cast<int>((radius_-1)/fold_)+1);\
}

#define getLayers(val, stencil, type)\
{\
    double fold_ = 1;\
    double outer_layer = 0.0;\
    double inner_layer = 0.0;\
    if(type == OUTER && stencil->dim==3)\
    {\
        fold_ = stencil->fold_x;\
        outer_layer = 2.0*((static_cast<int>((stencil->radius-1)/stencil->fold_x)+1));\
        inner_layer = 0;\
    }\
    else\
    {\
        fold_ = stencil->fold_x*stencil->fold_y;\
        inner_layer = 2.0*((static_cast<int>((stencil->radius-1)/stencil->fold_y)+1));\
        if(stencil->dim==3)\
        {\
            outer_layer = 2.0*((static_cast<int>((stencil->radius-1)/stencil->fold_x)+1));\
        }\
        else\
        {\
            outer_layer = 0;\
        }\
    }\
    double restContrib_ = numReadGrids + numWriteGrids - numStencils;\
    printf("outer layer = %f, inner layer = %f, numReads = %d, numWrites = %d, numStencils = %d\n", outer_layer, inner_layer, numReadGrids, numWriteGrids, numStencils);\
    double layers_ = fold_*(numStencils*(inner_layer + 1.0 + outer_layer) + restContrib_);\
    val = layers_;\
}

#define getMaxLayers(val, stencil, type)\
{\
    double fold_ = 1;\
    double outer_layer = 0.0;\
    double inner_layer = 0.0;\
    if(type == OUTER && stencil->dim==3)\
    {\
        fold_ = stencil->fold_x;\
        outer_layer = 2.0*((static_cast<int>((stencil->radius-1)/stencil->fold_x)+1));\
        inner_layer = 0;\
    }\
    else\
    {\
        fold_ = stencil->fold_x*stencil->fold_y;\
        inner_layer = 2.0*((static_cast<int>((stencil->radius-1)/stencil->fold_y)+1));\
        if(stencil->dim==3)\
        {\
            outer_layer = 2.0*((static_cast<int>((stencil->radius-1)/stencil->fold_x)+1));\
        }\
        else\
        {\
            outer_layer = 0;\
        }\
    }\
    double restContrib_ = stencil->s - stencil->maxNumStencils;\
    double layers_ = fold_*(stencil->maxNumStencils*(inner_layer + 1.0 + outer_layer) + restContrib_);\
    val = layers_;\
}

#define GET_PERM_OUT(str, stencil, data)\
    (strcmp(str,"x:y")==0)?stencil->getOutputAt(data,i,j,0):(strcmp(str,"y:x")==0)?stencil->getOutputAt(data,j,i,0):(strcmp(str,"y:z")==0)?stencil->getOutputAt(data,0,i,j):(strcmp(str,"z:y")==0)?stencil->getOutputAt(data,0,j,i):(strcmp(str,"x:z")==0)?stencil->getOutputAt(data,i,0,j):(strcmp(str,"z:x")==0)?stencil->getOutputAt(data,j,0,i):-1;

/*
#define CAST_Grid_TXYZ_ptr(stencil, grid_name_, pre_cmd,  ...)\
{\
    YASK_STENCIL_CONTEXT *_context_ = (YASK_STENCIL_CONTEXT*) stencil->stencilContext;\
    int ts = stencil->grids[stencil->gridMap[grid_name_]].time_steps;\
    switch(ts)\
    {\
        case 1 :\
            pre_cmd ((Grid_TXYZ<1>*) (_context_->gridMap[string(grid_name_)])) ## __VA_ARGS__;\
            break;\
        case 2 :\
            pre_cmd ((Grid_TXYZ<2>*) (_context_->gridMap[string(grid_name_)])) ## __VA_ARGS__;\
            break;\
        case 3 :\
            pre_cmd ((Grid_TXYZ<3>*) (_context_->gridMap[string(grid_name_)])) ## __VA_ARGS__;\
            break;\
        case 4 :\
            pre_cmd ((Grid_TXYZ<4>*) (_context_->gridMap[string(grid_name_)])) ## __VA_ARGS__;\
            break;\
        case 5 :\
            pre_cmd ((Grid_TXYZ<5>*) (_context_->gridMap[string(grid_name_)])) ## __VA_ARGS__;\
            break;\
        case 6 :\
            pre_cmd ((Grid_TXYZ<6>*) (_context_->gridMap[string(grid_name_)])) ## __VA_ARGS__;\
            break;\
        case 7 :\
            pre_cmd ((Grid_TXYZ<7>*) (_context_->gridMap[string(grid_name_)])) ## __VA_ARGS__;\
            break;\
        case 8 :\
            pre_cmd ((Grid_TXYZ<8>*) (_context_->gridMap[string(grid_name_)])) ## __VA_ARGS__;\
            break;\
        case 9 :\
            pre_cmd ((Grid_TXYZ<9>*) (_context_->gridMap[string(grid_name_)])) ## __VA_ARGS__;\
            break;\
        case 10 :\
            pre_cmd ((Grid_TXYZ<10>*) (_context_->gridMap[string(grid_name_)])) ## __VA_ARGS__;\
          break;\
        default :\
            ERROR_PRINT("allocation of more than 10 time-steps currently not supported");\
            pre_cmd ((Grid_TXYZ<1>*) (_context_->gridMap[string(grid_name_)])) ## __VA_ARGS__;\
    }\
}
*/

#endif
