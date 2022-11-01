#include <stdio.h>
#include "globals.hpp"
#include "symbolTable.hpp"
#include "analyze.hpp"
#include "c-.tab.h"
#include "util.hpp"
#include "errorMsg.hpp"
#include <map>
#include "colorPrint.hpp"

// #include <iostream>

using namespace std;

// extern int yytokentype;
extern int numErrors;
extern int numWarnings;

int numAnalyzeWarnings;
int numAnalyzeErrors;
int loopDepth = 0;
map<OpKind, OpTypeInfo> opInfoMap;
static int location = 0;
static void printAnalysis(SymbolTable *st, TreeNode *t, bool *enteredScope);

void InitOpTypeList()
{
    opInfoMap[AND] = OpTypeInfo(ExpType::Boolean, ExpType::Boolean, ExpType::Boolean, false, false, false, false, true);
    opInfoMap[OR] = OpTypeInfo(ExpType::Boolean, ExpType::Boolean, ExpType::Boolean, false, false, false, false, true);
    opInfoMap[EQ] = OpTypeInfo(ExpType::UndefinedType, ExpType::UndefinedType, ExpType::Boolean, true, false, false, true, true);
    opInfoMap[NEQ] = OpTypeInfo(ExpType::UndefinedType, ExpType::UndefinedType, ExpType::Boolean, true, false, false, true, true);
    opInfoMap['<'] = OpTypeInfo(ExpType::UndefinedType, ExpType::UndefinedType, ExpType::Boolean, true, false, false, true, true);
    opInfoMap[LEQ] = OpTypeInfo(ExpType::UndefinedType, ExpType::UndefinedType, ExpType::Boolean, true, false, false, true, true);
    opInfoMap['>'] = OpTypeInfo(ExpType::UndefinedType, ExpType::UndefinedType, ExpType::Boolean, true, false, false, true, true);
    opInfoMap[GEQ] = OpTypeInfo(ExpType::UndefinedType, ExpType::UndefinedType, ExpType::Boolean, true, false, false, true, true);
    opInfoMap['='] = OpTypeInfo(ExpType::UndefinedType, ExpType::UndefinedType, ExpType::Boolean, true, false, false, true, true);
    opInfoMap[ADDASS] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false, false);
    opInfoMap[SUBASS] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false, false);
    opInfoMap[MULASS] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false, false);
    opInfoMap[DIVASS] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false, false);
    opInfoMap['+'] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false, true);
    opInfoMap[SUBTRACT] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false, true);
    opInfoMap[MULTIPLY] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false, true);
    opInfoMap['/'] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false, true);
    opInfoMap[MODULO] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false, true);
    opInfoMap['['] = OpTypeInfo(ExpType::Array, ExpType::Integer, ExpType::LHS, false, false, true, true, false);
    // Unary
    opInfoMap[INC] = OpTypeInfo(ExpType::Integer, ExpType::Integer, false, false, false, false);
    opInfoMap[DEC] = OpTypeInfo(ExpType::Integer, ExpType::Integer, false, false, false, false);
    opInfoMap[NOT] = OpTypeInfo(ExpType::Boolean, ExpType::Boolean, false, false, false, false);
    opInfoMap[SIZEOF] = OpTypeInfo(ExpType::Array, ExpType::Integer, true, true, true, false);
    opInfoMap[NEGATIVE] = OpTypeInfo(ExpType::Integer, ExpType::Integer, false, false, false, false);
    opInfoMap['?'] = OpTypeInfo(ExpType::Integer, ExpType::Integer, false, false, false, false);
    // Array Operators
    // opInfoMap[NEGATIVE] = OpTypeInfo(ExpType::Array, ExpType::Integer, ExpType::LHS, false, false, true);
}

void InitIOToSymbolTable(SymbolTable *st)
{

    /*
        void output(int)
        void outputb(bool)
        void outputc(char)
        int input()
        bool inputb()
        char inputc()
        void outnl()
    */
}

static void checkUse(string, void *);

TreeNode *getSTNode(SymbolTable *st, TreeNode *t)
{
    if ((t->nodeKind == NodeKind::ExpK && (t->subkind.exp == ExpKind::IdK || t->subkind.exp == ExpKind::CallK || (t->subkind.exp == ExpKind::OpK && t->attr.op == '['))) || (t->nodeKind == NodeKind::DeclK))
    {
        if (t->subkind.exp == ExpKind::OpK && t->attr.op == '[')
            return (TreeNode *)st->lookup(t->child[0]->attr.string);
        return (TreeNode *)st->lookup(t->attr.string);
    }
    else
    {
        // printf("MyERROR(%d): Trying to look up an invalid node\n", t->lineno);
        return NULL;
    }
}

