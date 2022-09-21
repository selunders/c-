#ifndef SCANTYPE_H
#define SCANTYPE_H
struct TokenData {
    int tokenclass;  // token class
    int linenum;     // what line did this token occur on?
    char *tokenstr;
    int idIndex;     // index for id
    int numValue; // the value of the number as an INT?
    int boolValue;
    char charValue;
    char *stringValue;
};

#endif
