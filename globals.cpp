// #ifndef _GLOBALS_H_
// #define _GLOBALS_H_

// #include "symbolTable.hpp"
#include "globals.hpp"

// #ifndef FALSE
// #define FALSE 0
// #endif

// #ifndef TRUE
// #define TRUE 1
// #endif

char *refTypeToStr(RefType refType)
{
    switch (refType)
    {
    case RefType::None:
        return (char *)"None";
        break;
    case RefType::Local:
        return (char *)"Local";
        break;
    case RefType::LocalStatic:
        return (char *)"LocalStatic";
        break;
    case RefType::Global:
        return (char *)"Global";
        break;
    case RefType::Static:
        return (char *)"Static";
        break;
    case RefType::Parameter:
        return (char *)"Parameter";
        break;
    default:
        return (char *)"ERROR";
    break;
    }
}

// enum class OpType
// {
//     Bool,
//     Array,
//     Int,
//     LHS,
//     Char,
//     IntOrChar,
//     Any
// };

TreeNode::TreeNode()
{
    sibling = NULL;
    int i;
    for (i = 0; i < MAXCHILDREN; i++)
    {
        child[i] = NULL;
    }
    nodeKind = NodeKind::StmtK;
    isArray = false;
    isStatic = false;
    isInit = true;
    needsInitCheck = false;
    seenInit = false;
    isUsed = false;
    isDefined = false;
    expType = ExpType::UndefinedType;
    alreadyTraversed = false;
    isDeclared = false;
    declChecked = false;
    finalCheckDone = false;
    isIndexed = false;
    canEnterThisScope = true;
    isForVar = false;
    isConstantExp = false;
    declDepth = -1;
    // -1 means 'size has been checked'
    size = 1;
    location = 0;
    referenceType = RefType::None;
}

// #ifndef UNINDEXED_ARRAY
// #define UNINDEXED_ARRAY
bool isUnindexedArray(TreeNode *t)
{
    if (t->nodeKind == NodeKind::ExpK && t->subkind.exp == ExpKind::AssignK && t->attr.op == '=')
    {
        return isUnindexedArray(t->child[0]);
    }
    if (t->isArray)
    {
        if (t->child[0] != NULL)
            t->isIndexed = true;
        if (t->isIndexed)
            return false;
        else
            return true;
    }
    else
        return false;
}
// #endif

bool OpTypeInfo::passesEqualCheck(SymbolTable *st, TreeNode *t)
{
    TreeNode *LEFT = t->child[0];
    TreeNode *RIGHT = t->child[1];
    if (t->attr.op == '=')
    {
        TreeNode *tmp = NULL;
        if (LEFT != NULL && LEFT->nodeKind == NodeKind::ExpK && LEFT->subkind.exp == ExpKind::IdK)
        {
            tmp = (TreeNode *)st->lookup(LEFT->attr.string);
            if (tmp != NULL && tmp->subkind.decl == DeclKind::FuncK)
                return true;
        }
        if (RIGHT != NULL && RIGHT->nodeKind == NodeKind::ExpK && RIGHT->subkind.exp == ExpKind::IdK)
        {
            tmp = (TreeNode *)st->lookup(RIGHT->attr.string);
            if (tmp != NULL && tmp->subkind.decl == DeclKind::FuncK)
                return true;
        }

        if (LEFT->expType == ExpType::Void)
            return true;
    }
    if (LEFT->expType == ExpType::UndefinedType || t->child[1]->expType == ExpType::UndefinedType)
        // if (LEFT->expType == ExpType::UndefinedType || t->child[1]->expType == ExpType::UndefinedType || LEFT->expType == ExpType::Void || t->child[1]->expType == ExpType::Void)
        return true;
    if (equalTypes)
        return (LEFT->expType == t->child[1]->expType);
    else
        return true;
    // return (LEFT->expType != t->child[1]->expType);
}

bool OpTypeInfo::passesLeftCheck(TreeNode *t)
{
    ExpType LEFT = t->child[0]->expType;
    if (lhs == ExpType::UndefinedType || LEFT == ExpType::UndefinedType)
    // if (lhs == ExpType::UndefinedType || LEFT == ExpType::UndefinedType || lhs == ExpType::Void || LEFT == ExpType::Void)
    // if (lhs == ExpType::UndefinedType || LEFT == ExpType::UndefinedType)
    {
        return true;
    }
    else if (lhs == ExpType::CharInt)
    {
        if (LEFT == ExpType::Integer || LEFT == ExpType::Char)
        {
            return true;
        }
    }
    else if (lhs == ExpType::Array)
    {
        if (t->child[0]->isArray && !t->child[0]->isIndexed)
            return true;
    }
    else if (lhs == LEFT)
        return true;
    else
        return false;
    // printf("Couldn't match sides: Exp %d Got %d\n", lhs, LEFT);
}