static void traverse(SymbolTable *st, TreeNode *t, void (*preProc)(SymbolTable *, TreeNode *, bool *), void (*postProc)(SymbolTable *, TreeNode *, bool *), bool doErrorChecking)
{
    // printf("Starting with %d errors, %d warnings.\n", numAnalyzeErrors, numAnalyzeWarnings);
    TreeNode *tmp;
    bool enteredScope = false;
    bool enteredFunction = false;
    bool enteredLoop = false;
    bool skipErrorChecking = false;
    if (t != NULL)
    {
        // if (!t->isInit)
        // t->isInit = true;
        // printf("%d node isInit:%d\n", t->lineno, t->isInit);
        // TreeNode *tmp;
        if (t->nodeKind == NodeKind::DeclK)
        {
            t->declDepth = st->depth();
            // t->needsInitCheck = true;
            // printf("%s %s\n", t->attr.string, t->isInit ? "is init" : "is not init");
            t->isInit = false;
            // printf("%s %s\n", t->attr.string, t->isInit ? "is init" : "is not init");
            // printf("%d", t->declDepth);
            switch (t->subkind.decl)
            {
            case DeclKind::FuncK:;
                tmp = (TreeNode *)st->lookup(t->attr.string);
                // if (tmp == NULL)
                st->insert(t->attr.string, t);
                if (doErrorChecking)
                {
                    if (tmp != NULL)
                    {
                        printf(getErrMsg(errSymAlreadyDecl), t->lineno, t->attr.string, tmp->lineno);
                        skipErrorChecking = true;
                        numAnalyzeErrors++;
                    }
                }
                break;
            case DeclKind::ParamK:
                if (!st->insert(t->attr.string, t) && doErrorChecking)
                {
                    tmp = getSTNode(st, t);
                    printf(getErrMsg(errSymAlreadyDecl), t->lineno, t->attr.string, tmp->lineno);
                    skipErrorChecking = true;
                    numAnalyzeErrors++;
                }
                break;
            case DeclKind::VarK:
                if (!st->insert(t->attr.string, t) && doErrorChecking)
                {
                    tmp = getSTNode(st, t);
                    printf(getErrMsg(errSymAlreadyDecl), t->lineno, t->attr.string, tmp->lineno);
                    skipErrorChecking = true;
                    numAnalyzeErrors++;
                    // t->needsInitCheck = true;
                }
                break;
            }
            // if (!st->insert(t->attr.string, t) && doErrorChecking)
            // {
            //     tmp = getSTNode(st, t);
            //     // if (!tmp->declChecked)
            //     // {
            //     // tmp->declChecked = true;
            //     printf(getErrMsg(errSymAlreadyDecl), t->lineno, t->attr.string, tmp->lineno);
            //     skipErrorChecking = true;
            //     numAnalyzeErrors++;
            //     // }
            // }
        }
        switch (t->nodeKind)
        {
        case NodeKind::DeclK:
            switch (t->subkind.decl)
            {
            case DeclKind::FuncK:
                st->enter(t->attr.string);
                enteredScope = true;
                if (t->child[1] != NULL)
                {
                    t->child[1]->canEnterThisScope = false;
                }
                // enteredFunction = true;
                break;
            case DeclKind::ParamK:
                break;
            case DeclKind::VarK:
                break;
            }
            break;
        case NodeKind::StmtK:
            // printf("Aaaaaaaaaaaaaaaaaaaaaaaaahhhhhhhhhhhhhhhhhhhhhh\n");
            switch (t->subkind.stmt)
            {
            case StmtKind::BreakK:
                break;
            case StmtKind::CompoundK:
                if (t->canEnterThisScope)
                {
                    // if (st->depth() > 2)
                    // st->leave();
                    st->enter((string) "Compound Statement");
                    enteredScope = true;
                }
                break;
            case StmtKind::ForK:
                st->enter((string) "For");
                enteredScope = true;
                if (t->child[2] != NULL)
                {
                    t->child[2]->canEnterThisScope = false;
                }

                enteredLoop = true;
                loopDepth++;
                break;
            case StmtKind::IfK:
                // t->expType = ExpType::Boolean;
                st->enter((string) "If");
                enteredScope = true;
                if (t->child[1] != NULL)
                {
                    t->child[1]->canEnterThisScope = false;
                }
                break;
            case StmtKind::NullK:
                break;
            case StmtKind::RangeK:
                break;
            case StmtKind::ReturnK:
                break;
            case StmtKind::WhileK:
                st->enter((string) "While");
                enteredScope = true;
                if (t->child[1] != NULL)
                {
                    t->child[1]->canEnterThisScope = false;
                }

                loopDepth++;
                enteredLoop = true;

                break;
            default:
                break;
            }
            break;
        case NodeKind::ExpK:
            switch (t->subkind.exp)
            {
            case ExpKind::IdK:
                t->isInit = false;
                break;
            }
            break;
        default:
            break;
        }
        preProc(st, t, &enteredScope);
        {
            int i;
            for (i = 0; i < MAXCHILDREN; i++)
            {
                traverse(st, t->child[i], preProc, postProc, doErrorChecking);
            }
        }
        if (!skipErrorChecking)
            postProc(st, t, &enteredScope);
        if (enteredScope)
        {
            if (doErrorChecking)
                st->applyToAll(checkUse);
            st->leave();
            // printf("Left scope\n");
            enteredScope = false;
        }
        if (enteredLoop)
            loopDepth--;
        traverse(st, t->sibling, preProc, postProc, doErrorChecking);
        // st->print(pointerPrintNode);
    }
    // printf("Ending with %d errors, %d warnings.\n", numAnalyzeErrors, numAnalyzeWarnings);
}

bool nodeIsConstant(SymbolTable *st, TreeNode *t)
{
    // printf("Checking if node is constant\n");
    // return true;
    TreeNode *tmp = NULL;
    if (t == NULL)
        return false;
    switch (t->nodeKind)
    {
    case NodeKind::DeclK:
    {
        TreeNode *tmp = (TreeNode *)st->lookup(t->attr.string);
        if (tmp != NULL)
        {
            return tmp->isConstantExp;
        }
        else
            return false;
    }
    break;
    case NodeKind::ExpK:
        switch (t->subkind.exp)
        {
        case ExpKind::AssignK:
            if (t->attr.op == '=')
            {
                if (nodeIsConstant(st, t->child[1]))
                {
                    if (tmp != NULL)
                        tmp->isConstantExp = true;
                    return true;
                }
            }
            // return nodeIsConstant(st, t->child[1]);
            else
                return true;
            break;
        case ExpKind::CallK:
            tmp = (TreeNode *)st->lookup(t->attr.string);
            if (tmp != NULL)
                return tmp->isConstantExp;
            else
                return false;
            break;
        case ExpKind::ConstantK:
            return true;
            break;
        case ExpKind::IdK:
            // return false;
            tmp = (TreeNode *)st->lookup(t->attr.string);
            if (tmp != NULL)
            {
                if (tmp->isConstantExp || t->isConstantExp)
                {
                    tmp->isConstantExp = true;
                    t->isConstantExp = true;
                }
                return tmp->isConstantExp;
                // return nodeIsConstant(st, tmp);
            }
            else
                return t->isConstantExp;
            break;
        case ExpKind::InitK:
            return false;
            break;
        case ExpKind::OpK:
        {

            if (t->attr.op == '[')
            {
                return (!isUnindexedArray(t->child[0]));
            }
            if (t->attr.op == '?')
            {
                // printf("found ?\n");
                t->isConstantExp = false;
                return t->isConstantExp;
            }
            OpTypeInfo currentOp = opInfoMap[t->attr.op];

            if (currentOp.isUnary)
            {
                return nodeIsConstant(st, t->child[0]);
            }
            else if ((t->child[0] != NULL && nodeIsConstant(st, t->child[0])) && (t->child[1] != NULL && nodeIsConstant(st, t->child[1])))
                return true;
            else
                // return currentOp.isConstantExpression;
                return false;
            // {
            // }
            break;
        }
        }
        break;
    case NodeKind::StmtK:
        return false;
        break;
    }
}

