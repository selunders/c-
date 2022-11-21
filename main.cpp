#include <stdio.h>
#include "c-.tab.h"
#include <unistd.h>
#include "util.hpp"
#include "symbolTable.hpp"
#include "analyze.hpp"
#include "yyerror.h"

extern FILE *yyin;
extern FILE *yyout;
extern TreeNode *rootNode;
extern int yydebug;

extern void resetParse();
// extern void yyrestart();
extern void yyrestart(FILE *);
extern int yylex_destroy(void);
extern int numErrors;
extern int numWarnings;
extern void initErrorProcessing();

extern int goffset;
extern int finalOffset;

FILE *fileIn;
TreeNode *fileInRoot;

FILE *tmpFile;
TreeNode *tmpRoot;

TreeNode *parseFile(FILE *file);
TreeNode *createIOAST();

PrintMethod printOption = PrintMethod::Basic;

int main(int argc, char *argv[])
{
    bool printTreeFlag = false;
    bool printYYDEBUG = false;
    bool symbTabDEBUG = false;
    bool sizeLocationFlag = false;
    bool printTypeInfo = false;
    bool printHelp = false;

    int index;
    char *cvalue = NULL;
    int c;

    while ((c = getopt(argc, argv, "dDMpPh")) != -1)
    {
        switch (c)
        {
        case 'd':
            yydebug = 1;
            break;
        case 'D':
            symbTabDEBUG = true;
            break;
        case 'M':
            // sizeLocationFlag = true;
            printOption = PrintMethod::Location;
            break;
        case 'p':
            printOption = PrintMethod::Basic;
            // printTreeFlag = true;
            break;
        case 'P':
            printOption = PrintMethod::Typed;
            // printTypeInfo = true;
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
        printf("-M          - print the abstract syntax tree plus size & location information\n");
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

        // Create symbol table
        SymbolTable *symbolTable = new SymbolTable();
        initErrorProcessing();

        ///////////////
        // Read in the helper functions file(s)
        ////////
        // NOTE
        // This section of code works on the CS server, but doesn't seem to read from the
        //   io.c- file when ran on the submission site. I'm keeping it in here commented
        //   in case it comes in handy later, but for now will just manually add the IO tree to the SymbolTable.
        /*
            tmpFile = fopen("io.c-", "r");
            tmpRoot = parseFile(tmpFile);
            resetParse();
            fclose(tmpFile);
            */
        /*
        ASTtoSymbolTable(symbolTable, tmpRoot);
    */
        //
        ////////////////
        TreeNode *IORoot = createIOAST();
        ASTtoSymbolTable(symbolTable, IORoot);

        // printf("FILE: %s\n", argv[index] + 15);
        // printf("FILE: %s\n", argv[index]);
        // printf("====================================\n");
        tmpRoot = parseFile(fileIn);

        // Print initial
        // if (printTreeFlag && !printTypeInfo && !sizeLocationFlag)
        // {
        // printf("Not printing type info\n");
        // printTree(tmpRoot, false);
        // }

        if (printOption == PrintMethod::Basic)
        {
            printTree(tmpRoot, printOption);
        }

        // symbolTable->test();
        symbolTable->debug(symbTabDEBUG);
        if (numErrors == 0)
        {
            semanticAnalysis(symbolTable, tmpRoot, printOption);
            if (numErrors == 0)
                printf("Offset for end of global space: %d\n", finalOffset);
        }
        printf("Number of warnings: %d\n", numWarnings);
        printf("Number of errors: %d\n", numErrors);
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
    TreeNode *tmp;
    // yyin = file;
    resetParse();
    yyin = file;
    yyparse();
    tmp = rootNode;
    return tmp;
}

TreeNode *createASTNode(DeclKind declType, ExpType returnType, ExpType paramType, char *idName)
{
    TreeNode *paramTmp = NULL;
    if (paramType != ExpType::UndefinedType)
    {
        paramTmp = newDeclNode(DeclKind::ParamK, paramType, NULL, NULL, NULL, NULL);
        paramTmp->attr.string = (char *)"placeholder";
    }

    TreeNode *tmp = newDeclNode(DeclKind::FuncK, returnType, NULL, paramTmp, NULL, NULL);
    tmp->attr.string = strdup(idName);
    tmp->isUsed = true;
    tmp->isDeclared = true;
    tmp->isInit = true;
    tmp->isConstantExp = true;
    return tmp;
}

TreeNode *createIOAST()
{
    TreeNode *root;
    TreeNode *tmp;
    root = tmp = createASTNode(DeclKind::FuncK, ExpType::Integer, ExpType::UndefinedType, (char *)"input");
    tmp->sibling = createASTNode(DeclKind::FuncK, ExpType::Boolean, ExpType::UndefinedType, (char *)"inputb");
    tmp = tmp->sibling;
    tmp->sibling = createASTNode(DeclKind::FuncK, ExpType::Char, ExpType::UndefinedType, (char *)"inputc");
    tmp = tmp->sibling;
    tmp->sibling = createASTNode(DeclKind::FuncK, ExpType::Void, ExpType::Integer, (char *)"output");
    tmp = tmp->sibling;
    tmp->sibling = createASTNode(DeclKind::FuncK, ExpType::Void, ExpType::Boolean, (char *)"outputb");
    tmp = tmp->sibling;
    tmp->sibling = createASTNode(DeclKind::FuncK, ExpType::Void, ExpType::Char, (char *)"outputc");
    tmp = tmp->sibling;
    tmp->sibling = createASTNode(DeclKind::FuncK, ExpType::Void, ExpType::UndefinedType, (char *)"outnl");
    tmp = tmp->sibling;
    return root;
}