#ifndef _ANALYZE_HPP_
#define _ANALYZE_HPP_

#include "globals.hpp"

void semanticAnalysis(SymbolTable*, TreeNode*, bool);

void buildSymTab(TreeNode * );

void typeCheck(TreeNode * );

ExpType getType(SymbolTable*, TreeNode*);


#endif