bool nodeIsInit(SymbolTable *st, TreeNode *t)
{
    // printf("Checking if node is constant\n");
    // return true;
    TreeNode *tmp = NULL;
    if (t == NULL)
        return false;
    switch (t->nodeKind)
    {
    case NodeKind::DeclK:
    {
        printf("I don't think you should be here (nodeisinit->switch->declk\n\n");
        TreeNode *tmp = (TreeNode *)st->lookup(t->attr.string);
        if (tmp != NULL)
        {
            return tmp->isInit;
        }
        else
            return false;
    }
    break;
    case NodeKind::ExpK:
        switch (t->subkind.exp)
        {
        case ExpKind::AssignK:
            if (t->attr.op == '=')
            {
                if (nodeIsInit(st, t->child[1]))
                {
                    // if (tmp != NULL)
                    // tmp->isInit = true;
                    return true;
                }
            }
            // return nodeIsConstant(st, t->child[1]);
            else
                return true;
            break;
        case ExpKind::CallK:
            tmp = (TreeNode *)st->lookup(t->attr.string);
            if (tmp != NULL)
                return tmp->isInit;
            else
                return false;
            break;
        case ExpKind::ConstantK:
            return true;
            break;
        case ExpKind::IdK:
            // if (t->isInit)
            // return true;
            tmp = getSTNode(st, t);
            // tmp = (TreeNode *)st->lookup(t->attr.string);
            if (tmp != NULL)
            {
                // if(t->isInit)
                // return true;
                return tmp->isInit;
                // if (tmp->isInit || t->isInit)
                // {
                // tmp->isInit = true;
                // t->isInit = true;
                // }
                // return nodeIsConstant(st, tmp);
                // return t->isInit;
            }
            else
                return false;
            // return t->isInit;
            break;
        case ExpKind::InitK:
            return false;
            break;
        case ExpKind::OpK:
        {

            if (t->attr.op == '[')
            {
                // t->child[0]->isInit;
                // return false;
                return (nodeIsInit(st, t->child[0]));
            }
            // if (t->attr.op == '?')
            // {
            // printf("found ?\n");
            // t->isInit = false;
            // return t->isInit;
            // }
            OpTypeInfo currentOp = opInfoMap[t->attr.op];

            if (currentOp.isUnary)
            {
                return nodeIsInit(st, t->child[0]);
            }
            else if ((t->child[0] != NULL && nodeIsInit(st, t->child[0])) && (t->child[1] != NULL && nodeIsConstant(st, t->child[1])))
                return true;
            else
                // return currentOp.isInitression;
                return false;
            // {
            // }
            break;
        }
        }
        break;
    // case NodeKind::StmtK:
    // return true;
    // break;
    default:
        return true;
        break;
    }
}

static void nullProc(SymbolTable *st, TreeNode *t, bool *enteredScope)
{
    if (t == NULL)
        return;
    else
        return;
}

static void printProc(SymbolTable *st, TreeNode *t, bool *enteredScope)
{
    pointerPrintNode(t);
    // pointerPrintStr(t);
    // pointerPrintAddr(t);
}

static void setUsed(SymbolTable *st, TreeNode *t, bool *placeholder)
{
    // For use in library functions
    // TreeNode* tmp = (TreeNode *)
    t->isUsed = true;
    t->lineno = -1;
}

static void checkUse(string str, void *t)
{
    TreeNode *tmp = (TreeNode *)t;
    if (!tmp->isUsed)
    {
        char *declKind;
        switch (tmp->subkind.decl)
        {
        case DeclKind::FuncK:
            declKind = (char *)"function";
            break;
        case DeclKind::ParamK:
            declKind = (char *)"parameter";
            break;
        case DeclKind::VarK:
            declKind = (char *)"variable";
            break;
        }
        printf(getWarnMsg(warnDeclNotUsed), tmp->lineno, declKind, tmp->attr.string);
        numAnalyzeWarnings++;
    }
}

ExpType getType(SymbolTable *st, TreeNode *t)
{
    switch (t->nodeKind)
    {
    case NodeKind::DeclK:
        switch (t->subkind.decl)
        {
        case DeclKind::FuncK:
            // printf("Function %s is of type %s\n", t->attr.string, expToString(t->expType));
            return t->expType;
            break;
        case DeclKind::ParamK:
            return t->expType;
            // return ExpType::UndefinedType;
            break;
        case DeclKind::VarK:
            return t->expType;
            break;
        }
        // return ExpType::UndefinedType;
        break;
    case NodeKind::StmtK:
        return ExpType::UndefinedType;
        break;
    case NodeKind::ExpK:
        switch (t->subkind.exp)
        {
        case ExpKind::AssignK:
            if (t->attr.op == '=')
            {
                t->expType = getType(st, t->child[0]);
                t->isArray = t->child[0]->isArray;

                // nodeIsConstant(t);
                return t->expType;
            }
            else
            {
                // t->isInit = t->child[0]->isInit;
                t->isArray = t->child[0]->isArray;
                return ExpType::Integer;
            }
            break;
        case ExpKind::CallK:
        {
            t->expType = ExpType::UndefinedType;
            TreeNode *tmp = (TreeNode *)st->lookup(t->attr.string);
            TreeNode *tmp_g = (TreeNode *)st->lookupGlobal(t->attr.string);
            if (tmp != NULL)
            {
                // t->expType = getType(st, tmp);
                // t->expType = tmp->expType;
                // printf("MyError(%d): Call %s returns type %s\n", t->lineno, t->attr.string, expToString(t->expType));
                return tmp->expType;
            }
            else if (tmp_g != NULL)
            {
                return tmp_g->expType;
            }
            else
            {
                t->expType = ExpType::UndefinedType;
                return ExpType::UndefinedType;
            }
        }
        break;
        case ExpKind::ConstantK:
            return t->expType;
            break;
        case ExpKind::IdK:
        {
            // t->needsInitCheck = true;
            TreeNode *tmp = (TreeNode *)st->lookup(t->attr.string);
            if (tmp != NULL)
            {
                t->isArray = tmp->isArray;
                // tmp->isArray = t->isArray;
                if (t->isConstantExp || tmp->isConstantExp)
                {
                    tmp->isConstantExp = true;
                    t->isConstantExp = true;
                }
                return tmp->expType;
            }
            else
                return ExpType::UndefinedType;
        }
        break;
        // case ExpKind::InitK
        case ExpKind::OpK:
            switch (t->attr.op)
            {
            case '[':
                return getType(st, t->child[0]);
                break;
            default:
            {
                OpTypeInfo currentOp = opInfoMap[t->attr.op];
                t->expType = currentOp.returnType;
                return currentOp.returnType;
                break;
            }
            }
            break;
        }
        break;
    }
}

