#ifndef YASKSITE_CODEGEN
#define YASKSITE_CODEGEN

#include "config.h"
#include "macros.h"

//Right now generates derived stencil from a base stencil
/*genCode example:
  "GRID k1,k2;
   GRID_POINT k3;
   PARAM dt;
   k1 = STENCIL(INP);
   k2 = k1 + dt*a(2,1)*STENCIL(k1);
   k3 = k1 + a(3,1)/a(2,1) * (k2-k1) + dt*a(3,2)*STENCIL(k2);
   OUT = INP + dt*(b(1)*k1 + b(2)*k2 + b(3)*k3);"
*/

class codeGen
{
    char *derivedStencil;
    char *baseStencil;
    char *genCode;
    char* tmpInpCodeFile;
    void generateCode();
    int numStencils;

    public:
    codeGen(char* derivedStencil_, char* baseStencil_, char* genCode_);
    ~codeGen();
};

//MACRO for easy code creation
#define CODIFY(derived, base, ...)\
{\
    char* codeStr;\
    STRINGIFY(codeStr, __VA_ARGS__);\
    codeGen code(derived, base, codeStr);\
    free(codeStr);\
}

#endif
