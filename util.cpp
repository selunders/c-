// #include "globals.hpp"
#include "util.hpp"
#include "c-.tab.h"

using namespace std;

static int nodeCount = 0;
/*
enum class NodeKind { DeclK, StmtK, ExpK };
enum class DeclKind { VarK, FuncK, ParamK };
enum class StmtKind { NullK, IfK, WhileK, ForK, CompoundK, ReturnK, BreakK, RangeK };
enum class ExpKind  { OpK, ConstantK, IdK, AssignK, InitK, CallK };
enum class ExpType  { Void, Integer, Boolean, Char, CharInt, Equal, UndefinedType };
enum class VarKind  { None, Local, Global, Parameter, LocalStatic };
*/

TreeNode *newDeclNode(DeclKind kind, ExpType type, TokenData *token, TreeNode *c0, TreeNode *c1, TreeNode *c2)
{
    // nodeCount++;
    // printf("Nodecount: %d\n", nodeCount);
    // printf("Creating a new decl node with children at:\n");
    // printf("%p\n", c0);
    // printf("%p\n", c1);
    // printf("%p\n", c2);
    TreeNode *t = new TreeNode();
    // printf("New tree node created at %p ", t);
    // printf("of type: ");
    t->attr.string = strdup(token->tokenstr);
    // switch (kind)
    // {
    //     case DeclKind::VarK:
    //         t->attr.string = strdup(token->tokenstr);
    //         // printf("VarK\n");
    //         break;
    //     case DeclKind::FuncK:
    //         // printf("Setting function name to %s\n\n", strdup(token->tokenstr));
    //         t->attr.string = strdup(token->tokenstr);
    //         // printf("FuncK\n");
    //         break;
    //     case DeclKind::ParamK:
    //         t->attr.string = strdup(token->tokenstr);
    //         // printf("DeclK\n");j
    //         break;
    // }

    t->child[0] = c0;
    t->child[1] = c1;
    t->child[2] = c2;

    t->nodeKind = NodeKind::DeclK;
    t->subkind.decl = kind;
    t->expType = type;
    t->lineno = token->linenum;

    // switch (type)
    // {
    // case ExpType::Integer:
    //     t->attr.value = token->numValue;
    //     break;
    // case ExpType::Boolean:
    //     t->attr.value = token->boolValue;
    //     break;
    // case ExpType::Char:
    //     t->attr.cvalue = token->charValue;
    //     break;
    // // case ExpType::CharInt: // I think this is probably incorrect
    // // t->attr.cvalue = token->charValue;
    // // break;
    // default:
    //     break;
    // }

    return t;
}

TreeNode *newStmtNode(StmtKind kind, TokenData *token, TreeNode *c0, TreeNode *c1, TreeNode *c2)
{
    // nodeCount++;
    // printf("Nodecount: %d\n", nodeCount);
    TreeNode *t = new TreeNode();

    t->nodeKind = NodeKind::StmtK;
    t->subkind.stmt = kind;

    t->child[0] = c0;
    t->child[1] = c1;
    t->child[2] = c2;

    t->lineno = token->linenum;

    return t;
}

TreeNode *newExpNode(ExpKind kind, TokenData *token, TreeNode *c0, TreeNode *c1, TreeNode *c2)
{
    // nodeCount++;
    // printf("Nodecount: %d\n", nodeCount);
    TreeNode *t = new TreeNode();
    t->nodeKind = NodeKind::ExpK;

    t->child[0] = c0;
    t->child[1] = c1;
    t->child[2] = c2;

    t->lineno = token->linenum;

    t->subkind.exp = kind;

    switch (t->subkind.exp)
    {
    case ExpKind::OpK:
        t->attr.op = token->tokenclass;
        // printf("\n\n Set op to %d\n\n", token->tokenclass);
        break;
    case ExpKind::ConstantK:
        switch (token->tokenclass)
        {
        case NUMCONST:
            t->attr.value = token->numValue;
            break;
        case CHARCONST:
            t->attr.cvalue = token->charValue;
            break;
        case STRINGCONST:
            t->attr.string = strdup(token->stringValue);
            break;
        case BOOLCONST:
            t->attr.value = token->numValue;
            break;
        }
        break;
    case ExpKind::IdK:
        t->attr.string = strdup(token->tokenstr);
        // t->attr.idIndex = token->idIndex;
        break;
    case ExpKind::AssignK:
        t->attr.op = token->tokenclass;
        // t->attr.string = strdup(token->tokenstr);
        break;
    case ExpKind::InitK:
        // t->attr.string = strdup(token->tokenstr);
        break;
    case ExpKind::CallK:
        // t->attr.string = strdup(token->tokenstr);
        break;
    default:
        break;
    }

    return t;
}
//////////////////////
// Adding siblings