static void moveUpTypes(SymbolTable *st, TreeNode *t, bool *enteredScope)
{
    // t->isInit = !t->isInit; // See 'for' loop in .y for explanation of this logic
    switch (t->nodeKind)
    {
    case NodeKind::DeclK:
        // if (t->isInit)
        // printf("%d  %s isInit:%d\n", t->lineno, t->attr.string, t->isInit);
        switch (t->subkind.decl)
        {
        case DeclKind::FuncK:
            break;
        case DeclKind::VarK:
        {
            // printf("%s Init depth: %d\n", t->attr.string, st->depth());
            TreeNode *tmp = (TreeNode *)st->lookup(t->attr.string);
            if (tmp != NULL && (st->depth() > 1))
            {
                // tmp->needsInitCheck = true;
                // t->needsInitCheck = true;
            }
            if (t->child[0] != NULL)
            {
                t->isInit = true;
                if (nodeIsConstant(st, t->child[0]))
                {
                    t->isConstantExp = true;
                }
            }
            break;
        }
        case DeclKind::ParamK:
            t->isInit = true;
            break;
        }
        break;
    case NodeKind::ExpK:
        switch (t->subkind.exp)
        {
        case ExpKind::AssignK:
        {
            TreeNode *tmp = NULL;
            if (t->attr.op == '=')
            {

                t->expType = getType(st, t);

                if (t->child[1] != NULL)
                {
                    // t->child[1]->needsInitCheck = true;
                    if (nodeIsConstant(st, t->child[1]))
                    {

                        t->isConstantExp = true;
                        if (tmp != NULL)
                        {
                            tmp->isConstantExp = true;
                        }
                    }
                }
                // printf("Checking init %d\n", t->lineno);
                if (t->child[1] != NULL)
                {
                    TreeNode *initTmp = NULL;
                    initTmp = getSTNode(st, t->child[1]);
                    // if (initTmp != NULL)
                    //     printf("Checking %s (%d)\n", initTmp->attr.string, t->lineno);

                    // Don't print undeclared errors
                    if (initTmp != NULL && initTmp->declDepth != 1)
                    {
                        // initTmp->needsInitCheck = false;
                        // t->child[1]->isInit = false;
                        t->child[1]->isInit = initTmp->isInit;
                        // if (!t->child[1]->isInit)
                        // printf("Is maybe init %d\n", t->lineno);
                        if (!nodeIsInit(st, t->child[1]))
                        {
                            // printf("MoveUpTypes: ");
                            printf(getWarnMsg(warnUninitVar), t->child[1]->lineno, initTmp->attr.string);
                            numAnalyzeWarnings++;
                            initTmp->isInit = true;
                            // t->child[1]->isInit = true;
                        }
                    }
                    else
                    {
                        // if()
                        // printf("Here's your problem %d\n", t->lineno);
                    }
                }
                // t->child[0]->needsInitCheck = false;
                TreeNode *childTmp = getSTNode(st, t->child[0]);
                if (childTmp != NULL)
                {
                    childTmp->isInit = true;
                    t->child[0]->isInit = true;
                    // printf("Hellooooo %s\n\n", childTmp->attr.string);
                }
            }
            break;
        }
        case ExpKind::CallK:
        {
            TreeNode *tmp = (TreeNode *)st->lookup(t->attr.string);
            TreeNode *tmp_g = (TreeNode *)st->lookupGlobal(t->attr.string);
            if (tmp != NULL)
            {
                t->expType = tmp->expType;
            }
            else if (tmp_g != NULL)
            {
                printf("Found global %s\n", tmp_g->attr.string);
                t->expType = tmp_g->expType;
            }
            else
                t->expType = ExpType::UndefinedType;
            // t->expType = getType(st, t);
            // printf("%d call %s is %s\n", t->lineno, t->attr.string, expToString(t->expType));
        }
        case ExpKind::ConstantK:
            t->isConstantExp = true;
            break;
        case ExpKind::IdK:
        {
            // printf("%d %s isInit %d\n", t->lineno, t->attr.string, t->isInit);
            // t->needsInitCheck = true;
            TreeNode *tmp = (TreeNode *)st->lookup(t->attr.string);
            TreeNode *tmp_g = (TreeNode *)st->lookupGlobal(t->attr.string);
            if (tmp != NULL)
            {
                t->isArray = tmp->isArray;
                t->isStatic = tmp->isStatic;
                if (tmp->isConstantExp || t->isConstantExp)
                {
                    tmp->isConstantExp = true;
                    t->isConstantExp = true;
                }
            }
            else if (tmp_g != NULL)
            {
                // printf("Found global %s\n", tmp_g->attr.string);
                t->isArray = tmp->isArray;
                t->isStatic = tmp->isStatic;
                t->isConstantExp = tmp->isConstantExp;
            }
            t->expType = getType(st, t);
            t->isConstantExp = true;
            break;
        }
        case ExpKind::OpK:
        {
            OpTypeInfo currentOp = opInfoMap[t->attr.op];
            if (t->attr.op == '[')
            {
                if (t->child[0] != NULL)
                {
                    TreeNode *tmp = (TreeNode *)st->lookup(t->child[0]->attr.string);
                    if (tmp != NULL)
                    {
                        t->expType = getType(st, t);
                        t->isArray = tmp->isArray;

                        if (tmp->isConstantExp)
                        {
                            t->isConstantExp = true;
                        }
                        else if (t->child[1] != NULL && t->child[1]->isConstantExp)
                        {
                            tmp->isConstantExp = true;
                            t->isConstantExp = true;
                        }
                    }
                }
                if (t->child[1] != NULL)
                    t->isIndexed = true;
            }
            if (t->attr.op == NOT)
            {
                t->expType = ExpType::Boolean;
            }
            if (!currentOp.isUnary)
            {
                if (nodeIsConstant(st, t->child[0]) || nodeIsConstant(st, t->child[1]))
                    t->isConstantExp = true;
            }
            else
            {
                t->isConstantExp = nodeIsConstant(st, t);
            }
        }
        default:
            break;
        }
    case NodeKind::StmtK:
    {
        switch (t->subkind.stmt)
        {
        case StmtKind::ForK:
        {
            TreeNode *tmp = NULL;
            if (t->child[0] != NULL)
            {
                t->child[0]->isInit = true;
                if (t->child[0]->nodeKind == NodeKind::DeclK)
                    tmp = (TreeNode *)st->lookup(t->child[0]->attr.string);
            }
            if (tmp != NULL)
            {
                tmp->isInit = true;
            }
            tmp = getSTNode(st, t->child[0]);
            if (tmp != NULL)
                tmp->isInit = true; 
        }
        break;
        case StmtKind::ReturnK:
        {

            if (t->child[0] != NULL)
                t->expType = getType(st, t->child[0]);
            else
                t->expType = ExpType::UndefinedType;
            // t->expType = ExpType::Void;
            break;
        }
        case StmtKind::RangeK:
        {
            int i = 0;
            while (i < MAXCHILDREN)
            {
                if (t->child[i] != NULL)
                    t->child[i]->expType = getType(st, t->child[i]);
                // printf("Child %d is type %s\n", i, expToString(t->child[i]->expType));
                i++;
                // printf("Test\n\n");
            }
        }
        default:
            break;
        }
        break;
    }
    }
}

