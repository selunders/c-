#ifndef __GLOBALS_HPP__
#define __GLOBALS_HPP__
#define MAXCHILDREN 3

#include "scanType.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "symbolTable.hpp"

enum class PrintMethod
{
    None,
    Basic,
    Typed,
    Location
};

enum class RefType
{
    None,
    Local,
    LocalStatic,
    Global,
    Static,
    Parameter
};

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

char *refTypeToStr(RefType);
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
    int size;
    int location;
    RefType referenceType;

    int callLocation;
    bool seenByCodeGen;
    bool isAParam;
    int paramNum;

    TreeNode();
};

bool isUnindexedArray(TreeNode *t);

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

    bool passesEqualCheck(SymbolTable *, TreeNode *);
    bool passesLeftCheck(TreeNode *t);
    bool passesRightCheck(TreeNode *t);
    bool isArrayAndWorks(TreeNode *t);
    bool onlyArrayAndWorks(TreeNode *t);
    bool arrayCheck(TreeNode *t);
    bool arrayIndexedCorrectly(TreeNode *t);
    bool returnTypeCheck(TreeNode *t);
    bool bothArrsOrNot(SymbolTable *st, TreeNode *t);
    OpTypeInfo(ExpType LHS, ExpType RHS, ExpType ReturnType, bool SameTypes, bool BothAreArrays, bool LeftIsArray, bool WorksWithArrays, bool IsConstantExpression);
    OpTypeInfo(ExpType LHS, ExpType ReturnType, bool LeftIsArray, bool WorksWithArrays, bool OnlyWorksWithArrs, bool IsConstantExpression);
    OpTypeInfo();
};
#endif