TreeNode *addSibling(TreeNode *addToThis, TreeNode *nodeBeingAdded)
{
    if (nodeBeingAdded == NULL)
    {
        // printf("ERROR(SYSTEM): attempt to add a NULL to a sibling list.\n");
        return addToThis;
        // printf("ERROR(SYSTEM): never add a NULL to a sibling list.\n");
        // exit(1);
    }
    if (addToThis != NULL)
    {
        TreeNode *tmp;
        tmp = addToThis;
        while (tmp->sibling != NULL)
        {
            tmp = tmp->sibling;
        }
        tmp->sibling = nodeBeingAdded;
        tmp = NULL;
        return addToThis;
    }
    return nodeBeingAdded;
}

void setType(TreeNode *t, ExpType type, bool isStatic, bool needsInitChecked)
{
    while (t != NULL)
    {
        // printf("%d Setting %s from %s to %s\n", t->lineno, t->attr.string, expToString(t->expType), expToString(type));
        // if(t->expType == ExpType::UndefinedType)
        // {
        if (t->nodeKind != NodeKind::ExpK || t->subkind.exp != ExpKind::ConstantK)
        {
            t->expType = type;
        }
        // }
        t->isStatic = isStatic;
        t->needsInitCheck = needsInitChecked;
        t->isDeclared = true;
        t = t->sibling;
    }
}

//////////////////////
// Printing the AST

char *expToString(ExpType type)
{
    switch (type)
    {
    case ExpType::Void:
        return (char *)"void";
        break;
    case ExpType::Integer:
        return (char *)"int";
        break;
    case ExpType::Boolean:
        return (char *)"bool";
        break;
    case ExpType::Char:
        return (char *)"char";
        break;
    case ExpType::CharInt:
        return (char *)"CharInt";
        break;
    case ExpType::Equal:
        return (char *)"Equal";
        break;
    case ExpType::UndefinedType:
        return (char *)"UndefinedType";
        break;
    case ExpType::Array:
        return (char *)"Array";
        break;
    case ExpType::LHS:
        return (char *)"LHS";
        break;
    default:
        printf("\n\n Error with expToString in util.cpp\n\n");
        return (char *)"ERROR IN GLOBALS.HPP EXPTOSTRING";
        break;
    }
}

char *opToString(OpKind op)
{
    switch (op)
    {
    case OR:
        return (char *)"or";
        break;
    case AND:
        return (char *)"and";
        break;
    case NOT:
        return (char *)"not";
        break;
    case '<':
        return (char *)"<";
        break;
    case LEQ:
        return (char *)"<=";
        break;
    case '>':
        return (char *)">";
        break;
    case GEQ:
        return (char *)">=";
        break;
    case EQ:
        return (char *)"==";
        break;
    case NEQ:
        return (char *)"!=";
        break;
    case '+':
        return (char *)"+";
        break;
    case SUBTRACT:
        return (char *)"-";
        break;
    case MULTIPLY:
        return (char *)"*";
        break;
    case '/':
        return (char *)"/";
        break;
    case MODULO:
        return (char *)"%";
        break;
    case NEGATIVE:
        return (char *)"chsign";
        break;
    case '?':
        return (char *)"?";
        break;
    case SIZEOF:
        return (char *)"sizeof";
        break;
    case '[':
        return (char *)"[";
        break;
    default:
        return (char *)"ERROR(opToString() in util.cpp)";
        break;
    }
}

char *assignToString(OpKind op)
{
    switch (op)
    {
    case INC:
        return (char *)"++";
        break;
    case DEC:
        return (char *)"--";
        break;
    case ADDASS:
        return (char *)"+=";
        break;
    case SUBASS:
        return (char *)"-=";
        break;
    case MULASS:
        return (char *)"*=";
        break;
    case DIVASS:
        return (char *)"/=";
        break;
    case '=':
        return (char *)"=";
        break;
    default:
        return (char *)"ERROR(assignToString() in util.cpp)";
        break;
    }
}

