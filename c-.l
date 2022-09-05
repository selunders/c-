%{
// this *order* of these includes is mandatory
#include "scanType.h"  // TokenData Type
#include "c-.tab.h"  // token definitions from the bison
#include <stdio.h>
#include <stdlib.h>
#include "colorPrint.hh"
    // setPrintColor(PRINTCOLOR::GREEN);
    // setPrintColor(PRINTCOLOR::GREEN);
    // resetPrintColor();
    // resetPrintColor();

int line = 1;
int numErrors;    // ERR err count

void myFree(void * freeThis)
{
    if(freeThis != NULL)
    {
        free(freeThis);
    }
}

// This removes backslashes from STRINGCONSTS for the C- language.
void handleEscapeChars(char* tokString, char** outString)
{
    int i = 0;
    // printf("beep1\n");
    char c;
    size_t totalLen = strlen(tokString);
    size_t currentLen;
    char* tempStr = NULL;
    // char* oldStr;
    while(i < totalLen)
    {
        if(*outString != NULL)
            currentLen = strlen(*outString);
        else
            currentLen = 0;

	    tempStr = (char *) malloc((currentLen + 1 + 1)*sizeof(char));
        
        c = tokString[i];
        
        if(c == '\\')
        {
            i++;
            c = tokString[i];
        }

        if(*outString != NULL)
        {
            strcpy(tempStr, *outString);
        }
        
        tempStr[currentLen] = c;
        tempStr[currentLen+1] = '\0';
        
        free(*outString);
        
        *outString = tempStr;
        tempStr = NULL;
        i++;
    }
}

static int setValue(int linenum, int tokenClass, char *svalue)
{
    // printf("Linenum: %d tokenClass %d svalue %s\n", linenum, tokenClass, svalue);
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
            // strncpy(yylval.tokenData->charValue, yylval.tokenData->tokenstr+1, strlen(yylval.tokenData->tokenstr) - 2);
            if(strlen(yylval.tokenData->tokenstr) > 3)
            {
                printf("%s %d\n", yylval.tokenData->tokenstr, strlen(yylval.tokenData->tokenstr));
                // setPrintColor(PRINTCOLOR::GREEN);
                // printf("\n\n\nSomething's wrong heere. strlen of char is %d\n\n\n", strlen(yylval.tokenData->charValue));
                // resetPrintColor();
            }
            break;
        case STRINGCONST:
            handleEscapeChars(yylval.tokenData->tokenstr, &yylval.tokenData->stringValue);
            break;
        default:
            break;
    }
    // printf("Should be printing line something")
    // printf("Line %d Token: %s\n", yylval.tokenData->linenum, yylval.tokenData->tokenstr);
    // return the tokenclass
    return tokenClass;
}

%}

%option noyywrap

%%
\n                      { line++; }
"//".*                  { line++; }
[0-9]+                  { return setValue(line, NUMCONST, yytext);}
"if"                    { return setValue(line, IF, (char*) "IF"); }
"else"                  { return setValue(line, ELSE, (char*) "ELSE"); }
"then"                  { return setValue(line, THEN, (char*) "THEN"); }
"return"                { return setValue(line, RETURN, (char*) "RETURN"); }
"=="                    { return setValue(line, EQ, (char*) "EQ"); }
"int"                   { return setValue(line, INT, (char*) "INT"); }
\'[^\'^\n]*\'           { return setValue(line, CHARCONST, yytext); }
\"(\\.|[^\"])*\"        { return setValue(line, STRINGCONST, yytext); }
[a-zA-Z][a-zA-Z0-9]*    { return setValue(line, ID, yytext);}
[ \t]                   ;
[\=\+\-\*\/\(\)]        { return setValue(line, OPERATOR, yytext); }
[\,\;\{\}\'\\]              { return setValue(line, MISC, yytext); }
.                       { printf("ERROR(%d): char \'%c\' is not in the language\n", line, yytext[0]); numErrors++; }
%%
/* \"[^\"^\n]*\"           { return setValue(line, STRINGCONST, yytext); } */
/* "[a-zA-z]*"             { return setValue(line, STRINGCONST, yytext); } */

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
