#include "globals.hpp"
#include "symbolTable.hpp"
#include "analyze.hpp"

static int location = 0;

static void traverse(TreeNode * t, void (* preProc) (TreeNode *), void (* postProc) (TreeNode*))
{
    if(t != NULL)
    {
        preProc(t);
        {
            int i;
            for(i = 0; i < MAXCHILDREN; i++)
            {
                traverse(t->child[i], preProc, postProc);
            }
        }
        postProc(t);
        traverse(t->sibling, preProc, postProc);
    }
}

static void nullProc(TreeNode * t)
{
    if(t == NULL)
        return;
    else
        return;
}

static void insertNode(SymbolTable symbolTable, TreeNode * t)
{
    switch(t->nodeKind)
    {
        case NodeKind::DeclK:
            switch(t->subkind.decl)
            {
                case DeclKind::FuncK:
                break;
                
                case DeclKind::ParamK:
                break;
                
                case DeclKind::VarK:
                break;
                
                default:
                break;
            }
        break;
        case NodeKind::ExpK:
            switch(t->subkind.exp)
            {
                case ExpKind::AssignK:
                break;
                
                case ExpKind::CallK:
                break;
                
                case ExpKind::ConstantK:
                break;
                
                case ExpKind::IdK:
                    if(symbolTable.insert(t->attr.string, t))
                    {
                        // printf("Inserted id %s\n", t->attr.name);
                    }
                break;
                
                case ExpKind::InitK:
                break;
                
                case ExpKind::OpK:
                break;

                default:
                break;
            }
        break;
        case NodeKind::StmtK:
            switch(t->subkind.stmt)
            {
                case StmtKind::BreakK:
                break;
                case StmtKind::CompoundK:
                break;
                case StmtKind::ForK:
                break;
                case StmtKind::IfK:
                break;
                case StmtKind::NullK:
                break;
                case StmtKind::RangeK:
                break;
                case StmtKind::ReturnK:
                break;
                case StmtKind::WhileK:
                break;                
            }
        break;
        default:
        
        break;
    }
}