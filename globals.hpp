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
        seenInit = false;
        isUsed = false;
        isDefined = false;
        expType = ExpType::UndefinedType;
        alreadyTraversed = false;
        isDeclared = false;
        finalCheckDone = false;
        isIndexed = false;
        canEnterThisScope = true;
    }
};

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
    bool passesEqualCheck(TreeNode *t)
    {
        if (t->child[0]->expType == ExpType::UndefinedType || t->child[1]->expType == ExpType::UndefinedType)
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
        if (lhs == ExpType::UndefinedType || LEFT == ExpType::UndefinedType)
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
    }

    bool passesRightCheck(TreeNode *t)
    {
        ExpType RIGHT = t->child[1]->expType;
        if (rhs == ExpType::UndefinedType)
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
        else if (rhs == RIGHT)
            return true;
        else
            return false;
    }
    bool isArrayAndWorks(TreeNode *t)
    {
        // if (t->child[0]->isArray || t->child[1]->isArray)
        // {
        //     if (worksWithArrays)
        //     {
        //         return true;
        //     }
        //     else
        //         return false;
        // }
        // else
        //     return true;
        // if (t->attr.op == '[')
        // printf("LeftChild %s\n", t->child[0]->attr.string);
        // if (t->attr.op == '?')
        // {
        //     printf("Checking '?'\n");
        //     if (t->child[0] != NULL && t->child[0]->isIndexed)
        //         return true;
        //     else
        //         return false;
        // }
        bool leftIsArray = t->child[0]->isArray && !t->child[0]->isIndexed;
        bool rightIsArray;

        if (isUnary)
        {
            if (leftIsArray && worksWithArrays)
                return true;
            else if (onlyWorksWithArrays)
                return false;
            else
                return true;
        }
        else
        {
            rightIsArray = t->child[1]->isArray && !t->child[1]->isIndexed;
            if ((leftIsArray || rightIsArray) && worksWithArrays)
                return true;
            else if (onlyWorksWithArrays)
                return false;
            else
                return true;
        }
        // if (t->child[0]->isIndexed)
        // {
        //     printf("Is indexed\n");
        // }
        // if (t->child[1] != NULL)
        // {
        //     if (t->child[0]->isArray && !t->child[0]->isIndexed || t->child[1]->isArray && !t->child[1]->isIndexed)
        //     {
        //         if (worksWithArrays)
        //         {
        //             return true;
        //         }
        //         else
        //             return false;
        //     }
        //     else
        //         return true;
        // }
        // else
        // {
        //     if (t->child[0]->isArray && !t->child[0]->isIndexed)
        //     {
        //         if (worksWithArrays)
        //         {
        //             return true;
        //         }
        //         else
        //             return false;
        //     }
        //     else
        //         return true;
        // }

        // if (worksWithArrays)
        //     if (t->child[0]->isArray)
        //     {
        //         if (t->child[1] == NULL)
        //             return true;
        //         else if (t->child[1]->isArray)
        //             return true;
        //         else
        //             return false;
        //     }
        //     else if (t->child[0]->isArray)
        //         if (t->child[1] == NULL)
        //             return false;
        //         else if (t->child[1]->isArray)
        //             return false;
        //         else
        //             return true;
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

    OpTypeInfo(ExpType LHS, ExpType RHS, ExpType ReturnType, bool SameTypes, bool BothAreArrays, bool LeftIsArray, bool WorksWithArrays)
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
    }
    OpTypeInfo(ExpType LHS, ExpType ReturnType, bool LeftIsArray, bool WorksWithArrays, bool OnlyWorksWithArrs)
    {
        lhs = LHS;
        returnType = ReturnType;
        leftArray = LeftIsArray;
        isUnary = true;
        worksWithArrays = WorksWithArrays;
        onlyWorksWithArrays = OnlyWorksWithArrs;
    }
    OpTypeInfo()
    {
    }
};

#endif