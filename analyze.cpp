#include "globals.hpp"
#include "symbolTable.hpp"
#include "analyze.hpp"
#include "c-.tab.h"

using namespace std;

int numAnalyzeWarnings;
int numAnalyzeErrors;
static int location = 0;

static void traverse(SymbolTable *st, TreeNode *t, void (*preProc)(SymbolTable *, TreeNode *), void (*postProc)(SymbolTable *, TreeNode *))
{
    bool enteredScope = false; 
    if (t != NULL)
    {

        if(t->nodeKind == NodeKind::StmtK && t->subkind.stmt == StmtKind::CompoundK)
        {
            st->enter((string) "Compound Statement");
            enteredScope = true;
        }
        preProc(st, t);
        {
            int i;
            for (i = 0; i < MAXCHILDREN; i++)
            {
                traverse(st, t->child[i], preProc, postProc);
            }
        }
        if (enteredScope)
        {
            st->leave();
            enteredScope = false;
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

static void insertNode(SymbolTable* st, TreeNode *t)
{
    switch (t->nodeKind)
    {
    case NodeKind::DeclK:
    {
        TreeNode *tmp = (TreeNode *)st->lookup(t->attr.string);
        if (tmp != NULL)
        {
            printf("ERROR(%d): Symbol '%s' is already declared at line %d.\n", t->lineno, t->attr.string, tmp->lineno);
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
            TreeNode *tmp = (TreeNode *)st->lookup(t->attr.string);
            if (tmp != NULL)
            {
                if (tmp->nodeKind == NodeKind::DeclK)
                {
                    if (tmp->subkind.decl != DeclKind::FuncK)
                    {
                        printf("ERROR(%d): '%s' is a simple variable and cannot be called.\n", t->lineno, tmp->attr.string);
                        numAnalyzeErrors++;
                    }
                    // printf("(%d) Comparing %s to %s\n", (string) tmp->attr.string, (string) "main");
                    // if ((string) tmp->attr.string == "main")
                    // {
                    // printf("ERROR(%d) : Cannot use function '%s' as a variable.\n", tmp->lineno, tmp->attr.string);
                    // }
                    // else
                    // {
                    // }
                }
            }
            else
            {
                printf("ERROR(%d): Symbol '%s' is not declared.\n", t->lineno, t->attr.string);
                numAnalyzeErrors++;
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
                printf("ERROR(%d): Symbol '%s' is not declared.\n", t->lineno, t->attr.string);
                numAnalyzeErrors++;
            }
            else if (tmp->subkind.decl == DeclKind::FuncK)
            {
                printf("ERROR(%d) : Cannot use function '%s' as a variable.\n", t->lineno, t->attr.string);
            }
        }
        break;

        case ExpKind::InitK:
            break;

        case ExpKind::OpK:
            switch (t->attr.op)
            {
            case OR:
                break;
            case AND:
                break;
            case NOT:
                break;
            case '<':
                break;
            case LEQ:
                break;
            case '>':
                break;
            case GEQ:
                break;
            case EQ:
                break;
            case NEQ:
                break;
            case '+':
                break;
            case SUBTRACT:
                break;
            case MULTIPLY:
                break;
            case '/':
                break;
            case MODULO:
                break;
            case NEGATIVE:
                break;
            case '?':
                break;
            case SIZEOF:
                break;
            case '[':
                break;
            default:
                // return (char *)"ERROR(opToString() in util.cpp)";
                break;
            }

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
            // st->enter((string) "Compound Statement");
            // t->enteredScope = true;
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

static void insertNode(SymbolTable *st, TreeNode *t)
{
    switch (t->nodeKind)
    {
    case NodeKind::DeclK:
    {
        TreeNode *tmp = (TreeNode *)st->lookup(t->attr.string);
        if (tmp != NULL)
        {
            printf("ERROR(%d): Symbol '%s' is already declared at line %d.\n", t->lineno, t->attr.string, tmp->lineno);
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
            TreeNode *tmp = (TreeNode *)st->lookup(t->attr.string);
            if (tmp != NULL)
            {
                if (tmp->nodeKind == NodeKind::DeclK)
                {
                    if (tmp->subkind.decl != DeclKind::FuncK)
                    {
                        printf("ERROR(%d): '%s' is a simple variable and cannot be called.\n", t->lineno, tmp->attr.string);
                        numAnalyzeErrors++;
                    }
                    // printf("(%d) Comparing %s to %s\n", (string) tmp->attr.string, (string) "main");
                    // if ((string) tmp->attr.string == "main")
                    // {
                    // printf("ERROR(%d) : Cannot use function '%s' as a variable.\n", tmp->lineno, tmp->attr.string);
                    // }
                    // else
                    // {
                    // }
                }
            }
            else
            {
                printf("ERROR(%d): Symbol '%s' is not declared.\n", t->lineno, t->attr.string);
                numAnalyzeErrors++;
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
                printf("ERROR(%d): Symbol '%s' is not declared.\n", t->lineno, t->attr.string);
                numAnalyzeErrors++;
            }
            else if (tmp->subkind.decl == DeclKind::FuncK)
            {
                printf("ERROR(%d) : Cannot use function '%s' as a variable.\n", t->lineno, t->attr.string);
            }
        }
        break;

        case ExpKind::InitK:
            break;

        case ExpKind::OpK:
            switch (t->attr.op)
            {
            case OR:
                break;
            case AND:
                break;
            case NOT:
                break;
            case '<':
                break;
            case LEQ:
                break;
            case '>':
                break;
            case GEQ:
                break;
            case EQ:
                break;
            case NEQ:
                break;
            case '+':
                break;
            case SUBTRACT:
                break;
            case MULTIPLY:
                break;
            case '/':
                break;
            case MODULO:
                break;
            case NEGATIVE:
                break;
            case '?':
                break;
            case SIZEOF:
                break;
            case '[':
                break;
            default:
                // return (char *)"ERROR(opToString() in util.cpp)";
                break;
            }

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
            // st->enter((string) "Compound Statement");
            // t->enteredScope = true;
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

    // Do final check for main()
    TreeNode *mainPointer = (TreeNode *)st->lookup((string) "main");
    if ((mainPointer != NULL) && (mainPointer->nodeKind == NodeKind::DeclK) && (mainPointer->subkind.decl == DeclKind::FuncK))
    {
        // printf("Found main()\n\n");
    }
    else
    {
        printf("ERROR(LINKER): A function named 'main()' must be defined.\n");
        numAnalyzeErrors++;
    }
    printf("Number of warnings: %d\n", numAnalyzeWarnings);
    printf("Number of errors: %d\n", numAnalyzeErrors);
}