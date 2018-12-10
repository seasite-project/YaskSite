#ifndef _YASK_FORK_H
#define _YASK_FORK_H
#include "yaskSite.h"

extern "C"
{
    int YASKinit(yaskSite* opt);
    int YASKfinalize(yaskSite* opt);
    int YASKstencil(yaskSite* opt);
    double* YASKgetElPtr(yaskSite* opt, const char* data, int t, int x, int y, int z, bool checkBounds);
}

#endif
