%code requires {
    #include "globals.hpp"
}

%{
// // // // // // // // // // // // // // // // // // // // // // // 
// Based on:
// CS445 - Calculator Example Program written in the style of the C-
// compiler for the class.
//
// Robert Heckendorn
// Jan 21, 2021    

#include "globals.hpp"
// #include "scanType.h"  // TokenData Type
#include "util.hpp"
#include <stdio.h>
#include <string.h> 
#include "yyerror.h"

extern int yylex();
extern FILE *yyin;
extern int line;         // ERR line number from the scanner!!
extern int numErrors;    // ERR err count
extern void resetLexer();

TreeNode* rootNode;

#define YYERROR_VERBOSE
// void yyerror(const char *msg)
// {
//     printf("ERROR(%d): %s\n", line, msg);
//     numErrors++;
// }

void printCharByChar(char* stringToPrint)
{
    int i = 0;
    while(i < strlen(stringToPrint))
    {
        printf("%c", stringToPrint[i]);
        i++;
    }
}

void resetParse()
{
    line = 1;
    // numErrors = 0;
    resetLexer();
    // rootNode = NULL;
}

%}

// this is included in the tab.h file
// so scanType.h must be included before the tab.h file!!!!
%union {
    ExpType type;
    TreeNode* tree;
    TokenData* tokenData;
    // double value;
}

%token <tokenData> ID NUMCONST CHARCONST STRINGCONST BOOLCONST
%token <tokenData> IF ELSE THEN RETURN EQ NEQ GEQ LEQ INT OR
%token <tokenData> ADDASS SUBASS MULASS DIVASS '=' MODULO
%token <tokenData> STATIC BOOL CHAR
%token <tokenData> AND NOT FOR WHILE BREAK TO BY DO INC DEC
%token <tokenData> SIZEOF MULTIPLY NEGATIVE SUBTRACT
%token <tokenData> '+' '*' '-' '\\' '<' '>' '/' '?' ';' '(' '{' '['
%type <type> typeSpec
%type <tree> program declList decl varDecl scopedVarDecl varDeclList varDeclInit varDeclId // typeSpec
%type <tree> funDecl parms parmList parmTypeList parmIdList parmId stmt expStmt compoundStmt localDecls
%type <tree> stmtList selectSuperStmt open_iterStmt closed_iterStmt open_stmt closed_stmt otherStmt iterRange returnStmt breakStmt exp assignop simpleExp andExp
%type <tree> unaryRelExp relExp relop sumExp sumop mulExp mulop unaryExp unaryop factor mutable immutable 
%type <tree> call args argList constant
/* %type <tree>  */
/* %type <tree>  */
/* %type <tree>  */

/* %type <tree>  */

/* %type <tree> program declList decl varDecl varDeclList varDeclInit varDeclId funDecl */
/* %type <tree> parms parmList parmTypeList parmIdList parmId */
/* %type <tree> stmt selectSuperStmt open_iterStmt open_stmt closed_iterStmt closed_stmt */
/* %type <tree> compoundStmt */
/* %type <tokenData> constant */
/* %type  <value> expression sumexp mulexp unary factor */

%%
program
    : declList {rootNode = $1;}
    /* | error                                   { $$ = NULL; } */
    ;
declList
    : declList decl
        {
            $$ = addSibling($1, $2);
        }
    | decl
        {
            $$ = $1;
        }
    /* | error                                   { $$ = NULL; } */
    ;
decl
    : varDecl {$$ = $1;}
    | funDecl {$$ = $1;}
    | error   { $$ = NULL; }
    ;
varDecl
    : typeSpec[type] varDeclList[vdecllist] ';'
        {
            // $$ = newDeclNode(DeclKind::VarK, $[type], $3, $[vdecllist], NULL, NULL);
            $$ = $[vdecllist];
            setType($$, $[type], false, false);
            yyerrok;
        }
    | error varDeclList ';'
        {
            $$ = NULL;
            yyerrok;
        }
    | typeSpec error ';'
        {
            $$ = NULL;
            yyerrok;
        }
    ;
