#ifndef _ANALYZE_HPP_
#define _ANALYZE_HPP_

#include "globals.hpp"

void semanticAnalysis(SymbolTable*, TreeNode*);

void buildSymTab(TreeNode * );

void typeCheck(TreeNode * );

#endif