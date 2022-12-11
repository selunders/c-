#include <stdio.h>

#include "globals.hpp"
#include "util.hpp"
#include "codeGen.hpp"
#include "emitcode.h"
#include "c-.tab.h" // For operators
#include <map>

using namespace std;

extern map<OpKind, OpTypeInfo> opInfoMap;
void PreCodeGeneration(SymbolTable *, TreeNode *);
void PostCodeGeneration(SymbolTable *, TreeNode *, int);

int mainLocation = -1;
int tOffset = 0;
int globalsSize = 0;
extern int finalOffset;

bool inAssignment = false;

char *ioCode = (char *)"* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION input\n  1:     ST  3,-1(1)    Store return address \n  2:     IN  2,2,2      Grab int input \n  3:     LD  3,-1(1)    Load return address \n  4:     LD  1,0(1)     Adjust fp \n  5:    JMP  7,0(3)     Return \n* END FUNCTION input\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION output\n  6:     ST  3,-1(1)    Store return address \n  7:     LD  3,-2(1)    Load parameter \n  8:    OUT  3,3,3      Output integer \n  9:     LD  3,-1(1)    Load return address \n 10:     LD  1,0(1)     Adjust fp \n 11:    JMP  7,0(3)     Return \n* END FUNCTION output\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION inputb\n 12:     ST  3,-1(1)    Store return address \n 13:    INB  2,2,2      Grab bool input \n 14:     LD  3,-1(1)    Load return address \n 15:     LD  1,0(1)     Adjust fp \n 16:    JMP  7,0(3)     Return \n* END FUNCTION inputb\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION outputb\n 17:     ST  3,-1(1)    Store return address \n 18:     LD  3,-2(1)    Load parameter \n 19:   OUTB  3,3,3      Output bool \n 20:     LD  3,-1(1)    Load return address \n 21:     LD  1,0(1)     Adjust fp \n 22:    JMP  7,0(3)     Return \n* END FUNCTION outputb\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION inputc\n 23:     ST  3,-1(1)    Store return address \n 24:    INC  2,2,2      Grab char input \n 25:     LD  3,-1(1)    Load return address \n 26:     LD  1,0(1)     Adjust fp \n 27:    JMP  7,0(3)     Return \n* END FUNCTION inputc\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION outputc\n 28:     ST  3,-1(1)    Store return address \n 29:     LD  3,-2(1)    Load parameter \n 30:   OUTC  3,3,3      Output char \n 31:     LD  3,-1(1)    Load return address \n 32:     LD  1,0(1)     Adjust fp \n 33:    JMP  7,0(3)     Return \n* END FUNCTION outputc\n* \n* ** ** ** ** ** ** ** ** ** ** ** **\n* FUNCTION outnl\n 34:     ST  3,-1(1)    Store return address \n 35:  OUTNL  3,3,3      Output a newline \n 36:     LD  3,-1(1)    Load return address \n 37:     LD  1,0(1)     Adjust fp \n 38:    JMP  7,0(3)     Return \n* END FUNCTION outnl\n* \n";

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

static int siblingCount = 0;

void printDebug(TreeNode *t)
{
    printf("@%d:\ntOff:%d t.loc:%d t.size:%d\n", emitWhereAmI(), tOffset, t->location, t->size);
}

TreeNode *getFunctionDeclNode(SymbolTable *st, char *name)
{
    return (TreeNode *)st->lookup(name);
}

void markAsParams(TreeNode *t)
{
    int i = 1;
    while (t != NULL)
    {
        // printf("Marking %s as a param", t->attr.string);
        t->paramNum = i;
        t->isAParam = true;
        i++;
        t = t->sibling;
    }
}

void setupIO(SymbolTable *st)
{
    TreeNode *tmp = NULL;
    tmp = getFunctionDeclNode(st, (char *)"input");
    tmp->callLocation = 0;
    tmp = getFunctionDeclNode(st, (char *)"output");
    tmp->callLocation = 5;
    tmp = getFunctionDeclNode(st, (char *)"inputb");
    tmp->callLocation = 11;
    tmp = getFunctionDeclNode(st, (char *)"outputb");
    tmp->callLocation = 16;
    tmp = getFunctionDeclNode(st, (char *)"inputc");
    tmp->callLocation = 22;
    tmp = getFunctionDeclNode(st, (char *)"outputc");
    tmp->callLocation = 27;
    tmp = getFunctionDeclNode(st, (char *)"outnl");
    tmp->callLocation = 33;
}

