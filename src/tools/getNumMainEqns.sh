#!/bin/bash

# USAGE : ./getNumMainEqns.sh <stencilTmpFolder>

stencilTmpFolder=$1

#finding number of eqGroups
grep -c "Instr counts per instr" $stencilTmpFolder/assembly.s
