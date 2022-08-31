#include <iostream>
#include <string>

using namespace std;

std::string charArrayToString(char *text)
{
    std::string s(text);
    return s;
}

enum class TOKENCLASSES {BASE, ID, NUMCONST, CHARCONST, STRINGCONST};

class BASE_Token
{
    protected:
        TOKENCLASSES tokenClass;
        int lineNumber;
        std::string tokenString;

    public:
        std::string getString()
        {
            return tokenString;
        }

        BASE_Token(int lineNum, char *tokString)
        {
            tokenClass = TOKENCLASSES::BASE;
            lineNumber = lineNum;
            tokenString = charArrayToString(tokString);
        }

        BASE_Token(){} // Default
};

class ID_Token : public BASE_Token
{
    public:
        ID_Token(int lineNum, char *tokString)
        {
            tokenClass = TOKENCLASSES::ID;
            lineNumber = lineNum;
            tokenString = charArrayToString(tokString);
        }
};

class NUMCONST_Token : public BASE_Token
{
    public:
        NUMCONST_Token(int lineNum, char *tokString)
        {
            tokenClass = TOKENCLASSES::NUMCONST;
            lineNumber = lineNum;
            tokenString = charArrayToString(tokString);
        }
};

class CHARCONST_Token : public BASE_Token
{
    public:
        CHARCONST_Token(int lineNum, char *tokString)
        {
            tokenClass = TOKENCLASSES::CHARCONST;
            lineNumber = lineNum;
            tokenString = charArrayToString(tokString);
            if(tokenString.length() > 1)
            {
                cout << "WARNING(" << lineNumber << "): character is " << tokenString.length() << " characters long and not a single character: ''" << tokenString << "''.  The first char will be used." << endl;
                tokenString = tokenString[0];
            }
        }
};