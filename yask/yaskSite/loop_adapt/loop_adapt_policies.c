#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

#include <loop_adapt_policies.h>

#include <likwid.h>

static int loop_adapt_current_policy = LOOP_ADAPT_POL_NOTHING;

static int likwid_gid = 0;
static double likwid_last_runtime = 0;

static DataStore *datastore = NULL;

int loop_adapt_init_datastore()
{
    if (!datastore)
    {
        datastore = malloc(sizeof(DataStore));
        if (!datastore)
        {
            fprintf(stderr, "No Mem\n");
            return -ENOMEM;
        }
        datastore->nr_points = -1;
        datastore->points = NULL;
    }
    return 0;
}

int loop_adapt_add_datastore(DataPoint *p)
{
    int err = 0;
    if (!datastore)
    {
        err = loop_adapt_init_datastore();
        if (err < 0)
        {
            return err;
        }
    }
    if (datastore->nr_points == -1)
    {
        datastore->points = malloc(sizeof(DataPoint*));
        if (!datastore->points)
        {
            return -ENOMEM;
        }
        datastore->nr_points = 0;
        datastore->points[datastore->nr_points] = p;
        datastore->nr_points++;
    }
    else
    {
        DataPoint** tmp = NULL;
        tmp = realloc(datastore->points, (datastore->nr_points+1)*sizeof(DataPoint*));
        if (!tmp)
        {
            free(datastore->points);
            datastore->points = NULL;
            datastore->nr_points = -1;
            return -ENOMEM;
        }
        else
        {
            datastore->points = tmp;
        }
        datastore->points[datastore->nr_points] = p;
        datastore->nr_points++;
    }
    return 0;
}

void loop_adapt_free_datastore()
{
    int i = 0;
    if (datastore && datastore->nr_points != -1)
    {
        for (i = 0; i < datastore->nr_points; i++)
        {
            if (datastore->points[i])
                free(datastore->points[i]);
        }
    }
    if (datastore)
    {
        free(datastore);
    }
}

DataPoint * loop_adapt_get_last_datapoint()
{
    return datastore->points[datastore->nr_points - 1];
}

void loop_adapt_print_datapoint(DataPoint *p)
{
    int i;
    if (p)
    {
        printf("DataPoint for Step %d on CPU %d (ThreadID %d):\n", p->step, p->cpu, p->tid);
        printf("Runtime %f s\n", p->runtime);
        printf("List of %d metrics\n", p->nr_metrics);
        for (i = 0; i < p->nr_metrics; i++)
        {
            printf("Metric %d '%s': %f\n", i, p->mnames[i], p->metrics[i]);
        }
    }
}


int loop_adapt_init_policy_nothing()
{
    fprintf(stderr, "Init NOTHING policy\n");
    return 0;
}

int loop_adapt_apply_policy_nothing(int step)
{
    fprintf(stderr, "Applying NOTHING policy\n");
    return 0;
}

int loop_adapt_init_policy_dump()
{
    fprintf(stderr, "Init DUMP policy\n");
    int err = 0;
    int gid = 0;
    gid = perfmon_addEventSet("MEM_DP");
    if (gid < 0)
        return 1;
    likwid_gid = gid;
    fprintf(stderr, "Added MEM_DP group: ID %d\n", likwid_gid);
    int nmetrics = perfmon_getNumberOfMetrics(likwid_gid);
    fprintf(stderr, "MEM_DP group has %d metrics\n", nmetrics);
    err = perfmon_setupCounters(likwid_gid);

    return 0;
}
int loop_adapt_apply_policy_dump(int step)
{
    int cpu = affinity_threadGetProcessorId();
    int tid = 0;
    DataPoint* p = NULL;
    double lastTime = 0;
    for (int i= 0; i<loop_adapt_nr_cpus; i++)
    {
        if (cpu == loop_adapt_cpus[i])
        {
            tid = i;
        }
    }
    fprintf(stderr, "Applying DUMP policy\n");
    int nmetrics = perfmon_getNumberOfMetrics(likwid_gid);
    if (nmetrics == -1)
    {
        fprintf(stderr, "Applying DUMP policy\n");
        return 1;
    }
    p = (DataPoint*) malloc(sizeof(DataPoint));
    if (!p)
    {
        fprintf(stderr, "No Mem\n");
        return 1;
    }
    p->metrics = malloc(nmetrics * sizeof(double));
    if (!p->metrics)
    {
        fprintf(stderr, "No Mem\n");
        return 1;
    }
    p->mnames = malloc(nmetrics * sizeof(char*));
    if (!p->mnames)
    {
        fprintf(stderr, "No Mem\n");
        return 1;
    }
    p->cpu = cpu;
    p->tid = tid;
    p->step = step;
    lastTime = perfmon_getTimeOfGroup(likwid_gid);
    p->runtime = lastTime - likwid_last_runtime;
    likwid_last_runtime = lastTime;
    p->nr_metrics = nmetrics;


    for (int i=0; i<nmetrics; i++)
    {
        char * n;
        p->metrics[i] = perfmon_getLastMetric(likwid_gid, i, tid);
        n = perfmon_getMetricName(likwid_gid, i);
        p->mnames[i] = malloc((strlen(n)+1)*sizeof(char));
        strcpy(p->mnames[i], n);
    }
    loop_adapt_print_datapoint(p);
    free(p->metrics);
    for (int i=0; i<nmetrics; i++)
    {
        free(p->mnames[i]);
    }
    free(p->mnames);
    free(p);
    return 0;
}