static void analyzeChildren(SymbolTable *st, TreeNode *t)
{
    bool enteredScope;
    int i = 0;
    while (t != NULL)
    {
        t = t->child[i];
        printAnalysis(st, t, &enteredScope);
        if (enteredScope)
        {
            st->leave();
            enteredScope = false;
        }
        i++;
    }
}
static void analyzeSiblings(SymbolTable *st, TreeNode *t)
{
}

static void usageCheck(SymbolTable *st, TreeNode *t, bool *null)
{
    switch (t->nodeKind)
    {
    case NodeKind::DeclK:
        switch (t->subkind.decl)
        {
        case DeclKind::FuncK:
            break;
        case DeclKind::ParamK:
            break;
        case DeclKind::VarK:
        {
            TreeNode *tmp = (TreeNode *)st->lookup(t->attr.string);
            if (!tmp->finalCheckDone)
            {
                if (!tmp->isUsed)
                {
                    printf("You shouldn't be here?");
                    // printf(getWarnMsg(), t->lineno, t->attr.string);
                    // printf(getwarn, t->lineno, t->attr.string);
                    numAnalyzeWarnings++;
                }
                tmp->finalCheckDone = true;
            }
            break;
        }
        }
        break;
    case NodeKind::ExpK:
        break;
    case NodeKind::StmtK:
        break;
    }
}

