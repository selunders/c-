#!/bin/bash

echo "Assignment 1 Files:"
for file in samplePrograms/*.c-
do
    echo ./c- $file;
    ./c- $file
done

echo ""
echo "Assignment 2 files:"
for file in samplePrograms2/*.c-
do
    echo ./c- $file
    ./c- $file
done

echo ""
echo "Assignment 2 Error Files:"
for file in samplePrograms2_Errors/*.c-
do
    echo ./c- $file
    ./c- $file
done
