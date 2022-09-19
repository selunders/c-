#include "globals.hpp"
#include "util.hpp"
#include "c-.tab.h"

using namespace std;

TreeNode *newDeclNode(DeclKind kind, ExpType type, TokenData *token = NULL, TreeNode *c0 = NULL, TreeNode *c1 = NULL, TreeNode *c2 = NULL)
{
    TreeNode* t = new TreeNode;

    t->child[0] = c0;
    t->child[1] = c1;
    t->child[2] = c2;

    t->nodeKind = NodeKind::DeclK;
    t->subkind.decl = kind;
    t->expType = type;

    switch(type)
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

TreeNode *newStmtNode(StmtKind kind, TokenData *token, TreeNode *c0 = NULL, TreeNode *c1 = NULL, TreeNode *c2 = NULL)
{
    TreeNode*t = new TreeNode;

    t->child[0] = c0;
    t->child[1] = c1;
    t->child[2] = c2;

    return t;
}

TreeNode *newExpNode(ExpKind kind, TokenData *token, TreeNode *c0 = NULL, TreeNode *c1 = NULL, TreeNode *c2 = NULL)
{
    TreeNode*t = new TreeNode;

    t->child[0] = c0;
    t->child[1] = c1;
    t->child[2] = c2;

    t->subkind.exp = kind;
    
    switch(t->subkind.exp)
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