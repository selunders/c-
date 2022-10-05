#include "globals.hpp"
#include "symbolTable.hpp"
#include "analyze.hpp"
#include "c-.tab.h"
#include "util.hpp"
#include <map>

// #include <iostream>

using namespace std;

// extern int yytokentype;

int numAnalyzeWarnings;
int numAnalyzeErrors;
map<OpKind, OpTypeInfo> opInfoMap;
static int location = 0;
static void printAnalysis(SymbolTable *st, TreeNode *t, bool *enteredScope);

void InitOpTypeList()
{
    opInfoMap[AND] = OpTypeInfo(ExpType::Boolean, ExpType::Boolean, ExpType::Boolean, false, false, false, false);
    opInfoMap[OR] = OpTypeInfo(ExpType::Boolean, ExpType::Boolean, ExpType::Boolean, false, false, false, false);
    opInfoMap[EQ] = OpTypeInfo(ExpType::UndefinedType, ExpType::UndefinedType, ExpType::Boolean, true, false, false, true);
    opInfoMap[NEQ] = OpTypeInfo(ExpType::UndefinedType, ExpType::UndefinedType, ExpType::Boolean, true, false, false, true);
    opInfoMap['<'] = OpTypeInfo(ExpType::UndefinedType, ExpType::UndefinedType, ExpType::Boolean, true, false, false, true);
    opInfoMap[LEQ] = OpTypeInfo(ExpType::UndefinedType, ExpType::UndefinedType, ExpType::Boolean, true, false, false, true);
    opInfoMap['>'] = OpTypeInfo(ExpType::UndefinedType, ExpType::UndefinedType, ExpType::Boolean, true, false, false, true);
    opInfoMap[GEQ] = OpTypeInfo(ExpType::UndefinedType, ExpType::UndefinedType, ExpType::Boolean, true, false, false, true);
    opInfoMap['='] = OpTypeInfo(ExpType::UndefinedType, ExpType::UndefinedType, ExpType::Boolean, true, false, false, true);
    opInfoMap[ADDASS] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false);
    opInfoMap[SUBASS] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false);
    opInfoMap[MULASS] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false);
    opInfoMap[DIVASS] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false);
    opInfoMap['+'] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false);
    opInfoMap[SUBTRACT] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false);
    opInfoMap[MULTIPLY] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false);
    opInfoMap['\\'] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false);
    opInfoMap[MODULO] = OpTypeInfo(ExpType::Integer, ExpType::Integer, ExpType::Integer, false, false, false, false);
    opInfoMap['['] = OpTypeInfo(ExpType::Array, ExpType::Integer, ExpType::LHS, false, false, true, true);
    // Unary
    opInfoMap[INC] = OpTypeInfo(ExpType::Integer, ExpType::Integer, false, false, false);
    opInfoMap[DEC] = OpTypeInfo(ExpType::Integer, ExpType::Integer, false, false, false);
    opInfoMap[NOT] = OpTypeInfo(ExpType::Boolean, ExpType::Boolean, false, false, false);
    opInfoMap[SIZEOF] = OpTypeInfo(ExpType::Array, ExpType::Integer, true, true, true);
    opInfoMap[NEGATIVE] = OpTypeInfo(ExpType::Integer, ExpType::Integer, false, false, false);
    opInfoMap['?'] = OpTypeInfo(ExpType::Integer, ExpType::Integer, false, false, false);
    // Array Operators
    // opInfoMap[NEGATIVE] = OpTypeInfo(ExpType::Array, ExpType::Integer, ExpType::LHS, false, false, true);
}

static void traverse(SymbolTable *st, TreeNode *t, void (*preProc)(SymbolTable *, TreeNode *, bool *), void (*postProc)(SymbolTable *, TreeNode *, bool *))
{
    bool enteredScope = false;
    if (t != NULL)
    {
        // if (t->nodeKind == NodeKind::StmtK)
        // {
        //     switch (t->subkind.stmt)
        //     {
        //     case StmtKind::BreakK:
        //         break;
        //     case StmtKind::CompoundK:
        //         st->enter((string) "Compound Statement");
        //         enteredScope = true;
        //         break;
        //     case StmtKind::ForK:
        //         st->enter((string) "For");
        //         enteredScope = true;
        //         break;
        //     case StmtKind::IfK:
        //         st->enter((string) "If");
        //         enteredScope = true;
        //         break;
        //     case StmtKind::NullK:
        //         break;
        //     case StmtKind::RangeK:
        //         break;
        //     case StmtKind::ReturnK:
        //         break;
        //     case StmtKind::WhileK:
        //         st->enter((string) "While");
        //         break;
        //     default:
        //         break;
        //     }
        //     // printf("Entered compound statement\n");
        // }
        preProc(st, t, &enteredScope);
        {
            int i;
            for (i = 0; i < MAXCHILDREN; i++)
            {
                traverse(st, t->child[i], preProc, postProc);
            }
        }
        postProc(st, t, &enteredScope);
        if (enteredScope)
        {
            st->leave();
            // printf("Left scop\n");
            enteredScope = false;
        }
        traverse(st, t->sibling, preProc, postProc);
    }
}

