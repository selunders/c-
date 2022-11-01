#include "errorMsg.hpp"

char *errmsg[50];
char *warnmsg[50];
extern int numErrors;
extern int numWarnings;

char* getErrMsg(ERR_ID errMsg)
{
    numErrors++;
    return errmsg[errMsg];
}

char* getWarnMsg(WARNING_ID warningMsg)
{
    numWarnings++;
    return warnmsg[warningMsg];
}

void initMsgTables()
{
    errmsg[errSimple] = (char *)"ERROR(%d): '%s' is a simple variable and cannot be called.\n";
    errmsg[errBothOpArr] = (char *)"ERROR(%d): '%s' requires both operands be arrays or not but lhs is %san array and rhs is %san array.\n";
    errmsg[errLeftCheck] = (char *)"ERROR(%d): '%s' requires operands of type %s but lhs is of type %s.\n";
    errmsg[errRightCheck] = (char *)"ERROR(%d): '%s' requires operands of type %s but rhs is of type %s.\n";
    errmsg[errEqTypeCheck] = (char *)"ERROR(%d): '%s' requires operands of the same type but lhs is type %s and rhs is type %s.\n";
    errmsg[errIndexTypeCheck] = (char *)"ERROR(%d): Array '%s' should be indexed by type int but got type %s.\n";
    errmsg[errIndexWithUnindexed] = (char *)"ERROR(%d): Array index is the unindexed array '%s'.\n";
    errmsg[errBreakOutsideLoop] = (char *)"ERROR(%d): Cannot have a break statement outside of loop.\n";
    errmsg[errIndexNonArr] = (char *)"ERROR(%d): Cannot index nonarray '%s'.\n";
    errmsg[errReturnArr] = (char *)"ERROR(%d): Cannot return an array.\n";
    errmsg[errArrInTestCondition] = (char *)"ERROR(%d): Cannot use array as test condition in %s statement.\n";
    errmsg[errArrInForRange] = (char *)"ERROR(%d): Cannot use array in position %d in range of for statement.\n";
    errmsg[errFuncAsVar] = (char *)"ERROR(%d): Cannot use function '%s' as a variable.\n";
    errmsg[errParmType] = (char *) "ERROR(%d): Expecting type %s in parameter %d of call to '%s' declared on line %d but got type %s.\n";
    errmsg[errIntInRange] = (char *) "ERROR(%d): Expecting type int in position %d in range of for statement but got type %s.\n";
    errmsg[errExpBool] = (char *) "ERROR(%d): Expecting Boolean test condition in %s statement but got type %s.\n";
    errmsg[errExpArrParam] = (char *) "ERROR(%d): %s array in parameter %d of call to '%s' declared on line %d.\n";
    errmsg[errExpNoReturn] = (char *) "ERROR(%d): Function '%s' at line %d is expecting no return value, but return has a value.\n";
    errmsg[errNoReturnVal] = (char *) "ERROR(%d): Function '%s' at line %d is expecting to return type %s but return has no value.\n";
    errmsg[errReturnWrongType] = (char *) "ERROR(%d): Function '%s' at line %d is expecting to return type %s but returns type %s.\n";
    errmsg[errInitNotConst] = (char *) "ERROR(%d): Initializer for variable '%s' is not a constant expression.\n";
    errmsg[errWrongInitType] = (char *) "ERROR(%d): Initializer for variable '%s' of type %s is of type %s\n";
    errmsg[errInitBothArr] = (char *) "ERROR(%d): Initializer for variable '%s' requires both operands be arrays or not but variable is%san array and rhs is%san array.\n";
    errmsg[errSymAlreadyDecl] = (char *) "ERROR(%d): Symbol '%s' is already declared at line %d.\n";
    errmsg[errSymNotDecl] = (char *) "ERROR(%d): Symbol '%s' is not declared.\n";
    errmsg[errWorksOnlyWithArr] = (char *) "ERROR(%d): The operation '%s' %s with arrays.\n";
    errmsg[errParamCount] = (char *) "ERROR(%d): Too %s parameters passed for function '%s' declared on line %d.\n";
    errmsg[errUnaryType] = (char *) "ERROR(%d): Unary '%s' requires an operand of type %s but was given type %s.\n";
    errmsg[errMainDef] = (char *) "ERROR(LINKER): A function named 'main' with no parameters must be defined.\n";
    
    warnmsg[warnDeclNotUsed] = (char *) "WARNING(%d): The %s '%s' seems not to be used.\n";
    warnmsg[warnMissingReturn] = (char *) "WARNING(%d): Expecting to return type %s but function '%s' has no return statement.\n";
    warnmsg[warnUninitVar] = (char *) "WARNING(%d): Variable \'%s\' may be uninitialized when used here.\n";
}