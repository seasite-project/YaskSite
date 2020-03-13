#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parse.h"

my_option::my_option(const char* name_, int has_arg_, int* flag_, int val_, char*desc_): desc(desc_)
{
    gnu_opt = {name_, has_arg_, flag_, val_};
}

my_option::my_option()
{
}

parser::parser():kernel(NULL), iter(100), cores(1), smt(1), innerDim(NULL), radius(1), fold("1:1:1"), dp(true), prefetch(false), path("default"), outDir(NULL), opt("plain:spatial:temporal"), prgname("a.out"), mcFile(NULL), numOptions(15)
{
    long_options = new my_option[numOptions+1];

    long_options[0]  = {"kernel",  required_argument, 0,  'k', "stencil kernel: dimension"};
    long_options[1]  = {"iter",    required_argument, 0,  'i', "Iterations to be carried out" };
    long_options[2]  = {"cores",   required_argument, 0,  'c', "Number of cores to be used" };
    long_options[3]  = {"smt",     required_argument, 0,  't', "Number of threads per core to be used (recommended 1)" };
    long_options[4]  = {"Range",   required_argument, 0,  'R', "Inner dimension range start:inc:end " };
    long_options[5]  = {"radius",  required_argument, 0,  'r', "stencil radius (only for stencils defined with radius)" };
    long_options[6]  = {"fold",    required_argument, 0,  'f', "Vector Folding fold_x:fold_y:fold_z" };
    long_options[7]  = {"sp",    required_argument, 0,  's', "Use single precision" };
    long_options[8]  = {"prefetch",no_argument,       0,  'P', "Enable prefetching" };
    long_options[9]  = {"path",    required_argument, 0,  'p', "Path valid values default or serpentine" };
    long_options[10]  = {"out",     required_argument, 0,  'o', "Output directory (if not provided output to stdout)" };
    long_options[11] = {"opt",     required_argument, 0,  'O', "Optimisations [options: plain,spatial,temporal]; use ':' for combining"};
    long_options[12] = {"mc_file", required_argument, 0,  'm', "Machine file for ECM prediction"};
    long_options[13] = {"help",    no_argument,       0,  'h', "Prints this help informations" };
    long_options[14] = {0,         0,                 0,   0 ,  0 };

    gnuOptions = new option[numOptions+1];

    for(int i=0; i<numOptions+1; ++i)
    {
        gnuOptions[i] = long_options[i].gnu_opt;
    }
}

parser::~parser()
{
    delete[] long_options;
    delete[] gnuOptions;
}

bool parser::parse_arg(int argc, char **argv)
{
    prgname = argv[0];
    while (1) {
        int option_index = 0, c;
        c = getopt_long(argc, argv, "0:k:i:c:t:R:r:f:p:o:O:m:hPs",
                gnuOptions, &option_index);

        if (c == -1)
            break;

        switch (c) {
            case 0:
                {
                    printf("No parameters specified.\n To see options use %s -h\n", prgname);
                    return false;
                    break;
                }
            case 'k':
                {
                    kernel = optarg;
                    break;
                }
            case 'i':
                {
                    iter = atoi(optarg);
                    break;
                }
            case 'c':
                {
                    printf("cores = %s\n", optarg);
                    cores = atoi(optarg);
                    break;
                }
            case 't':
                {
                    smt = atoi(optarg);
                    break;
                }
            case 'R':
                {
                    innerDim = optarg;
                    break;
                }
            case 'r':
                {
                    radius = atoi(optarg);
                    break;
                }
            case 'f':
                {
                    fold = optarg;
                    break;
                }
            case 'P':
                {
                    prefetch = true;
                    break;
                }
            case 's':
                {
                    dp = false;
                    break;
                }
            case 'p':
                {
                    path = optarg;
                    break;
                }
            case 'o':
                {
                    outDir = optarg;
                    break;
                }
            case 'O':
                {
                    opt = optarg;
                    break;
                }
            case 'm':
                {
                    mcFile = optarg;
                    break;
                }
            case 'h':
                {
                    help();
                    return 0;
                    break;
                }
            default:
                break;
        }
    }

    if(kernel==NULL)
    {
        printf("Please provide the kernel to evaluate \n\n");
        help();
        return false;
    }
    else
    {
        return true;
    }
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

std::vector<int> getRange(char* range)
{
    char* range_dup = strdup(range);
    char** splitted_range = str_split(range_dup, ':');

    std::vector<int> rangeVec;

    int i=0;
    while(splitted_range[i])
    {
        rangeVec.push_back(atoi(splitted_range[i]));
        ++i;
    }

    free(range_dup);
    free(splitted_range);
    return rangeVec;
}

std::vector<char*> splitChar(char* range)
{
    char* range_dup = strdup(range);
    char** splitted_range = str_split(range_dup, ':');

    std::vector<char*> rangeVec;

    int i=0;
    while(splitted_range[i])
    {
        rangeVec.push_back(splitted_range[i]);
        ++i;
    }

    free(range_dup);
    free(splitted_range);
    return rangeVec;
}

/*checks if a string is present in the vector and returns the idx*/
int findChar(std::vector<char*> vec, const char* str)
{
    int ret = -1;
    for(int i=0; i<((int)vec.size()); ++i)
    {
        if(strcmp(vec[i],str)==0)
        {
            ret = i;
        }
    }

    return ret;
}

void parser::help()
{
    printf("Usage: %s [OPTION]...\n",prgname);
    printf("Valid options are:\n\n");
    char* HLINE = "─────────────────────────────────────────────────────────────────────────────────────────────────";
    printf("%s\n",HLINE);
    printf("\t%s\t\t\t%s\n", "options", "description");
    printf("%s\n",HLINE);
    for(int i=0; i<numOptions; ++i)
    {
        char* long_opt;
        asprintf(&long_opt,"--%s", long_options[i].gnu_opt.name);
        printf("-%c or  %-12s |\t %-70s |\n", ((char) long_options[i].gnu_opt.val), long_opt, long_options[i].desc);
        free(long_opt);
    }

    printf("%s\n\n", HLINE);
    exit(0);
}
