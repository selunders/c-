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

#endif