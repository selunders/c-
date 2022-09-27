#include<stdio.h>
#include "c-.tab.h"
#include <unistd.h>
#include "util.hpp"

extern FILE* yyin;
extern FILE* yyout;
extern TreeNode* rootNode;
extern int yydebug;

int main(int argc, char *argv[])
{
    bool printTreeFlag = false;
    bool printYYDEBUG = false;
    bool sybtabDEBUG = false;
    bool printTypeInfo = false;
    bool printHelp = false;

    int index;
    char* cvalue = NULL;
    int c;

    /* if (argc > 1) {
        if ((yyin = fopen(argv[1], "r"))) {
            // file open successful
        }
        else {
            // failed to open file
            printf("ERROR: failed to open \'%s\'\n", argv[1]);
            exit(1);
        }
    } */

    while((c = getopt(argc, argv, "dDpPh")) != -1)
    {
        switch(c)
        {
            case 'd':
                yydebug = 1;
            break;
            case 'D':
                sybtabDEBUG = true;
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
            exit(1); 
        }
        // init variables a through z
        /* for (int i=0; i<26; i++) vars[i] = 0.0; */

        // do the parsing
        int numErrors = 0;
        int numWarnings = 0;
        
        yyparse();
        if(printTreeFlag)
        {
            printTree(rootNode);
        }
        /* printf("Number of errors: %d\n", numErrors);   // ERR */
    }

    return 0;
}
