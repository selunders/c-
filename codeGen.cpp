#include <stdio.h>

#include "globals.hpp"
#include "util.hpp"
#include "codeGen.hpp"
#include "emitcode.h"

using namespace std;

char *ioCode = (char *)"* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION input\n1:     ST  3,-1(1)	Store return address \n2:     IN  2,2,2	Grab int input \n3:     LD  3,-1(1)	Load return address \n4:     LD  1,0(1)	Adjust fp \n5:    JMP  7,0(3)	Return \n* END FUNCTION input\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION inputb\n6:     ST  3,-1(1)	Store return address \n7:    INB  2,2,2	Grab bool input \n8:     LD  3,-1(1)	Load return address \n9:     LD  1,0(1)	Adjust fp \n10:    JMP  7,0(3)	Return \n* END FUNCTION inputb\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION inputc\n11:     ST  3,-1(1)	Store return address \n12:    INC  2,2,2	Grab char input \n13:     LD  3,-1(1)	Load return address \n14:     LD  1,0(1)	Adjust fp \n15:    JMP  7,0(3)	Return \n* END FUNCTION inputc\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION output\n16:     ST  3,-1(1)	Store return address \n17:     LD  3,-2(1)	Load parameter \n18:    OUT  3,3,3	Output integer \n19:     LD  3,-1(1)	Load return address \n20:     LD  1,0(1)	Adjust fp \n21:    JMP  7,0(3)	Return \n* END FUNCTION output\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION outputb\n22:     ST  3,-1(1)	Store return address \n23:     LD  3,-2(1)	Load parameter \n24:   OUTB  3,3,3	Output bool \n25:     LD  3,-1(1)	Load return address \n26:     LD  1,0(1)	Adjust fp \n27:    JMP  7,0(3)	Return \n* END FUNCTION outputb\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION outputc\n28:     ST  3,-1(1)	Store return address \n29:     LD  3,-2(1)	Load parameter \n30:   OUTC  3,3,3	Output char \n31:     LD  3,-1(1)	Load return address \n32:     LD  1,0(1)	Adjust fp \n33:    JMP  7,0(3)	Return \n* END FUNCTION outputc\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION outnl\n34:     ST  3,-1(1)	Store return address \n35:  OUTNL  3,3,3	Output a newline \n36:     LD  3,-1(1)	Load return address \n37:     LD  1,0(1)	Adjust fp \n38:    JMP  7,0(3)	Return \n* END FUNCTION outnl\n";

FILE *code;
extern char *fileInName;
char *fileOutName;

FILE *createOutputFile(char *name)
{
    FILE *outputFile = fopen(name, "wb+");
    return outputFile;
}

void traverse(TreeNode *t, void (*preProc)(TreeNode *), void (*postProc)(TreeNode *))
{
    if (t != NULL)
    {
        preProc(t);
        {
            int i;
            for (i = 0; i < MAXCHILDREN; i++)
            {
                traverse(t->child[i], preProc, postProc);
            }
        }
        postProc(t);
        traverse(t->sibling, preProc, postProc);
    }
};

void PreCodeGeneration(TreeNode *t)
{
    if (t == NULL)
        return;
    switch (t->nodeKind)
    {
    case NodeKind::DeclK:
    {
        switch (t->subkind.decl)
        {
        case DeclKind::FuncK:
            emitComment((char *)"** ** ** ** ** ** ** ** ** ** ** **");
            emitComment((char *)"FUNCTION", t->attr.string);
            break;
        case DeclKind::ParamK:
            break;
        case DeclKind::VarK:
            break;
        }
    }
    break;
    case NodeKind::ExpK:
    {
    }
    break;
    case NodeKind::StmtK:
    {
    }
    break;
    }
}

void PostCodeGeneration(TreeNode *t)
{
    if (t == NULL)
        return;
    switch (t->nodeKind)
    {
    case NodeKind::DeclK:
    {
        switch (t->subkind.decl)
        {
        case DeclKind::FuncK:
            emitComment((char*)"END FUNCTION", t->attr.string);
            break;
        case DeclKind::ParamK:
            break;
        case DeclKind::VarK:
            break;
        }
    }
    break;
    case NodeKind::ExpK:
    {
    }
    break;
    case NodeKind::StmtK:
    {
    }
    break;
    }
}

void doCodeGen(TreeNode *root)
{
    printf("Starting Code Generation\n");
    char *baseFileName = strdup(fileInName);
    fileOutName = strcat(fileInName, ".tm");
    printf("Input file: %s.c-, Output File: %s.tm\n", baseFileName, baseFileName);
    code = createOutputFile(fileOutName);
    fprintf(code, "* C- compiler version F22\n");
    fprintf(code, "* File compiled: %s.c-\n* \n", baseFileName);
    fprintf(code, ioCode);

    if (root != NULL)
    {
        printf("\nDoing CodeGen traversal\n");
        traverse(root, PreCodeGeneration, PostCodeGeneration);
    }
    else
    {
        printf("\ntmpRoot is NULL\n");
    }
}