scopedVarDecl
    : STATIC typeSpec[type] varDeclList[vdecllist] ';'
        {
            // $$ = newDeclNode(DeclKind::VarK, $[type], $1, $[vdecllist], NULL, NULL);
            $$ = $[vdecllist];
            setType($$, $[type], true, false);
            // Might need to actually do this:
            // setType($$->child[0], ... )
            yyerrok;
        }
    | typeSpec[type] varDeclList[vdecllist] ';'
        {
            // $$ = newDeclNode(DeclKind::VarK, $[type], $3, $[vdecllist], NULL, NULL);
            $$ = $[vdecllist];
            setType($$, $[type], false, false);
            // Might need to actually do this:
            // setType($$->child[0], ... )
            yyerrok;
        }
    ;
varDeclList
    : varDeclList[vdecllist] ',' varDeclInit[vdeclinit]
        {
            $$ = $[vdecllist];
            addSibling($$, $[vdeclinit]);
            yyerrok;
        }
    | varDeclInit
        {
            $$ = $1;
        }
    | varDeclList ',' error
        {
            $$ = NULL;
        }
    | error { $$ = NULL; }
    ;
varDeclInit
    : varDeclId
        {
            $$ = $1;
        }
    | varDeclId[vdeclid] ':' simpleExp[simpleexp]
        {
            $$ = $[vdeclid];
            if($$ != NULL)
                $$->child[0] = $[simpleexp];
            // $$->isUsed = true;
            // $$->isInit = false; 
            // if($[simpleexp]->nodeKind == NodeKind::ExpK && $[simpleexp]->subkind.exp == ExpKind::ConstantK && $[simpleexp]->isArray)
            // {
                // int len = strlen($[simpleexp]->attr.string);
                // if(len > $$->size)
                    // $$->size = len;
            // }
        }
    | error ':' simpleExp { $$ = NULL; yyerrok; }
    /* | varDeclId[vdeclid] ':' error
    {
        $$ = $[vdeclid];
        yyerrok;
    } */
    ;
varDeclId
    : ID
        {
            $$ = newDeclNode(DeclKind::VarK, ExpType::UndefinedType, $1, NULL, NULL, NULL);
            // $$ = newExpNode(ExpKind::IdK, $1, NULL, NULL, NULL);
            // printf("Found ID: %s\n\n", $1->tokenstr);
        }
    | ID '['NUMCONST']'
        {
            // TreeNode* ntmp = newExpNode(ExpKind::ConstantK, $3, NULL, NULL, NULL);
            $$ = newDeclNode(DeclKind::VarK, ExpType::UndefinedType, $1, NULL, NULL, NULL);
            // $$ = newExpNode(ExpKind::IdK, $1, NULL, NULL, NULL);
            // $$ = newExpNode(ExpKind::IdK, $1, NULL, NULL, NULL);
            $$->isArray = true;
            $$->isIndexed = false;
            // $$->size = $3->numValue;
            $$->size = $3->numValue + 1;
            // $$->isInit = false;
            // printf("Found ID: %s\n\n", $1->tokenstr);
        }
    | ID '[' error
        {
            $$ = NULL;
            // Might be an issue here, refer to grammarmods.txt
            // yyerrok;    
        }
    | error ']'
        {
            $$ = NULL;
            yyerrok;
        }
    ;
typeSpec
    : BOOL
        {
            $$ = ExpType::Boolean;
            //  printf("Line %d Token: %s, Value: %d\n", $1->linenum, $1->tokenstr, $1->boolValue);
        }
    | CHAR
        {
            $$ = ExpType::Char;
            //  printf("Line %d Token: %s char: %c\n", $1->linenum, $1->tokenstr, $1->charValue);
        }
    | INT
        {
            $$ = ExpType::Integer;
            //  printf("Line %d Token: %s int: %c\n", $1->linenum, $1->tokenstr, $1->numValue);
        }
    ;
