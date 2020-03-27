#include "codeGen.h"
#include "util.h"
#include <stdio.h>

std::vector<std::string> extraStencils;

codeGen::codeGen(char* derivedStencil_, char* baseStencil_, char* genCode_):derivedStencil(derivedStencil_),baseStencil(baseStencil_), genCode(genCode_)
{
    STRINGIFY(tmpInpCodeFile, "%s/temp_YS_inpCode.txt", TEMP_DIR);
    generateCode();
}

//just add the stencil_file to the list
codeGen::codeGen(char* stencil_file):tmpInpCodeFile(NULL)
{
    std::string stencilName = file_basename(stencil_file, ".hpp");
    extraStencils.push_back(stencilName);

    //It will overwrite if stencil in same name exists
    SYSTEM(NULL, "cp %s %s/.", stencil_file, STENCIL_RAW_DIR);
    //now update the list of stencils to include the new file
    SYSTEM(NULL, "mkdir -p %s/stencils_gen && %s/updateStencil.sh %s %s/stencils_gen", TEMP_DIR, TOOL_DIR, STENCIL_RAW_DIR, TEMP_DIR);
}

codeGen::~codeGen()
{
    if(tmpInpCodeFile)
    {
        free(tmpInpCodeFile);
    }
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

    extraStencils.push_back(std::string(derivedStencil));

    printf("script called\n");
    //now call script to generate the required stencil file
    SYSTEM(NULL, "%s/codeGenerator/codeGenerator.sh %s %s/%s.hpp %s %s %s", TOOL_DIR, tmpInpCodeFile, STENCIL_RAW_DIR, derivedStencil, STENCIL_RAW_DIR, TOOL_DIR, TEMP_DIR);
    printf("script finished\n");

    //now update the list of stencils to include the new file
    SYSTEM(NULL, "mkdir -p %s/stencils_gen && %s/updateStencil.sh %s %s/stencils_gen", TEMP_DIR, TOOL_DIR, STENCIL_RAW_DIR, TEMP_DIR);
}