static void nullProc(SymbolTable *st, TreeNode *t, bool *enteredScope)
{
    if (t == NULL)
        return;
    else
        return;
}

static void typeCheck(SymbolTable *st, TreeNode *t)
{
    switch (t->nodeKind)
    {
    case NodeKind::DeclK:
        break;
    case NodeKind::ExpK:
        switch (t->subkind.exp)
        case ExpKind::OpK:
            switch (t->attr.op)
            case '=':
                t->expType = t->child[0]->expType;
        break;
    default:
        break;
        break;
        break;
    case NodeKind::StmtK:
        break;
    }
}

static void insertSymbols(SymbolTable *st, TreeNode *t)
{
    if (t->nodeKind == NodeKind::DeclK)
        if (st->lookup(t->attr.string) == NULL)
            st->insert(t->attr.string, t);
}

// static ExpType disperseTypes(SymbolTable* st, TreeNode *t)
// {
//     switch (t->nodeKind)
//     {
//         case NodeKind::DeclK:
//         break;
//         case NodeKind::ExpK:
//             switch(t->subkind.exp)
//             {
//                 case ExpKind::IdK:
//                     if()
//                 break;
//                 default:
//                 break;
//             }
//         break;
//         case NodeKind::StmtK:
//         break;
//     }
//     if(st->lookup(t->attr.string))
// }

