#include <yaskSite.h>
#include "timing.h"

#define PERF_RUN(stencil)\
{\
    INIT_TIME(stencil_);\
    START_TIME(stencil_);\
    stencil.run();\
    STOP_TIME(stencil_);\
    double dt=stencil.dt;\
    double dx=stencil.dx;\
    double dy=stencil.dy;\
    double dz=stencil.dz;\
    double mlups=dt*dx*(dy*dz*1e-6);\
    double time=GET_TIME(stencil_);\
    printf("Perf = %f MLUP/s, Time = %fs\n", mlups/time, time);\
}\

void main(int argc, char** argv)
{
    MPI_Manager mpiMan(&argc, &argv);

#if 0
    yaskSite stencil_1(&mpiMan, "heat_8order_3d", 3, 1, 1, 1);
    stencil_1.setThread(10,1);
    stencil_1.setDim(512, 512, 512, 50);
    stencil_1.run();//init goes here
    PERF_RUN(stencil_1);

    //block in L2
    stencil_1.spatialTuner("L2");
    PERF_RUN(stencil_1);

    stencil_1.blockTuner("L3","L2");
    PERF_RUN(stencil_1);

    yaskSite stencil_0(&mpiMan, "heat_8order_3d", 3, 4, 1, 1);
    stencil_0.setThread(10,1);
    stencil_0.setDim(512, 512, 512, 50);
    stencil_0.run();//init goes here
    PERF_RUN(stencil_0);

    //block in L2
    stencil_0.spatialTuner("L2");
    PERF_RUN(stencil_0);

    stencil_0.blockTuner("L3","L2");
    PERF_RUN(stencil_0);

#endif

#if 0
    //2D example
    yaskSite stencil_1(&mpiMan, "heat2d", 2, 1, 1, 1);
    stencil_1.setThread(10,1);
    stencil_1.setDim(60000, 2000, 1, 50);
    stencil_1.run();//init goes here
    PERF_RUN(stencil_1);

    //block in L2
    stencil_1.spatialTuner("L2");
    PERF_RUN(stencil_1);

    //after temporal blocking one needs to call for spatial
    //stencil_1.temporalTuner("L3");
    //stencil_1.spatialTuner("L3");
    //alternative
    stencil_1.blockTuner("L3","L2");
    PERF_RUN(stencil_1);


    yaskSite stencil_0(&mpiMan, "heat2d", 2, 4, 1, 1);
    stencil_0.setThread(10,1);
    stencil_0.setDim(60000, 2000, 1, 50);
    stencil_0.run();//init goes here
    PERF_RUN(stencil_0);

    //block in L2
    stencil_0.spatialTuner("L2");
    PERF_RUN(stencil_0);

    stencil_0.blockTuner("L3","L2");
    PERF_RUN(stencil_0);

    yaskSite stencil_2(&mpiMan, "heat2d", 2, 1, 4, 1);
    stencil_2.setThread(10,1);
    stencil_2.setDim(60000, 2000, 1, 50);
    stencil_2.run();//init goes here
    PERF_RUN(stencil_2);

    //block in L2
    stencil_2.spatialTuner("L2");
    PERF_RUN(stencil_2);

    stencil_2.blockTuner("L3","L2");
    PERF_RUN(stencil_2);
#endif

#if 1

    //3D example
    yaskSite stencil_2(&mpiMan, "heat3d", 3, 3, 4, 1, 1);
    stencil_2.setThread(10,1);
    stencil_2.setDim(1300, 1300, 193, 20);


    stencil_2.init();//init goes here
/*    PERF_RUN(stencil_2);

    stencil_2.spatialTuner("L3", "L2", 0.4, 0.4);
    PERF_RUN(stencil_2);
*/
    //after temporal blocking one needs to call for spatial
    //stencil_1.temporalTuner("L3");
    //stencil_1.spatialTuner("L3", "L2");
    //alternative
    stencil_2.blockTuner("L3","L3","L2", 0.4, 0.4, 0.4);
    //stencil_2.setBlock(260, 10, 24);
    PERF_RUN(stencil_2);

#if 0
    //change path
    yaskSite stencil_3(&mpiMan, "heat3d", 3, 1, 1, 1, 1, false, "square_wave serpentine");
    stencil_3.setThread(10,1);
    stencil_3.setDim(1300, 1300, 300, 20);

    stencil_3.init();
    //copy data so new stencil executes it
    //stencil_2.transferData(&stencil_1);
/*    PERF_RUN(stencil_3);

    //reduce safety factor because compared to default path
    //serpentine needs to keep more layer in cache
    stencil_3.spatialTuner("L3","L2",0.25,0.25);
    PERF_RUN(stencil_3);
*/
    stencil_3.blockTuner("L3", "L3", "L2",0.25,0.25,0.25);
    PERF_RUN(stencil_3);
#endif
#endif
}
