#will convert all unit to lowest (for e.g. bytes)
#Usage: ./unitconvert.sh SIZE

SIZE=$1
val=$(echo $SIZE | grep -Eo "[+-]?[0-9]+([.][0-9]+)?")
unit=$(echo $SIZE | sed -e 's/[0-9]*//g' | sed -e 's/\.//g')

multiple=1
if [[ $unit = *"k"* || $unit = *"K"* ]]; then
    let multiple=1024
elif [[ $unit = *"m"* || $unit = *"M"* ]]; then
    let multiple=1024*1024
else
    let multiple=1024*1024*1024
fi

bytes=$(echo "$val*$multiple" | bc -l)
echo $bytes
