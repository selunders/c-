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
    switch (kind)
    {
        case DeclKind::VarK:
            t->attr.name = strdup(token->tokenstr);
            // printf("VarK\n");
            break;
        case DeclKind::FuncK:
            // printf("Setting function name to %s\n\n", strdup(token->tokenstr));
            t->attr.name = strdup(token->tokenstr);
            // printf("FuncK\n");
            break;
        case DeclKind::ParamK:
            // printf("DeclK\n");j
            break;
    }

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
        break;
    case ExpKind::ConstantK:
        switch(token->tokenclass)
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
        t->attr.name = strdup(token->tokenstr);
        // t->attr.idIndex = token->idIndex;
        break;
    case ExpKind::AssignK:
        // t->attr.name = strdup(token->tokenstr);
        break;
    case ExpKind::InitK:
        // t->attr.name = strdup(token->tokenstr);
        break;
    case ExpKind::CallK:
        // t->attr.name = strdup(token->tokenstr);
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

void setType(TreeNode *t, ExpType type, bool isStatic)
{
    while (t)
    {
        t->expType = type;
        t->isStatic = isStatic;
        t = t->sibling;
    }
}

//////////////////////
// Printing the AST

char* expToString(ExpType type)
{
    switch(type)
    {
        case ExpType::Void:
            return (char*) "Void";
            break;
        case ExpType::Integer:
            return (char*) "int";
            break;
        case ExpType::Boolean:
            return (char*) "Boolean";
            break;
        case ExpType::Char:
            return (char*) "char";
            break;
        case ExpType::CharInt:
            return (char*) "CharInt";
            break;
        case ExpType::Equal:
            return (char*) "Equal";
            break;
        case ExpType::UndefinedType:
            return (char*) "UndefinedType";
            break;
        default:
            printf("\n\n Error with expToString in globals.hpp\n\n");
            return (char*) "ERROR IN GLOBALS.HPP EXPTOSTRING";
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

void printSTree(TreeNode* tree, NodeRelation relation, int relIndex, int layer)
{
    if(tree != NULL)
    {
        printSpaces(layer);
        printf("Test");
        printf(" [line: %d]\n", tree->lineno);
    }
}

void printRTree(TreeNode *tree, NodeRelation relation, int id, int layer)
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
            switch(tree->nodeKind)
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
                        printf("Var: %s ", tree->attr.name);
                        if(tree->isArray)
                            printf("of array ");
                        printf("of type %s", expToString(tree->expType));
                        break;
                    case DeclKind::FuncK:
                        // printf("Var: %s ", tree->attr.name);
                        // printf("About to print\n");
                        printf("Func: %s ", tree->attr.name);
                        // printf("Func: %s", tree->attr.name);
                        break;
                    case DeclKind::ParamK:
                        printf("ParamK");
                        break;
                }
                break;
            case NodeKind::ExpK:
                // printf("ExpK");
                switch(tree->subkind.exp)
                {
                    case ExpKind::IdK:
                        printf("Id: %s", tree->attr.name);
                        break;
                    default:
                        break;
                }
                break;
            case NodeKind::StmtK:
                printf("StmtK");
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
                printRTree(tree->child[i], NodeRelation::Child, i, layer+1);
                // printf("child %d is not NULL %p\n", i, tree->child[i]);
            }
            // else
            // printf("child %d is NULL\n", i);
        }
        // UNINDENT;
            if (tree->sibling != NULL)
            {
                if(relation == NodeRelation::Sibling)
                    printRTree(tree->sibling, NodeRelation::Sibling, id+1, layer);
                else
                    printRTree(tree->sibling, NodeRelation::Sibling, 1, layer);
            }
        // INDENT;
        // tree = NULL;
        // tree = tree->sibling;
    }
    UNINDENT;
}

void printTree(TreeNode *tree)
{
    printRTree(tree, NodeRelation::Parent, 0, 0);
}
//
//////////////////////