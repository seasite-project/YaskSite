#!/bin/bash

#USAGE ./codeGenerator.sh code.txt generatedFile.txt StencilDir toolDir tempDir template_file(if not default)

real_code_file=$1
dest_file=$2
generateDir=$3
toolDirBase=$4
toolDir="$toolDirBase/codeGenerator/"
tempDir=$5
code_file="$tempDir/tmp_ys_InpCode_dup.txt"
generated_file="$tempDir/generated_temp.hpp" #this is the generated file will only
                                             #be pushed to real destination if
#code_file lines would be deleted once its processing is over
cp $real_code_file $code_file
#1. Get Base class name
stencil_name=$($toolDir/getBtwString.sh "BASE_STENCIL=" ";" $code_file)
$toolDir/rmLine.sh "BASE_STENCIL=" $code_file
#check if it is a radius defined stencil
isRadius=$($toolDir/isStencilRadius.sh $stencil_name $generateDir "$toolDirBase")
default_template_file="$toolDir/template.h"

if [[ $isRadius == 1 ]]; then
    default_template_file="$toolDir/template_radius.h"
fi

#detect if the base stencil has radius
template_file=${6:-"$default_template_file"}


cp $template_file $generated_file
base_file_w_path=$($toolDir/getStencilFile.sh $stencil_name $generateDir)
base_file=`basename $base_file_w_path`
$toolDir/substitute.sh "BASE_STENCIL_FILE" "$base_file" $generated_file

base_class=$($toolDir/getStencilClass.sh $stencil_name $base_file_w_path)
$toolDir/substitute.sh "BASE_STENCIL" "$base_class" $generated_file

class=$($toolDir/getBtwString.sh "DERIVED_STENCIL=" ";" $code_file)
$toolDir/rmLine.sh "DERIVED_STENCIL=" $code_file

$toolDir/substitute.sh "DERIVED_STENCIL" "$class" $generated_file
#2. Grid generation
grids=$($toolDir/getObj.sh GRID $code_file)
$toolDir/rmLine.sh "GRID .*" $code_file

declare_code=""
define_code=""

for grid in $grids; do
    declare_code=$(printf "$declare_code Grid $grid;")
    define_code=$(printf "$define_code INIT_GRID_4D($grid, t, x, y, z);")
done

# Grid Array generation; they are simply unrolled
grid_arrays=$($toolDir/getObj.sh GRID_ARRAY $code_file)
$toolDir/rmLine.sh "GRID_ARRAY .*" $code_file

for grid_array in $grid_arrays; do
    grid_name=$(echo $grid_array | cut -d "[" -f 1-1)
    num_grids=$(echo $grid_array | cut -d "[" -f 2-2 | cut -d "]" -f 1-1 | grep -m1 -Eo '[0-9]*\.?[0-9]+')
    for ((idx=0; idx<$num_grids; ++idx)); do
        unrolled_grid=$grid_name"_"$idx
        grids="$grids $unrolled_grid"
        declare_code=$(printf "$declare_code Grid $unrolled_grid;")
        define_code=$(printf "$define_code INIT_GRID_4D($unrolled_grid, t, x, y, z);")
    done
done


$toolDir/substitute.sh "DECLARE_EXTRA_GRIDS" "$declare_code" $generated_file
$toolDir/substitute.sh "DEFINE_EXTRA_GRIDS" "$define_code" $generated_file


#3. Param generation
params=$($toolDir/getObj.sh PARAM $code_file)
$toolDir/rmLine.sh "PARAM .*" $code_file


declare_code=""
define_code=""

for param in $params; do
    declare_code=$(printf "$declare_code Param $param;")
    define_code=$(printf "$define_code INIT_PARAM($param);")
done

$toolDir/substitute.sh "DECLARE_EXTRA_PARAMS" "$declare_code" $generated_file
$toolDir/substitute.sh "DEFINE_EXTRA_PARAMS" "$define_code" $generated_file

gridPts=$($toolDir/getObj.sh GRID_POINT $code_file)
$toolDir/rmLine.sh "GRID_POINT .*" $code_file

