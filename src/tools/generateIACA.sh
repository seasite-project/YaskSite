#!/bin/bash

#generates IACA of the given main Eqn group
# USAGE : ./generateIACA.sh <idxMainEqn> <stencilTmpFolder> <iaca_path> <arch> <outFolder>

#1. Make the assembly file IACA compatible
#2. insert IACA markers
#3. Compile the file
#4. Run IACA
#5. Write the result into a file

idxEqn=$1
stencilTmpFolder=$2
iacaPath=$3
arch=$4
outFolder=$5

#create outFolder if it doesn't exist
mkdir -p $outFolder

#Insert marker start to file
echo "movl     \$111, %ebx # INSERTED BY YASKSITE
.byte     100        # INSERTED BY YASKSITE
.byte     103        # INSERTED BY YASKSITE
.byte     144        # INSERTED BY YASKSITE">$stencilTmpFolder/assembly_iaca_comp.s

start_string="yask::StencilBundle_stencil_bundle_$((idxEqn-1))"
#extract the required region based on idx
awk "/${start_string}/,/total instrs/" $stencilTmpFolder/assembly.s>$stencilTmpFolder/extracted_assembly.s
#awk "/^${start_string}/,/^total instrs/{if(/^${start_string}/)n++;if(n==$idxEqn)print}" $stencilTmpFolder/assembly.s>$stencilTmpFolder/extracted_assembly.s
sed -e "0,/${start_string}/d" -e '/total instrs/,$d' $stencilTmpFolder/extracted_assembly.s>>$stencilTmpFolder/assembly_iaca_comp.s
#Insert marker end to file
echo "movl     \$222, %ebx # INSERTED BY YASKSITE
.byte     100        # INSERTED BY YASKSITE
.byte     103        # INSERTED BY YASKSITE
.byte     144        # INSERTED BY YASKSITE">>$stencilTmpFolder/assembly_iaca_comp.s

#now compile the assemblly
cc=mpiicpc
$cc -c $stencilTmpFolder/assembly_iaca_comp.s -o $stencilTmpFolder/assembly_iaca_comp.o

#now run IACA
iaca_exec=""
if [[ $iacaPath == "" ]]
then
    #assuming already in path
    iaca_exec="iaca.sh"
else
    iaca_exec="$iacaPath/iaca.sh"
fi

$iaca_exec -64 -arch $arch $stencilTmpFolder/assembly_iaca_comp.o >$outFolder/iaca_out.txt

#get Port names
str=$(grep -o "|  Port  |.*" $outFolder/iaca_out.txt | cut -f2 -d "|")
echo $str>$outFolder/iaca_port.txt

ctr=3
while [ "$str" != "" ]; do
    str=$(grep -o "|  Port  |.*" $outFolder/iaca_out.txt | cut -f$ctr -d "|")
    echo $str>>$outFolder/iaca_port.txt
    let ctr=$ctr+1
done

#get Cycle counts
str=$(grep -o "| Cycles |.*" $outFolder/iaca_out.txt | cut -f2 -d "|")
echo $str>$outFolder/iaca_cycle.txt

ctr=3
while [ "$str" != "" ]; do
    str=$(grep -o "| Cycles |.*" $outFolder/iaca_out.txt | cut -f$ctr -d "|")
    echo $str>>$outFolder/iaca_cycle.txt
    let ctr=$ctr+1
done

#get Block Throughput
grep -o -P "(?<=Block Throughput:).*(?=Cycles)" $outFolder/iaca_out.txt > $outFolder/iaca_blockThroughput.txt