static void printAnalysis(SymbolTable *st, TreeNode *t, bool *enteredScope)
{
    TreeNode *tmp;
    TreeNode *tLeft = t->child[0];
    TreeNode *tRight = t->child[1];
    switch (t->nodeKind)
    {
    case NodeKind::DeclK:
    {
        // printf("Aaaaaahhhhhh\n");
        tmp = (TreeNode *)st->lookup(t->attr.string);
        // if (tmp != NULL)
        // {
        // printf(getErrMsg(errSymAlreadyDecl), t->lineno, t->attr.string, tmp->lineno);
        // numAnalyzeErrors++;
        // }
        switch (t->subkind.decl)
        {
        case DeclKind::FuncK:
            doReturnTypeCheck(&numAnalyzeErrors, &numAnalyzeWarnings, t, tmp);
            break;
        case DeclKind::ParamK:
        {
            if (tmp != NULL)
            {
                // tmp->needsInitCheck = false;
            }
            // t->needsInitCheck = false;
            break;
        }
        case DeclKind::VarK:
            if (tLeft && tLeft->nodeKind == NodeKind::ExpK)
                switch (tLeft->subkind.exp)
                {
                case ExpKind::AssignK:
                    break;
                case ExpKind::CallK:
                    break;
                case ExpKind::ConstantK:
                    t->isConstantExp = true;
                    break;
                case ExpKind::IdK:
                    tmp = getSTNode(st, tLeft);
                    // tmp->isInit = true;
                    break;
                case ExpKind::InitK:
                    break;
                case ExpKind::OpK:
                    // if(st->depth() == 1)
                    // t->isInit = true;
                    break;
                }
            if (t->child[0] != NULL)
            {

                // if (nodeIsConstant(st, t))
                if (nodeIsConstant(st, t->child[0]))
                {
                    // do nothing
                }
                else
                {
                    ////////
                    // NOTE
                    // This should be combined with the other assignment checking
                    // printf("Shouldn't be here? %d %s\n\n", t->lineno, t->attr.string);
                    printf(getErrMsg(errInitNotConst), t->lineno, t->attr.string);
                    numAnalyzeErrors++;
                    // printf(getWarnMsg(warnUnintVar), t->lineno, t->attr.string);
                    // numAnalyzeWarnings++;
                }
                // Initialized correctly?
                // printf("ERROR(%d): Initializer for variable '%s' is a constant expression.\n", t->lineno, t->attr.string);
                if (t->expType != t->child[0]->expType)
                {
                    printf(getErrMsg(errWrongInitType), t->lineno, t->attr.string, expToString(t->expType), expToString(t->child[0]->expType));
                    numAnalyzeErrors++;
                }

                // if (isUnindexedArray(t) && )
                // if(t->isArray != !isUnindexedArray(t->child[0]))
                if (t->isArray != t->child[0]->isArray)
                {
                    // printf("That's fine\n");
                    printf(getErrMsg(errInitBothArr), t->lineno, t->attr.string, t->isArray ? " " : " not ", t->child[0]->isArray ? " " : " not ");
                    numAnalyzeErrors++;
                }
            }
            break;
        }
        break;
    }
    case NodeKind::ExpK:
        // printf("Aaaaaaaaaaahhhhhhh\n");
        switch (t->subkind.exp)
        {
        case ExpKind::AssignK:
        {
            OpTypeInfo currentOp = opInfoMap[t->attr.op];
            if (t->attr.op == '=')
            {
                if (nodeIsConstant(st, t->child[1]))
                {
                    // Initialized correctly?
                    // printf("ERROR(%d): Initializer for variable '%s' is a constant expression.\n", t->lineno, t->child[0]->attr.string);
                }
                else
                {
                    ////////////
                    // WARNING
                    //   The current way this is set up, it doesn't check for arrays on the left.
                    if (t->child[0] != NULL && t->child[0]->nodeKind == NodeKind::ExpK && t->child[0]->subkind.exp == ExpKind::IdK)
                    {
                        printf(getErrMsg(errInitNotConst), t->lineno, t->child[0]->attr.string);
                        numAnalyzeErrors++;
                    }
                }
                if (t->child[0] != NULL && t->child[0]->nodeKind == NodeKind::ExpK && t->child[0]->subkind.exp == ExpKind::OpK && t->child[0]->attr.op == '[')
                {
                    // t->child[0]->child[0]->isInit = true;
                    // t->child[0]->child[0]->needsInitCheck = false;
                }
                if (t->expType != t->child[0]->expType)
                {
                    printf(getErrMsg(errWrongInitType), t->lineno, t->attr.string, expToString(t->expType), expToString(t->child[0]->expType));
                    numAnalyzeErrors++;
                }
                TreeNode *lhTmp = NULL;
                // if (t->child[0]->needsInitCheck)
                // lhTmp = (TreeNode *)st->lookup(t->child[0]->attr.string);
                lhTmp = getSTNode(st, t->child[0]);
                if (lhTmp != NULL)
                {
                    lhTmp->isInit = true;
                    // lhTmp->needsInitCheck = false;
                }
                t->child[0]->isInit = true;
                // t->child[0]->needsInitCheck = false;
                if (t->child[0]->child[0] != NULL)
                {
                    t->child[0]->child[0]->isInit = true;
                    // t->child[0]->child[0]->needsInitCheck = false;
                }
            }
            if (t->attr.op == '=' || t->attr.op == '[')
            {
                t->expType = getType(st, t);
            }
            else
                t->expType = currentOp.returnType;
            if (!currentOp.isUnary)
            {
                if (!currentOp.passesLeftCheck(t))
                {
                    printf(getErrMsg(errLeftCheck), t->lineno, assignToString(t->attr.op), expToString(currentOp.lhs), expToString(t->child[0]->expType));
                    numAnalyzeErrors++;
                }
                if (!currentOp.passesRightCheck(t))
                {
                    printf(getErrMsg(errRightCheck), t->lineno, assignToString(t->attr.op), expToString(currentOp.lhs), expToString(t->child[1]->expType));
                    numAnalyzeErrors++;
                }
                if (!currentOp.passesEqualCheck(st, t))
                {
                    printf(getErrMsg(errEqTypeCheck), t->lineno, assignToString(t->attr.op), expToString(t->child[0]->expType), expToString(t->child[1]->expType));
                    numAnalyzeErrors++;
                }
                if (!currentOp.isArrayAndWorks(t))
                {
                    printf(getErrMsg(errWorksOnlyWithArr), t->lineno, assignToString(t->attr.op), "does not work");
                    numAnalyzeErrors++;
                }
                if (!currentOp.onlyArrayAndWorks(t))
                {
                    printf(getErrMsg(errWorksOnlyWithArr), t->lineno, assignToString(t->attr.op), "only works");
                    numAnalyzeErrors++;
                }
                if (currentOp.equalTypes)
                {
                    if (!currentOp.bothArrsOrNot(st, t))
                    {
                        // printf("BotharrsOrNot: %d\n", currentOp.bothArrsOrNot(st, t));
                        // printf("Assign EqTypes: %d LIsArr: %d RisArr %d LIsInd: %d RIsInd %d\n", currentOp.equalTypes, t->child[0]->isArray, t->child[1]->isArray, t->child[0]->isIndexed, t->child[1]->isIndexed);
                        printf(getErrMsg(errBothOpArr), t->lineno, assignToString(t->attr.op), t->child[0]->isArray && !t->child[0]->isIndexed ? "" : "not ", t->child[1]->isArray && !t->child[1]->isIndexed ? "" : "not ");
                        numAnalyzeErrors++;
                    }
                }
            }

            break;
        }
        case ExpKind::CallK:
        {
            TreeNode *tmp = (TreeNode *)st->lookup(t->attr.string);
            // printf("Looking for %s, %s\n", t->attr.string, tmp ? "found it" : "did not find it.\n");
            if (tmp != NULL)
            {
                // if()
                t->expType = tmp->expType;
                if (tmp->nodeKind == NodeKind::DeclK)
                {
                    if (tmp->subkind.decl != DeclKind::FuncK)
                    {
                        printf(getErrMsg(errSimple), t->lineno, tmp->attr.string);
                        // printf("ERROR(%d): '%s' is a simple variable and cannot be called.\n", t->lineno, tmp->attr.string);

                        // to match expected output
                        tmp->isUsed = true;

                        numAnalyzeErrors++;
                    }
                    else
                    {

                        tmp->isUsed = true;
                        TreeNode *paramNode = tmp->child[0];
                        TreeNode *callNode = t->child[0];
                        int paramCount = countSiblingListLength(paramNode);
                        int callCount = countSiblingListLength(callNode);

                        // else
                        // printf("Starting with %d errors, %d warnings.\n", numAnalyzeErrors, numAnalyzeWarnings);
                        checkParamTypes(&numAnalyzeErrors, &numAnalyzeWarnings, t, tmp, paramNode, callNode);
                        // printf("Ending with %d errors, %d warnings.\n", numAnalyzeErrors, numAnalyzeWarnings);
                        if (paramCount != callCount)
                        {
                            printf(getErrMsg(errParamCount), t->lineno, callCount < paramCount ? "few" : "many", tmp->attr.string, tmp->lineno);
                            numAnalyzeErrors++;
                        }
                        // printf("SUCCESS(%d) Seems to be correct # of args: param %d call %d\n", t->lineno, paramCount, callCount);}
                    }
                }
            }
            else
            {
                printf(getErrMsg(errSymNotDecl), t->lineno, t->attr.string);
                numAnalyzeErrors++;
                st->insert(t->attr.string, t);
                t->isDeclared = true;
            }
            break;
        }
        case ExpKind::ConstantK:
            break;

        case ExpKind::IdK:
        {
            // printf("Searching tree for %s\n", t->attr.string);
            TreeNode *tmp = (TreeNode *)st->lookup(t->attr.string);
            if (tmp == NULL)
            {
                printf(getErrMsg(errSymNotDecl), t->lineno, t->attr.string);
                numAnalyzeErrors++;
            }
            else
            {
                // tmp->expType = getType(st, t);
                if (tmp->isArray)
                    t->isArray;
                t->isArray = tmp->isArray;
                // if(t->isArray)
                // t->needsInitCheck = false;
                if (tmp->isDeclared)
                    t->isDeclared = tmp->isDeclared;
                t->isStatic = tmp->isStatic;
                // if (t->isStatic)
                // t->needsInitCheck = false;
                // t->needsInitCheck = tmp->needsInitCheck;
                if (tmp->subkind.decl == DeclKind::FuncK)
                {

                    printf(getErrMsg(errFuncAsVar), t->lineno, t->attr.string);
                    numAnalyzeErrors++;

                    // To match submission page
                    tmp->isUsed = true;
                }
                else
                {
                    tmp->isUsed = true;
                }
                // printf("Maybe this'll help: '%s' init: %d needsinit: %d\n", t->attr.string, t->isInit, t->needsInitCheck);
                if (t->isInit)
                {
                    tmp->isInit = true;
                    // tmp->needsInitCheck = false;
                    // t->needsInitCheck = false;
                }
                if (t->isDeclared)
                {
                    if (!t->isStatic)
                    {
                        if (tmp != NULL)
                            // printf("Checking %s (%d)\n", tmp->attr.string, t->lineno);

                            // if (tmp->declDepth)
                            // if (t->needsInitCheck)
                            // {
                            // printf("Start Init Check %s\n", t->attr.string);
                            // if ((st->depth() > 1) && !tmp->isInit && !t->isArray)
                            // printf("declDepth: %d, stDetph: %d, tmp->isInit: %d, t->isInit: %d, needsInitCheck: %d\n", tmp->declDepth, st->depth(), tmp->isInit, t->isInit, t->needsInitCheck);
                            // printf("%d '%s' t->isInit: %d, tmp->isInit: %d\n", t->lineno, tmp->attr.string, t->isInit, tmp->isInit);
                        // printf("'%s' tmp->declDepth: %d, st->depth: %d, tmp->isInit: %d\n", tmp->attr.string, tmp->declDepth, st->depth(), tmp->isInit);
                        if (tmp->declDepth != 1 && (st->depth() > 1) && !nodeIsInit(st, t))
                        // if (tmp->declDepth != 1 && (st->depth() > 1) && nodeIsInit(t)!tmp->isInit)
                        // if ((st->depth() > 1) && !tmp->isInit && isUnindexedArray(t))
                        {
                            // printf("printAnalysis: ");
                            // setPrintColor(PRINTCOLOR::PURPLE);
                            printf(getWarnMsg(warnUninitVar), t->lineno, t->attr.string);
                            // resetPrintColor();
                            tmp->isInit = true;
                            numAnalyzeWarnings++;
                            tmp->needsInitCheck = false;
                            t->needsInitCheck = false;
                        }
                        // printf("End %s\n", t->attr.string);
                        // }
                    }
                    // else
                    // printf("Needs no init check\n");
                }
                else if (!t->isStatic)
                {
                    printf(getErrMsg(errSymNotDecl), t->lineno, t->attr.string);
                    numAnalyzeErrors++;
                }
            }
            tmp = NULL;
        }
        break;

        case ExpKind::InitK:
            break;

        case ExpKind::OpK:
        {
            if (t->attr.op == '[')
            {
                t->expType = getType(st, t);
            }
            switch (t->attr.op)
            {
            case '[':
            {
                // if (t->child[0] != NULL)
                // {
                // t->child[0]->needsInitCheck = false;
                // }
                // TreeNode* tmp = (TreeNode*) st->lookup(t->child[0]->attr.string);
                t->expType = getType(st, t);
                if ((st->lookup(t->child[0]->attr.string) != NULL) && (t->child[0] == NULL || !t->child[0]->isArray && t->child[0]->isIndexed || !t->isArray))
                {
                    printf(getErrMsg(errIndexNonArr), t->lineno, t->child[0]->attr.string);
                    numAnalyzeErrors++;
                }
                else if ((st->lookup(t->child[0]->attr.string) == NULL) && (t->child[0] != NULL || !t->child[0]->isArray && !t->child[0]->isIndexed || !t->isArray))
                {
                    printf(getErrMsg(errIndexNonArr), t->lineno, t->child[0]->attr.string);
                    numAnalyzeErrors++;
                }
                else
                {
                    if (t->child[1] != NULL && t->child[1]->isArray && !t->child[1]->isIndexed)
                    {
                        if (t->child[1]->nodeKind == NodeKind::ExpK && t->child[1]->subkind.exp == ExpKind::IdK)
                        {
                            printf(getErrMsg(errIndexWithUnindexed), t->lineno, t->child[1]->attr.string);
                            numAnalyzeErrors++;
                        }
                    }
                }
                if (t->child[1] != NULL && (getType(st, t->child[1]) != ExpType::Integer && getType(st, t->child[1]) != ExpType::UndefinedType))
                {
                    printf(getErrMsg(errIndexTypeCheck), t->lineno, t->child[0]->attr.string, expToString(getType(st, t->child[1])));
                    numAnalyzeErrors++;
                }
                break;
            }
            // case OR:
            // t->child[0]->needsInitCheck = true;
            // t->child[1]->needsInitCheck = true;
            // break;
            // case AND:
            // t->child[0]->needsInitCheck = true;
            // t->child[1]->needsInitCheck = true;
            // break;
            default:
                // return (char *)"ERROR(opToString() in util.cpp)";
                break;
            }
            if (t->attr.op != '[')
            {
                OpTypeInfo currentOp = opInfoMap[t->attr.op];
                t->expType = currentOp.returnType;
                if (!currentOp.isUnary)
                {
                    if (!currentOp.passesLeftCheck(t))
                    {
                        printf(getErrMsg(errLeftCheck), t->lineno, opToString(t->attr.op), expToString(currentOp.lhs), expToString(getType(st, t->child[0])));
                        numAnalyzeErrors++;
                    }
                    // printf("ERROR(%d): '%s' requires operands of type %s but rhs is of type %s.\n", t->lineno, opToString(t->attr.op), expToString(currentOp.lhs), expToString(getType(st, t->child[1])));
                    if (!currentOp.passesRightCheck(t))
                    {
                        printf(getErrMsg(errRightCheck), t->lineno, opToString(t->attr.op), expToString(currentOp.lhs), expToString(getType(st, t->child[1])));
                        numAnalyzeErrors++;
                    }
                    if (!currentOp.passesEqualCheck(st, t))
                    {
                        printf(getErrMsg(errEqTypeCheck), t->lineno, opToString(t->attr.op), expToString(getType(st, t->child[0])), expToString(getType(st, t->child[1])));
                        numAnalyzeErrors++;
                    }
                    if (!currentOp.isArrayAndWorks(t))
                    {
                        printf(getErrMsg(errWorksOnlyWithArr), t->lineno, opToString(t->attr.op), "does not work");
                        numAnalyzeErrors++;
                    }
                    if (!currentOp.onlyArrayAndWorks(t))
                    {
                        printf(getErrMsg(errWorksOnlyWithArr), t->lineno, opToString(t->attr.op), "only works");
                        numAnalyzeErrors++;
                    }
                    if (currentOp.equalTypes)
                    {
                        if (!currentOp.bothArrsOrNot(st, t))
                        {
                            // printf("Op EqTypes: %d LIsArr: %d RisArr %d LIsInd: %d RIsInd %d\n", currentOp.equalTypes, t->child[0]->isArray, t->child[1]->isArray, t->child[0]->isIndexed, t->child[1]->isIndexed);
                            printf(getErrMsg(errBothOpArr), t->lineno, opToString(t->attr.op), t->child[0]->isArray && !t->child[0]->isIndexed ? "" : "not ", t->child[1]->isArray && !t->child[1]->isIndexed ? "" : "not ");
                            numAnalyzeErrors++;
                        }
                    }
                }
                else
                {
                    if (!currentOp.passesLeftCheck(t) && t->attr.op != SIZEOF)
                    {
                        printf(getErrMsg(errUnaryType), t->lineno, opToString(t->attr.op), expToString(currentOp.lhs), expToString(getType(st, t->child[0])));
                        numAnalyzeErrors++;
                    }
                    if (!currentOp.onlyArrayAndWorks(t))
                    {
                        printf(getErrMsg(errWorksOnlyWithArr), t->lineno, opToString(t->attr.op), "only works");
                        numAnalyzeErrors++;
                    }
                    if (!currentOp.isArrayAndWorks(t))
                    {
                        printf(getErrMsg(errWorksOnlyWithArr), t->lineno, opToString(t->attr.op), "does not work");
                        numAnalyzeErrors++;
                    }
                }
            }
            break;
        }
        default:
            break;
        }
        break;
    case NodeKind::StmtK:
        // printf("Aaaaaaaaaaaaaaaaaaaaaaaaahhhhhhhhhhhhhhhhhhhhhh\n");
        switch (t->subkind.stmt)
        {
        case StmtKind::BreakK:
            if (loopDepth <= 0)
            {
                printf(getErrMsg(errBreakOutsideLoop), t->lineno);
                numAnalyzeErrors++;
            }
            break;
        case StmtKind::CompoundK:
            break;
        case StmtKind::ForK:
        {
        }
        break;
        case StmtKind::IfK:
            if (t->child[0]->expType != ExpType::Boolean && t->child[0]->expType != ExpType::UndefinedType)
            {
                printf(getErrMsg(errExpBool), t->lineno, "if", expToString(t->child[0]->expType));
                numAnalyzeErrors++;
            }
            if (isUnindexedArray(t->child[0]))
            {
                printf(getErrMsg(errArrInTestCondition), t->lineno, "if");
                numAnalyzeErrors++;
            }
            break;
        case StmtKind::NullK:
            break;
        case StmtKind::RangeK:
            doRangeTypeCheck(&numAnalyzeErrors, &numAnalyzeWarnings, st, t);
            break;
        case StmtKind::ReturnK:
            if (t->child[0] != NULL && t->child[0]->isArray && !t->child[0]->isIndexed)
            {
                printf(getErrMsg(errReturnArr), t->lineno);
                numAnalyzeErrors++;
            }
            break;
        case StmtKind::WhileK:
            if (t->child[0]->expType != ExpType::Boolean && t->child[0]->expType != ExpType::UndefinedType)
            {
                printf(getErrMsg(errExpBool), t->lineno, "while", expToString(t->child[0]->expType));
                numAnalyzeErrors++;
            }
            if (isUnindexedArray(t->child[0]))
            {
                printf(getErrMsg(errArrInTestCondition), t->lineno, "while");
                numAnalyzeErrors++;
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}

void ASTtoSymbolTable(SymbolTable *st, TreeNode *root)
{
    traverse(st, root, setUsed, nullProc, false);
}

void semanticAnalysis(SymbolTable *st, TreeNode *root, bool printTypedTree)
{
    InitOpTypeList();
    initMsgTables();
    InitIOToSymbolTable(st);
    numAnalyzeWarnings = 0;
    numAnalyzeErrors = 0;
    // traverse(st, root, nullProc, moveUpTypes, false);
    // traverse(st, root, nullProc, moveUpTypes, false);
    // Clear ST when types are dispersed
    // bool debugState = st->getDebugState();
    // delete st;
    // st = new SymbolTable();
    // st->debug(debugState);
    // printTree(root, true);
    traverse(st, root, moveUpTypes, printAnalysis, true);
    // traverse(st, root, printAnalysis, nullProc, true);
    TreeNode *mainPointer = (TreeNode *)st->lookup((string) "main");
    if (mainPointer != NULL)
        mainPointer->isUsed = true;

    st->applyToAllGlobal(checkUse);
    // traverse(st, root, printProc, nullProc, false);

    // Do final check for main()
    if ((mainPointer != NULL) && (mainPointer->nodeKind == NodeKind::DeclK) && (mainPointer->subkind.decl == DeclKind::FuncK) && mainPointer->child[0] == NULL)
    {
        // printf("Found main()\n\n");
    }
    else
    {
        printf(getErrMsg(errMainDef));
        numAnalyzeErrors++;
    }
    // if (printTypedTree)
    if (printTypedTree && numAnalyzeErrors == 0)
    {
        printTree(root, true);
        // printf("\033[0;33m");
        // printf("MyWarning: Printing Typed Tree even though there may be errors!\n");
        // printf("\033[0m");
    }
    // printf("Number of warnings: %d\n", numAnalyzeWarnings);
    // printf("Number of errors: %d\n", numAnalyzeErrors);

    // printf("Symbol Table:\n\n");
    // st->print(pointerPrintStr);
}