# Grid Array generation; they are simply unrolled
gridPt_arrays=$($toolDir/getObj.sh GRID_POINT_ARRAY $code_file)
$toolDir/rmLine.sh "GRID_POINT_ARRAY .*" $code_file

for gridPt_array in $gridPt_arrays; do
    gridPt_name=$(echo $gridPt_array | cut -d "[" -f 1-1)
    num_gridPts=$(echo $gridPt_array | cut -d "[" -f 2-2 | cut -d "]" -f 1-1 | grep -m1 -Eo '[0-9]*\.?[0-9]+')
    for ((idx=0; idx<$num_gridPts; ++idx)); do
        unrolled_grid=$gridPt_name"_"$idx
        gridPts="$gridPts $unrolled_grid"
        declare_code=$(printf "$declare_code GridValue $unrolled_grid;")
    done
done

declare_code=""
for gridPt in $gridPts; do
    declare_code=$(printf "$declare_code GridValue $gridPt;")
done


$toolDir/substitute.sh "DECLARE_GRID_POINTS" "$declare_code" $generated_file



#Now UNROLL all FOR loops
#cat $code_file

sed -i 's@FOR*\(\([^]]*\)\)@for(\1)@g' $code_file

#Now find [a], and if 'a' is variable in for loop replace it with $a
for_var=$($toolDir/getBtwString.sh "for\(\(" "=" $code_file)
#not alpha; for example F[0]
sed -i 's@\[\([^][:alpha:]]*\)\]@\"_\1\"@g' $code_file
#not digit; for example F[l]
sed -i 's@\[\([^]]*\)\]@\"_\"$\1@g' $code_file

cp $toolDir/exec_code.sh $tempDir/.

exec_file=$tempDir/exec_code.sh
#put echo infront of all lines that do not have keywords: for, do or done
while IFS='' read -r line || [[ -n "$line" ]]; do
    forPresent=$(echo $line | grep -c "for((.*)")
    doPresent=$(echo $line | grep -c "do")
    donePresent=$(echo $line | grep -c "done")

    if [[ $forPresent == 0 ]] && [[ $doPresent == 0 ]] && [[ $donePresent == 0 ]]
    then
        echo "echo \"$line\"" >> $exec_file
    else
        echo "$line" >> $exec_file
    fi
done < $code_file

#Now unroll by executing the code
$exec_file > $code_file

#cat $code_file

containsElement () {
    local e="$2"
    match="$1"
    for el in $e; do
        if [[ "$el" == "$match" ]]
        then
            echo 1
            return 1;
        fi
    done
    echo 0
    return 0
}

#4. generate code
#awk '/#START_CODE#/{flag=1;next}/#END_CODE#/{flag=0}flag' $code_file > codePart.txt

code=""
var=""
operators="= + - * / ; ( )"
operators=("$operators")

