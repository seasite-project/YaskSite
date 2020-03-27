#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "print.h"
#include "macros.h"
#include <iostream>
#include <math.h>

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

FILE* popenCallUtil(const char* cmd, const char* type, char* sysLogFileName)
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

    return (popen(cmd, type));
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

int find_string(std::vector<const char*> arr, const char* val)
{
    for(int i=0; i<((int)arr.size()); ++i)
    {
        if(strcmp(arr[i],val)==0)
        {
            return i;
        }
    }

    return -1;
}

int readIntVar(FILE* filename)
{
    int var = 0;
    fscanf(filename, "%d", &var);

    return var;
}

double readDoubleVar(FILE* filename)
{
    float var=0;
    fscanf(filename, "%f", &var);

    return ((double)var);
}

char* readStrVar(FILE* filename)
{
    char* var = NULL;
    size_t n = 0;
    size_t chars = getline(&var, &n, filename);

    if ((var)[chars - 1] == '\n')
    {
        (var)[chars - 1] = '\0';
    }
    return var;
}


void dummy(void *tmp)
{
    UNUSED(tmp);
}

std::vector<std::string> split(std::string str, char delim)
{
    std::vector<std::string> out;

    std::istringstream str_stream(str);

    std::string word;
    while(std::getline(str_stream,word,delim))
    {
        out.push_back(word);
    }

    return out;
}

std::vector<double> split_double(std::string str, char delim)
{
    std::vector<double> out;

    std::istringstream str_stream(str);

    std::string word;
    while(std::getline(str_stream,word,delim))
    {
        out.push_back(atof(word.c_str()));
    }

    return out;
}

std::vector<int> split_int(std::string str, char delim)
{
    std::vector<int> out;

    std::istringstream str_stream(str);

    std::string word;
    while(std::getline(str_stream,word,delim))
    {
        out.push_back(atoi(word.c_str()));
    }

    return out;
}


char* CAPITALIZE(char * temp) {
    char* name = strdup(temp);

    // Convert to upper case
    char *s = name;
    while (*s) {
        *s = (char) toupper((unsigned char) *s);
        s++;
    }

    return name;
}

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    count += last_comma < (a_str + strlen(a_str) - 1);
    count++;

    result = (char **) malloc(sizeof(char*) * count);
    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            if(idx > count) {
                printf("Error in parsing\n");
            };
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        if(idx != count - 1) {
            printf("Error in parsing\n");
        };

        *(result + idx) = 0;
    }

    return result;
}

//|------|--------------|
//x------z--------------y
double interpolate(double f_y, double f_x, double y, double x, double z)
{
    return ( ( (z-x)*( (f_y-f_x)/(y-x) ) ) + f_x );
}

int tolCeil(double num, double tol)
{
    int highInt = static_cast<int>(num)+1;
    int lowInt = static_cast<int>(num);

    if((highInt-num) < tol)
    {
        num = highInt;
    }
    else if((num-lowInt) < tol)
    {
        num = lowInt;
    }
    else
    {
        num = ceil(num);
    }

    return static_cast<int>(num);
}

std::string file_basename(char const *path, char *ext)
{
    const char *s = strrchr(path, '/');
    char* base_str;
    if (!s)
        base_str = strdup(path);
    else
        base_str = strdup(s + 1);

    std::string out_str(base_str);
    free(base_str);

    if(ext)
    {
        //remove extension
        size_t pos = out_str.find(ext);

        if (pos != std::string::npos)
        {
            // If found then erase it from string
            out_str.erase(pos, std::string(ext).length());
        }
    }

    return out_str;
}