funDecl
    : typeSpec[type] ID[id] '(' parms[prms] ')' compoundStmt[cstmt]
        {
            $$ = newDeclNode(DeclKind::FuncK, $[type], $[id], $[prms], $[cstmt], NULL);
            
            $$->attr.string = strdup($[id]->tokenstr);
            // $[cstmt]->attr.string = strdup($[id]->tokenstr);
            
            $$->isDefined = true;
            // $$->attr.string = strdup($)
            // setType($$, $[type], $$->isStatic, false);
            // $[cstmt]->attr.string = $[id]->tokenstr ? strdup($[id]->tokenstr) : (char*) "";
            // $$->attr.idIndex = $id->idIndex;
            // $$->isInit = false;
            $$->isInit = false;
            // $$->isUsed = true;
            $$->isDeclared = true;
            $$->isConstantExp = true;
            $[cstmt]->canEnterThisScope = false;
        }
    | ID[id] '(' parms[prms] ')' compoundStmt[cstmt]
        {
            $$ = newDeclNode(DeclKind::FuncK, ExpType::Void, $[id], $[prms], $[cstmt], NULL);
            $[cstmt]->attr.string = strdup($[id]->tokenstr);
            $$->isDefined = true;
            // setType($$, ExpType::Void, $$->isStatic, false);
            $$->isInit = false;
            // $$->isUsed = true;
            $$->isDeclared = true;
            $[cstmt]->canEnterThisScope = false;
        }
    | typeSpec error            { $$ = NULL; }
    | typeSpec ID '(' error     { $$ = NULL; }
    | ID '(' error              { $$ = NULL; }
    | ID '(' parms ')' error    { $$ = NULL; }
    ;
parms
    : %empty
        {
            $$ = NULL;
        }
    | parmList
        {
            $$ = $1;
            // $$ = newDeclNode(DeclKind::ParamK, ExpType::UndefinedType, );
        }
    ;
parmList
    : parmList[prmList] ';' parmTypeList[prmTypeList]
        {
            $$ = addSibling($[prmList], $[prmTypeList]);
        }
    | parmTypeList
        {
            $$ = $1;
        }
    | parmList ';' error    { $$ = NULL; }
    | error                 { $$ = NULL; }
    ;
parmTypeList
    : typeSpec[type] parmIdList[prmidlist]
        {
            $$ = $[prmidlist];
            // printf("Found a list of parameters\n");
            if($$ != NULL)
                setType($$, $[type], $$->isStatic, false);
        }
    | typeSpec error { $$ = NULL; }
    ;
parmIdList
    : parmIdList[prmidlist] ',' parmId[prmid]
        {
            $$ = addSibling($[prmidlist], $[prmid]);
            yyerrok;
        }
    | parmId
        {
            $$ = $1;
            // yyerrok;
        }
    | parmIdList ',' error { $$ = NULL; }
    | error { $$ = NULL; }
    ;
parmId
    : ID
        {
            $$ = newDeclNode(DeclKind::ParamK, ExpType::UndefinedType,  $1, NULL, NULL, NULL);
            // $$ = newExpNode(ExpKind::IdK, $1, NULL, NULL, NULL);
            // $$->isArray = false;
            // $$->needsInitCheck = false;
            // printf("Found ID: %s\n\n", $1->tokenstr);
        }
    | ID '['']'
        {
            $$ = newDeclNode(DeclKind::ParamK, ExpType::UndefinedType, $1, NULL, NULL, NULL);
            // $$ = newExpNode(ExpKind::IdK, $1, NULL, NULL, NULL);
            $$->isArray = true;
            // $$->needsInitCheck = false; 
            // printf("Found ID array: %s\n\n", $1->tokenstr);
        }
stmt
    : selectSuperStmt
        {
            $$ = $1;
        }
    ;
otherStmt
    : expStmt
        {
            $$ = $1;
        }
    | compoundStmt
        {
            $$ = $1;
        }
    | closed_iterStmt
        {
            $$ = $1;
        }
    | returnStmt
        {
            $$ = $1;
        }
    | breakStmt
        {
            $$ = $1;
        }
    ;
expStmt
    : exp ';'
        {
            $$ = $1;
        }
    | ';'
        {
            $$ = NULL;
        }
    | error ';'
        {
            $$ = NULL;
            yyerrok;
        }
    ;
compoundStmt
    : '{' localDecls[lcldecls] stmtList[stmtlist] '}'
        {
            $$ = newStmtNode(StmtKind::CompoundK, $1, $[lcldecls], $[stmtlist], NULL);
            yyerrok;
        }
    ;
