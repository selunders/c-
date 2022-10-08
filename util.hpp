#ifndef _UTIL_HPP_
#define _UTIL_HPP_

#include "globals.hpp"
// #include "scanType.h"

TreeNode* newDeclNode(  DeclKind kind,
                        ExpType type,
                        TokenData* token=NULL,
                        TreeNode* c0=NULL,
                        TreeNode* c1=NULL,
                        TreeNode* c2=NULL);


TreeNode* newStmtNode(  StmtKind kind,
                        TokenData* token,
                        TreeNode* c0=NULL,
                        TreeNode* c1=NULL,
                        TreeNode* c2=NULL);

TreeNode* newExpNode(   ExpKind kind,
                        TokenData* token,
                        TreeNode* c0=NULL,
                        TreeNode* c1=NULL,
                        TreeNode* c2=NULL);

TreeNode* addSibling(TreeNode* addToThis, TreeNode* nodeBeingAdded);
void setType(TreeNode* t, ExpType type, bool isStatic, bool needsInitCheck);

char* assignToString(OpKind op);
char* expToString(ExpType type);
char* opToString(OpKind op);
TreeNode *getSTNode(SymbolTable *st, TreeNode *t);
static void printSpaces(int layer);
void printTree(TreeNode* tree, bool printTypeTree);
 

#endif