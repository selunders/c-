%{
// // // // // // // // // // // // // // // // // // // // // // // 
// CS445 - Calculator Example Program written in the style of the C-
// compiler for the class.
//
// Robert Heckendorn
// Jan 21, 2021    

// https://www.theurbanpenguin.com/4184-2/

#include "scanType.h"  // TokenData Type
#include <stdio.h>
#include <string.h>

// double vars[26];    

extern int yylex();
extern FILE *yyin;
extern int line;         // ERR line number from the scanner!!
extern int numErrors;    // ERR err count

#define YYERROR_VERBOSE
void yyerror(const char *msg)
{
    printf("ERROR(%d): %s\n", line, msg);
    numErrors++;
}

void printCharByChar(char* stringToPrint)
{
    int i = 0;
    while(i < strlen(stringToPrint))
    {
        printf("%c", stringToPrint[i]);
        i++;
    }
}

%}

// this is included in the tab.h file
// so scanType.h must be included before the tab.h file!!!!
%union {
    TokenData *tokenData;
    double value;
}

%token <tokenData> ID NUMCONST CHARCONST STRINGCONST BOOLCONST
%token <tokenData> IF ELSE THEN RETURN EQ NEQ GEQ LEQ INT OR
%token <tokenData> ADDASS SUBASS MULASS DIVASS
%token <tokenData> STATIC BOOL CHAR
%token <tokenData> AND NOT FOR WHILE BREAK TO BY DO INC DEC
/* %type  <value> expression sumexp mulexp unary factor */

%%
program
    : decList
    ;
decList             
    : decList decl
    | decl
    ;
decl
    : varDecl
    | funDecl
    ;
varDecl
    : typeSpec varDeclList ';'
    ;
scopedVarDecl
    : STATIC typeSpec varDeclList ';'
    | typeSpec varDeclList ';'
    ;
varDeclList
    : varDeclList ',' varDeclInit
    | varDeclInit
    ;
varDeclInit
    : varDeclId
    | varDeclId ':' simpleExp
    ;
varDeclId
    : ID
    | ID '['NUMCONST']'
    ;
typeSpec
    : BOOL                                      { /*printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr)*/; }
    | CHAR                                      { /*printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr)*/; }
    | INT                                       { /*printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr)*/; }
    ;
funDecl
    : typeSpec ID '(' parms ')' compoundStmt
    | ID '(' parms ')' compoundStmt
    ;
parms
    : %empty
    | parmList
    ;
parmList
    : parmList ';' parmTypeList
    | parmTypeList
    ;
parmTypeList
    : typeSpec parmIdList
    ;
parmIdList
    : parmIdList ',' parmId
    | parmId
    ;
parmId
    : ID
    | ID '['']'
stmt
    : selectSuperStmt
    ;
otherStmt
    : expStmt
    | compoundStmt
    | closed_iterStmt
    | returnStmt
    | breakStmt
    ;
expStmt
    : exp ';'
    | ';'
    ;
compoundStmt
    : '{' localDecls stmtList '}'
    ;
localDecls
    : %empty
    | localDecls scopedVarDecl
    ;
stmtList
    : %empty
    | stmtList stmt
    ;
selectSuperStmt
    : open_stmt
    | closed_stmt
    ;
open_stmt
    : IF simpleExp THEN selectSuperStmt
    | IF simpleExp THEN closed_stmt ELSE open_stmt
    | open_iterStmt
    ;
closed_stmt
    : otherStmt
    | IF simpleExp THEN closed_stmt ELSE closed_stmt 
    ;
open_iterStmt
    : WHILE simpleExp DO open_stmt
    | FOR ID '=' iterRange DO open_stmt
    ;
closed_iterStmt
    : WHILE simpleExp DO closed_stmt
    | FOR ID '=' iterRange DO closed_stmt
    ;
iterRange
    : simpleExp TO simpleExp
    | simpleExp TO simpleExp BY simpleExp
returnStmt
    : RETURN ';'
    | RETURN exp ';'
    ;
breakStmt
    : BREAK ';'
    ;
exp
    : mutable assignop exp
    | mutable INC
    | mutable DEC
    | simpleExp
    ;
assignop
    : '='
    | ADDASS
    | SUBASS
    | MULASS
    | DIVASS
    ;
simpleExp
    : simpleExp OR andExp
    | andExp
    ;
andExp
    : andExp AND unaryRelExp
    | unaryRelExp
    ;
unaryRelExp
    : NOT unaryRelExp
    | relExp
    ;
relExp
    : sumExp relop sumExp
    | sumExp
    ;
relop
    : '<'
    | LEQ
    | '>'
    | GEQ
    | EQ
    | NEQ
    ;
sumExp
    : sumExp sumop mulExp
    | mulExp
    ;
sumop
    : '+'
    | '-'
    ;
mulExp
    : mulExp mulop unaryExp
    | unaryExp
    ;
mulop
    : '*'
    | '/'
    | '%'
    ;
unaryExp
    : unaryop unaryExp
    | factor
    ;
unaryop
    : '-'
    | '*'
    | '?'
    ;
factor
    : mutable
    | immutable
    ;
mutable
    : ID
    | ID '[' exp ']'
    ;
immutable
    : '(' exp ')'
    | call
    | constant
    ;
call
    : ID '(' args ')'
    ;
args
    : %empty
    | argList
    ;
argList
    : argList ',' exp
    | exp
    ;
constant
    : NUMCONST
    | CHARCONST
    | STRINGCONST
    | BOOLCONST
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


extern int yydebug;
int main(int argc, char *argv[])
{
    if (argc > 1) {
        if ((yyin = fopen(argv[1], "r"))) {
            // file open successful
        }
        else {
            // failed to open file
            printf("ERROR: failed to open \'%s\'\n", argv[1]);
            exit(1);
        }
    }

    // init variables a through z
    /* for (int i=0; i<26; i++) vars[i] = 0.0; */

    // do the parsing
    numErrors = 0;
    yyparse();

    /* printf("Number of errors: %d\n", numErrors);   // ERR */

    return 0;
}
