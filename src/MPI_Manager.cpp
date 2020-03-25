#include "MPI_Manager.h"
#include "print.h"
#include <stdlib.h>
#include "types.h"
#include "macros.h"

#ifdef yaskSite_HAVE_OpenMP
#include <omp.h>
#endif

#ifdef ENABLE_LOOPADAPT
    #include <loop_adapt.h>
#endif

#ifdef LIKWID_PERFMON
    #include <likwid.h>
#endif

MPI_Manager::MPI_Manager(int* argc_, char*** argv_, char* mcFile, int printRank_):argc(argc_), argv(argv_), nRanks(1), myRank(0), group(0), shm_nRanks(0), shm_myRank(0), shm_group(0), maxThreads(1), printRank(printRank_)
{
#ifdef yaskSite_HAVE_MPI
    int is_init = false;
    MPI_Initialized(&is_init);

    // No MPI communicator provided.
    if (!is_init) {
        int provided = 0;
        MPI_Init_thread(argc, argv, MPI_THREAD_MULTIPLE, &provided);
        if (provided < MPI_THREAD_SERIALIZED) {
            ERROR_PRINT("error: MPI_THREAD_SERIALIZED or MPI_THREAD_MULTIPLE not provided");
            exit(-1);
        }
        is_init = true;
    }
    else
    {
        WARNING_PRINT("MPI already initialized, yasksite will not reinitialize");
    }
    comm = MPI_COMM_WORLD;
    MPI_Comm_rank(comm, &myRank);
    MPI_Comm_size(comm,  &nRanks);
    MPI_Comm_group(comm, &group);

    if(nRanks < 1)
    {
        ERROR_PRINT("error: MPI_Comm_size() returns less than one rank");
        exit(-1);
    }

    // Create a shm communicator.
    MPI_Comm_split_type(comm, MPI_COMM_TYPE_SHARED, 0, MPI_INFO_NULL, &shm_comm);
    MPI_Comm_rank(shm_comm, &shm_myRank);
    MPI_Comm_group(shm_comm, &shm_group);
    MPI_Comm_size(shm_comm, &shm_nRanks);


#else
    comm = 0;
    shm_comm = 0;
#endif

    //
#ifdef yaskSite_HAVE_OpenMP
    //just triggering openmp
    //omp_get_num_procs();
    // Set env vars needed by OMP.
    // TODO: make this visible to the user.
    int ret = setenv("OMP_PLACES", "cores", 0); // default placement for outer loop.
    if(ret!=0)
    {
        ERROR_PRINT("Error setting environment OMP_PLACES");
    }
    ret = setenv("KMP_HOT_TEAMS_MODE", "1", 0); // more efficient nesting.
    if(ret!=0)
    {
        ERROR_PRINT("Error setting environment KMP_HOT_TEAMS_MODE");
    }
    ret = setenv("KMP_HOT_TEAMS_MAX_LEVEL", "2", 0); // 2-level nesting.
    if(ret!=0)
    {
        ERROR_PRINT("Error setting environment KMP_HOT_TEAMS_MAX_LEVEL");
    }

    // Save initial value of OMP max threads.
    // Side effect: causes OMP to dump debug info if env var set.
    //if (!max_threads)
     maxThreads = omp_get_max_threads();

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

    initializeCaches(mcFile);
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
