#include "globals.hpp"
#include "symbolTable.hpp"
#include "analyze.hpp"

using namespace std;

static int location = 0;

static void traverse(SymbolTable* st, TreeNode * t, void (* preProc) (SymbolTable*, TreeNode *), void (* postProc) (SymbolTable*, TreeNode*))
{
    if(t != NULL)
    {
        preProc(st, t);
        {
            int i;
            for(i = 0; i < MAXCHILDREN; i++)
            {
                traverse(st, t->child[i], preProc, postProc);
            }
        }
        postProc(st, t);
        traverse(st, t->sibling, preProc, postProc);
    }
}

static void nullProc(TreeNode * t)
{
    if(t == NULL)
        return;
    else
        return;
}

static void insertNode(SymbolTable* symbolTable, TreeNode * t)
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
                    // symbolTable->enter((string) t->attr.string);
                    printf("\n\nEntered Compound statement scope %s in symtab\n\n", t->attr.string);
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

void semanticAnalysis(SymbolTable* st, TreeNode* root)
{
    traverse(st, root, insertNode, insertNode);
}