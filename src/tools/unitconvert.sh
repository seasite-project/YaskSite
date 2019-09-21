#will convert all unit to lowest (for e.g. bytes)
#Usage: ./unitconvert.sh SIZE [optional scaling]

SIZE=$1
scaling=$2
if [[ $scaling == "" ]]; then
    scaling=1024
fi

val=$(echo $SIZE | grep -Eo "[+-]?[0-9]+([.][0-9]+)?")
unit=$(echo $SIZE | sed -e 's/[0-9]*//g' | sed -e 's/\.//g')
multiple=1
if [[ $unit = *"k"* || $unit = *"K"* ]]; then
    let multiple=$scaling
elif [[ $unit = *"m"* || $unit = *"M"* ]]; then
    let multiple=$scaling*$scaling
else
    let multiple=$scaling*$scaling*$scaling
fi

bytes=$(echo "$val*$multiple" | bc -l)
echo $bytes