localDecls
    : %empty
        {
            $$ = NULL;
        }
    | localDecls[lcldecls] scopedVarDecl[svardecl]
        {
            $$ = addSibling($[lcldecls], $[svardecl]);
        }
    ;
stmtList
    : %empty
        {
            $$ = NULL;
        }
    | stmtList[slist] stmt[s]
        {
            $$ = addSibling($[slist], $[s]);
        }
    ;
selectSuperStmt
    : open_stmt
        {
            $$ = $1;
        }
    | closed_stmt
        {
            $$ = $1;
        }
    ;
open_stmt
    : IF simpleExp[simpleexp] THEN selectSuperStmt[slctSperStmt]
        {
            $$ = newStmtNode(StmtKind::IfK, $1, $[simpleexp], $[slctSperStmt], NULL);
        }
    | IF simpleExp[simpleexp] THEN closed_stmt[clsdstmt] ELSE open_stmt[opnstmt]
        {
            $$ = newStmtNode(StmtKind::IfK, $1, $[simpleexp], $[clsdstmt], $[opnstmt]);
        }
    | open_iterStmt
        {
            $$ = $1;
        }
    | IF error THEN selectSuperStmt[slctSperStmt]
        {
            $$ = NULL; yyerrok;
        }
    | IF error THEN closed_stmt[clsdstmt] ELSE open_stmt[opnstmt]
        {
            $$ = NULL;
            yyerrok;
        }
    ;
closed_stmt
    : otherStmt
        {
            $$ = $1;
        }
    | IF simpleExp[simpleexp] THEN closed_stmt[clsstmt1] ELSE closed_stmt[clsstmt2]
        {
            $$ = newStmtNode(StmtKind::IfK, $1, $[simpleexp], $[clsstmt1], $[clsstmt2]);
        }
    | IF error { $$ = NULL; }
    | IF error ELSE closed_stmt { $$ = NULL; yyerrok; }
    | IF error THEN closed_stmt ELSE closed_stmt { $$ = NULL; yyerrok; }
    ;
open_iterStmt
    : WHILE simpleExp[simpleexp] DO open_stmt[opnstmt]
        {
            $$ = newStmtNode(StmtKind::WhileK, $1, $[simpleexp], $[opnstmt], NULL);
        }
    | FOR ID[id] '=' iterRange[itrrng] DO open_stmt[opnstmt]
        {
            TreeNode* tmp = newDeclNode(DeclKind::VarK, ExpType::Integer, $[id], NULL, NULL, NULL);
            
            // All isInit is set to true at creation, any set to false will be set to true. This prevents the need for another flag.
            tmp->isInit = false;
            // printf("%d Setting %s to uninit\n", tmp->lineno, tmp->attr.string);
            // tmp->isUsed = true;
            tmp->isDeclared = true;
            // TreeNode* tmp = newExpNode(ExpKind::IdK, $[id], NULL, NULL, NULL);
            $$ = newStmtNode(StmtKind::ForK, $1, tmp, $[itrrng], $[opnstmt]);
        }
    | WHILE error DO open_iterStmt[opnstmt]
        {
            $$ = NULL; yyerrok;
        }
    /* | FOR ID '=' error DO open_stmt { $$ = NULL; yyerrok; } */
    /* | FOR error { $$ = NULL; } */
    ;
closed_iterStmt
    : WHILE simpleExp[simpleexp] DO closed_stmt[clsdstmt]
        {
            $$ = newStmtNode(StmtKind::WhileK, $1, $[simpleexp], $[clsdstmt], NULL);
        }
    | FOR ID[id] '=' iterRange[itrrng] DO closed_stmt[clsdstmt]
        {
            TreeNode* tmp = newDeclNode(DeclKind::VarK, ExpType::Integer, $[id], NULL, NULL, NULL);
            tmp->isInit = false;
            // tmp->isUsed = true;
            // printf("%d Setting %s to uninit (close)\n", tmp->lineno, tmp->attr.string);
            tmp->isDeclared = true;
            // TreeNode* tmp = newExpNode(ExpKind::IdK, $[id], NULL, NULL, NULL);
            $$ = newStmtNode(StmtKind::ForK, $1, tmp, $[itrrng], $[clsdstmt]);
        }
    | WHILE error DO closed_iterStmt[opnstmt]
        {
            $$ = NULL; yyerrok;
        }
    | WHILE error
        {
            $$ = NULL;
        }
    | FOR ID '=' error DO closed_stmt {$$ = NULL; yyerrok; }
    | FOR error { $$ = NULL; }
    ;
