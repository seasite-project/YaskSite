#ifndef LOOP_ADAPT_TYPES_H
#define LOOP_ADAPT_TYPES_H

static int loop_adapt_nr_cpus;
static int* loop_adapt_cpus;


typedef enum {
    LOOP_ADAPT_POL_NOTHING = 0,
    LOOP_ADAPT_POL_DUMP,
    LOOP_ADAPT_POL_MIN_ENERGY,
    LOOP_ADAPT_POL_MAX_THROUGHPUT,
    LOOP_ADAPT_MAX_POLS,
} LoopAdaptPolicy;


typedef struct {
    int step;
    int cpu;
    int tid;
    double runtime;
    int nr_metrics;
    double* metrics;
    char** mnames;
} DataPoint;

typedef struct {
    int nr_points;
    DataPoint** points;
} DataStore;


#endif /* LOOP_ADAPT_TYPES_H */
