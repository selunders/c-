#include "globals.hpp"
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
        // printf("VarK\n");
        break;
    case DeclKind::FuncK:
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

    switch (type)
    {
    case ExpType::Integer:
        t->attr.value = token->numValue;
        break;
    case ExpType::Boolean:
        t->attr.value = token->boolValue;
        break;
    case ExpType::Char:
        t->attr.cvalue = token->charValue;
        break;
    // case ExpType::CharInt: // I think this is probably incorrect
    // t->attr.cvalue = token->charValue;
    // break;
    default:
        break;
    }

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

    t->subkind.exp = kind;

    switch (t->subkind.exp)
    {
    case ExpKind::OpK:
        t->attr.op = token->tokenclass;
        break;
    case ExpKind::ConstantK:
        break;
    case ExpKind::IdK:
        t->attr.idIndex = token->idIndex;
        break;
    case ExpKind::AssignK:
        break;
    case ExpKind::InitK:
        break;
    case ExpKind::CallK:
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

static int indentno = -1;

#define INDENT indentno += 1
#define UNINDENT indentno -= 1

static void printSpaces()
{
    int i;
    for (i = 0; i < indentno; i++)
    {
        // putchar(' ');
        printf(".   ");
        // fputs(". ", stdout);
    }
}

void printTree(TreeNode *tree)
{
    int i;
    INDENT;
    // printf("My addr:%p\n My Children:\n%p\n%p\n%p\n\n", tree, tree->child[0], tree->child[1], tree->child[2]);
    while (tree != NULL)
    {
        printSpaces();
        switch (tree->nodeKind)
        {
        case NodeKind::DeclK:
            printf("DeclK with subtype ");
            switch(tree->subkind.decl)
                {
                    case DeclKind::VarK:
                        printf("VarK\n");
                        break;
                    case DeclKind::FuncK:
                        printf("FuncK\n");
                        break;
                    case DeclKind::ParamK:
                        printf("ParamK\n");
                        break;
                }
            break;
        case NodeKind::ExpK:
            printf("ExpK\n");
            break;
        case NodeKind::StmtK:
            printf("StmtK\n");
            break;
        default:
            printf("Found unknown node???\n\n");
            break;
        }
        // nodeCount++;
        // printf("%d\n", nodeCount);
        for (i = 0; i < MAXCHILDREN; i++)
        {
            if (tree->child[i] != NULL)
            {
                printTree(tree->child[i]);
                // printf("child %d is not NULL %p\n", i, tree->child[i]);
            }
            // else
            // printf("child %d is NULL\n", i);
        }
        tree = tree->sibling;
    }
    UNINDENT;
}
//
//////////////////////