%{
// this *order* of these includes is mandatory
#include "scanType.h"  // TokenData Type
#include "c-.tab.h"  // token definitions from the bison

int line;
int numErrors;    // ERR err count

static int setValue(int linenum, int tokenClass, char *svalue)
{
    printf("Linenum: %d tokenClass %d svalue %s\n", linenum, tokenClass, svalue);
    // create the pass-back data space
    yylval.tokenData = new TokenData;

    // fill it up
    yylval.tokenData->linenum = linenum;
    yylval.tokenData->tokenstr = strdup(svalue);  // duplicating string!!

    switch(tokenClass)
    {
        case ID:
            yylval.tokenData->idIndex = svalue[0]-'a';
            break;
        case NUMCONST:
            // printf("Found a number\n");
            yylval.tokenData->numValue = atoi(svalue);
            break;
        case CHARCONST:
            break;
        case STRINGCONST:
            break;
        default:
            // printf("Uhhh.... you shouldn't be here??\n");
            break;
    }
    // printf("Should be printing line something")
    printf("Line %d Token: %s\n", yylval.tokenData->linenum, yylval.tokenData->tokenstr);
    // return the tokenclass
    return tokenClass;
}

%}

%option noyywrap

%%
[0-9]+              { return setValue(line, NUMCONST, yytext);}
"[a-zA-z]*"         { return setValue(line, STRINGCONST, yytext); }
[ \t]               ;
\n                  { line++; }
.                   { printf("ERROR(%d): char \'%c\' is not in the language\n", line, yytext[0]); numErrors++; }
%%
//[\=\+\-\*\/\(\)]    { return setValue(line, yytext[0], yytext); }

/* "stop"|"quit"|"exit"   { return setValue(line, QUIT, yytext); }
"pi"                   { return setValue(line, NUMBER, (char *)"3.141592653589793238"); }
[0-9]+|[0-9]*\.[0-9]+  { return setValue(line, NUMBER, yytext); }
[0-9]+\.[0-9]*         { return setValue(line, NUMBER, yytext); }
[a-z]                  { return setValue(line, ID, yytext); }
[\=\+\-\*\/\(\)]       { return setValue(line, yytext[0], yytext ); }
[ \t]                  ;
\/\/.*                 ;
\n                     { line++; return setValue(line, yytext[0], yytext ); }
.                      { printf("ERROR(%d): char \'%c\' is not in the language\n", line, yytext[0]); numErrors++; } */