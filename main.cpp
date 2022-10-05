#include<stdio.h>
#include "c-.tab.h"
#include <unistd.h>
#include "util.hpp"
#include "symbolTable.hpp"
#include "analyze.hpp"

extern FILE* yyin;
extern FILE* yyout;
extern TreeNode* rootNode;
extern int yydebug;

int main(int argc, char *argv[])
{
    bool printTreeFlag = false;
    bool printYYDEBUG = false;
    bool symbTabDEBUG = false;
    bool printTypeInfo = false;
    bool printHelp = false;

    int index;
    char* cvalue = NULL;
    int c;

    while((c = getopt(argc, argv, "dDpPh")) != -1)
    {
        switch(c)
        {
            case 'd':
                yydebug = 1;
            break;
            case 'D':
                symbTabDEBUG = true;
            break;
            case 'p':
                printTreeFlag = true;
            break;
            case 'P':
                printTypeInfo = true;
            break;
            case 'h':
                printHelp = true;
            default:
            break; 
        }
    }
    
    // If printHelp flag is selected, ONLY print the help statement
    if(printHelp)
    {
        printf("usage: c- [options] [sourcefile]\n");
        printf("options:\n");
        printf("-d          - turn on parser debugging\n");
        printf("-D          - turn on symbol table debugging\n");
        printf("-h          - print this usage message\n");
        printf("-p          - print the abstract syntax tree\n");
        printf("-P          - print the abstract syntax tree plus type information\n");
    }
    else {
        for(index = optind; index < argc; index++)
        {
            if((yyin = fopen(argv[index], "r")))
            {
                break;
            }
        }
        if(!yyin)
        {
            printf("Could not open file\n");
            // printf("Command: ");
            // int i;
            // for(i = 0; i < argc; i++)
            // {
            //     printf(" %s", argv[i]);
            // }
            // printf("\n");
            exit(1); 
        }
        // init variables a through z
        /* for (int i=0; i<26; i++) vars[i] = 0.0; */

        // do the parsing
        yyparse();
        if(printTreeFlag)
        {
            // printf("Not printing type info\n");
            printTree(rootNode, false);
        }
        else if(printTypeInfo)
        {
            // printf("Printing type info\n");
            printTree(rootNode, true);
        }
        

        SymbolTable* symbolTable = new SymbolTable();
        symbolTable->debug(symbTabDEBUG);
        semanticAnalysis(symbolTable, rootNode);
            // printTree(rootNode, true);
        /* printf("Number of errors: %d\n", numErrors);   // ERR */
    }

    return 0;
}