iterRange
    : simpleExp[low] TO simpleExp[high]
        {
            $$ = newStmtNode(StmtKind::RangeK, $2, $[low], $[high], NULL);
        }
    | simpleExp[low] TO simpleExp[high] BY simpleExp[step]
        {
            $$ = newStmtNode(StmtKind::RangeK, $2, $[low], $[high], $[step]);
        }
    | simpleExp TO error { $$ = NULL; }
    | error BY error { $$ = NULL; yyerrok; }
    | simpleExp TO simpleExp BY error { $$ = NULL; }
    ;
returnStmt
    : RETURN ';'
        {
            $$ = newStmtNode(StmtKind::ReturnK, $1, NULL, NULL, NULL);
            $$->expType = ExpType::Void;
        }
    | RETURN exp[e] ';'
        {
            $$ = newStmtNode(StmtKind::ReturnK, $1, $[e], NULL, NULL);
            yyerrok;
        }
    | RETURN error ';'
        {
            $$ = NULL;
            yyerrok;
        }
    ;
breakStmt
    : BREAK ';'
        {
            $$ = newStmtNode(StmtKind::BreakK, $1, NULL, NULL, NULL);
        }
    ;
exp
    : mutable[m] assignop[aop] exp[e]
        {
            $$ = $[aop];
            $$->child[0] = $[m];
            // $[m]->isUsed = true;
            // $[m]->isInit = false;
            $$->child[1] = $[e];
            // $[e]->needsInitCheck = true;
        }
    | mutable[m] INC[inc]
        {
            $$ = newExpNode(ExpKind::AssignK, $[inc], $[m], NULL, NULL);
            // $[m]->needsInitCheck = true;
        }
    | mutable[m] DEC[dec]
        {
            $$ = newExpNode(ExpKind::AssignK, $[dec], $[m], NULL, NULL);
            // $[m]->needsInitCheck = true;
        }
    | simpleExp
        {
            $$ = $1;
        }
    | error assignop exp
        {
            $$ = NULL;
            yyerrok;
        }
    | mutable assignop error
        {
            $$ = NULL;
        }
    | error INC
        {
            $$ = NULL;
            yyerrok;
        }
    | error DEC
        {
            $$ = NULL;
            yyerrok;
        }
    ;
assignop
    : '='
        {
            $$ = newExpNode(ExpKind::AssignK, $1, NULL, NULL, NULL);
            // $$->expType = ExpType::UndefinedType;
        }
    | ADDASS
        {
            $$ = newExpNode(ExpKind::AssignK, $1, NULL, NULL, NULL);
            // $$->expType = ExpType::Integer;
        }
    | SUBASS
        {
            $$ = newExpNode(ExpKind::AssignK, $1, NULL, NULL, NULL);
            // $$->expType = ExpType::Integer;
        }
    | MULASS
        {
            $$ = newExpNode(ExpKind::AssignK, $1, NULL, NULL, NULL);
            // $$->expType = ExpType::Integer;
        }
    | DIVASS
        {
            $$ = newExpNode(ExpKind::AssignK, $1, NULL, NULL, NULL);
            // $$->expType = ExpType::Integer;
        }
    ;
simpleExp
    : simpleExp[sexp] OR[or] andExp[aexp]
        {
            $$ = newExpNode(ExpKind::OpK, $[or], $[sexp], $[aexp], NULL);
            
            ///************************************************///
            // Pretty sure these should be true (same with AND) //
            ///************************************************/// 
            
            // $[sexp]->needsInitCheck = false;
            // $[aexp]->needsInitCheck = false;
            $$->expType = ExpType::Boolean;
        }
    | andExp
        {
            $$ = $1;
        }
    | simpleExp OR error
        {
            $$ = NULL;
        }
    ;
