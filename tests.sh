#!/bin/bash

if [ -z $1 ]; then
    echo 'make c-'
    make clean; make;
    echo 'Running ./c- on all the files in sampleOutput3'
    # for file in samplePrograms2/*.c-
    for file in sampleOutput3/*.c-
    do
        x=${file%.c-}
        y=${x##*/}
        # echo "./c- $file > myOutput/$y.out" 
        ./c- $file -p > myOutput/$y.out
    done

    echo -e "Complete. Files stored in myOutput/\n"

    echo 'Diffing outputs'
    for file in sampleOutput3/*.out
    do
        x=${file%.out}
        y=${x##*/}
        echo -e "\nsdiff -l -s myOutput/$y.out $file"
        sdiff -l -s myOutput/$y.out $file
        # rm myOutput/$y.out
    done
else
    ./c- sampleOutput3/$1.c- -p > myOutput/$1.out
    echo "sdiff myOutput/$1.out sampleOutput3/$1.out"
    sdiff myOutput/$1.out sampleOutput3/$1.out
fi
