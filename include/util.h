#ifndef YASKSITE_UTIL
#define YASKSITE_UTIL

#include "config.h"
#include "types.h"
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
void systemCallUtil(char* cmd, char* sysLogFileName);
FILE* popenCallUtil(const char* cmd, const char* type, char* sysLogFileName);

char* removeSpaces(char* str);

int find_string(std::vector<const char*> arr, const char* val);

int readIntVar(FILE* filename);
double readDoubleVar(FILE* filename);
char* readStrVar(FILE* filename);


void dummy(void *tmp);

#if 0
template <typename T> inline bool readTable(FILE* file, std::vector<T> &table, int start_row, int end_row, int start_col, int end_col)
{
    //std::ifstream file(filename);
    //FILE *file;
    //file = fopen(filename, "r");

    if(!file)
    {
        printf("File could not be opened\n");
        return false;
    }
    int row_inf = (end_row<0)?true:false;
    int col_inf = (end_col<0)?true:false;

    //skip till start row
    for(int i=0; i<start_row; ++i)
    {
        //std::string line;
        //std::getline(file, line);
        //
        char *line = NULL;
        size_t n = 0;
        getline(&line, &n, file);
        free(line);
    }

    char* line = NULL;
    size_t n = 0;
    size_t linesize = getline(&line, &n, file);
    int i = start_row;
    while( (row_inf || (i<=end_row)) && (linesize!=0) ) //(std::getline(file,line)) )
    {
        std::istringstream line_stream(line);

        //skip till start_col
        for(int j=0; j<start_col; ++j)
        {
            std::string word;
            line_stream>>word;
        }

        int j=start_col;
        T word;

        while( (col_inf || (j<=end_col)) &&  (line_stream>>word) )
        {
            table.push_back(word);
            ++j;
        }

        ++i;

        linesize = getline(&line, &n, file);
    }

    return true;
}
#endif

template <typename T> inline bool readTable(const char* filename, std::vector<T> &table, int start_row, int end_row, int start_col, int end_col)
{
    std::ifstream file(filename);

    if(!file)
    {
        printf("File %s not found\n", filename);
        return false;
    }
    int row_inf = (end_row<0)?true:false;
    int col_inf = (end_col<0)?true:false;

    //skip till start row
    for(int i=0; i<start_row; ++i)
    {
        std::string line;
        std::getline(file, line);
    }

    std::string line;
    int i = start_row;
    while( (row_inf || (i<=end_row)) && (std::getline(file,line)) )
    {
        std::istringstream line_stream(line);

        //skip till start_col
        for(int j=0; j<start_col; ++j)
        {
            std::string word;
            line_stream>>word;
        }

        int j=start_col;
        T word;

        while( (col_inf || (j<=end_col)) &&  (line_stream>>word) )
        {
            table.push_back(word);
            ++j;
        }

        ++i;
    }
    return true;
}


std::vector<std::string> split(std::string str, char delim);
std::vector<double> split_double(std::string str, char delim);
std::vector<int> split_int(std::string str, char delim);


template <typename T> inline void sort(T *arr, int range_lo, int range_hi, bool rev=false)
{
    if(rev == false) {
        std::stable_sort(arr+range_lo,arr+range_hi);
    } else {
        std::stable_sort(arr+range_lo,arr+range_hi, [&](const int& a, const int& b) { return (arr[a] > arr[b]); });
    }
}

char* CAPITALIZE(char * temp);

char** str_split(char* a_str, const char a_delim);

//|------|--------------|
//x------z--------------y
double interpolate(double f_y, double f_x, double y, double x, double z);

int tolCeil(double num, double tol=0.24);

std::string file_basename(char const *path, char *ext=NULL);

#define DOUBLE(a)\
    static_cast<double>(a)


#endif
