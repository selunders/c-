#!/usr/bin/env bash
allTests_flag=false
diff_flag=false
debug_flag=false
help_flag=false
make_flag=false
print_flag=false
printTypes_flag=false
justView_flag=false
fileIn=''

while getopts 'AdDhmpPf:v' flag; do
    case "${flag}" in
        A) allTests_flag=true ;;
        d) diff_flag=true ;;
        D) debug_flag=true ;;
        h) help_flag=true ;;
        m) make_flag=true ;;
        p) print_flag=true ;;
        P) printTypes_flag=true ;;
        f)
            fileIn="${OPTARG}" 
            echo "Setting fileIn to ${OPTARG}"
            ;;
        v) justView_flag=true ;;
    esac
done

if [ "$help_flag" = true ] ; then
    echo        "A) allTests_flag"
    echo        "d) diff_flag"
    echo        "D) debug_flag"
    echo        "h) Help"
    echo        "m) Make"
    echo        "p) Print AST"
    echo        "P) Print Typed AST"
    echo        "f) Usage: -f filename (without extension)"
    echo        "v) Usage: -v filename (without extension)"
elif [ "$allTests_flag" = true ]; then
    if [ "$make_flag" = true ]; then
        echo 'make c-'
        make clean; make;
    fi
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
        sdiff -l -s  myOutput/$y.out $file
        # rm myOutput/$y.out
    done
else
    if [ "$make_flag" = true ]; then
        echo 'make c-'
        make clean; make;
    fi
    if [ "$justView_flag" = true ]; then
        cat -n sampleOutput3/$fileIn.c-
    elif [ "$debug_flag" = true ]; then
        if [ "$printTypes_flag" = true ]; then
            ./c- sampleOutput3/$fileIn.c- -D -P
        elif [ "$print_flag" = true ]; then
            ./c- sampleOutput3/$fileIn.c- -D -p
        else
            ./c- sampleOutput3/$fileIn.c- -D
        fi
    elif [ "$diff_flag" = true ]; then
        ./c- sampleOutput3/$fileIn.c- -p > myOutput/$fileIn.out
        sdiff myOutput/$fileIn.out sampleOutput3/$fileIn.out
    else
        if [ "$printTypes_flag" = true ]; then
            ./c- sampleOutput3/$fileIn.c- -P
        elif [ "$print_flag" = true ]; then
            ./c- sampleOutput3/$fileIn.c- -p
        else
            ./c- sampleOutput3/$fileIn.c-
        fi
    fi
fi


# if [ -z $1 ]; then
#     echo 'make c-'
#     make clean; make;
#     echo 'Running ./c- on all the files in sampleOutput3'
#     # for file in samplePrograms2/*.c-
#     for file in sampleOutput3/*.c-
#     do
#         x=${file%.c-}
#         y=${x##*/}
#         # echo "./c- $file > myOutput/$y.out" 
#         ./c- $file -p > myOutput/$y.out
#     done

#     echo -e "Complete. Files stored in myOutput/\n"

#     echo 'Diffing outputs'
#     for file in sampleOutput3/*.out
#     do
#         x=${file%.out}
#         y=${x##*/}
#         echo -e "\nsdiff -l -s myOutput/$y.out $file"
#         sdiff -l -s  myOutput/$y.out $file
#         # rm myOutput/$y.out
#     done
# else
#     case $2 in
#     "-l")
#     make clean; make;
#     ./c- sampleOutput3/$1.c- -p > myOutput/$1.out
#     echo "sdiff myOutput/$1.out sampleOutput3/$1.out"
#     sdiff myOutput/$1.out sampleOutput3/$1.out
# fi