FILE *createOutputFile(char *name)
{
    FILE *outputFile = fopen(name, "wb+");
    return outputFile;
}

void traverse(SymbolTable *st, TreeNode *t, void (*preProc)(SymbolTable *, TreeNode *), void (*postProc)(SymbolTable *, TreeNode *, int))
{
    int thisSibling = siblingCount;
    int tmp_toffset = tOffset;
    bool tmp_inAssignment = inAssignment;
    // printf("Storing offset as %d\n", tOffset);
    if (t != NULL)
    {
        if (t->seenByCodeGen)
            return;
        preProc(st, t);
        inAssignment = false;
        {
            int i;
            for (i = 0; i < MAXCHILDREN; i++)
            {
                traverse(st, t->child[i], preProc, postProc);
            }
        }
        inAssignment = tmp_inAssignment;
        postProc(st, t, tmp_toffset);
        siblingCount++;
        traverse(st, t->sibling, preProc, postProc);
        siblingCount = thisSibling;
        inAssignment = tmp_inAssignment;
    }
}

void PreCodeGeneration(SymbolTable *st, TreeNode *t)
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
            t->callLocation = emitWhereAmI() - 1;
            // if (!strcmp(t->attr.string, (char *)"main"))
            // {
            // mainLocation = emitWhereAmI()-1;
            // printf("Main is at %d\n", mainLocation);
            // }
            emitComment((char *)"** ** ** ** ** ** ** ** ** ** ** **");
            emitComment((char *)"FUNCTION", t->attr.string);
            tOffset = t->size;
            emitComment((char *)"TOFF set:", tOffset);
            emitRM((char *)"ST", AC, tOffset + 1, FP, (char *)"Store return address");
            // emitRM((char *)"ST", AC, -1, FP, (char *)"Store return address");
            // if(t->child[0] != NULL)
            // {
            // traverse(t->child[0]);
            // }
            break;
        case DeclKind::ParamK:
            emitComment((char *)"TOFF dec:", --tOffset);
            emitComment((char *)"Param", siblingCount + 1);
            if (t->referenceType == RefType::Global)
            {
                globalsSize -= t->size;
            }
            break;
        case DeclKind::VarK:
            if (t->referenceType == RefType::Global)
            {
                globalsSize -= t->size;
            }
            if (t->isArray)
            {
                emitRM((char *)"LDC", AC, t->size - 1, AC3, (char *)"load size of array", t->attr.string);
                emitRM((char *)"ST", AC, t->location + 1, t->referenceType == RefType::Global ? GP : FP, (char *)"save size of array", t->attr.string);
            }
            break;
        }
    }
    break;
    case NodeKind::ExpK:
    {
        if (t->isAParam)
        {
            // printf("Hey i'm a param (%s)\n", t->attr.string);
            // t->seenByCodeGen = true;
            emitComment((char *)"TOFF dec:", --tOffset);
            emitComment((char *)"Param", t->paramNum);
            // emitRM((char *)"LDC", AC, tOffset, FP, (char *)"Load variable", t->attr.string);
        }
        switch (t->subkind.exp)
        {
        case ExpKind::AssignK:
        {
            inAssignment = true;
            emitComment((char *)"EXPRESSION");
            TreeNode *leftChild = t->child[0];
            TreeNode *rightChild = t->child[1];
            switch (t->attr.op)
            {
            case '=':
            {
                if (leftChild->isArray)
                {
                    // emitComment((char*)"ARRAY");
                    traverse(st, leftChild, PreCodeGeneration, PostCodeGeneration);
                    leftChild->seenByCodeGen = true;
                    // tOffset--;
                    // emitComment((char *)"array = something");
                }
                // emitRM((char *)"LDC", AC, rightChild->attr.value, AC3, (char *)"Load integer constant");
                traverse(st, rightChild, PreCodeGeneration, PostCodeGeneration);
                rightChild->seenByCodeGen = true;
                // printf("var %s is %s\n", t->attr.string, refTypeToStr(t->referenceType));
                if (leftChild->isArray)
                {
                    emitComment((char *)"TOFF inc:", ++tOffset);
                    emitRM((char *)"LD", AC1, tOffset, FP, (char *)"Pop index");
                    emitRM((char *)"LDA", AC2, /*++tOffset*/ leftChild->child[0]->location, leftChild->child[0]->referenceType == RefType::Global ? GP : FP, (char *)"Load address of base of array", leftChild->child[0]->attr.string);
                    emitRO((char *)"SUB", AC2, AC2, AC1, (char *)"Compute offset of value");
                    // emitRM((char *)"ST", AC, leftChild->child[0]->referenceType == RefType::Global ? GP : FP, AC2, (char *)"Store variable", leftChild->child[0]->attr.string);
                    emitRM((char *)"ST", AC, leftChild->location, AC2, (char *)"Store variable", leftChild->child[0]->attr.string);
                    printDebug(leftChild);
                    printDebug(leftChild->child[0]);
                    // tOffset--;
                }
                else
                {
                    emitRM((char *)"ST", AC, leftChild->location, leftChild->referenceType == RefType::Global ? GP : FP, (char *)"Store variable", leftChild->attr.string);
                }
                // traverse(st, leftChild, PreCodeGeneration, PostCodeGeneration);
                leftChild->seenByCodeGen = true;
                break;
            }
            default:
                traverse(st, rightChild, PreCodeGeneration, PostCodeGeneration);
                rightChild->seenByCodeGen = true;
                emitRM((char *)"LD", AC1, leftChild->location, leftChild->referenceType == RefType::Global ? GP : FP, (char *)"load lhs variable", leftChild->attr.string);
                // printDebug(leftChild);
                switch (t->attr.op)
                {
                case ADDASS:
                    emitRO((char *)"ADD", AC, AC1, AC, (char *)"Op", (char *)"+=");
                    break;
                case SUBASS:
                    emitRO((char *)"SUB", AC, AC1, AC, (char *)"Op", (char *)"-=");
                    break;
                case MULASS:
                    emitRO((char *)"MUL", AC, AC1, AC, (char *)"Op", (char *)"*=");
                    break;
                case DIVASS:
                    emitRO((char *)"DIV", AC, AC1, AC, (char *)"Op", (char *)"/=");
                    break;
                }
                emitRM((char *)"ST", AC, leftChild->location, leftChild->referenceType == RefType::Global ? GP : FP, (char *)"Store variable", leftChild->attr.string);
                leftChild->seenByCodeGen = true;
                // emitRO((char *)"TEQ", AC, AC1, AC, (char *)"Op", (char *)"==");
                break;
            }
            break;
        }
        case ExpKind::CallK:
        {
            t->callLocation = emitWhereAmI() - 1;
            emitComment((char *)"EXPRESSION");
            emitComment((char *)"CALL", t->attr.string);
            emitRM((char *)"ST", FP, tOffset, FP, (char *)"Store fp in ghost frame for", t->attr.string);
            // tOffset = tOffset - 1;
            emitComment((char *)"TOFF dec:", --tOffset);

            markAsParams(t->child[0]);
            // tOffset = tOffset - 1;
            // emitComment((char *)"TOFF dec:", --tOffset);
            // emitComment((char *)"Param", 1);

            // const K

            // emitRM((char *)"ST", AC, tOffset, FP, (char *)"Push parameter");
        }

        break;
        case ExpKind::ConstantK:
            if (!t->seenByCodeGen)
            {

                switch (t->expType)
                {
                case ExpType::Boolean:
                    emitRM((char *)"LDC", AC, t->attr.value, AC3, (char *)"Load boolean constant");
                    break;
                case ExpType::Integer:
                    emitRM((char *)"LDC", AC, t->attr.value, AC3, (char *)"Load integer constant");
                    break;
                case ExpType::Char:
                    emitRM((char *)"LDC", AC, t->attr.cvalue, AC3, (char *)"Load character constant");
                    break;
                }
            }
            break;
        case ExpKind::IdK:
        {
            if (!t->seenByCodeGen)
            {
                // tOffset--;
                emitRM((char *)"LD", AC, t->location, t->referenceType == RefType::Global ? GP : FP, (char *)"Load variable", t->attr.string);
            }
            break;
        }
        case ExpKind::InitK:
            break;
        case ExpKind::OpK:
        {
            TreeNode *leftChild = t->child[0];
            TreeNode *rightChild = t->child[1];
            OpTypeInfo currentOp = opInfoMap[t->attr.op];
            if (currentOp.isUnary)
            {
                traverse(st, leftChild, PreCodeGeneration, PostCodeGeneration);
                leftChild->seenByCodeGen = true;
                // emitRM((char *)"ST", AC, tOffset, FP, (char *)"Push left side");
                // emitComment((char *)"TOFF dec:", --tOffset);
                // traverse(st, rightChild, PreCodeGeneration, PostCodeGeneration);
                // emitComment((char *)"TOFF inc:", ++tOffset);
                // rightChild->seenByCodeGen = true;
                // emitRM((char *)"LD", AC1, tOffset, FP, (char *)"Pop left into AC1");
                switch (t->attr.op)
                {
                case NEGATIVE:
                    emitRO((char *)"NEG", AC, AC, AC, (char *)"Op unary", (char *)"-");
                    break;
                case NOT:
                    emitRO((char *)"LDC", AC1, 1, AC3, (char *)"Load 1");
                    emitRO((char *)"XOR", AC, AC, AC1, (char *)"XOR to get logical not");
                    break;
                case '?':
                    emitRO((char *)"RND", AC, AC, AC3, (char *)"Op", (char *)"?");
                }
            }
            else if (t->attr.op == '[')
            {
                if (inAssignment)
                {
                    // emitComment("\n\n");
                    if (rightChild != NULL)
                    {
                        traverse(st, rightChild, PreCodeGeneration, PostCodeGeneration);
                        rightChild->seenByCodeGen = true;
                    }
                    leftChild->seenByCodeGen = true;
                    emitRM((char *)"ST", AC, tOffset, FP, (char *)"Push index");
                    emitComment((char *)"TOFF dec", --tOffset);
                    // traverse(st, leftChild, PreCodeGeneration, PostCodeGeneration);
                }
                else
                {
                    // emitComment((char *)"ArrayY");
                    emitRM((char *)"LDA", AC, leftChild->location, leftChild->referenceType == RefType::Global ? GP : FP, (char *)"Load address of base of array", leftChild->attr.string);
                    emitRM((char *)"ST", AC, tOffset, FP, (char *)"Push left side");
                    emitComment((char *)"TOFF dec:", --tOffset);
                    traverse(st, rightChild, PreCodeGeneration, PostCodeGeneration);
                    rightChild->seenByCodeGen = true;
                    emitComment((char *)"TOFF inc:", ++tOffset);
                    emitRM((char *)"LD", AC1, tOffset, FP, (char *)"Push left into ac1");
                    // emitRM((char *)"LD", AC1, leftChild->location, FP, (char *)"Push left into ac1");
                    emitRO((char *)"SUB", AC, AC1, AC, (char *)"compute location from index");
                    emitRM((char *)"LD", AC, t->location, AC, (char *)"Load array element");
                    // emitComment((char *)"TOFF dec:", --tOffset);

                    leftChild->seenByCodeGen = true;
                    /*
                    * TOFF inc: -4
                    58:     LD  4,-4(1)    Pop left into ac1
                    59:    SUB  3,4,3      compute location from index
                    60:     LD  3,0(3)     Load array element
                    */
                }
            }
            else
            {
                traverse(st, leftChild, PreCodeGeneration, PostCodeGeneration);
                leftChild->seenByCodeGen = true;
                emitRM((char *)"ST", AC, tOffset, FP, (char *)"Push left side");
                emitComment((char *)"TOFF dec:", --tOffset);
                traverse(st, rightChild, PreCodeGeneration, PostCodeGeneration);
                emitComment((char *)"TOFF inc:", ++tOffset);
                rightChild->seenByCodeGen = true;
                emitRM((char *)"LD", AC1, tOffset, FP, (char *)"Pop left into AC1");
                switch (t->attr.op)
                {
                case '+':
                    emitRO((char *)"ADD", AC, AC1, AC, (char *)"Op", (char *)"+");
                    break;
                case '/':
                    emitRO((char *)"DIV", AC, AC1, AC, (char *)"Op", (char *)"/");
                    break;
                case '<':
                    emitRO((char *)"TGT", AC, AC1, AC, (char *)"Op", (char *)"<");
                    break;
                case '>':
                    emitRO((char *)"TGT", AC, AC1, AC, (char *)"Op", (char *)">");
                    break;
                case EQ:
                    emitRO((char *)"TEQ", AC, AC1, AC, (char *)"Op", (char *)"==");
                    break;
                case LEQ:
                    emitRO((char *)"TGT", AC, AC1, AC, (char *)"Op", (char *)"<=");
                    break;
                case GEQ:
                    emitRO((char *)"TGT", AC, AC1, AC, (char *)"Op", (char *)">=");
                    break;
                case AND:
                    emitRO((char *)"AND", AC, AC1, AC, (char *)"Op", (char *)"AND");
                    break;
                case MODULO:
                    emitRO((char *)"MOD", AC, AC1, AC, (char *)"Op", (char *)"%");
                    break;
                case MULTIPLY:
                    emitRO((char *)"MUL", AC, AC1, AC, (char *)"Op", (char *)"*");
                    break;
                case OR:
                    emitRO((char *)"OR", AC, AC1, AC, (char *)"Op", (char *)"OR");
                    break;
                case SUBTRACT:
                    emitRO((char *)"SUB", AC, AC1, AC, (char *)"Op", (char *)"-");
                    break;
                }
            }

            //////////////////
            // Works for MUL

            // switch (t->attr.op)
            // {
            // case MULTIPLY:
            //     traverse(st, leftChild, PreCodeGeneration, PostCodeGeneration);
            //     leftChild->seenByCodeGen = true;
            //     emitRM((char *)"ST", AC, 0, FP, (char *)"Push left side");
            //     emitComment((char *)"TOFF dec:", --tOffset);
            //     traverse(st, rightChild, PreCodeGeneration, PostCodeGeneration);
            //     emitComment((char *)"TOFF inc:", ++tOffset);
            //     rightChild->seenByCodeGen = true;
            //     emitRM((char *)"LD", AC1, 0, FP, (char *)"Pop left into AC1");
            //     emitRM((char *)"MUL", AC, AC1, AC, (char *)"Op", (char *)"*");
            //     break;
            // default:
            //     break;
            // }

            //
            //////
            break;
        }
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
            tOffset = t->size;
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

void PostCodeGeneration(SymbolTable *st, TreeNode *t, int tmp_toffset)
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
            // tOffset = t->size;
            // emitComment((char *)"TOFF set", tOffset);
            emitComment((char *)"END FUNCTION", t->attr.string);
            break;
        case DeclKind::ParamK:
            emitRM((char *)"ST", AC, tOffset, FP, (char *)"Push parameter");
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
            emitComment((char *)"TOFF dec:", --tOffset);
            emitComment((char *)"Param end", t->attr.string);
            tOffset = tmp_toffset;
            TreeNode *tmp = getFunctionDeclNode(st, t->attr.string);
            emitRM((char *)"LDA", FP, tOffset, FP, (char *)"Ghost frame becomes new active frame");
            emitRM((char *)"LDA", AC, 1, PC, (char *)"Return address in ac");
            emitRM((char *)"JMP", PC, tmp->callLocation - emitWhereAmI(), PC, (char *)"CALL", t->attr.string);
            emitRM((char *)"LDA", AC, 0, RT, (char *)"Save the result in ac");
            emitComment((char *)"Call end", t->attr.string);
            emitComment((char *)"TOFF set:", tOffset);
            /*
            43:    LDA  1,-2(1)    Ghost frame becomes new active frame
            44:    LDA  3,1(7)     Return address in ac
            45:    JMP  7,-29(7)   CALL outputb
            46:    LDA  3,0(2)     Save the result in ac
            */
            break;
        }
        case ExpKind::OpK:
        {
            OpTypeInfo currOp = opInfoMap[t->attr.op];

            if (currOp.isUnary)
            {
            }
            else if (t->attr.op == '[')
            {
                if (inAssignment)
                {
                }
                else
                {
                    // emitComment((char *)"Yup that's it");
                }
                //     emitComment((char *)"TOFF inc", ++tOffset);
                //     emitRM((char *)"LD", AC1, tOffset, FP, (char *)"Pop index");
            }
            else
            {
            }
            break;
        }
        }
        if (t->isAParam)
        {
            // printf("Hey i'm still a param (%s)\n", t->attr.string);
            emitRM((char *)"ST", AC, tOffset, FP, (char *)"Push parameter");
        }
    }
    break;
    case NodeKind::StmtK:
    {
        switch (t->subkind.stmt)
        {
        case StmtKind::CompoundK:
            // tOffset = t->size;
            tOffset = tmp_toffset;
            emitComment((char *)"TOFF set:", tOffset);
            emitComment((char *)"END COMPOUND");
            break;
        }
    }
    break;
    }
}