andExp
    : andExp[aexp] AND[and] unaryRelExp[urexp]
        {
            $$ = newExpNode(ExpKind::OpK, $[and], $[aexp], $[urexp], NULL);
            // $[aexp]->needsInitCheck = false;
            // $[urexp]->needsInitCheck = false;
            $$->expType = ExpType::Boolean;
        }
    | unaryRelExp
        {
            $$ = $1;
        }
    | andExp AND error
        {
            $$ = NULL;
        }
    ;
unaryRelExp
    : NOT unaryRelExp[urexp]
        {
            $$ = newExpNode(ExpKind::OpK, $1, $2, NULL, NULL);
            // $[urexp]->needsInitCheck = true;
            // $$->expType == ExpType::Boolean;
        }
    | relExp
        {
            $$ = $1;
        }
    | NOT error
        {
            $$ = NULL;
        }
    ;
relExp
    : sumExp[sexp] relop[rop] sumExp[sexp2]
        {
            $$ = $[rop];
            $$->child[0] = $[sexp];
            // $[sexp]->isUsed = true;
            $$->child[1] = $[sexp2];
            // $[sexp2]->isUsed = true;
            $$->expType = ExpType::Boolean;
            // $[sexp]->needsInitCheck = true;
            // $[sexp2]->needsInitCheck = true;
        }
    | sumExp
        {
            $$ = $1;
        }
    ;