let tmpCtr=0
while IFS='' read -r line || [[ -n "$line" ]]; do
    line=$(echo "$line" | tr -d ' ')

    #find STENCIL in line if present
    stencilPresent=$(echo $line | grep -c "STENCIL(.*)")

    if [[ $stencilPresent == 1 ]]
    then
        #generate code for stencil
        #1. extract STENCIL input
        stencilInp=$(echo $line | grep -o -P "STENCIL.*\(([^()]|(?R))*\)" | grep -o -P "(?<=\().*(?=\))")
        stencilInp_wo_t=$(echo $stencilInp | sed -e 's@<[^<>]*>@@g')
        #don't support macros like INP and OUT
        if [[ $stencilInp_wo_t == "DATA" ]]
        then
            stencilInp_wo_t="data"
        fi
        ts=$(echo $stencilInp | cut -d "<" -f 2 | cut -d ">" -f 1)
        #check if decimal
        if [[ $ts =~ ^-?[0-9]+$ ]]; then
            ts=$ts
        else
            echo "Error no time-step stated for Grid $stencilInp_wo_t"
        fi

        #2. generate CODE
        stencilCode="$base_class::boundaryHandling($stencilInp_wo_t,t+$ts,x,y,z, at_first_x,at_last_x, at_first_y, at_last_y, at_first_z, at_last_z);
        GridValue tmp_ys_$tmpCtr;
        $base_class::addPoints(tmp_ys_$tmpCtr,$stencilInp_wo_t,t+$ts,x,y,z);"
        code=$(echo "$code$stencilCode")

        #3. Replace STENCIL in line
        line=$(echo $line|sed -e "s/STENCIL(.*)/tmp_ys_$tmpCtr/g")
        let tmpCtr=$tmpCtr+1
    fi

    left=1
    var_wo_t=""
    for (( i=0; i<${#line}; i++ )); do
        word="${line:$i:1}"
        if [[ "$operators" =~ "$word" ]]
        then
            let nxtI=i+1;
            nxtWord="${line:$nxtI:1}"
            #substitute "=" with EQUALS if LHS in grids
            #TODO handle +=, -=, ....
            if [[ "$word" == "=" ]] && [[ "$nxtWord" != "=" ]] && [[ $(containsElement "$var_wo_t" "$grids") == 1 ]]
            then
                word=" EQUALS "
            fi

            #check if var is in grids
            if [[ $(containsElement "$var_wo_t" "$grids") == 1 ]]
            then
                #find time step
                ts=$(echo $var | cut -d "<" -f 2 | cut -d ">" -f 1)
                #check if decimal
                if [[ $ts =~ ^-?[0-9]+$ ]]; then
                    var=$(echo "$var_wo_t(t+$ts,x,y,z)")
                else
                    echo "Error no time-step stated for Grid $var_wo_t"
                fi
                #if [[ $left == "1" ]]; then
                #    var=$(echo "$var(t+1,x,y,z)")
                #else
                #    var=$(echo "$var(t,x,y,z)")
                #fi
            fi

            if [[ "$word" ==  " EQUALS " ]]; then
                left=0
            fi
            code=$(echo "$code$var$word")
            var="" #make empty again
            var_wo_t=""
        else
            var="$var$word"
            var_wo_t=$(echo $var | sed -e 's@<[^<>]*>@@g')
        fi
    done

done < $code_file

rm $code_file

#replace restover OUT and INP with appropriate grids
#code=$(echo $code | sed -e "s/INP/data(t,x,y,z)/g")
#code=$(echo $code | sed -e "s/OUT.*=/data(t+1,x,y,z) EQUALS /g")
#code=$(echo $code | sed -e "s/OUT/data(t+1,x,y,z)/g") #if there is any restover OUT
#"s@DATA.*<\(.*\)>@data(t+\1,x,y,z)@g"

code=$(echo $code | sed -e "s@DATA<\([[:digit:]]\)>=@data(t+\1,x,y,z) EQUALS @g")
code=$(echo $code | sed -e "s@DATA<\([[:digit:]]\)>@data(t+\1,x,y,z)@g")

unindentedTempCode="$tempDir/code_ys_temp.txt"
indentedTempCode="$tempDir/code_ys_temp_2.txt"

echo $code > $unindentedTempCode
cat $unindentedTempCode | tr \; \\n | while read line ; do if [[ line != "" ]]; then echo $line\;; fi; done > $indentedTempCode

FILE2=$(<$generated_file)
FILE1=$(<$indentedTempCode)

tmpIndentedFinalCode="$tempDir/tmp_ys_indentedFinalCode.hpp"
echo "${FILE2//"#GENERATED_CODE#"/$FILE1}" > $tmpIndentedFinalCode
rm "$unindentedTempCode" "$indentedTempCode" "$generated_file"
mv "$tmpIndentedFinalCode" "$generated_file"

if [[ -e $dest_file ]]
then
    echo "generated = $generated_file"
    echo "dest = $dest_file"
    #check if we need to update generated_file
    diff -w -q $generated_file $dest_file>/dev/null
    diffFlag=$(echo $?)
    if [[ $diffFlag == 1 ]] #if differ update
    then
        echo "Generated Code"
        mv $generated_file $dest_file
    elif [[ $diffFlag == 0 ]]
    then
        echo "Code not generated, found in cache"
    else
        echo "Error: Code could not be generated"
    fi
else
    cp $generated_file $dest_file
fi

rm -f $generated_file

#keep a copy in stencil directory for cache checking
#cp $dest_file $tempDir/stencils/.
#$toolDir/substitute.sh "GENERATED_CODE" "$code" $generated_file