static int indentno = -1;

#define INDENT indentno += 1
#define UNINDENT indentno -= 1

static void printSpaces(int layer)
{
    int i;
    for (i = 0; i < layer; i++)
    {
        // putchar(' ');
        printf(".   ");
        // fputs(". ", stdout);
    }
}

enum class NodeRelation
{
    Parent,
    Child,
    Sibling
};

void printSTree(TreeNode *tree, NodeRelation relation, int relIndex, int layer)
{
    if (tree != NULL)
    {
        printSpaces(layer);
        printf("Test");
        printf(" [line: %d]\n", tree->lineno);
    }
}

void printBasicTree(TreeNode *tree, NodeRelation relation, int id, int layer)
{
    int i;
    INDENT;
    // printf("My addr:%p\n My Children:\n%p\n%p\n%p\n\n", tree, tree->child[0], tree->child[1], tree->child[2]);
    // while (tree != NULL)
    if (tree != NULL)
    {
        printSpaces(layer);
        switch (relation)
        {
        case NodeRelation::Parent:
            switch (tree->nodeKind)
            {
                // case NodeKind::DeclK:
                //     switch(tree->subkind.decl)
                //     {
                //         case DeclKind::VarK:
                //             printf("Var: ");
                //             break;
                //         case DeclKind::FuncK:
                //             printf("Func: ");
                //             break;
                //         default:
                //             printf("Couldn't tell: ");
                //             break;
                //         // case DeclKind::ParamK:
                //             // printf("Param");
                //             // break;
                //     }
                //     break;
                // case NodeKind::ExpK:
                //     switch(tree->subkind.exp)
                //     {
                //         case ExpKind::IdK:
                //             printf("Var: ");
                //             if(tree->isArray)
                //                 printf("of array ");
                //             printf("of type ", expToString(tree->expType));
                //         break;
                //     }
                //     break;
                // case NodeKind::StmtK:
                //     switch(tree->subkind.stmt)
                //     {

                //     }
                //     break;
            }
            // printf("Staring with node of type: %d\n\n", tree->subkind.decl);
            break;
        case NodeRelation::Child:
            printf("Child: %d  ", id);
            break;
        case NodeRelation::Sibling:
            printf("Sibling: %d  ", id);
            break;
        default:
            printf("You shouldn't be here?\n\n\n");
            break;
        }
        switch (tree->nodeKind)
        {
        case NodeKind::DeclK:
            // printf("DeclK with subtype ");
            switch (tree->subkind.decl)
            {
            case DeclKind::VarK:
                printf("Var: %s ", tree->attr.string);
                if (tree->isArray)
                    printf("is array ");
                if (tree->isStatic)
                    // printf("of static type %s", expToString(tree->expType));
                    printf("of type %s", expToString(tree->expType));
                else
                    printf("of type %s", expToString(tree->expType));
                break;
            case DeclKind::FuncK:
                // printf("Var: %s ", tree->attr.string);
                // printf("About to print\n");
                printf("Func: %s ", tree->attr.string);
                printf("returns type %s", expToString(tree->expType));
                // printf("Func: %s", tree->attr.string);
                break;
            case DeclKind::ParamK:
                printf("Parm: %s ", tree->attr.string);
                if (tree->isArray)
                    printf("is array ");
                printf("of type %s", expToString(tree->expType));
                break;
            }
            break;
        case NodeKind::ExpK:
            // printf("ExpK");
            switch (tree->subkind.exp)
            {
            case ExpKind::OpK:
                printf("Op: %s", opToString(tree->attr.op));
                break;
            case ExpKind::ConstantK:
                switch (tree->expType)
                {
                case ExpType::Boolean:
                    if (tree->attr.value == 0)
                        printf("Const false");
                    else
                        printf("Const true");
                    break;
                case ExpType::Char:
                    if (tree->isArray)
                        printf("Const %s", tree->attr.string);
                    else
                        printf("Const \'%c\'", tree->attr.cvalue);
                    break;
                case ExpType::Integer:
                    printf("Const %d", tree->attr.value);
                    break;
                default:
                    printf("How'd you find this???\n\n\n");
                    break;
                }
                break;
            case ExpKind::IdK:
                printf("Id: %s", tree->attr.string);
                break;
            case ExpKind::AssignK:
                printf("Assign: %s", assignToString(tree->attr.op));
                break;
            case ExpKind::InitK:
                break;
            case ExpKind::CallK:
                printf("Call: %s", tree->attr.string);
                break;
            default:
                break;
            }
            break;
        case NodeKind::StmtK:
            // printf("StmtK");
            switch (tree->subkind.stmt)
            {
            case StmtKind::NullK:
                printf("Null");
                break;
            case StmtKind::IfK:
                printf("If");
                break;
            case StmtKind::WhileK:
                printf("While");
                break;
            case StmtKind::ForK:
                printf("For");
                break;
            case StmtKind::CompoundK:
                printf("Compound");
                break;
            case StmtKind::ReturnK:
                printf("Return");
                break;
            case StmtKind::BreakK:
                printf("Break");
                break;
            case StmtKind::RangeK:
                printf("Range");
                break;
            default:
                printf("... how'd you get here? (util.cpp ~380ish)\n\n");
                break;
            }
            break;
        default:
            printf("Found unknown node???\n\n");
            break;
        }
        // nodeCount++;
        // printf("%d\n", nodeCount);
        printf(" [line: %d]\n", tree->lineno);
        // printf(" [line: %d]\nexpType::%s\n", tree->lineno, expToString(tree->expType));
        for (i = 0; i < MAXCHILDREN; i++)
        {
            if (tree->child[i] != NULL)
            {
                printBasicTree(tree->child[i], NodeRelation::Child, i, layer + 1);
                // printf("child %d is not NULL %p\n", i, tree->child[i]);
            }
            // else
            // printf("child %d is NULL\n", i);
        }
        // UNINDENT;
        if (tree->sibling != NULL)
        {
            if (relation == NodeRelation::Sibling)
                printBasicTree(tree->sibling, NodeRelation::Sibling, id + 1, layer);
            else
                printBasicTree(tree->sibling, NodeRelation::Sibling, 1, layer);
        }
        // INDENT;
        // tree = NULL;
        // tree = tree->sibling;
    }
    UNINDENT;
}

