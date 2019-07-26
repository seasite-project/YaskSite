#include <fstream>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>
#include "yaml-cpp/eventhandler.h"
#include "yaml-cpp/yaml.h"  // IWYU pragma: keep

class NullEventHandler : public YAML::EventHandler {
    public:
        void OnDocumentStart(const YAML::Mark&) override {}
        void OnDocumentEnd() override {}

        void OnNull(const YAML::Mark&, YAML::anchor_t) override {}
        void OnAlias(const YAML::Mark&, YAML::anchor_t) override {}
        void OnScalar(const YAML::Mark&, const std::string&, YAML::anchor_t,
                const std::string&) override {}

        void OnSequenceStart(const YAML::Mark&, const std::string&, YAML::anchor_t,
                YAML::EmitterStyle::value) override {}
        void OnSequenceEnd() override {}

        void OnMapStart(const YAML::Mark&, const std::string&, YAML::anchor_t,
                YAML::EmitterStyle::value) override {}
        void OnMapEnd() override {}
};

/* General variant will not work due to yaml-cpp's problem, as it uses reference
 * with '=' operator
#define YAML_READ(val, key)\
for(int i=1; i<key.size(); ++i)\
{\
val=val[key[i]];\
}\


#define YAML_WRITE(val, key)\
for(int i=1; i<key.size(); ++i)\
{\
if(val[key[i]])\
{\
val=val[key[i]];\
std::cout<<"found "<<key[i]<<"\n";\
}\
else\
{\
std::cout<<"not found "<<key[i]<<"\n";\
std::cout<<"before val = " << val << "\n";\
val[key[i]] = "";\
val=val[key[i]];\
std::cout<<"after val = " << val << "\n";\
}\
}\
*/

#define YAML_READ(val, doc, key)\
    switch(key.size()) \
{\
    case 1:\
           val=doc[key[0]];\
    break;\
    case 2:\
           val=doc[key[0]][key[1]];\
    break;\
    case 3:\
           val=doc[key[0]][key[1]][key[2]];\
    break;\
    case 4:\
           val=doc[key[0]][key[1]][key[2]][key[3]];\
    break;\
    case 5:\
           val=doc[key[0]][key[1]][key[2]][key[3]][key[4]];\
    break;\
    case 6:\
           val=doc[key[0]][key[1]][key[2]][key[3]][key[4]][key[5]];\
    break;\
    case 7:\
           val=doc[key[0]][key[1]][key[2]][key[3]][key[4]][key[5]][key[6]];\
    break;\
    case 8:\
           val=doc[key[0]][key[1]][key[2]][key[3]][key[4]][key[5]][key[6]][key[7]];\
    break;\
    case 9:\
           val=doc[key[0]][key[1]][key[2]][key[3]][key[4]][key[5]][key[6]][key[7]][key[8]];\
    break;\
    case 10:\
            val=doc[key[0]][key[1]][key[2]][key[3]][key[4]][key[5]][key[6]][key[7]][key[8]][key[9]];\
    break;\
    default:\
            printf("Error : Only 10 deep hierarchy supported\n");\
    exit(-1);\
}\


#define YAML_WRITE(doc, key, val)\
    switch(key.size()) \
{\
    case 1:\
           doc[key[0]]=YAML::Load(val);\
    break;\
    case 2:\
           doc[key[0]][key[1]]=YAML::Load(val);\
    break;\
    case 3:\
           doc[key[0]][key[1]][key[2]]=YAML::Load(val);\
    break;\
    case 4:\
           doc[key[0]][key[1]][key[2]][key[3]]=YAML::Load(val);\
    break;\
    case 5:\
           doc[key[0]][key[1]][key[2]][key[3]][key[4]]=YAML::Load(val);\
    break;\
    case 6:\
           doc[key[0]][key[1]][key[2]][key[3]][key[4]][key[5]]=YAML::Load(val);\
    break;\
    case 7:\
           doc[key[0]][key[1]][key[2]][key[3]][key[4]][key[5]][key[6]]=YAML::Load(val);\
    break;\
    case 8:\
           doc[key[0]][key[1]][key[2]][key[3]][key[4]][key[5]][key[6]][key[7]]=YAML::Load(val);\
    break;\
    case 9:\
           doc[key[0]][key[1]][key[2]][key[3]][key[4]][key[5]][key[6]][key[7]][key[8]]=YAML::Load(val);\
    break;\
    case 10:\
            doc[key[0]][key[1]][key[2]][key[3]][key[4]][key[5]][key[6]][key[7]][key[8]][key[9]]=YAML::Load(val);\
    break;\
    default:\
            printf("Error : Only 10 deep hierarchy supported\n");\
    exit(-1);\
}\



void read(YAML::Node doc, std::vector<std::string> key)
{
    try {
        //YAML::Node node = doc[key[0]];
        YAML::Node val;
        YAML_READ(val, doc, key);
        std::cout << val << "\n";
    } catch (const YAML::Exception& e) {
        std::cerr << e.what() << "\n";
    }
}


void write(YAML::Node doc, std::vector<std::string> key, std::string val)
{
    try {
        //  YAML::Node node = doc[key[0]];
        //  node = doc;
        YAML_WRITE(doc, key, val);
    } catch (const YAML::Exception& e) {
        std::cerr << e.what() << "\n";
    }
}

std::vector<std::string> split(char *str, char *delim)
{
    std::vector<std::string> vec_str;
    char *pch = strtok(str, delim);
    while (pch != NULL)
    {
        vec_str.push_back(std::string(pch));
        pch = strtok (NULL, delim);
    }

    return vec_str;
}

int main(int argc, char **argv)
{
    if(argc <= 2)
    {
        printf("Usage : %s [filename] [key value, semi-colon separated for deeper hierarchy] [-o optional value to be written]\n", argv[0]);
    }
    else
    {
        std::vector<std::string> key = split(argv[2], ";");
        std::ifstream file;
        file.open(argv[1]);
        YAML::Node doc = YAML::Load(file);

        if(argc == 3)
        {
            read(doc, key);
        }
        else if((argc > 4) && (strcmp(argv[3], "-o")==0))
        {

            write(doc, key, argv[4]);
            std::ofstream fout(argv[1]);
            fout << doc;
            fout.close();
        }

        file.close();
    }

    return 0;
}
