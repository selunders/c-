%{
// this *order* of these includes is mandatory
#include "scanType.h"  // TokenData Type
#include "c-.tab.h"  // token definitions from the bison
#include <stdio.h>
#include <stdlib.h>
// #include "colorPrint.hh"
    // setPrintColor(PRINTCOLOR::GREEN);
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

void resetLexer()
{
    YY_FLUSH_BUFFER;
    // line = 1;
    // yy_flush_buffer(YY_CURRENT_BUFFER);
    // BEGIN(0);
}

// This removes backslashes from STRINGCONSTS for the C- language.
void handleStrEscapeChars(char* tokString, char** outString)
{
    int i = 0;
    // printf("beep1\n");
    char c;
    size_t totalLen = strlen(tokString);
    size_t currentLen;
    char* tempStr = NULL;
    *outString = NULL;
    while(i < totalLen)
    {
        c = tokString[i];
        if(*outString != NULL)
            currentLen = strlen(*outString);
        else
            currentLen = 0;

	    tempStr = (char *) malloc((currentLen + 1 + 1)*sizeof(char));
        
    
        if(c == '\\')
        {
            i++;
            c = tokString[i];
            if(c == 'n')
                c = '\n';
        }

        if(*outString != NULL)
        {
            strcpy(tempStr, *outString);
        }
        
        tempStr[currentLen] = c;
        tempStr[currentLen+1] = '\0';
        
        if(*outString != NULL)
            free(*outString);
        
        *outString = tempStr;
        tempStr = NULL;
        i++;
    }
}

void handleCharEscapeChars(char* tokString, char* outChar)
{
    int i = 1;
    char c = tokString[i];
    // bool isTooLong = false;
    if(c == '\\')
    {
        i++;
        c = tokString[i];
        // This switch statement handles all the valid escape characters for the C- language
        switch(c)
        {
            case 'n':
                c = '\n';
            break;
            case '0':
                c = '\0';
            break;
            default:
            break;
        } 
        if(strlen(yylval.tokenData->tokenstr) > 4)
        {
            printf("WARNING(%d): character is %d characters long and not a single character: \'%s\'.  The first char will be used.\n", yylval.tokenData->linenum, strlen(yylval.tokenData->tokenstr)-2, yylval.tokenData->tokenstr);
        }
    }
    else if(strlen(yylval.tokenData->tokenstr) > 3)
    {
        printf("WARNING(%d): character is %d characters long and not a single character: \'%s\'.  The first char will be used.\n", yylval.tokenData->linenum, strlen(yylval.tokenData->tokenstr)-2, yylval.tokenData->tokenstr);
    }
    // setPrintColor(PRINTCOLOR::GREEN);
    // printf("The token is: %c\n", c);
    // resetPrintColor();
    *outChar = c;
}

static int setValue(int linenum, int tokenClass, char *svalue)
{
    // printf("Linenum: %d tokenClass %d svalue %s\n", linenum, tokenClass, svalue);
    // create the pass-back data space
    // yylval.tokenData = new TokenData;
    yylval.tokenData = (TokenData*) malloc(sizeof(TokenData));
    // yylval.tokenData = new TokenData;

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
            handleCharEscapeChars(yylval.tokenData->tokenstr, &yylval.tokenData->charValue);
            break;
        case STRINGCONST:
            handleStrEscapeChars(yylval.tokenData->tokenstr, &yylval.tokenData->stringValue);
            // yylval.tokenData->isArray = true;
            break;
        case BOOLCONST:
            if(!strcmp(svalue, "true"))
            {
                yylval.tokenData->boolValue = 1;
            }
            else if(!strcmp(svalue, "false"))
            {
                yylval.tokenData->boolValue = 0;
            }
        default:
            break;
    }
    yylval.tokenData->tokenclass = tokenClass;
    // printf("Line %d Token: %s\n", yylval.tokenData->linenum, yylval.tokenData->tokenstr);
    return tokenClass;
}

%}

%option noyywrap

%%
\n                      { line++; }
"//".*\n                { line++; }
[0-9]+                  { return setValue(line, NUMCONST, yytext);}
"if"                    { return setValue(line, IF, (char*) "IF"); }
"or"                    { return setValue(line, OR, (char*) "OR"); }
"else"                  { return setValue(line, ELSE, (char*) "ELSE"); }
"then"                  { return setValue(line, THEN, (char*) "THEN"); }
"return"                { return setValue(line, RETURN, (char*) "RETURN"); }
"char"                  { return setValue(line, CHAR, (char*) "CHAR"); }
"bool"                  { return setValue(line, BOOL, (char*) "BOOL"); }
"static"                { return setValue(line, STATIC, (char*) "STATIC"); }
"and"                   { return setValue(line, AND, (char*) "AND"); }
"not"                   { return setValue(line, NOT, (char*) "NOT"); }
"for"                   { return setValue(line, FOR, (char*) "FOR"); }
"while"                 { return setValue(line, WHILE, (char*) "WHILE"); }
"break"                 { return setValue(line, BREAK, (char*) "BREAK"); }
"to"                    { return setValue(line, TO, (char*) "TO"); }
"by"                    { return setValue(line, BY, (char*) "BY"); }
"do"                    { return setValue(line, DO, (char*) "DO"); }
'='                     { return setValue(line, yytext[0], yytext); }
"++"                    { return setValue(line, INC, (char*) "INC"); }
"--"                    { return setValue(line, DEC, (char*) "DEC"); }
"+="                    { return setValue(line, ADDASS, (char*) "ADDASS"); }
"-="                    { return setValue(line, SUBASS, (char*) "SUBASS"); }
"*="                    { return setValue(line, MULASS, (char*) "MULASS"); }
"/="                    { return setValue(line, DIVASS, (char*) "DIVASS"); }
"=="                    { return setValue(line, EQ, (char*) "EQ"); }
"!="                    { return setValue(line, NEQ, (char*) "NEQ"); }
">="                    { return setValue(line, GEQ, (char*) "GEQ"); }
"<="                    { return setValue(line, LEQ, (char*) "LEQ"); }
"int"                   { return setValue(line, INT, (char*) "INT"); }
"true"                  { return setValue(line, BOOLCONST, yytext); }
"false"                 { return setValue(line, BOOLCONST, yytext); }
"\'\\'\'"               { return setValue(line, CHARCONST, yytext); }
\'[^\'\n]*\'            { return setValue(line, CHARCONST, yytext); }
\"(\\.|[^\"\n])*\"      { return setValue(line, STRINGCONST, yytext); }
[a-zA-Z][a-zA-Z0-9]*    { return setValue(line, ID, yytext);}
[ \t]                   ;
[\%]                    { return setValue(line, MODULO, yytext); }
[\=\+\-\*\/\(\)]        { return setValue(line, yytext[0], yytext); }
[\'\"\.\@\#\$\^\&\_\|\\\~\!]                { printf("ERROR(%d): Invalid or misplaced input character: \'%s\'. Character Ignored.\n", line, yytext); }
[\%\*\(\)\-\{\}\[\]\;\:\<\>\?\,\/]          { return setValue(line, yytext[0], yytext); }
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
