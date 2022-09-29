#include "globals.hpp"
#include "symbolTable.hpp"
#include "analyze.hpp"

using namespace std;

int numAnalyzeWarnings;
int numAnalyzeErrors;
static int location = 0;

static void traverse(SymbolTable *st, TreeNode *t, void (*preProc)(SymbolTable *, TreeNode *), void (*postProc)(SymbolTable *, TreeNode *))
{
    if (t != NULL)
    {
        preProc(st, t);
        {
            int i;
            for (i = 0; i < MAXCHILDREN; i++)
            {
                traverse(st, t->child[i], preProc, postProc);
            }
        }
        postProc(st, t);
        traverse(st, t->sibling, preProc, postProc);
    }
}

static void nullProc(SymbolTable *st, TreeNode *t)
{
    if (t == NULL)
        return;
    else
        return;
}

static void insertNode(SymbolTable *st, TreeNode *t)
{
    switch (t->nodeKind)
    {
    case NodeKind::DeclK:
    {
        TreeNode* tmp = (TreeNode *)st->lookup(t->attr.string);
        if (tmp != NULL)
        {
            printf("ERROR(%d): Symbol '%s' is already declared at line %d.\n", t->lineno, t->attr.string,tmp->lineno);
            numAnalyzeErrors++;
        }
            // printf("ERROR(%Dd: Symbol '%s' is already declared at line %d.\n", tmp->lineno);
        else
        {
            switch (t->subkind.decl)
            {
            case DeclKind::FuncK:
                st->insert(t->attr.string, t);
                break;

            case DeclKind::ParamK:
                st->insert(t->attr.string, t);
                break;

            case DeclKind::VarK:
                st->insert(t->attr.string, t);
                break;

            default:
                break;
            }
        }
        break;
    }
    case NodeKind::ExpK:
        switch (t->subkind.exp)
        {
        case ExpKind::AssignK:
            break;

        case ExpKind::CallK:
        {
            TreeNode* tmp = (TreeNode*) st->lookup(t->attr.string);
            if(tmp != NULL)
            {
                if((tmp->nodeKind == NodeKind::DeclK) && (tmp->subkind.decl != DeclKind::FuncK))
                {
                    printf("ERROR(%d): '%s' is a simple variable and cannot be called.\n", t->lineno, tmp->attr.string);
                }
            }

            break;
        }
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
        switch (t->subkind.stmt)
        {
        case StmtKind::BreakK:
            break;
        case StmtKind::CompoundK:
            // if(t->attr.string)
            // symbolTable->enter((string) t->attr.string);
            st->enter((string) "Compound Statement");
            // printf("MYDEBUG(SymbolTable) Entered Compound statement [Line %d]\n", t->lineno);
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

void semanticAnalysis(SymbolTable *st, TreeNode *root)
{
    numAnalyzeWarnings = 0;
    numAnalyzeErrors = 0;
    st->enter((string) "Global");
    traverse(st, root, insertNode, nullProc);
    TreeNode* mainPointer = (TreeNode*) st->lookup((string) "main");
    if((mainPointer != NULL) && (mainPointer->nodeKind == NodeKind::DeclK) && (mainPointer->subkind.decl == DeclKind::FuncK))
    {}
    else
        printf("ERROR(LINKER): A function named 'main()' must be defined.\n");
}