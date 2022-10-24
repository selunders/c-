#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "scanType.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "symbolTable.hpp"

#define MAXCHILDREN 3
// #ifndef FALSE
// #define FALSE 0
// #endif

// #ifndef TRUE
// #define TRUE 1
// #endif

typedef int OpKind;

enum class TokenType
{
    ID,
    NUMCONST,
    CHARCONST,
    STRINGCONST,
    BOOLCONST,
    IF,
    ELSE,
    THEN,
    RETURN,
    EQ,
    NEQ,
    GEQ,
    LEQ,
    INT,
    OR,
    ASSIGN,
    ADD,
    ADDASS,
    SUBASS,
    MULASS,
    DIVASS,
    STATIC,
    BOOL,
    CHAR,
    AND,
    NOT,
    FOR,
    WHILE,
    BREAK,
    TO,
    BY,
    DO,
    INC,
    DEC
};

// typedef TokenType TT;

enum class NodeKind
{
    DeclK,
    StmtK,
    ExpK
};
enum class DeclKind
{
    VarK,
    FuncK,
    ParamK
};
enum class StmtKind
{
    NullK,
    IfK,
    WhileK,
    ForK,
    CompoundK,
    ReturnK,
    BreakK,
    RangeK
};
enum class ExpKind
{
    OpK,
    ConstantK,
    IdK,
    AssignK,
    InitK,
    CallK
};
enum class ExpType
{
    Void,
    Integer,
    Boolean,
    Char,
    CharInt,
    Equal,
    UndefinedType,
    Array,
    LHS
};
enum class VarKind
{
    None,
    Local,
    Global,
    Parameter,
    LocalStatic
};

// enum class OpType
// {
//     Bool,
//     Array,
//     Int,
//     LHS,
//     Char,
//     IntOrChar,
//     Any
// };

class TreeNode
{
public:
    TreeNode *child[MAXCHILDREN];
    TreeNode *sibling;

    int lineno;
    NodeKind nodeKind;

    union
    {
        DeclKind decl; // DeclK
        StmtKind stmt; // StmtK
        ExpKind exp;   // ExpK
    } subkind;

    union
    {
        OpKind op;            // type of token (same as bison)
        int value;            // for int or boolean
        unsigned char cvalue; // character
        char *string;         // IdK
        // char* string;           // string constant
        // Learning some about how unions work. Since I've already decleared a char*, I can't assign
        // to 'name' because 'string' already exists. I can still read from 'name' though...
        // int idIndex;            // IdK
        // char* name;             // IdK
    } attr;

    ExpType expType;
    bool isArray;
    bool isStatic;
    bool isInit;
    bool seenInit;
    bool isDefined;
    bool isDeclared;
    bool isUsed;
    bool finalCheckDone;
    bool alreadyTraversed;
    bool isIndexed;
    bool canEnterThisScope;
    bool needsInitCheck;
    int declDepth;
    bool declChecked;
    bool isForVar;
    bool isConstantExp;
    // bool enteredScope;
    TreeNode()
    {
        sibling = NULL;
        int i;
        for (i = 0; i < MAXCHILDREN; i++)
        {
            child[i] = NULL;
        }
        nodeKind = NodeKind::StmtK;
        isArray = false;
        isStatic = false;
        isInit = false;
        needsInitCheck = false;
        seenInit = false;
        isUsed = false;
        isDefined = false;
        expType = ExpType::UndefinedType;
        alreadyTraversed = false;
        isDeclared = false;
        declChecked = false;
        finalCheckDone = false;
        isIndexed = false;
        canEnterThisScope = true;
        isForVar = false;
        isConstantExp = false;
    }
};

// #ifndef UNINDEXED_ARRAY
// #define UNINDEXED_ARRAY
static bool isUnindexedArray(TreeNode *t)
{
    if (t->nodeKind == NodeKind::ExpK && t->subkind.exp == ExpKind::AssignK && t->attr.op == '=')
    {
        return isUnindexedArray(t->child[0]);
    }
    if (t->isArray)
    {
        if (t->child[0] != NULL)
            t->isIndexed = true;
        if (t->isIndexed)
            return false;
        else
            return true;
    }
    else
        return false;
}
// #endif

