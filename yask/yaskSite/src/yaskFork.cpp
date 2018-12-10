// Base classes for stencil code.
//
// This is just a template each builds should generate
// functions of this kind; which will be linked on the fly

#include "yaskFork.h"
#include "print.h"
#include "config.h"
#include "macros.h"

#ifdef GENERATED

#include "stencil_calc.hpp"

#ifdef yaskSite_HAVE_MPI
#include <mpi.h>
#endif

#ifdef LIKWID_PERFMON
#include <likwid.h>
#endif

using namespace std;
using namespace yask;

extern "C" {
    int YASKinit(yaskSite* opt)
    {

        //Right now new allocation is done only if
        //dimension is changed; Need to think aboout
        //when nthreads changes and the associated
        //NUMA problems that can arise.
        bool needAlloc = opt->dimUpdated;

        StencilSettings* setting;
        YASK_STENCIL_CONTEXT *context;

        if(needAlloc)
        {
            //delete all settings and context if dimension
            //is updated
            if(opt->stencilSettings)
            {
                //delete old setting if it exists
                StencilSettings* settingOld = (StencilSettings*) opt->stencilSettings;
                delete settingOld;
            }
            if(opt->stencilContext)
            {
                //delete old context if it exists
                YASK_STENCIL_CONTEXT* contextOld = (YASK_STENCIL_CONTEXT*) opt->stencilContext;
                delete contextOld;
            }

            //We need setting also has pointer, due to the internal 
            //design of YASK; which I think is a bad design
            setting = new StencilSettings;
            // Object containing data and parameters for stencil eval.
            context = new YASK_STENCIL_CONTEXT(*setting);

            //set mpi
            if(!(opt->mpi_man))
            {
                ERROR_PRINT("You should have initialized MPI at the beginning");
            }
            else
            {
                context->comm = opt->mpi_man->comm;
                context->num_ranks = opt->mpi_man->nRanks;
                context->my_rank = opt->mpi_man->myRank;

#if yaskSite_VERBOSITY < 2
                std::ostream* null_os = new ofstream;
                context->set_ostr(null_os);
#endif

            }
        }
        else
        {
            setting = (StencilSettings*) opt->stencilSettings;
            context = (YASK_STENCIL_CONTEXT *) opt->stencilContext;
        }

        setting->dt = opt->dt;
        setting->dw = 1; //currently we consider only till 3D cases
        setting->dx = opt->dx;
        setting->dy = opt->dy;
        setting->dz = opt->dz;

        setting->rt = opt->rt;
        setting->rx = opt->rx;
        setting->ry = opt->ry;
        setting->rz = opt->rz;

        //bg*, sbg* is also not used currently; no 'grouped'
        //block loops
        setting->bx = opt->bx;
        setting->by = opt->by;
        setting->bz = opt->bz;

        setting->sbx = opt->sbx;
        setting->sby = opt->sby;
        setting->sbz = opt->sbz;

        //MPI settings is left to default
        setting->max_threads = opt->nthreads;
        setting->num_block_threads = opt->threadPerBlock;

        if(needAlloc)
        {
            // Alloc memory, create lists of grids, etc.
            context->allocAll();
            context->initData();

            //copy some useful datas
            //reallocate opt->grids
            opt->gridName.clear();
            opt->gridIdx.clear();

            //now copy grid names
            for(auto it = context->outputGridMap.begin(); it != context->outputGridMap.end(); ++it) {
                opt->gridName.push_back(it->first.c_str());
            }

            //No need to sort since it would already be from map
            //std::sort(opt->gridName.begin(), opt->gridName.end());

            for(int i=0; i<context->outputGridMap.size(); ++i)
            {
                Grid_TXYZ<2>* grid= (Grid_TXYZ<2>*) context->outputGridMap[((std::string)opt->gridName[i])];
                std::vector<int> currIdx(6);
                currIdx[0]=grid->get_first_x();
                currIdx[1]=grid->get_last_x();
                currIdx[2]=grid->get_first_y();
                currIdx[3]=grid->get_last_y();
                currIdx[4]=grid->get_first_z();
                currIdx[5]=grid->get_last_z();
                opt->gridIdx.push_back(currIdx);
            }
        }

        if (context->tot_numpts_dt < 1) {
            ERROR_PRINT("Exiting because there are zero points to evaluate.");
            exit_yask(1);
        }

        opt->stencilSettings = ((void*) setting);
        opt->stencilContext =  ((void*)context);

        context->global_barrier();
        return 0;
    }

    int YASKfinalize(yaskSite* opt)
    {
        StencilSettings* setting = (StencilSettings*) opt->stencilSettings;
        YASK_STENCIL_CONTEXT* context = (YASK_STENCIL_CONTEXT*) opt->stencilContext;

        context->global_barrier();

        delete setting;
        delete context;
        return 0;
    }

    int YASKstencil( yaskSite* opt)
    {
        YASK_STENCIL_CONTEXT* context = (YASK_STENCIL_CONTEXT*) opt->stencilContext;

        context->calc_rank_opt();

        context->global_barrier();

        return 0;
    }

    double* YASKgetElPtr(yaskSite* opt, const char* data, int t, int x, int y, int z, bool checkBounds)
    {
        YASK_STENCIL_CONTEXT *context = (YASK_STENCIL_CONTEXT*) opt->stencilContext;
        Grid_TXYZ<2>* grid= (Grid_TXYZ<2>*) context->outputGridMap[((std::string)data)];
        return grid->getElemPtr(t,x,y,z,checkBounds);
    }

}

#else

extern "C"
{
    int YASKinit(yaskSite* opt)
    {
        UNUSED(opt);
        return -1;
    }

    int YASKfinalize(yaskSite* opt)
    {
        UNUSED(opt);
        return -1;
    }

    int YASKstencil(yaskSite* opt)
    {
        UNUSED(opt);
        return -1;
    }

    double* YASKgetElPtr(yaskSite*opt, const char* data, int t, int x, int y, int z, bool checkBounds)
    {
        UNUSED(opt);
        UNUSED(data);
        UNUSED(t);
        UNUSED(x);
        UNUSED(y);
        UNUSED(z);
        UNUSED(checkBounds);
        return NULL;
    }
}
#endif