bool OpTypeInfo::passesRightCheck(TreeNode *t)
{
    // pointerPrintNode(t);
    ExpType RIGHT = t->child[1]->expType;
    if (rhs == ExpType::UndefinedType || RIGHT == ExpType::UndefinedType || rhs == ExpType::Void || RIGHT == ExpType::Void)
    // if (rhs == ExpType::UndefinedType || RIGHT == ExpType::UndefinedType)
    {
        return true;
    }
    else if (rhs == ExpType::CharInt)
    {
        if (RIGHT == ExpType::Integer || RIGHT == ExpType::Char)
        {
            return true;
        }
    }
    else if (rhs == ExpType::Array)
    {
        if (t->child[1]->isArray && !t->child[1]->isIndexed)
            return true;
    }
    else if (rhs == RIGHT)
        return true;
    else
        return false;
}

bool OpTypeInfo::isArrayAndWorks(TreeNode *t)
{
    bool leftIsArray = (t->child[0]->isArray && !t->child[0]->isIndexed);
    bool rightIsArray;

    if (isUnary)
    {
        // printf("left is arr: %d works with arrs: %d\n", leftIsArray, worksWithArrays);
        if ((leftIsArray && worksWithArrays) || !leftIsArray)
            return true;
        else
            return false;
    }
    else // is binary
    {
        rightIsArray = (t->child[1]->isArray && !t->child[1]->isIndexed);
        // printf("Left is array: '%d' Right is array: '%d'\n", leftIsArray, rightIsArray);
        // printf("Left is type: '%d' Right is type: '%d'\n", t->child[0]->subkind.stmt, t->child[1]->subkind.stmt);
        if (((leftIsArray || rightIsArray) && worksWithArrays) || (!leftIsArray && !rightIsArray))
            return true;
        // else if (onlyWorksWithArrays)
        //     return false;
        else
            return false;
    }
    // printf("Left is array: %d, Right is array: %d\n", leftIsArray, rightIsArray);
}

bool OpTypeInfo::onlyArrayAndWorks(TreeNode *t)
{
    // Only used for unary sizeof op I believe
    // printf("%d %s array\n", t->child[0]->attr.op, t->child[0]->isArray ? (char*) "is" : (char*) "is not");
    if (onlyWorksWithArrays)
    {
        // if (t->child[0]->isArray)
        if (t->child[0]->isArray && !t->child[0]->isIndexed || t->child[0]->expType == ExpType::UndefinedType)
            return true;
        else
            return false;
    }
    else
        return true;
}

bool OpTypeInfo::arrayCheck(TreeNode *t)
{
    bool leftIsArr = (t->child[0]->isArray && !t->child[0]->isIndexed);
    bool rightIsArr = (t->child[1]->isArray && !t->child[1]->isIndexed);
    bool passesCheck = true;
    if (bothArrays)
        if (!leftIsArr || !rightIsArr)
            passesCheck = false;
    if (equalTypes)
        if (leftIsArr != rightIsArr)
            passesCheck = false;
    if (leftArray)
        if (!leftIsArr || rightIsArr)
            passesCheck = false;
    return passesCheck;
}

bool OpTypeInfo::arrayIndexedCorrectly(TreeNode *t)
{
    if (leftArray)
        if (t->child[0]->isArray && t->child[1] != NULL)
            if (t->child[1]->isArray)
                return t->child[1]->isIndexed;
            else
                return true;
    return (t->child[1]->isArray || t->child[1]->isIndexed);
}

bool OpTypeInfo::returnTypeCheck(TreeNode *t)
{
    ExpType retType = t->expType;
    bool isArray = t->isArray;
    if (isArray)
        return false;
    if (returnType == ExpType::UndefinedType)
    {
        return true;
    }
    else if (returnType == ExpType::CharInt)
    {
        if (retType == ExpType::Integer || retType == ExpType::Char)
        {
            return true;
        }
    }
    else if (returnType == retType)
        return true;
    return false;
}

bool OpTypeInfo::bothArrsOrNot(SymbolTable *st, TreeNode *t)
{
    TreeNode *tl = t->child[0];
    TreeNode *tr = t->child[1];
    bool leftIsArray = isUnindexedArray(tl);
    bool rightIsArray = isUnindexedArray(tr);
    if (leftIsArray == rightIsArray)
        return true;
    else
        return false;
}

OpTypeInfo::OpTypeInfo(ExpType LHS, ExpType RHS, ExpType ReturnType, bool SameTypes, bool BothAreArrays, bool LeftIsArray, bool WorksWithArrays, bool IsConstantExpression)
{
    lhs = LHS;
    rhs = RHS;
    returnType = ReturnType;
    equalTypes = SameTypes;
    bothArrays = BothAreArrays;
    leftArray = LeftIsArray;
    isUnary = false;
    worksWithArrays = WorksWithArrays;
    onlyWorksWithArrays = false;
    isConstantExpression = IsConstantExpression;
}
OpTypeInfo::OpTypeInfo(ExpType LHS, ExpType ReturnType, bool LeftIsArray, bool WorksWithArrays, bool OnlyWorksWithArrs, bool IsConstantExpression)
{
    lhs = LHS;
    returnType = ReturnType;
    leftArray = LeftIsArray;
    isUnary = true;
    worksWithArrays = WorksWithArrays;
    onlyWorksWithArrays = OnlyWorksWithArrs;
    isConstantExpression = IsConstantExpression;
}
OpTypeInfo::OpTypeInfo()
{
}