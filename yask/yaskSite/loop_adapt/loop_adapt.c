#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#include <loop_adapt.h>

#include <likwid.h>

static int disable_loop_adapt = 0;
static int current_loop_adapt_step = -1;
static char* region_name = NULL;

static inline int loop_adapt_region_name(int *step, char* name)
{
    int err = 0;
    int s = *step;
    s++;
    err = sprintf(name, "step%d", *step);
    name[err] = '\0';
    *step = s;
    return err;
}

void loop_adapt_pin(int cpu)
{
    likwid_pinThread(cpu);
}


static inline int skip_loop_adapt(size_t iter)
{
    if (iter < analyze_iters || iter > (analyze_iters + (adapt_iters_freq * adapt_iters_max_steps)))
    {
        return 1;
    }
    return 0;
}

void loop_adapt_init(int nr_cpus, int* cpus)
{
    int err = 0;

    current_loop_adapt_step = -2;

    /* store values like frequency, prefetcher state */

    topology_init();
    affinity_init();
    loop_adapt_nr_cpus = nr_cpus;
    loop_adapt_cpus = malloc(loop_adapt_nr_cpus * sizeof(int));
    if (!loop_adapt_cpus)
    {
        fprintf(stderr, "No Mem\n");
        disable_loop_adapt = 1;
        return;
    }
    for (int i=0; i< nr_cpus; i++)
    {
        printf("Adding CPU %d to loop_adapt\n", cpus[i]);
        printf("initing\n");
        loop_adapt_cpus[i] = cpus[i];
    }
    err = perfmon_init(loop_adapt_nr_cpus, loop_adapt_cpus);
    if (err)
    {
        fprintf(stderr, "Perfmon initialized failed?\n");
        disable_loop_adapt = 1;
        return;
    }
    fprintf(stderr, "Perfmon initialized with %d CPUs\n", nr_cpus);
    region_name = malloc(7 * sizeof(char));
    if (!region_name)
    {
        disable_loop_adapt = 1;
        return;
    }
    loop_adapt_region_name(&current_loop_adapt_step, region_name);
    loop_adapt_init_policy();
    return;
}

void loop_adapt_begin(size_t iter, size_t max_iter)
{
    int err;
    if (!disable_loop_adapt && iter == 0)
    {
        err = perfmon_startCounters();
        if (err)
            disable_loop_adapt = 1;
        return;
    }
}

void loop_adapt_end(size_t iter, size_t max_iter)
{
    int err = 0;

    if (disable_loop_adapt)
    {
        return;
    }

    if ((iter == analyze_iters) ||
            ((iter > analyze_iters) &&
             (iter <= analyze_iters + ((adapt_iters_max_steps) * adapt_iters_freq)) &&
             ((iter - analyze_iters) % adapt_iters_freq == 0)))
    {
        perfmon_stopCounters();

        err = loop_adapt_apply_policy(current_loop_adapt_step);
        if (err)
        {
            fprintf(stderr, "Failed to apply policy\n");
            disable_loop_adapt = 1;
            return;
        }
        err = loop_adapt_region_name(&current_loop_adapt_step, region_name);
        if (err < 0)
        {
            fprintf(stderr, "Failed to set new region name\n");
        }
        perfmon_startCounters();
    }
}

void loop_adapt_close()
{
    if (!disable_loop_adapt)
    {
        perfmon_stopCounters();
        perfmon_finalize();
    }
    if (loop_adapt_cpus != NULL)
    {
        free(loop_adapt_cpus);
        loop_adapt_cpus = NULL;
    }
    if (region_name != NULL)
    {
        free(region_name);
        region_name = NULL;
    }
}