void printTypedTree(TreeNode *tree, NodeRelation relation, int id, int layer)
{
    int i;
    INDENT;
    // printf("My addr:%p\n My Children:\n%p\n%p\n%p\n\n", tree, tree->child[0], tree->child[1], tree->child[2]);
    // while (tree != NULL)
    if (tree != NULL)
    {
        printSpaces(layer);
        switch (relation)
        {
        case NodeRelation::Parent:
            switch (tree->nodeKind)
            {
                // case NodeKind::DeclK:
                //     switch(tree->subkind.decl)
                //     {
                //         case DeclKind::VarK:
                //             printf("Var: ");
                //             break;
                //         case DeclKind::FuncK:
                //             printf("Func: ");
                //             break;
                //         default:
                //             printf("Couldn't tell: ");
                //             break;
                //         // case DeclKind::ParamK:
                //             // printf("Param");
                //             // break;
                //     }
                //     break;
                // case NodeKind::ExpK:
                //     switch(tree->subkind.exp)
                //     {
                //         case ExpKind::IdK:
                //             printf("Var: ");
                //             if(tree->isArray)
                //                 printf("of array ");
                //             printf("of type ", expToString(tree->expType));
                //         break;
                //     }
                //     break;
                // case NodeKind::StmtK:
                //     switch(tree->subkind.stmt)
                //     {

                //     }
                //     break;
            }
            // printf("Staring with node of type: %d\n\n", tree->subkind.decl);
            break;
        case NodeRelation::Child:
            printf("Child: %d  ", id);
            break;
        case NodeRelation::Sibling:
            printf("Sibling: %d  ", id);
            break;
        default:
            printf("You shouldn't be here?\n\n\n");
            break;
        }
        switch (tree->nodeKind)
        {
        case NodeKind::DeclK:
            // printf("DeclK with subtype ");
            switch (tree->subkind.decl)
            {
            case DeclKind::VarK:
                printf("Var: %s ", tree->attr.string);
                if (tree->isArray)
                    printf("is array ");
                if (tree->isStatic)
                    // printf("of static type %s", expToString(tree->expType));
                    printf("of type %s", expToString(tree->expType));
                else
                    printf("of type %s", expToString(tree->expType));
                break;
            case DeclKind::FuncK:
                // printf("Var: %s ", tree->attr.string);
                // printf("About to print\n");
                printf("Func: %s ", tree->attr.string);
                printf("returns type %s", expToString(tree->expType));
                // printf("Func: %s", tree->attr.string);
                break;
            case DeclKind::ParamK:
                printf("Parm: %s ", tree->attr.string);
                if (tree->isArray)
                    printf("is array ");
                printf("of type %s", expToString(tree->expType));
                break;
            }
            break;
        case NodeKind::ExpK:
            // printf("ExpK");
            switch (tree->subkind.exp)
            {
            case ExpKind::OpK:
                if (tree->expType == ExpType::UndefinedType)
                    printf("Op: %s of undefined type", opToString(tree->attr.op));
                else
                    printf("Op: %s of type %s", opToString(tree->attr.op), expToString(tree->expType));
                break;
            case ExpKind::ConstantK:
                switch (tree->expType)
                {
                case ExpType::Boolean:
                    if (tree->attr.value == 0)
                        printf("Const false");
                    else
                        printf("Const true");
                    printf(" of type bool");
                    break;
                case ExpType::Char:
                    if (tree->isArray)
                        printf("Const %s", tree->attr.string);
                    else
                        printf("Const \'%c\'", tree->attr.cvalue);
                    printf(" of type char");
                    break;
                case ExpType::Integer:
                    printf("Const %d", tree->attr.value);
                    printf(" of type int");
                    break;
                default:
                    printf("How'd you find this???\n\n\n");
                    break;
                }
                break;
            case ExpKind::IdK:

                if (tree->expType == ExpType::UndefinedType)
                    printf("Id: %s of undefined type", tree->attr.string);
                else
                    printf("Id: %s of type %s", tree->attr.string, expToString(tree->expType));
                break;
            case ExpKind::AssignK:

                printf("Assign: %s of type %s", assignToString(tree->attr.op), expToString(tree->expType));
                break;
            case ExpKind::InitK:
                break;
            case ExpKind::CallK:

                printf("Call: %s of type %s", tree->attr.string, expToString(tree->expType));
                break;
            default:
                break;
            }
            break;
        case NodeKind::StmtK:
            // printf("StmtK");
            switch (tree->subkind.stmt)
            {
            case StmtKind::NullK:
                printf("Null");
                break;
            case StmtKind::IfK:
                printf("If");
                break;
            case StmtKind::WhileK:
                printf("While");
                break;
            case StmtKind::ForK:
                printf("For");
                break;
            case StmtKind::CompoundK:
                printf("Compound");
                break;
            case StmtKind::ReturnK:
                printf("Return");
                break;
            case StmtKind::BreakK:
                printf("Break");
                break;
            case StmtKind::RangeK:
                printf("Range");
                break;
            default:
                printf("... how'd you get here? (util.cpp ~380 haha actually 728ish)\n\n");
                break;
            }
            break;
        default:
            printf("Found unknown node???\n\n");
            break;
        }
        // nodeCount++;
        // printf("%d\n", nodeCount);
        printf(" [line: %d]\n", tree->lineno);
        for (i = 0; i < MAXCHILDREN; i++)
        {
            if (tree->child[i] != NULL)
            {
                printTypedTree(tree->child[i], NodeRelation::Child, i, layer + 1);
                // printf("child %d is not NULL %p\n", i, tree->child[i]);
            }
            // else
            // printf("child %d is NULL\n", i);
        }
        // UNINDENT;
        if (tree->sibling != NULL)
        {
            if (relation == NodeRelation::Sibling)
                printTypedTree(tree->sibling, NodeRelation::Sibling, id + 1, layer);
            else
                printTypedTree(tree->sibling, NodeRelation::Sibling, 1, layer);
        }
        // INDENT;
        // tree = NULL;
        // tree = tree->sibling;
    }
    UNINDENT;
}

void printTree(TreeNode *tree, bool printTypeInfo)
{
    if (printTypeInfo)
        printTypedTree(tree, NodeRelation::Parent, 0, 0);
    else
        printBasicTree(tree, NodeRelation::Parent, 0, 0);
}
//
//////////////////////