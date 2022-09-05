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

%token <tokenData> ID NUMCONST CHARCONST STRINGCONST
%token <tokenData> IF ELSE THEN RETURN EQ INT
%token <tokenData> OPERATOR
%token <tokenData> MISC
/* %type  <value> expression sumexp mulexp unary factor */

%%
statementlist : statementlist statement
              | statement
              ;

statement   : ID            { printf("Line %d Token: ID Value: %s\n", $1->linenum, $1->tokenstr); }
            | NUMCONST      { printf("Line %d Token: NUMCONST Value: %d  Input: %s\n", $1->linenum, $1->numValue, $1->tokenstr); }
            | CHARCONST     { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
            | STRINGCONST   { printf("Line %d Token: STRINGCONST Value: %s Len: %d Input: ", $1->linenum, $1->stringValue, strlen($1->tokenstr) - 2); printf(R"(%s)", $1->tokenstr); printf("\n"); }
            | IF            { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
            | ELSE          { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
            | THEN          { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
            | RETURN        { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
            | EQ            { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
            | INT           { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
            | OPERATOR      { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
            | MISC          { printf("Line %d Token: %s\n", $1->linenum, $1->tokenstr); }
            ;

%%
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
