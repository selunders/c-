#include <stdio.h>

#include "globals.hpp"
#include "util.hpp"
#include "codeGen.hpp"
#include "emitcode.h"

using namespace std;

void PreCodeGeneration(TreeNode *);
void PostCodeGeneration(TreeNode *);

int mainLocation = -1;

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
            if(!strcmp(t->attr.string, (char*)"main"))
            {
                mainLocation = emitWhereAmI();
                // printf("Main is at %d\n", mainLocation);
            }
            emitComment((char *)"** ** ** ** ** ** ** ** ** ** ** **");
            emitComment((char *)"FUNCTION", t->attr.string);
            emitComment((char *)"TOFF set:", t->location + t->size);
            emitRM((char*)"ST", AC, -1, -1, (char*) "Not sure what #s go here");
            // if(t->child[0] != NULL)
            // {
                // traverse(t->child[0]);
            // }
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
        switch (t->subkind.exp)
        {
        case ExpKind::AssignK:
            break;
        case ExpKind::CallK:
            break;
        case ExpKind::ConstantK:
            break;
        case ExpKind::IdK:
            break;
        case ExpKind::InitK:
            break;
        case ExpKind::OpK:
            break;
        }
    }
    break;
    case NodeKind::StmtK:
    {
        switch (t->subkind.stmt)
        {
        case StmtKind::BreakK:
            break;
        case StmtKind::CompoundK:
            emitComment((char*) "COMPOUND");
            emitComment((char*) "TOFF set:", t->location + t->size);
            emitComment((char*) "Compound Body");
            break;
        case StmtKind::ForK:
            break;
        case StmtKind::IfK:
        {
            // int rememberif;
            // traverse(t->child[0], PreCodeGeneration, PostCodeGeneration);
            // rememberif = emitSkip(1);                                     // one location
            // traverse(t->child[1], PreCodeGeneration, PostCodeGeneration); // do the 'then' part
            // backPatchAJumpToHere(rememberif, (char *)"Jump to ? [backpatch]");
            break;
        }
        case StmtKind::NullK:
            break;
        case StmtKind::RangeK:
            break;
        case StmtKind::ReturnK:
            break;
        case StmtKind::WhileK:
            break;
        }
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
            if(t->child[0] == NULL)
            {
                emitComment((char*) "Add standard closing in case there is no return statement");
                emitRM((char*)"LDC",RT,t->location,AC3, (char*)"Set return value to 0"); 
                emitRM((char*)"LD",AC,t->location-1,AC3, (char*)"Load return address"); 
                emitRM((char*)"LD",FP,t->location,FP, (char*)"Adjust fp");
                emitRM((char*)"JMP", RT,t->location,AC, (char*)"Return");
            }
            emitComment((char *)"END FUNCTION", t->attr.string);
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
        switch(t->subkind.stmt)
        {
            case StmtKind::CompoundK:
                emitComment((char*) "TOFF set:", t->location + t->size);
                emitComment((char*) "END COMPOUND");
            break;
        }
    }
    break;
    }
}

void PostTraversal()
{
    int i = emitWhereAmI();
    emitNewLoc(0);
    emitRM((char*)"JMP", PC, i,PC, (char*)"Jump to init [backpatch]");
    emitNewLoc(i);

    emitComment((char*)"INIT");
    emitRM((char*)"LDA", FP, GP, GP, (char*)"set first frame at end of globals");
    emitRM((char*)"ST", FP, 0, FP, (char*)"store old fp (point to self");
    emitComment((char*)"INIT GLOBALS AND STATICS");
    emitComment((char*)"END INIT GLOBALS AND STATICS");
    emitRM((char*)"LDA", AC, FP, PC, (char*)"Return address in ac");
    emitRM((char*)"JMP", PC, mainLocation-emitWhereAmI(), PC, (char*)"Jump to main");
    emitRM((char*)"HALT", 0, 0, 0, (char*)"DONE!");
    emitComment((char*)"END INIT");

    // backPatchAJumpToHere(emitWhereAmI(), (char*)"");
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
        PostTraversal();
    }
    else
    {
        printf("\ntmpRoot is NULL\n");
    }
}