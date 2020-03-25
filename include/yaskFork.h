#ifndef _YASK_FORK_H
#define _YASK_FORK_H
#include "yaskSite.h"

extern "C"
{
    int YASKinit(yaskSite* opt, bool noAlloc);
    int YASKfinalize(yaskSite* opt);
    int YASKstencil(yaskSite* opt, int timeOffset);
    void YASKautoTuner(yaskSite* opt, bool enable);
    double YASKgetElement(yaskSite* opt, const char* data, int t, int x, int y, int z);
    void YASKsetElement(yaskSite* opt, const char* data, int t, int x, int y, int z, double val);
    void YASKsetAll(yaskSite* opt, const char* data, double val);
    void* YASKgetGridPtr(yaskSite* opt, const char* grid);
    void YASKfuseGrid(yaskSite *to_opt, const char *to_grid_name, yaskSite *from_opt, const char *from_grid_name);
    int YASKgetHalo(yaskSite* opt, const char* grid, int dim);
}

#endif
