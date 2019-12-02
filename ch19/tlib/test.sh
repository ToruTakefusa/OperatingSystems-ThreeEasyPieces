#!/bin/bash
i=0
while [ $i -lt 1000 ]
do
    i=`expr $i + 2`
    ./tlib $i 1 >> ./foo.txt
done