class OpTypeInfo
{
public:
    ExpType lhs;
    ExpType rhs;
    ExpType returnType;
    bool equalTypes;
    bool bothArrays;
    bool leftArray;
    bool isUnary;
    bool worksWithArrays;
    bool onlyWorksWithArrays;
    bool isConstantExpression;
    bool passesEqualCheck(TreeNode *t)
    {
        // if (t->child[0]->expType == ExpType::UndefinedType || t->child[1]->expType == ExpType::UndefinedType)
            if (t->child[0]->expType == ExpType::UndefinedType || t->child[1]->expType == ExpType::UndefinedType || t->child[0]->expType == ExpType::Void || t->child[1]->expType == ExpType::Void)
            return true;
        if (equalTypes)
            return (t->child[0]->expType == t->child[1]->expType);
        else
            return true;
        // return (t->child[0]->expType != t->child[1]->expType);
    }

    bool passesLeftCheck(TreeNode *t)
    {
        ExpType LEFT = t->child[0]->expType;
        // if (lhs == ExpType::UndefinedType || LEFT == ExpType::UndefinedType)
        if (lhs == ExpType::UndefinedType || LEFT == ExpType::UndefinedType || lhs == ExpType::Void || LEFT == ExpType::Void)
        // if (lhs == ExpType::UndefinedType || LEFT == ExpType::UndefinedType)
        {
            return true;
        }
        else if (lhs == ExpType::CharInt)
        {
            if (LEFT == ExpType::Integer || LEFT == ExpType::Char)
            {
                return true;
            }
        }
        else if (lhs == ExpType::Array)
        {
            if (t->child[0]->isArray && !t->child[0]->isIndexed)
                return true;
        }
        else if (lhs == LEFT)
            return true;
        else
            return false;
            // printf("Couldn't match sides: Exp %d Got %d\n", lhs, LEFT);
    }

    bool passesRightCheck(TreeNode *t)
    {
        // pointerPrintNode(t);
        ExpType RIGHT = t->child[1]->expType;
        if (rhs == ExpType::UndefinedType || RIGHT == ExpType::UndefinedType || rhs == ExpType::Void || RIGHT == ExpType::Void)
        // if (rhs == ExpType::UndefinedType || RIGHT == ExpType::UndefinedType)
        {
            return true;
        }
        else if (rhs == ExpType::CharInt)
        {
            if (RIGHT == ExpType::Integer || RIGHT == ExpType::Char)
            {
                return true;
            }
        }
        else if (rhs == ExpType::Array)
        {
            if (t->child[1]->isArray && !t->child[1]->isIndexed)
                return true;
        }
        else if (rhs == RIGHT)
            return true;
        else
            return false;
    }
    bool isArrayAndWorks(TreeNode *t)
    {
        bool leftIsArray = (t->child[0]->isArray && !t->child[0]->isIndexed);
        bool rightIsArray;

        if (isUnary)
        {
            // printf("left is arr: %d works with arrs: %d\n", leftIsArray, worksWithArrays);
            if ((leftIsArray && worksWithArrays) || !leftIsArray)
                return true;
            else
                return false;
        }
        else // is binary
        {
            rightIsArray = (t->child[1]->isArray && !t->child[1]->isIndexed);
            // printf("Left is array: '%d' Right is array: '%d'\n", leftIsArray, rightIsArray);
            // printf("Left is type: '%d' Right is type: '%d'\n", t->child[0]->subkind.stmt, t->child[1]->subkind.stmt);
            if (((leftIsArray || rightIsArray) && worksWithArrays) || (!leftIsArray && !rightIsArray))
                return true;
            // else if (onlyWorksWithArrays)
            //     return false;
            else
                return false;
        }
        // printf("Left is array: %d, Right is array: %d\n", leftIsArray, rightIsArray);
    }

    bool onlyArrayAndWorks(TreeNode *t)
    {
        // Only used for unary sizeof op I believe
        // printf("%d %s array\n", t->child[0]->attr.op, t->child[0]->isArray ? (char*) "is" : (char*) "is not");
        if (onlyWorksWithArrays)
        {
            // if (t->child[0]->isArray)
            if (t->child[0]->isArray && !t->child[0]->isIndexed || t->child[0]->expType == ExpType::UndefinedType)
                return true;
            else
                return false;
        }
        else
            return true;
    }