static void findTypes(SymbolTable *st, TreeNode *t)
{
    // int i = 0;
    // while(t->child[i] != NULL)
    // {
    //     findTypes(st, t->child[i]);
    //     i++;
    // }
    // printf("Aaaaahhhhh\n");

    if (t->expType != ExpType::UndefinedType)
    {
        // printf("Aaaaaaaaahhhhhhhh\n");
        // return t->expType;
    }
    else
    {
        // printf("Aaaaaaaaaaaaahhhhhhhhhhh\n");
        switch (t->nodeKind)
        {
        case NodeKind::DeclK:
            printf("\nERROR(MYERROR) Error 1 in findType function (analyze.cpp)\n");
            break;
        case NodeKind::ExpK:
            switch (t->subkind.exp)
            {
            case ExpKind::AssignK:
                switch (t->attr.op)
                {
                case INC:
                    t->expType = ExpType::Integer;
                    // return t->expType;
                    break;
                case DEC:
                    t->expType = ExpType::Integer;
                    // return t->expType;
                    break;
                case ADDASS:
                    t->expType = ExpType::Integer;
                    // return t->expType;
                    break;
                case SUBASS:
                    t->expType = ExpType::Integer;
                    // return t->expType;
                    break;
                case MULASS:
                    t->expType = ExpType::Integer;
                    // return t->expType;
                    break;
                case DIVASS:
                    t->expType = ExpType::Integer;
                    // return t->expType;
                    break;
                case '=':
                    t->expType = t->child[0]->expType;

                    printf("Assigning '=' at line %d to type %s of %s\n", t->lineno, expToString(t->child[0]->expType), t->child[0]->attr.string);
                    // return t->expType;
                    break;
                default:
                    printf("ERROR(MYERROR) error 2 findType() in analyze.cpp");
                    break;
                }
                // t->expType = findType(st, t->child[1]);
                // return t->expType;
                break;
            case ExpKind::CallK:
                // t->expType = t->child[1]->expType;
                // return t->expType;
                break;
            case ExpKind::ConstantK:
                break;

            case ExpKind::IdK:
            {
                TreeNode *tmp = (TreeNode *)st->lookup(t->attr.string);
                if (tmp != NULL)
                {
                    printf("Found %s of type %s\n", t->attr.string, expToString(tmp->expType));
                    t->expType = tmp->expType;
                }
                // return t->expType;
                break;
            }
            case ExpKind::InitK:
                break;

            case ExpKind::OpK:
                switch (t->attr.op)
                {
                case OR:
                    t->expType = ExpType::Boolean;
                    // return t->expType;
                    break;
                case AND:
                    t->expType = ExpType::Boolean;
                    // return t->expType;
                    break;
                case NOT:
                    t->expType = ExpType::Boolean;
                    // return t->expType;
                    break;
                case '<':
                    // if(t->child[0]->isArray)
                    // t->expType = ExpType::
                    t->expType = ExpType::Boolean;
                    // return t->expType;
                    break;
                case LEQ:
                    t->expType = ExpType::Boolean;
                    // return t->expType;
                    break;
                case '>':
                    t->expType = ExpType::Boolean;
                    // return t->expType;
                    break;
                case GEQ:
                    t->expType = ExpType::Boolean;
                    // return t->expType;
                    break;
                case EQ:
                    t->expType = ExpType::Boolean;
                    // return t->expType;
                    break;
                case NEQ:
                    t->expType = ExpType::Boolean;
                    // return t->expType;
                    break;
                case '+':
                    t->expType = ExpType::Integer;
                    // return t->expType;
                    break;
                case SUBTRACT:
                    t->expType = ExpType::Integer;
                    // return t->expType;
                    break;
                case MULTIPLY:
                    t->expType = ExpType::Integer;
                    // return t->expType;
                    break;
                case '/':
                    t->expType = ExpType::Integer;
                    // return t->expType;
                    break;
                case MODULO:
                    t->expType = ExpType::Integer;
                    // return t->expType;
                    break;
                case NEGATIVE:
                    t->expType = ExpType::Integer;
                    // return t->expType;
                    break;
                case '?':
                    t->expType = ExpType::Integer;
                    // return t->expType;
                    break;
                case SIZEOF:
                    t->expType = ExpType::Integer;
                    // return t->expType;
                    break;
                case '[':
                {
                    // Type of Left-hand Child
                    TreeNode *tmp = (TreeNode *)st->lookup(t->child[0]->attr.string);
                    findTypes(st, t->child[0]);
                    t->expType = t->child[0]->expType;
                    // return t->expType;
                    break;
                }
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
            break;
        }
    }
    // return ExpType::UndefinedType;
}

static void moveUpTypes(SymbolTable *st, TreeNode *t, bool *null = NULL)
{
    switch (t->nodeKind)
    {

    case NodeKind::ExpK:
        switch (t->subkind.exp)
        {
        case ExpKind::AssignK:
        {
            if (t->attr.op == '=')
            {
                t->expType = t->child[0]->expType;
            }
            break;
        }
        default:
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

static void printAnalysis(SymbolTable *st, TreeNode *t, bool *enteredScope)
{
    switch (t->nodeKind)
    {
    case NodeKind::DeclK:
    {
        TreeNode *tmp = (TreeNode *)st->lookup(t->attr.string);
        if (!st->insert(t->attr.string, t))
        {
            printf("ERROR(%d): Symbol '%s' is already declared at line %d.\n", t->lineno, t->attr.string, tmp->lineno);
        };
        break;
    }
    case NodeKind::ExpK:
        switch (t->subkind.exp)
        {
        case ExpKind::AssignK:
        {
            if (t->attr.op == '=' || t->attr.op == '[')
            {
                t->expType = t->child[0]->expType;
            }
            OpTypeInfo currentOp = opInfoMap[t->attr.op];
            if (!currentOp.isUnary)
            {
                if (!currentOp.passesLeftCheck(t))
                {
                    printf("ERROR(%d): '%s' requires operands of type %s but lhs is of type %s.\n", t->lineno, opToString(t->attr.op), expToString(currentOp.lhs), expToString(t->child[0]->expType));
                    numAnalyzeErrors++;
                }
                if (!currentOp.passesRightCheck(t))
                {
                    printf("ERROR(%d): '%s' requires operands of type %s but rhs is of type %s.\n", t->lineno, opToString(t->attr.op), expToString(currentOp.lhs), expToString(t->child[1]->expType));
                    numAnalyzeErrors++;
                }
                if (!currentOp.passesEqualCheck(t))
                {
                    printf("ERROR(%d): '%s' requires operands of the same type but lhs is type %s and rhs is type %s.\n", t->lineno, assignToString(t->attr.op), expToString(t->child[0]->expType), expToString(t->child[1]->expType));
                    numAnalyzeErrors++;
                }
                if (!currentOp.isArrayAndWorks(t))
                {
                    printf("ERROR(%d): The operation '%s' does not work with arrays.\n", t->lineno, assignToString(t->attr.op));
                    numAnalyzeErrors++;
                }
                if (!currentOp.onlyArrayAndWorks(t))
                {
                    printf("ERROR(%d): The operation '%s' only works with arrays.\n", t->lineno, assignToString(t->attr.op));
                    numAnalyzeErrors++;
                }
            }
            break;
        }
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
            else
            {
                t->isDeclared = tmp->isDeclared;
                t->isArray = tmp->isArray;
                if (tmp->expType != ExpType::UndefinedType)
                {
                    t->expType = tmp->expType;
                }
                if (tmp->subkind.decl == DeclKind::FuncK)
                {
                    printf("ERROR(%d): Cannot use function '%s' as a variable.\n", t->lineno, t->attr.string);
                    numAnalyzeErrors++;
                }
                if (t->isInit)
                    tmp->isInit == true;
                if (tmp->isInit == false && tmp->subkind.decl == DeclKind::VarK)
                {
                    // printf("%s is in initialization state: %d", tmp->isInit)
                    printf("WARNING(%d): Variable \'%s\' may be uninitialized when used here.\n", t->lineno, t->attr.string);
                    // printf("WARNING(%d): Variable \'%s\' %p may be uninitialized when used here.\n", t->lineno, t->attr.string, tmp);
                    tmp->isInit = true;
                    // printf("Set %s to initialized\n", tmp->attr.string);
                    numAnalyzeWarnings++;
                }
            }
        }
        break;

        case ExpKind::InitK:
            break;

        case ExpKind::OpK:
        {

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
                t->expType = t->child[0]->expType;
                break;
            default:
                // return (char *)"ERROR(opToString() in util.cpp)";
                break;
            }

            OpTypeInfo currentOp = opInfoMap[t->attr.op];
            if (t->child[0]->isDeclared)
            {
                if (!currentOp.isUnary)
                {
                    if (!currentOp.passesLeftCheck(t))
                    {
                        printf("ERROR(%d): '%s' requires operands of type %s but lhs is of type %s.\n", t->lineno, opToString(t->attr.op), expToString(currentOp.lhs), expToString(t->child[0]->expType));
                        numAnalyzeErrors++;
                    }
                    if (!currentOp.passesRightCheck(t))
                    {
                        printf("ERROR(%d): '%s' requires operands of type %s but rhs is of type %s.\n", t->lineno, opToString(t->attr.op), expToString(currentOp.lhs), expToString(t->child[1]->expType));
                        numAnalyzeErrors++;
                    }
                    if (!currentOp.passesEqualCheck(t))
                    {
                        printf("ERROR(%d): '%s' requires operands of the same type but lhs is type %s and rhs is type %s.\n", t->lineno, opToString(t->attr.op), expToString(t->child[0]->expType), expToString(t->child[1]->expType));
                        numAnalyzeErrors++;
                    }
                    if (!currentOp.isArrayAndWorks(t))
                    {
                        printf("ERROR(%d): The operation '%s' does not work with arrays.\n", t->lineno, opToString(t->attr.op));
                        numAnalyzeErrors++;
                    }
                    if (!currentOp.onlyArrayAndWorks(t))
                    {
                        printf("ERROR(%d): The operation '%s' only works with arrays.\n", t->lineno, opToString(t->attr.op));
                        numAnalyzeErrors++;
                    }
                }
                else
                {
                    if (!currentOp.passesLeftCheck(t))
                    {
                        printf("ERROR(%d): Unary '%s' requires an operand of type %s but was given type %s.\n", t->lineno, opToString(t->attr.op), expToString(currentOp.lhs), expToString(t->child[0]->expType));
                    }
                    if (!currentOp.onlyArrayAndWorks(t))
                    {
                        printf("ERROR(%d): The operation '%s' only works with arrays.\n", t->lineno, opToString(t->attr.op));
                        numAnalyzeErrors++;
                    }
                    if (!currentOp.isArrayAndWorks(t))
                    {
                        printf("ERROR(%d): The operation '%s' does not work with arrays.\n", t->lineno, opToString(t->attr.op));
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
        switch (t->subkind.stmt)
        {
        case StmtKind::BreakK:
            break;
        case StmtKind::CompoundK:
            st->enter((string) "Compound Statement");
            *enteredScope = true;
            break;
        case StmtKind::ForK:
            st->enter((string) "For");
            *enteredScope = true;
            break;
        case StmtKind::IfK:
            st->enter((string) "If");
            *enteredScope = true;
            break;
        case StmtKind::NullK:
            break;
        case StmtKind::RangeK:
            break;
        case StmtKind::ReturnK:
            break;
        case StmtKind::WhileK:
            st->enter((string) "While");
            break;
        default:
            break;
        }
        break;
    default:

        break;
    }
}

static void analyzeTree(SymbolTable *st, TreeNode *t)
{
}

void semanticAnalysis(SymbolTable *st, TreeNode *root)
{
    // findTypes(st, root);
    InitOpTypeList();
    // cout << opInfoMap[AND].arrayCheck(false, true) << endl;
    numAnalyzeWarnings = 0;
    numAnalyzeErrors = 0;
    st->enter((string) "Global");
    // traverse(st, root, insertSymbols, nullProc);
    // traverse(st, root, nullProc, findTypes);
    // traverse(st, root, findTypes, findTypes);
    // traverse(st, root, nullProc, moveUpTypes);
    traverse(st, root, nullProc, printAnalysis);
    // traverse(st, root, printAnalysis, nullProc);
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

    // printf("Symbol Table:\n\n");
    // st->print(pointerPrintStr);
}