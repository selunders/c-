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
    bool isUsed;
    bool alreadyTraversed;

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
    bool passesEqualCheck(SymbolTable* st, TreeNode* t)
    {
        if(equalTypes)
            return (t->child[0]->expType == t->child[1]->expType);
        else
            return (t->child[0]->expType != t->child[1]->expType);
    }

    bool passesLeftCheck(ExpType LEFT)
    {
        if (lhs == ExpType::UndefinedType)
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
        else if (lhs == LEFT)
            return true;
        else
            return false;
    }

    bool passesRightCheck(ExpType RIGHT)
    {
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

    bool arrayCheck(bool leftIsArr, bool rightIsArr)
    {
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

    bool returnTypeCheck(ExpType retType, bool isArray)
    {
        if(isArray)
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

    OpTypeInfo(ExpType LHS, ExpType RHS, ExpType ReturnType, bool SameTypes, bool BothAreArrays, bool LeftIsArray)
    {
        lhs = LHS;
        rhs = RHS;
        returnType = ReturnType;
        equalTypes = SameTypes;
        bothArrays = BothAreArrays;
        leftArray = LeftIsArray;
        isUnary = false;
    }
    OpTypeInfo(ExpType LHS, ExpType ReturnType, bool LeftIsArray)
    {
        lhs = LHS;
        returnType = ReturnType;
        leftArray = LeftIsArray;
        isUnary = true;
    }
    OpTypeInfo()
    {
    }
};

#endif