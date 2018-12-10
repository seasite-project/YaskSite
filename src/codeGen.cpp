#include "codeGen.h"
#include "util.h"
#include <stdio.h>

codeGen::codeGen(char* derivedStencil_, char* baseStencil_, char* genCode_):derivedStencil(derivedStencil_),baseStencil(baseStencil_), genCode(genCode_)
{
    STRINGIFY(tmpInpCodeFile, "%s/temp_YS_inpCode.txt", TEMP_DIR);
    generateCode();
}

codeGen::~codeGen()
{
    free(tmpInpCodeFile);
}

void codeGen::generateCode()
{
    FILE *file;
    file=fopen(tmpInpCodeFile, "w+");

    fprintf(file, "BASE_STENCIL=%s;\n", baseStencil);
    fprintf(file, "DERIVED_STENCIL=%s;\n", derivedStencil);
    //now input code
    fprintf(file,"%s", genCode);
    fclose(file);

    //now call script to generate the required stencil file
    SYSTEM(NULL, "%s/codeGenerator/codeGenerator.sh %s %s/%s.hpp %s %s %s", TOOL_DIR, tmpInpCodeFile, STENCIL_DIR, derivedStencil, STENCIL_DIR, TOOL_DIR, TEMP_DIR);

    //now update the list of stencils to include the new file
    SYSTEM(NULL, "%s/updateStencil.sh %s %s %s", TOOL_DIR, STENCIL_DIR, YASK_PATH, TEMP_DIR);
}
