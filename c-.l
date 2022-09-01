%{
// this *order* of these includes is mandatory
#include "scanType.h"  // TokenData Type
#include "c-.tab.h"  // token definitions from the bison

int line;
int numErrors;    // ERR err count

static int setValue(int linenum, int tokenClass, char *svalue)
{
    // create the pass-back data space
    // yylval.tokenData = new TokenData;
    // fill it up
    // yylval.tokenData->linenum = linenum;
    // yylval.tokenData->tokenstr = strdup(svalue);  // duplicating string!!
  
    // if (tokenClass == NUMBER) {
    //     yylval.tokenData->numValue = atof(svalue); 
    // }
    // else if (tokenClass == ID) {
    //     yylval.tokenData->idIndex = svalue[0]-'a';
    // }

    switch(tokenClass)
    {
        case BASE:
            break;

        case ID:
            yylval.tokenData = new ID_Token(linenum, svalue);
            break;

        case NUMCONST:
            yylval.tokenData = new NUMCONST_Token(linenum, svalue);
            break;

        case CHARCONST:
            break;

        case STRINGCONST:
            break;

        default:
            cerr << "UNEXPECTED TOKEN FOUND" << endl;
            break;
    }

    // return the TOKENCLASSES
    return tokenClass;
}

%}

%option noyywrap

%%
[a-z][a-z0-9A-Z]*   { return setValue(line, ID, yytext); }
.                   { return setValue(line, BASE, yytext); }
[0-9]+              { return setValue(line, NUMCONST, yytext); }
.                   { return setValue(line, CHARCONST, yytext); }
.                   { return setValue(line, STRINGCONST, yytext); }
.                      { printf("ERROR(%d): char \'%c\' is not in the language\n", line, yytext[0]); numErrors++; }
%%

/*
"stop"|"quit"|"exit"   { return setValue(line, QUIT, yytext); }
"pi"                   { return setValue(line, NUMBER, (char *)"3.141592653589793238"); }
[0-9]+|[0-9]*\.[0-9]+  { return setValue(line, NUMBER, yytext); }
[0-9]+\.[0-9]*         { return setValue(line, NUMBER, yytext); }
[a-z]                  { return setValue(line, ID, yytext); }
[\=\+\-\*\/\(\)]       { return setValue(line, yytext[0], yytext ); }
[ \t]                  ;
\/\/.*                 ;
\n                     { line++; return setValue(line, yytext[0], yytext ); }

*/