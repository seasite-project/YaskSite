#ifndef YASKSITE_UTIL
#define YASKSITE_UTIL

#include "config.h"

void systemCallUtil(char* cmd, char* sysLogFileName);

struct cache_info
{
    double bytes;
    double words;
    char* name;
    bool shared;

    cache_info(char* cache_str, int bytePerWord=8);
};

static cache_info L1(L1_cache);
static cache_info L2(L2_cache);
static cache_info L3(L3_cache);

cache_info cache(char* str);
char* removeSpaces(char* str);

#endif
