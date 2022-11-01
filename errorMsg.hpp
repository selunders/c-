
#ifndef _ERRORMSG_HPP_
#define _ERRORMSG_HPP_
#include <map>

enum ERR_ID
{
    errSimple,
    errBothOpArr,
    errLeftCheck,
    errRightCheck,
    errEqTypeCheck,
    errIndexTypeCheck,
    errIndexWithUnindexed,
    errBreakOutsideLoop,
    errIndexNonArr,
    errReturnArr,
    errArrInTestCondition,
    errArrInForRange,
    errFuncAsVar,
    errParmType,
    errIntInRange,
    errExpBool,
    errExpArrParam,
    errExpNoReturn,
    errNoReturnVal,
    errReturnWrongType,
    errInitNotConst,
    errWrongInitType,
    errInitBothArr,
    errSymAlreadyDecl,
    errSymNotDecl,
    errWorksOnlyWithArr,
    errParamCount,
    errUnaryType,
    errMainDef,
};

enum WARNING_ID
{
    warnUninitVar,
    warnDeclNotUsed,
    warnMissingReturn
};
// map<const char *, const char *> errmsg;

char* getErrMsg(ERR_ID errMsg);
char* getWarnMsg(WARNING_ID warnMsg);
void initMsgTables();

#endif