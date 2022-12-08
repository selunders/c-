#include <stdio.h>

#include "globals.hpp"
#include "util.hpp"
#include "codeGen.hpp"
#include "emitcode.h"

using namespace std;

void PreCodeGeneration(TreeNode *);
void PostCodeGeneration(TreeNode *);

int mainLocation = -1;
int tOffset = -2;

char *ioCode = (char *)"* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION input\n1:     ST  3,-1(1)	Store return address \n2:     IN  2,2,2	Grab int input \n3:     LD  3,-1(1)	Load return address \n4:     LD  1,0(1)	Adjust fp \n5:    JMP  7,0(3)	Return \n* END FUNCTION input\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION inputb\n6:     ST  3,-1(1)	Store return address \n7:    INB  2,2,2	Grab bool input \n8:     LD  3,-1(1)	Load return address \n9:     LD  1,0(1)	Adjust fp \n10:    JMP  7,0(3)	Return \n* END FUNCTION inputb\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION inputc\n11:     ST  3,-1(1)	Store return address \n12:    INC  2,2,2	Grab char input \n13:     LD  3,-1(1)	Load return address \n14:     LD  1,0(1)	Adjust fp \n15:    JMP  7,0(3)	Return \n* END FUNCTION inputc\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION output\n16:     ST  3,-1(1)	Store return address \n17:     LD  3,-2(1)	Load parameter \n18:    OUT  3,3,3	Output integer \n19:     LD  3,-1(1)	Load return address \n20:     LD  1,0(1)	Adjust fp \n21:    JMP  7,0(3)	Return \n* END FUNCTION output\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION outputb\n22:     ST  3,-1(1)	Store return address \n23:     LD  3,-2(1)	Load parameter \n24:   OUTB  3,3,3	Output bool \n25:     LD  3,-1(1)	Load return address \n26:     LD  1,0(1)	Adjust fp \n27:    JMP  7,0(3)	Return \n* END FUNCTION outputb\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION outputc\n28:     ST  3,-1(1)	Store return address \n29:     LD  3,-2(1)	Load parameter \n30:   OUTC  3,3,3	Output char \n31:     LD  3,-1(1)	Load return address \n32:     LD  1,0(1)	Adjust fp \n33:    JMP  7,0(3)	Return \n* END FUNCTION outputc\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION outnl\n34:     ST  3,-1(1)	Store return address \n35:  OUTNL  3,3,3	Output a newline \n36:     LD  3,-1(1)	Load return address \n37:     LD  1,0(1)	Adjust fp \n38:    JMP  7,0(3)	Return \n* END FUNCTION outnl\n";

FILE *code;
extern char *fileInName;
char *fileOutName;

static int inputLocation = 0;
static int outputLocation = 5;
static int inputbLocation = 11;
static int outputblocation = 16;
static int inputcLocation = 22;
static int outputcLocation = 27;
static int outnlLocation = 33;

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
            if (!strcmp(t->attr.string, (char *)"main"))
            {
                mainLocation = emitWhereAmI();
                // printf("Main is at %d\n", mainLocation);
            }
            emitComment((char *)"** ** ** ** ** ** ** ** ** ** ** **");
            emitComment((char *)"FUNCTION", t->attr.string);
            emitComment((char *)"TOFF set:", tOffset);
            emitRM((char *)"ST", AC, -1, FP, (char *)"Store return address");
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
        {
            t->callLocation = emitWhereAmI();
            emitComment((char *)"EXPRESSION");
            emitComment((char *)"CALL", t->attr.string);
            emitRM((char *)"ST", FP, tOffset, FP, (char *)"Store fp in ghost frame for ", t->attr.string);
            // tOffset = tOffset - 1;
            emitComment((char *)"TOFF dec:", --tOffset);
            int i = 0;
            bool foundParam = false;
            for (i = 0; i < MAXCHILDREN; i++)
            {
                TreeNode *tmpChild = t->child[i];
                if (tmpChild != NULL)
                {
                    foundParam = true;
                    emitComment((char *)"TOFF dec:", --tOffset);
                    // tOffset = tOffset - 1;
                    emitComment((char *)"Param", i + 1);
                    switch (tmpChild->expType)
                    {
                    case ExpType::Boolean:
                        emitRM((char *)"LDC", AC, 0, AC3, (char *)"Load boolean constant");
                        break;
                    case ExpType::Integer:
                        emitRM((char *)"LDC", AC, 0, AC3, (char *)"Load integer constant");
                        break;
                    case ExpType::Char:
                        emitRM((char *)"LDC", AC, 0, AC3, (char *)"Load character constant");
                        break;
                        // emitRM((char *)"LDC", AC, 0, AC3, (char *)"Load string constant");
                    }
                    emitRM((char *)"ST", AC, tOffset, FP, (char *)"Push parameter");
                }
            }
            if (foundParam)
                emitComment((char *)"Param end", t->attr.string);
        }
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
        {
            emitComment((char *)"COMPOUND");
            emitComment((char *)"TOFF set:", tOffset);
            emitComment((char *)"Compound Body");
            // traverse(t->child[1], PreCodeGeneration, PostCodeGeneration);
            break;
        }
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
            if (t->child[0] == NULL)
            {
                emitComment((char *)"Add standard closing in case there is no return statement");
                emitRM((char *)"LDC", RT, 0, AC3, (char *)"Set return value to 0");
                emitRM((char *)"LD", AC, -1, FP, (char *)"Load return address");
                emitRM((char *)"LD", FP, 0, FP, (char *)"Adjust fp");
                emitRM((char *)"JMP", PC, 0, AC, (char *)"Return");
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
        switch (t->subkind.exp)
        {
        case ExpKind::CallK:
        {
            emitRM((char *)"LDA", FP, tOffset, FP, (char *)"Ghost frame becomes new active frame");
            emitRM((char *)"LDA", AC, 1, PC, (char *)"Return address in ac");
            emitRM((char *)"JMP", PC, outputblocation - emitWhereAmI(), PC, (char *)"CALL ", t->attr.string);
            emitRM((char *)"LDA", AC, 0, RT, (char *)"Save result in ac");
            /*
            43:    LDA  1,-2(1)    Ghost frame becomes new active frame
            44:    LDA  3,1(7)     Return address in ac
            45:    JMP  7,-29(7)   CALL outputb
            46:    LDA  3,0(2)     Save the result in ac
            */
            break;
        }
        }
    }
    break;
    case NodeKind::StmtK:
    {
        switch (t->subkind.stmt)
        {
        case StmtKind::CompoundK:
            emitComment((char *)"TOFF dec:", --tOffset);
            emitComment((char *)"END COMPOUND");
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
    emitRM((char *)"JMP", PC, i-1, PC, (char *)"Jump to init [backpatch]");
    emitNewLoc(i);

    emitComment((char *)"INIT");
    emitRM((char *)"LDA", FP, GP, GP, (char *)"set first frame at end of globals");
    emitRM((char *)"ST", FP, 0, FP, (char *)"store old fp (point to self");
    emitComment((char *)"INIT GLOBALS AND STATICS");
    emitComment((char *)"END INIT GLOBALS AND STATICS");
    emitRM((char *)"LDA", AC, FP, PC, (char *)"Return address in ac");
    emitRM((char *)"JMP", PC, mainLocation - emitWhereAmI(), PC, (char *)"Jump to main");
    emitRM((char *)"HALT", 0, 0, 0, (char *)"DONE!");
    emitComment((char *)"END INIT");

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