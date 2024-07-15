#!/bin/bash

for b in 10p80 11p00 11p20 11p40; do
    d=conf_nc4nf1_248_b${b}_m0p1000

    out=out_$d.txt
    echo -n "" > ${out}
    for f in $(ls ${d}/log.*|sort); do 
	tail -1 $f |awk '{print $4}' | sed 's/{(//g' | sed 's/)}//g' | sed 's/,/ /g'>> ${out}
    done
    echo -n $b
    awk '{sum+=$1; sumsq+=$1*$1; count+=1} END {avg=sum/count; stddev=sqrt(sumsq/count - avg*avg)/sqrt(count-1); printf(": %.4f(%.0f)\n", avg*(8**3),stddev*1e4*(8**3))}' $out
done
