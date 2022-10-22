#include <stdio.h>
#include "c-.tab.h"
#include <unistd.h>
#include "util.hpp"
#include "symbolTable.hpp"
#include "analyze.hpp"

extern FILE *yyin;
extern FILE *yyout;
extern TreeNode *rootNode;
extern int yydebug;

extern void resetParse();
// extern void yyrestart();
extern void yyrestart(FILE *);
extern int yylex_destroy(void);

FILE *fileIn;
TreeNode *fileInRoot;

FILE *tmpFile;
TreeNode *tmpRoot;

TreeNode *parseFile(FILE *file);

int main(int argc, char *argv[])
{
    bool printTreeFlag = false;
    bool printYYDEBUG = false;
    bool symbTabDEBUG = false;
    bool printTypeInfo = false;
    bool printHelp = false;

    int index;
    char *cvalue = NULL;
    int c;

    while ((c = getopt(argc, argv, "dDpPh")) != -1)
    {
        switch (c)
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
    if (printHelp)
    {
        printf("usage: c- [options] [sourcefile]\n");
        printf("options:\n");
        printf("-d          - turn on parser debugging\n");
        printf("-D          - turn on symbol table debugging\n");
        printf("-h          - print this usage message\n");
        printf("-p          - print the abstract syntax tree\n");
        printf("-P          - print the abstract syntax tree plus type information\n");
    }
    else
    {
        for (index = optind; index < argc; index++)
        {
            if ((fileIn = fopen(argv[index], "r")))
            {
                break;
            }
        }
        if (!fileIn)
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

        // tmpFile = fopen("sample/assign4/small.c-", "r");
        tmpFile = fopen("io.c-", "r");
        // printf("%s io.c-\n", tmpFile ? "Found" : "Couldn't find");
        tmpRoot = parseFile(tmpFile);
        resetParse();
        fclose(tmpFile);
        SymbolTable *symbolTable = new SymbolTable();
        // printTree(tmpRoot, false);
        ASTtoSymbolTable(symbolTable, tmpRoot);
        tmpRoot = parseFile(fileIn);
        if (printTreeFlag && !printTypeInfo)
        {
            // printf("Not printing type info\n");
            printTree(rootNode, false);
        }

        // symbolTable->test();
        symbolTable->debug(symbTabDEBUG);

        semanticAnalysis(symbolTable, rootNode, printTypeInfo);
        // symbolTable->print(pointerPrintNode);
        // symbolTable->print(pointerPrintStr);
        // symbolTable->applyToAll();

        // if(printTypeInfo)
        // {
        //     // printf("Printing type info\n");
        //     printTree(rootNode, true);
        // }

        // printTree(rootNode, true);
        /* printf("Number of errors: %d\n", numErrors);   // ERR */
    }

    return 0;
}

TreeNode *parseFile(FILE *file)
{
    TreeNode* tmp;
    // yyin = file;
    resetParse();
    yyin = file;
    yyparse();
    tmp = rootNode;
    return tmp;
}