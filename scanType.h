#ifndef SCANTYPE_H
#define SCANTYPE_H

#include <iostream>
#include <string>

using namespace std;

string charArrayToString(char *text)
{
    string s(text);
    return s;
}

enum class TOKENCLASS
{
    BASE,
    ID,
    NUMCONST,
    CHARCONST,
    STRINGCONST
};

class BASE_Token
{
protected:
    TOKENCLASS tokenClass;
    int lineNumber;
    string tokenString;

public:
    string getString()
    {
        return tokenString;
    }

    BASE_Token(int lineNum, char *tokString)
    {
        tokenClass = TOKENCLASS::BASE;
        lineNumber = lineNum;
        tokenString = charArrayToString(tokString);
    }

    BASE_Token() {} // Needed for subclasses
};

class ID_Token : public BASE_Token
{
public:
    ID_Token(int lineNum, char *tokString)
    {
        tokenClass = TOKENCLASS::ID;
        lineNumber = lineNum;
        tokenString = charArrayToString(tokString);
    }
};

class NUMCONST_Token : public BASE_Token
{
private:
    int Value;

public:
    NUMCONST_Token(int lineNum, char *tokString)
    {
        tokenClass = TOKENCLASS::NUMCONST;
        lineNumber = lineNum;
        tokenString = charArrayToString(tokString);
        Value = stoi(tokenString);
    }
};

class CHARCONST_Token : public BASE_Token
{
private:
    char characterValue;

public:
    CHARCONST_Token(int lineNum, char *tokString)
    {
        tokenClass = TOKENCLASS::CHARCONST;
        lineNumber = lineNum;
        tokenString = charArrayToString(tokString);
        if (tokenString.length() > 1)
        {
            cout << "WARNING(" << lineNumber << "): character is " << tokenString.length() << " characters long and not a single character: ''" << tokenString << "''.  The first char will be used." << endl;
            tokenString = tokenString[0];
        }
    }
};

class STRINGCONST_Token : public BASE_Token
{
private:
    string Value;

public:
    STRINGCONST_Token(int lineNum, char *tokString)
    {
        tokenClass = TOKENCLASS::STRINGCONST;
        lineNumber = lineNum;
        tokenString = charArrayToString(tokString);
    }
};

#endif