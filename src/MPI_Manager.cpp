#include "MPI_Manager.h"
#include "print.h"
#include <stdlib.h>

#ifdef yaskSite_HAVE_OpenMP
#include <omp.h>
#endif

#ifdef ENABLE_LOOPADAPT
    #include <loop_adapt.h>
#endif

#ifdef LIKWID_PERFMON
    #include <likwid.h>
#endif

MPI_Manager::MPI_Manager(int* argc_, char*** argv_, int printRank_):argc(argc_), argv(argv_), nRanks(1), myRank(0), printRank(printRank_)
{
    printf("Am i here\n");
#ifdef yaskSite_HAVE_MPI
    int provided = 0;
    MPI_Init_thread(argc, argv, MPI_THREAD_SERIALIZED, &provided);
    printf("inited\n");
    if(provided < MPI_THREAD_SERIALIZED)
    {
        ERROR_PRINT("MPI error: MPI_THREAD_SERIALIZED not provided.\n");
        exit(-1);
    }

    comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &myRank);
    MPI_Comm_size(comm,  &nRanks);
#else
    comm = 0;
#endif

    //
#ifdef yaskSite_HAVE_OpenMP
    //just triggering openmp
    omp_get_num_procs();
#endif

    global_barrier();

#ifdef ENABLE_LOOPADAPT
    int cpu[1] = {2};
    loop_adapt_pin(cpu[0]);
    loop_adapt_set_policy(LOOP_ADAPT_POL_DUMP);
    loop_adapt_set_analyze_iters(1);
    loop_adapt_set_adapt_iters_freq(1);
    loop_adapt_set_adapt_iters_max_steps(500);
    loop_adapt_init(1, (int*)cpu);
#endif

//Initing LIKWID
#ifdef LIKWID_PERFMON
    LIKWID_MARKER_INIT;

#ifdef yaskSite_HAVE_OpenMP
#pragma omp parallel
    {
        LIKWID_MARKER_THREADINIT;
    }
#endif
#endif

}

MPI_Manager::~MPI_Manager()
{
#ifdef yaskSite_HAVE_MPI
    global_barrier();
    MPI_Finalize();
#endif

#ifdef ENABLE_LOOPADAPT
    loop_adapt_close();
#endif

//Finalise likwid
#ifdef LIKWID_PERFMON
    LIKWID_MARKER_CLOSE;
#endif


}

void MPI_Manager::global_barrier()
{
#ifdef yaskSite_HAVE_MPI
    MPI_Barrier(comm);
#endif
}
