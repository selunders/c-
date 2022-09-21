#!/bin/bash



if [ -z $1 ]; then
    echo 'make c-'
    make clean; make;
    echo -e "\n"
    echo "Assignment 2 files:"
    for file in samplePrograms2/*.c-
    do
        x=${file%.c-}
        y=${x##*/}
        echo 'Running ./c- on all the files in samplePrograms2'
        # echo "./c- $file > myOutput/$y.out" 
        ./c- $file > myOutput/$y.out
    done

    echo -e "Complete. Files stored in myOutput/\n"

    echo 'Diffing outputs'
    for file in samplePrograms2/*.out
    do
        x=${file%.out}
        y=${x##*/}
        echo -e "\nsdiff -l -s myOutput/$y.out $file"
        sdiff -l -s myOutput/$y.out $file
    done
else
    echo "sdiff myOutput/$1.out samplePrograms2/$1.out"
    sdiff myOutput/$1.out samplePrograms2/$1.out
fi