int loop_adapt_init_policy_min_energy()
{
    fprintf(stderr, "Init MIN_ENERGY policy\n");
    int err = 0;
    int gid = 0;
    gid = perfmon_addEventSet("CYC_ACTIV_ENERGY");
    if (gid < 0)
        return 1;
    likwid_gid = gid;
    fprintf(stderr, "Added CYC_ACTIV_ENERGY group: ID %d\n", likwid_gid);
    int nmetrics = perfmon_getNumberOfMetrics(likwid_gid);
    fprintf(stderr, "CYC_ACTIV_ENERGY group has %d metrics\n", nmetrics);
    err = perfmon_setupCounters(likwid_gid);
    return 0;
}
int loop_adapt_apply_policy_min_energy(int step)
{
    fprintf(stderr, "Applying MIN_ENERGY policy\n");
    int cpu = affinity_threadGetProcessorId();
    int tid = 0;
    DataPoint* p = NULL;
    double lastTime = 0;
    int cyc_ldm_idx = 0;
    int cyc_no_exec_idx = 0;
    int pkg_e_idx = 0;
    int dram_e_idx = 0;
    int direction = 0;
    for (int i= 0; i<loop_adapt_nr_cpus; i++)
    {
        if (cpu == loop_adapt_cpus[i])
        {
            tid = i;
        }
    }
    fprintf(stderr, "Applying MIN_ENERGY policy\n");
    int nmetrics = perfmon_getNumberOfMetrics(likwid_gid);
    if (nmetrics == -1)
    {
        fprintf(stderr, "Applying MIN_ENERGY policy\n");
        return 1;
    }
    p = (DataPoint*) malloc(sizeof(DataPoint));
    if (!p)
    {
        fprintf(stderr, "No Mem\n");
        return 1;
    }
    p->metrics = malloc(nmetrics * sizeof(double));
    if (!p->metrics)
    {
        fprintf(stderr, "No Mem\n");
        return 1;
    }
    p->mnames = malloc(nmetrics * sizeof(char*));
    if (!p->mnames)
    {
        fprintf(stderr, "No Mem\n");
        return 1;
    }
    p->cpu = cpu;
    p->tid = tid;
    p->step = step;
    lastTime = perfmon_getTimeOfGroup(likwid_gid);
    p->runtime = lastTime - likwid_last_runtime;
    likwid_last_runtime = lastTime;
    p->nr_metrics = nmetrics;


    for (int i=0; i<nmetrics; i++)
    {
        char * n;
        p->metrics[i] = perfmon_getLastMetric(likwid_gid, i, tid);
        n = perfmon_getMetricName(likwid_gid, i);
        p->mnames[i] = malloc((strlen(n)+1)*sizeof(char));
        strcpy(p->mnames[i], n);
        if (strstr(n, "LDM") != NULL)
            cyc_ldm_idx = i;
        else if (strstr(n, "NO_EXEC") != NULL)
            cyc_no_exec_idx = i;
        else if (strstr(n, "PKG") != NULL)
            pkg_e_idx = i;
        else if (strstr(n, "DRAM") != NULL)
            dram_e_idx = i;
    }
    loop_adapt_print_datapoint(p);
    
    if (p->metrics[cyc_ldm_idx] > 10 && p->metrics[cyc_no_exec_idx] > 10)
    {
        direction = -1;
    }
    else if (p->metrics[cyc_ldm_idx] < 5 && p->metrics[cyc_no_exec_idx] < 5)
        direction = 1;
    
    free(p->metrics);
    for (int i=0; i<nmetrics; i++)
    {
        free(p->mnames[i]);
    }
    free(p->mnames);
    free(p);
    return 0;
}

int loop_adapt_init_policy_max_throughput()
{
    fprintf(stderr, "Init MAX_THROUGHPUT policy\n");
    return 0;
}
int loop_adapt_apply_policy_max_throughput(int step)
{
    fprintf(stderr, "Applying MAX_THROUGHPUT policy\n");
    return 0;
}

static int (*loop_adapt_policy_init_functions[LOOP_ADAPT_MAX_POLS]) () = {
    [LOOP_ADAPT_POL_NOTHING] = loop_adapt_init_policy_nothing,
    [LOOP_ADAPT_POL_DUMP] = loop_adapt_init_policy_dump,
    [LOOP_ADAPT_POL_MIN_ENERGY] = loop_adapt_init_policy_min_energy,
    [LOOP_ADAPT_POL_MAX_THROUGHPUT] = loop_adapt_init_policy_max_throughput,
};

static int (*loop_adapt_policy_apply_functions[LOOP_ADAPT_MAX_POLS]) (int step) = {
    [LOOP_ADAPT_POL_NOTHING] = loop_adapt_apply_policy_nothing,
    [LOOP_ADAPT_POL_DUMP] = loop_adapt_apply_policy_dump,
    [LOOP_ADAPT_POL_MIN_ENERGY] = loop_adapt_apply_policy_min_energy,
    [LOOP_ADAPT_POL_MAX_THROUGHPUT] = loop_adapt_apply_policy_max_throughput,
};

int loop_adapt_set_policy(LoopAdaptPolicy pol)
{
    if (pol < 0 || pol >= LOOP_ADAPT_MAX_POLS)
    {
        fprintf(stderr, "Unknown policy\n");
        return 1;
    }
    loop_adapt_current_policy = pol;
    return 0;
}

int loop_adapt_init_policy()
{
    return (*loop_adapt_policy_init_functions[loop_adapt_current_policy]) ();
}

int loop_adapt_apply_policy(int step)
{
    return (*loop_adapt_policy_apply_functions[loop_adapt_current_policy]) (step);
}
