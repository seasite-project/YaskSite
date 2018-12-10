#ifndef LOOP_ADAPT_H
#define LOOP_ADAPT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <loop_adapt_types.h>
#include <loop_adapt_policies.h>

static size_t analyze_iters = 20;
static size_t adapt_iters_freq = 10;
static size_t adapt_iters_max_steps = 5;
static size_t adapt_num_threads = 0;

static void loop_adapt_set_analyze_iters(int iters)
{
    analyze_iters = iters;
}

static void loop_adapt_set_adapt_iters_freq(int freq)
{
    adapt_iters_freq = freq;
}

static void loop_adapt_set_adapt_iters_max_steps(int iters)
{
    adapt_iters_max_steps = iters;
}


void loop_adapt_init(int nr_cpus, int* cpus);
void loop_adapt_begin(size_t iter, size_t max_iter);
void loop_adapt_end(size_t iter, size_t max_iter);
void loop_adapt_close();

/* helpers */
void loop_adapt_pin(int cpu);



#ifdef __cplusplus
}
#endif

#endif /* LOOP_ADAPT_H */
