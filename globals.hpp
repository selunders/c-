#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "scanType.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


#define MAXCHILDREN 3
// #ifndef FALSE
// #define FALSE 0
// #endif

// #ifndef TRUE
// #define TRUE 1
// #endif

typedef int OpKind;

enum class TokenType {
    ID, NUMCONST, CHARCONST, STRINGCONST, BOOLCONST,
    IF, ELSE, THEN, RETURN, EQ, NEQ, GEQ, LEQ, INT, OR,
    ADDASS, SUBASS, MULASS, DIVASS,
    STATIC, BOOL, CHAR,
    AND, NOT, FOR, WHILE, BREAK, TO, BY, DO, INC, DEC
};

// typedef TokenType TT;

enum class NodeKind { DeclK, StmtK, ExpK };
enum class DeclKind { VarK, FuncK, ParamK };
enum class StmtKind { NullK, IfK, WhileK, ForK, CompoundK, ReturnK, BreakK, RangeK };
enum class ExpKind  { OpK, ConstantK, IdK, AssignK, InitK, CallK };
enum class ExpType  { Void, Integer, Boolean, Char, CharInt, Equal, UndefinedType };
enum class VarKind  { None, Local, Global, Parameter, LocalStatic };


class TreeNode
{
    public:
        TreeNode* child[MAXCHILDREN];
        TreeNode* sibling;

        int lineno;
        NodeKind nodeKind;

        union
        {
            DeclKind    decl;   // DeclK
            StmtKind    stmt;   // StmtK
            ExpKind     exp;    // ExpK
        } subkind;
        
        union
        {
            OpKind op;              // type of token (same as bison)
            int value;              // for int or boolean
            unsigned char cvalue;   // character
            char* string;           // string constant
            char* name;             // IdK
        // Learning some about how unions work. Since I've already decleared a char*, I can't assign
            // to 'name' because 'string' already exists. I can still read from 'name' though...
            // int idIndex;            // IdK
            // char* name;             // IdK
        } attr;

        ExpType expType;
        bool isArray;
        bool isStatic;
        bool isUsed;

        TreeNode()
        {
            sibling = NULL;
            int i;
            for(i = 0; i < MAXCHILDREN; i++)
            {
                child[i] = NULL;
            }
            nodeKind = NodeKind::StmtK;
            isArray = false;
            isStatic = false;
            isUsed = false;
        }
};

#endif