void handleGlobalsAndStatics(TreeNode *t)
{
    if (t != NULL)
    {
        /////////////
        // Do Stuff
        if (t->nodeKind == NodeKind::DeclK && t->subkind.decl != DeclKind::FuncK)
        {
            RefType rt = t->referenceType;
            // if(rt == RefType::Global || rt == RefType::Static || rt == RefType::LocalStatic)
            if (rt == RefType::Global)
            {
                if (t->isArray)
                {
                    emitRM((char *)"LDC", AC, t->size - 1, AC3, (char *)"load size of array", t->attr.string);
                    emitRM((char *)"ST", AC, t->location + 1, GP, (char *)"save size of array", t->attr.string);
                }
                // printf("Add %d to globals\n", t->size);
                // printDebug(t);
            }
        }
        //
        /////

        int i = 0;
        for (i; i < MAXCHILDREN; i++)
        {
            handleGlobalsAndStatics(t->child[i]);
        }
        handleGlobalsAndStatics(t->sibling);
    }
}

void PostTraversal(SymbolTable *st, TreeNode *root)
{
    TreeNode *mainPtr = getFunctionDeclNode(st, (char *)"main");
    int i = emitWhereAmI();
    emitNewLoc(0);
    emitRM((char *)"JMP", PC, i - 1, PC, (char *)"Jump to init [backpatch]");
    emitNewLoc(i);

    emitComment((char *)"INIT");
    emitRM((char *)"LDA", FP, globalsSize, GP, (char *)"set first frame at end of globals");
    emitRM((char *)"ST", FP, 0, FP, (char *)"store old fp (point to self)");
    emitComment((char *)"INIT GLOBALS AND STATICS");
    handleGlobalsAndStatics(root);
    emitComment((char *)"END INIT GLOBALS AND STATICS");
    emitRM((char *)"LDA", AC, FP, PC, (char *)"Return address in ac");
    emitRM((char *)"JMP", PC, mainPtr->callLocation - emitWhereAmI(), PC, (char *)"Jump to main");
    emitRO((char *)"HALT", 0, 0, 0, (char *)"DONE!");
    emitComment((char *)"END INIT");

    // backPatchAJumpToHere(emitWhereAmI(), (char*)"");
}

void doCodeGen(SymbolTable *st, TreeNode *root)
{
    // printf("Starting Code Generation\n");

    // setup IO in SymbolTable
    setupIO(st);

    // create output .tm file
    char *baseFileName = strdup(fileInName);
    fileOutName = strcat(fileInName, ".tm");
    // printf("Loading file: %s.tm\n", baseFileName);
    // printf("Input file: %s.c-, Output File: %s.tm\n", baseFileName, baseFileName);
    code = createOutputFile(fileOutName);
    // set up file header
    fprintf(code, "* C- compiler version F22_lunders\n");
    fprintf(code, "* File compiled: %s.c-\n* \n", baseFileName);
    fprintf(code, ioCode);

    // do code gen
    if (root != NULL)
    {
        // printf("\nDoing CodeGen traversal\n");
        traverse(st, root, PreCodeGeneration, PostCodeGeneration);
        PostTraversal(st, root);
    }
    else
    {
        printf("\ntmpRoot is NULL\n");
    }
    // printf("Bye.\n");
}