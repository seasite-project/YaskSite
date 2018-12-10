#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#include <loop_adapt.h>


int main(int argc, char* argv[])
{
    int i,j;
    int max_iters = 180;
    int size = 10485760;
    int cpu[1] = {2};


    loop_adapt_pin(cpu[0]);
    loop_adapt_set_policy(LOOP_ADAPT_POL_DUMP);
    loop_adapt_set_analyze_iters(20);
    loop_adapt_set_adapt_iters_freq(20);
    loop_adapt_init(1, (int*)cpu);

    int* array1 = malloc(size * sizeof(int));
    if (!array1)
    {
        return 1;
    }
    int* array2 = malloc(size * sizeof(int));
    if (!array2)
    {
        free(array1);
        return 1;
    }
    for (j=0; j<size; j++)
    {
        array2[j] = 12345;
    }

    for (i=0; i<max_iters; i++)
    {
        if (i < max_iters)
            printf("Iteration %d\n", i);
        loop_adapt_begin(i, max_iters);
        for (j=0; j<size; j++)
        {
            array1[j] = array2[j];
        }
        loop_adapt_end(i, max_iters);
    }

    for (j=0; j<size; j++)
    {
        array1[j] = 0;
        array2[j] = 0;
    }
    printf("Closing\n");
    free(array1);
    free(array2);
    loop_adapt_close();

}
