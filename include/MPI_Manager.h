#ifndef YASKSITE_MPI_H
#define YASKSITE_MPI_H

#include "config.h"

#ifdef yaskSite_HAVE_MPI
#include <mpi.h>
#endif

//All important MPI tools
//should come here
struct MPI_Manager
{
    int *argc;
    char*** argv;
    int nRanks;
    int myRank;
    int printRank;

#ifdef yaskSite_HAVE_MPI
    MPI_Comm comm;
#else
    int comm;
#endif

    MPI_Manager(int* argc, char*** argv, int printRank=yaskSite_PRINT_RANK);
    ~MPI_Manager();

    void global_barrier();
};

#endif
