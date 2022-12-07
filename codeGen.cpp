#include<stdio.h>

#include"globals.hpp"
#include"codeGen.hpp"
#include"emitcode.h"

using namespace std;
extern TreeNode* tmpRoot;


FILE* code;
extern char* fileInName;
char* fileOutName;


FILE* createOutputFile(char* name){
    FILE* outputFile = fopen(name, "wb+");   
    return outputFile;
}

void doCodeGen(){
    printf("Starting Code Generation\n");
    char * baseFileName = strdup(fileInName);
    fileOutName = strcat(fileInName,".tm");
    printf("Input file: %s.c-, Output File: %s.tm\n", baseFileName, baseFileName);
    code = createOutputFile(fileOutName);
    fprintf(code, "* C- compiler version F22\n");
    fprintf(code, "* File compiled: %s.c-\n", baseFileName);

}