relop
    : '<'
        {
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    | LEQ
        {
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    | '>'
        {
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    | GEQ
        {
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    | EQ
        {
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    | NEQ
        {
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    ;
sumExp
    : sumExp[sexp] sumop[sop] mulExp[mexp]
        {
            $$ = $[sop];
            $$->child[0] = $[sexp];
            // $[sexp]->isUsed = true;
            $$->child[1] = $[mexp];
            // $[sexp]->needsInitCheck = true;
            // $[mexp]->needsInitCheck = true;
            // $[mexp]->isUsed = true;
            // $$->expType = ExpType::Integer;
        }
    | mulExp
        {
            $$ = $1;
        }
    | sumExp sumop error
        {
            $$ = NULL;
        }
    ;
sumop
    : '+'
        {
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    | '-'
        {
            $1->tokenclass = SUBTRACT;
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    | SUBTRACT
        {
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    ;
mulExp
    : mulExp[mexp] mulop[mop] unaryExp[uexp]
        {
            $$ = $[mop];
            $$->child[0] = $[mexp];
            // $[mexp]->isUsed = true;
            $$->child[1] = $[uexp];
            // $[mexp]->needsInitCheck = true;
            // $[uexp]->needsInitCheck = true;
            // $[uexp]->isUsed = true;
            // $$->expType = ExpType::Integer;
        }
    | unaryExp
        {
            $$ = $1;
        }
    | mulExp mulop error
        {
            $$ = NULL;
        }
    ;
mulop
    : '*'
        {
            $1->tokenclass = MULTIPLY;
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
            $$->expType = ExpType::Integer;
        }
    | '/'
        {
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    | MODULO
        {
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    | MULTIPLY
        {
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    ;
unaryExp
    : unaryop[uop] unaryExp[uexp]
        {
            $$ = $[uop];
            $$->child[0] = $[uexp];
            // $$->isConstantExp = true;
            // $[uexp]->isConstantExp = true;
            // $[uexp]->needsInitCheck = true;
            
            // $$->expType = ExpType::Integer;
        }
    | factor
        {
            $$ = $1;
        }
    | unaryop error
        {
            $$ = NULL;
        }
    ;
unaryop
    : '-'
        {
            $1->tokenclass = NEGATIVE;
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    | '*'
        {
            $1->tokenclass = SIZEOF;
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    | '?'
        {
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    | SIZEOF
        {
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    | NEGATIVE
        {
            $$ = newExpNode(ExpKind::OpK, $1, NULL, NULL, NULL);
        }
    ;
factor
    : mutable
        {
            $$ = $1;
            // $$->isUsed = true;
        }
    | immutable
        {
            $$ = $1;
            // $$->isUsed = true;
        }
    ;
mutable
    : ID
        {
            $$ = newExpNode(ExpKind::IdK, $1, NULL, NULL, NULL);
            $$->attr.string = strdup($1->tokenstr);
            $$->isInit = false;
        }
    | ID '[' exp[e] ']'
        {
            TreeNode* tmp = newExpNode(ExpKind::IdK, $1, NULL, NULL, NULL);
            tmp->attr.string = strdup($1->tokenstr);
            tmp->isArray = false;
            // tmp->isArray = true;
            tmp->isIndexed = true;
            // tmp->isConstantExp = true;
            // tmp->isConstantExp = true;
            // $$->isUsed = true;
            tmp->isInit = false;
            $$ = newExpNode(ExpKind::OpK, $2, tmp, $[e], NULL);
            // $[e]->needsInitCheck = false;
            // $[e]->isInit = false;
            // $[e]->needsInitCheck = true;
        }
    ;
immutable
    : '(' exp ')'
        {
            $$ = $2;
            yyerrok;
        }
    | call
        {
            $$ = $1;
        }
    | constant
        {
            $$ = $1;
        }
    | '(' error
        {
            $$ = NULL;
            yyerrok;
        }
    ;
call
    : ID '(' args[arguments] ')'
        {
            // TreeNode* tmp = newExpNode(ExpKind::IdK)
            $$ = newExpNode(ExpKind::CallK, $1, $[arguments], NULL, NULL);
            $$->expType = ExpType::UndefinedType;
            $$->attr.string = strdup($1->tokenstr);
            if($[arguments] != NULL)
            {
                // $[arguments]->needsInitCheck = true;
            }
        }
    | error '('
        {
            $$ = NULL;
            yyerrok;
        }
    ;
args
    : %empty
        {
            $$ = NULL;
        }
    | argList
        {
            $$ = $1;
        }
    ;
argList
    : argList[args] ',' exp[e]
        {
            $$ = addSibling($[args], $[e]);
            yyerrok;
        }
    | exp
        {
            $$ = $1;
        }
    | argList ',' error
        {
            $$ = NULL;
        }
    ;
constant
    : NUMCONST
        {
            $$ = newExpNode(ExpKind::ConstantK, $1, NULL, NULL, NULL);
            $$->attr.value = $1->numValue;
            $$->expType = ExpType::Integer;
            $$->isConstantExp = true;
        }
    | CHARCONST
        {
            $$ = newExpNode(ExpKind::ConstantK, $1, NULL, NULL, NULL);
            $$->attr.cvalue = $1->charValue;
            $$->expType = ExpType::Char;
            // $$->isArray = false;
            $$->isConstantExp = true;
        }
    | STRINGCONST
        {
            $$ = newExpNode(ExpKind::ConstantK, $1, NULL, NULL, NULL);
            $$->attr.string = strdup($1->stringValue);
            $$->expType = ExpType::Char;
            $$->isArray = true;
            $$->isConstantExp = true;
            $$->size = strlen($$->attr.string) - 1;
        }
    | BOOLCONST
        {
            $$ = newExpNode(ExpKind::ConstantK, $1, NULL, NULL, NULL);
            $$->attr.value = $1->boolValue;
            $$->expType = ExpType::Boolean;
            $$->isConstantExp = true;
        }
    ;

%%
//////////////////////////////////
// This is the code from Assign1
//////////////////////////////////
/* statementlist : statementlist statement
              | statement
              ;

statement   
    : ID            { printf("Line %d Token: ID Value: %s\n", $1->linenum, $1->tokenstr); }
    | NUMCONST      { printf("Line %d Token: NUMCONST Value: %d  Input: %s\n", $1->linenum, $1->numValue, $1->tokenstr); }
    | CHARCONST     { printf("Line %d Token: CHARCONST Value: \'%c\'  Input: %s\n", $1->linenum, $1->charValue, $1->tokenstr); }
    | STRINGCONST   { printf("Line %d Token: STRINGCONST Value: %s  Len: %d  Input: ", $1->linenum, $1->stringValue, strlen($1->stringValue) - 2); printf(R"(%s)", $1->tokenstr); printf("\n"); }
    | BOOLCONST     { printf("Line %d Token: BOOLCONST Value: %d  Input: %s\n", $1->linenum, $1->boolValue, $1->tokenstr); }
    | IF            { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | ELSE          { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | THEN          { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | RETURN        { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | EQ            { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | NEQ           { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | GEQ           { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | LEQ           { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | INT           { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | OPERATOR      { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | OR            { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | ADDASS        { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | SUBASS        { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | MULTASS       { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | DIVASS        { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | BOOL          { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | CHAR          { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | STATIC        { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | AND           { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | NOT           { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | FOR           { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | WHILE         { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | BREAK         { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | TO            { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | BY            { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | DO            { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | INC           { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | DEC           { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    | MISC          { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
    ; */
/* WARNING(30): character is 2 characters long and not a single character: ''^I''.  The first char will be used. */
/* keyword     : IF
            | ELSE
            | THEN
            | RETURN           
            | EQ
            | INT
            |
            |
            | */

/* statementlist : statementlist statement
              | statement
              ;

statement     : '\n'
              | expression '\n'         { printf("THE ANSWER IS %lg\n", $1); }
              | QUIT '\n'               { exit(0); }
              ;

expression    : ID '=' expression   { vars[$1->idIndex] = $3; $$ = $3;}
              | sumexp              { $$ = $1; }

sumexp        : sumexp '+' mulexp     { $$ = $1 + $3; }
              | sumexp '-' mulexp     { $$ = $1 - $3; }
              | mulexp                { $$ = $1; }
              ;

mulexp        : mulexp '*' unary   { $$ = $1 * $3; }
              | mulexp '/' unary   { $$ = $1 / $3; } // no check div zero
              | unary              { $$ = $1; }
              ;

unary         : '-' unary          { $$ = - $2; }
              | factor             { $$ = $1; }

factor        : ID                  { $$ = vars[$1->idIndex]; }
              | '(' expression ')'  { $$ = $2; }
              | NUMBER              { $$ = $1->numValue; }
              ; */


/* extern int yydebug; */
///* int bisonMain(int argc, char *argv[])
//{
//    bool printTreeFlag = false;
//    bool printYYDEBUG = false;
//    bool sybtabDEBUG = false;
//    bool printTypeInfo = false;
//    bool printHelp = false;
//
//    int index;
//    char* cvalue = NULL;
//    int c;
//
//    /* if (argc > 1) {
//        if ((yyin = fopen(argv[1], "r"))) {
//            // file open successful
//        }
//        else {
//            // failed to open file
//            printf("ERROR: failed to open \'%s\'\n", argv[1]);
//            exit(1);
//        }
//    } */
//
//    while((c = getopt(argc, argv, "dDpPh")) != -1)
//    {
//        switch(c)
//        {
//            case 'd':
//                yydebug = 1;
//            break;
//            case 'D':
//                sybtabDEBUG = true;
//            break;
//            case 'p':
//                printTreeFlag = true;
//            break;
//            case 'P':
//                printTypeInfo = true;
//            break;
//            case 'h':
//                printHelp = true;
//            default:
//            break; 
//        }
//    }
//    
//    if(printHelp)
//    {
//        printf("usage: c- [options] [sourcefile]\n");
//        printf("options:\n");
//        printf("-d          - turn on parser debugging\n");
//        printf("-D          - turn on symbol table debugging\n");
//        printf("-h          - print this usage message\n");
//        printf("-p          - print the abstract syntax tree\n");
//        printf("-P          - print the abstract syntax tree plus type information\n");
//    }
//    else {
//        for(index = optind; index < argc; index++)
//        {
//            if((yyin = fopen(argv[index], "r")))
//            {
//                break;
//            }
//        }
//        if(!yyin)
//        {
//            printf("Could not open file\n");
//            exit(1); 
//        }
//        // init variables a through z
//        /* for (int i=0; i<26; i++) vars[i] = 0.0; */
//
//        // do the parsing
//        numErrors = 0;
//        numWarnings = 0;
//        
//        yyparse();
//        if(printTreeFlag)
//        {
//            printTree(rootNode);
//        }
//        /* printf("Number of errors: %d\n", numErrors);   // ERR */
//    }
//
//    return 0;
//} */