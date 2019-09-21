#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <likwid.h>

#define DISABLE_PREFETCHER \
    printf("Disabling prefetcher\n");\
    cpuFeatures_disable(0, FEAT_HW_PREFETCHER, 0); \
    cpuFeatures_disable(0, FEAT_CL_PREFETCHER, 0); \
    cpuFeatures_disable(0, FEAT_DCU_PREFETCHER, 0); \
    cpuFeatures_disable(0, FEAT_IP_PREFETCHER, 0);

#define ENABLE_PREFETCHER \
    printf("Enabling prefetcher\n");\
    cpuFeatures_enable(0, FEAT_HW_PREFETCHER, 0); \
    cpuFeatures_enable(0, FEAT_CL_PREFETCHER, 0); \
    cpuFeatures_enable(0, FEAT_DCU_PREFETCHER, 0); \
    cpuFeatures_enable(0, FEAT_IP_PREFETCHER, 0);

void print_help(char* exec)
{
    printf("Usage: %s -b <size> -t <thread> (-r) (-d)\n", exec);
    printf("\n");
    printf("-b <size>\t: Size of arrays in KB, prefix m, g for MB, GB (default: 200MB)\n");
    printf("-t <threads>\t: Noise threads (avoid 0 since it is the latency thread, defalt(1-1))\n");
    printf("-r\t\t: Use randomly assigned indicies\n");
    printf("-d \t\t: Injection delay time in seconds (default 0s)\n");
    printf("-w \t\t: Read write ratio\n");
    printf("\n");
}

typedef struct
{
    char* nthreads;
    char* size;
    bool random;
    int delay;
    int w;
} options;

options* getcmdline(int argc, char* argv[])
{
    options* opts = (options*)malloc(sizeof(options));

    //default values
    opts->size = "200m";
    opts->nthreads = "1-1";
    opts->delay = 0;
    opts->random = false;
    opts->w = 21;

    int opt;
    while ((opt = getopt(argc, argv, "hb:t:rd:w:")) != -1) {
        switch (opt) {
            case 'h':
                print_help(argv[0]);
                exit(EXIT_SUCCESS);
            case 't':
                opts->nthreads = optarg;
                break;
            case 'r':
                opts->random = true;
                break;
            case 'b':
                opts->size = optarg;
                break;
            case 'd':
                opts->delay = atoi(optarg);
            case 'w':
                opts->w = atoi(optarg);
        }
    }
    return opts;
}


void main(int argc, char* argv[])
{
    cpuFeatures_init();

    options* opts = getcmdline(argc, argv);

    char* cmd;
    int readWrite = opts->w;
    int out = asprintf(&cmd, "./mlc --loaded_latency -e -W%d -b%s -k%s -d%d %s", readWrite, opts->size, opts->nthreads, opts->delay, (opts->random)?"-r":"");

    DISABLE_PREFETCHER;
    out = system(cmd);
    ENABLE_PREFETCHER;

    free(cmd);
    free(opts);
}
