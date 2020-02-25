#ifndef _YASK_FORK_H
#define _YASK_FORK_H
#include "yaskSite.h"

extern "C"
{
    int YASKinit(yaskSite* opt, bool noAlloc);
    int YASKfinalize(yaskSite* opt);
    int YASKstencil(yaskSite* opt, int timeOffset);
    void* YASKgetElPtr(yaskSite* opt, const char* data, int t, int x, int y, int z, bool checkBounds);
    void* YASKgetGridPtr(yaskSite* opt, const char* grid);
    void YASKsetGridPtr(yaskSite* opt, const char* grid, void *ptr);
    int YASKgetHalo(yaskSite* opt, const char* grid, int dim);
    void YASKsetHalo(yaskSite* opt, const char* grid, int dim, int val);
}

#endif
