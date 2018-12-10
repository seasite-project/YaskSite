#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "print.h"
#include "macros.h"

void systemCallUtil(char* cmd, char* sysLogFileName)
{
#ifdef LOG_SYS_CALL
    if(sysLogFileName)
    {
        FILE *sysCallLog;
        sysCallLog = fopen(sysLogFileName, "a");
        if(sysCallLog==NULL)
        {
            ERROR_PRINT("Could not open sys log file %s", sysLogFileName);
        }
        fprintf(sysCallLog, "%s\n", cmd);
        fclose(sysCallLog);
    }
#else
    UNUSED(sysLogFileName);
#endif

    system(cmd);
}

cache_info::cache_info(char* cache_str, int bytePerWord)
{
    //writable string
    char* cache_str_w = strdup(cache_str);
    char* byte_str = strtok(cache_str_w,";");
    char* shared_str = strtok(NULL,";");
    bytes=atof(byte_str);
    shared=(strstr(shared_str, "shared")!=NULL)?true:false;
    name=cache_str_w;
    words=bytes/bytePerWord;
    free(cache_str_w);
}

cache_info cache(char* str)
{
    if(strcmp(str,"L1")==0)
    {
        return L1;
    }
    else if(strcmp(str,"L2")==0)
    {
        return L2;
    }
    else
    {
        return L3;
    }
}

char* removeSpaces(char* str)
{
    char* str_cpy = strdup(str);
    char* i = str_cpy;
    while(*i != 0)
    {
       if(isspace(*i))
       {
           *i =  '_';
       }
       i++;
    }

    return str_cpy;
}