    bool arrayCheck(TreeNode *t)
    {
        bool leftIsArr = (t->child[0]->isArray && !t->child[0]->isIndexed);
        bool rightIsArr = (t->child[1]->isArray && !t->child[1]->isIndexed);
        bool passesCheck = true;
        if (bothArrays)
            if (!leftIsArr || !rightIsArr)
                passesCheck = false;
        if (equalTypes)
            if (leftIsArr != rightIsArr)
                passesCheck = false;
        if (leftArray)
            if (!leftIsArr || rightIsArr)
                passesCheck = false;
        return passesCheck;
    }

    bool arrayIndexedCorrectly(TreeNode *t)
    {
        if (leftArray)
            if (t->child[0]->isArray && t->child[1] != NULL)
                if (t->child[1]->isArray)
                    return t->child[1]->isIndexed;
                else
                    return true;
        return (t->child[1]->isArray || t->child[1]->isIndexed);
    }

    bool returnTypeCheck(TreeNode *t)
    {
        ExpType retType = t->expType;
        bool isArray = t->isArray;
        if (isArray)
            return false;
        if (returnType == ExpType::UndefinedType)
        {
            return true;
        }
        else if (returnType == ExpType::CharInt)
        {
            if (retType == ExpType::Integer || retType == ExpType::Char)
            {
                return true;
            }
        }
        else if (returnType == retType)
            return true;
        return false;
    }

    bool bothArrsOrNot(SymbolTable *st, TreeNode *t)
    {
        // TreeNode *tl = ((t->child[0]->nodeKind == NodeKind::ExpK) && (t->child[0]->subkind.exp == ExpKind::IdK)) ? (TreeNode *)st->lookup(t->child[0]->attr.string) : t->child[0];
        // TreeNode *tr = ((t->child[1]->nodeKind == NodeKind::ExpK) && (t->child[1]->subkind.exp == ExpKind::IdK)) ? (TreeNode *)st->lookup(t->child[1]->attr.string) : t->child[1];
        TreeNode *tl = t->child[0];
        TreeNode *tr = t->child[1];

        bool leftIsArray = isUnindexedArray(tl);
        // if (tl != NULL && tl->isArray)
        //     if (tl->isIndexed)
        //         leftIsArray = false;
        //     else
        //         leftIsArray = true;
        // else
        //     leftIsArray = false;

        bool rightIsArray = isUnindexedArray(tr);
        // if (tr != NULL && tr->isArray)
        //     if (tr->isIndexed)
        //         rightIsArray = false;
        //     else
        //         rightIsArray = true;
        // else
        //     rightIsArray = false;
        // bool rightIsArray = (!tr->isArray || (tr->isArray && !tr->isIndexed));
        // printf("LeftIsArr: %s RightIsArr: %s\n", leftIsArray ? "true" : "false", rightIsArray ? "true" : "false");
        if (leftIsArray == rightIsArray)
            return true;
        else
            return false;
    }

    OpTypeInfo(ExpType LHS, ExpType RHS, ExpType ReturnType, bool SameTypes, bool BothAreArrays, bool LeftIsArray, bool WorksWithArrays, bool IsConstantExpression)
    {
        lhs = LHS;
        rhs = RHS;
        returnType = ReturnType;
        equalTypes = SameTypes;
        bothArrays = BothAreArrays;
        leftArray = LeftIsArray;
        isUnary = false;
        worksWithArrays = WorksWithArrays;
        onlyWorksWithArrays = false;
        isConstantExpression = IsConstantExpression;
    }
    OpTypeInfo(ExpType LHS, ExpType ReturnType, bool LeftIsArray, bool WorksWithArrays, bool OnlyWorksWithArrs, bool IsConstantExpression)
    {
        lhs = LHS;
        returnType = ReturnType;
        leftArray = LeftIsArray;
        isUnary = true;
        worksWithArrays = WorksWithArrays;
        onlyWorksWithArrays = OnlyWorksWithArrs;
        isConstantExpression = IsConstantExpression;
    }
    OpTypeInfo()
    {
    }
};

// class LibraryFunction
// {
// public:
//     ExpType returnType;
//     char* funName;
//     ExpType paramType;


//     LibraryFunction()
//     {

//     }
// };

#endif
// class ParentInfo
// {
// public:
//     TreeNode *parentNode;

//     bool enteredScope;
//     bool enteredFunction;
//     bool enteredLoop;

//     ParentInfo()
//     {
//         enteredScope = false;
//         enteredFunction = false;
//         enteredLoop = false;
//     }

//     ~ParentInfo()
//     {
//         parentNode = NULL;
//     }
// };

/*



*/