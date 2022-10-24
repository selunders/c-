#include "symbolTable.hpp"
#include "globals.hpp"
#include "util.hpp"
#include "colorPrint.hh"
// // // // // // // // // // // // // // // // // // // //
//
// Introduction
//
// This symbol table library supplies basic insert and lookup for
// symbols linked to void * pointers of data. The is expected to use
// ONLY the SymbolTable class and NOT the Scope class. The Scope class
// is used by SymbolTable in its implementation.
//
// Plenty of room for improvement inlcuding: better debugging setup,
// passing of refs rather than values and purpose built char *
// routines, and C support.
//
// WARNING: lookup will return NULL pointer if key is not in table.
// This means the void * cannot have zero as a legal value! Attempting
// to save a NULL pointer will get a error.
//
// A main() is commented out and has testing code in it.
//
// Robert Heckendorn   Apr 3, 2021
//

// // // // // // // // // // // // // // // // // // // //
//
// Some sample void * printing routines.   User shoud supply their own.
// The print routine will print the name of the symbol and then
// use a user supplied function to print the pointer.
//

// print nothing about the pointer
void pointerPrintNothing(void *data)
{
}

// print the pointer as a hex address
void pointerPrintAddr(void *data)
{
    printf("0x%016llx ", (unsigned long long int)(data));
}

// print the pointer as a long long int
void pointerPrintLongInteger(void *data)
{
    printf("%18lld ", (unsigned long long int)(data));
}

// print the pointer as a char * string
void pointerPrintStr(void *data)
{
    printf("%s ", (char *)(data));
}

char *boolToStr(bool b)
{
    return b ? (char *)"true" : (char *)"false";
}

void pointerPrintNode(void *data)
{
    TreeNode *t = (TreeNode *)data;
    if (t != NULL)
    {
        switch (t->nodeKind)
        {
        case NodeKind::DeclK:
            // printf("DeclK with subtype ");
            switch (t->subkind.decl)
            {
            case DeclKind::VarK:
                printf("Var: %s ", t->attr.string);
                if (t->isArray)
                    printf("is array ");
                if (t->isStatic)
                    // printf("of static type %s", expToString(t->expType));
                    printf("of type %s", expToString(t->expType));
                else
                    printf("of type %s", expToString(t->expType));
                break;
            case DeclKind::FuncK:
                // printf("Var: %s ", t->attr.string);
                // printf("About to print\n");
                printf("Func: %s ", t->attr.string);
                printf("returns type %s", expToString(t->expType));
                // printf("Func: %s", t->attr.string);
                break;
            case DeclKind::ParamK:
                printf("Parm: %s ", t->attr.string);
                if (t->isArray)
                    printf("is array ");
                printf("of type %s", expToString(t->expType));
                break;
            }
            break;
        case NodeKind::ExpK:
            // printf("ExpK");
            switch (t->subkind.exp)
            {
            case ExpKind::OpK:
                printf("Op: %s", opToString(t->attr.op));
                break;
            case ExpKind::ConstantK:
                switch (t->expType)
                {
                case ExpType::Boolean:
                    if (t->attr.value == 0)
                        printf("Const false");
                    else
                        printf("Const true");
                    break;
                case ExpType::Char:
                    if (t->isArray)
                        printf("Const %s", t->attr.string);
                    else
                        printf("Const \'%c\'", t->attr.cvalue);
                    break;
                case ExpType::Integer:
                    printf("Const %d", t->attr.value);
                    break;
                default:
                    printf("How'd you find this???\n\n\n");
                    break;
                }
                break;
            case ExpKind::IdK:
                printf("Id: %s", t->attr.string);
                break;
            case ExpKind::AssignK:
                printf("Assign: %s", assignToString(t->attr.op));
                break;
            case ExpKind::InitK:
                break;
            case ExpKind::CallK:
                printf("Call: %s", t->attr.string);
                break;
            default:
                break;
            }
            break;
        case NodeKind::StmtK:
            // printf("StmtK");
            switch (t->subkind.stmt)
            {
            case StmtKind::NullK:
                printf("Null");
                break;
            case StmtKind::IfK:
                printf("If");
                break;
            case StmtKind::WhileK:
                printf("While");
                break;
            case StmtKind::ForK:
                printf("For");
                break;
            case StmtKind::CompoundK:
                printf("Compound");
                break;
            case StmtKind::ReturnK:
                printf("Return");
                break;
            case StmtKind::BreakK:
                printf("Break");
                break;
            case StmtKind::RangeK:
                printf("Range");
                break;
            default:
                printf("... how'd you get here? (util.cpp ~380ish)\n\n");
                break;
            }
            break;
        default:
            printf("Found unknown node???\n\n");
            break;
        }
        // nodeCount++;
        // printf("%d\n", nodeCount);
        printf(" [line: %d] || etype:%s isInit: %s needsInitCheck: %s isArray: %s\n", t->lineno, expToString(t->expType), boolToStr(t->isInit), boolToStr(t->needsInitCheck), boolToStr(t->isArray));
        // // printf(" [line: %d]\nexpType::%s\n", t->lineno, expToString(t->expType));
        // for (i = 0; i < MAXCHILDREN; i++)
        // {
        //     if (t->child[i] != NULL)
        //     {
        //         printBasict(t->child[i], NodeRelation::Child, i, layer + 1);
        //         // printf("child %d is not NULL %p\n", i, t->child[i]);
        //     }
        //     // else
        //     // printf("child %d is NULL\n", i);
        // }
        // UNINDENT;
        // if (t->sibling != NULL)
        // {
        //     if (relation == NodeRelation::Sibling)
        //         printBasict(t->sibling, NodeRelation::Sibling, id + 1, layer);
        //     else
        //         printBasict(t->sibling, NodeRelation::Sibling, 1, layer);
        // }
        // INDENT;
        // t = NULL;
        // t = t->sibling;
    }
    // UNINDENT;
}

// // // // // // // // // // // // // // // // // // // //
//
// Class: Scope
//
// Helper class for SymbolTable
//

class SymbolTable::Scope
{
private:
    static bool debugFlg;                  // turn on tedious debugging
    std::string name;                      // name of scope
    std::map<std::string, void *> symbols; // use an ordered map (not as fast as unordered)

public:
    Scope(std::string newname);
    ~Scope();
    std::string scopeName();                              // returns name of scope
    void debug(bool state);                               // sets the debug flag to state
    void print(void (*printData)(void *));                // prints the table using the supplied function to print the void *
    void applyToAll(void (*action)(std::string, void *)); // applies func to all symbol/data pairs
    bool insert(std::string sym, void *ptr);              // inserts a new ptr associated with symbol sym
                                                          // returns false if already defined
    void *lookup(std::string sym);                        // returns the ptr associated with sym
                                                          // returns NULL if symbol not found
};

SymbolTable::Scope::Scope(std::string newname)
{
    name = newname;
    debugFlg = false;
}

SymbolTable::Scope::~Scope()
{
}

// returns char *name of scope
std::string SymbolTable::Scope::scopeName()
{
    return name;
}

// set scope debugging
void SymbolTable::Scope::debug(bool state)
{
    debugFlg = state;
}

// print the scope
void SymbolTable::Scope::print(void (*printData)(void *))
{
    printf("Scope: %-15s -----------------\n", name.c_str());
    for (std::map<std::string, void *>::iterator it = symbols.begin(); it != symbols.end(); it++)
    {
        printf("%20s: ", (it->first).c_str());
        printData(it->second);
        printf("\n");
    }
}

// apply the function to each symbol in this scope
void SymbolTable::Scope::applyToAll(void (*action)(std::string, void *))
{
    for (std::map<std::string, void *>::iterator it = symbols.begin(); it != symbols.end(); it++)
    {
        action(it->first, it->second);
    }
}

// returns true if insert was successful and false if symbol already in this scope
bool SymbolTable::Scope::insert(std::string sym, void *ptr)
{
    if (symbols.find(sym) == symbols.end())
    {
        if (debugFlg)
        {
            setPrintColor(PRINTCOLOR::GREEN);
            printf("DEBUG(Scope): insert in \"%s\" the symbol \"%s\".\n",
                   name.c_str(),
                   sym.c_str());
            resetPrintColor();
        }
        if (ptr == NULL)
        {
            printf("ERROR(SymbolTable): Attempting to save a NULL pointer for the symbol '%s'.\n",
                   sym.c_str());
        }
        symbols[sym] = ptr;
        return true;
    }
    else
    {
        if (debugFlg)
        {
            setPrintColor(PRINTCOLOR::GREEN);
            printf("DEBUG(Scope): insert in \"%s\" the symbol \"%s\" but symbol already there!\n", name.c_str(), sym.c_str());
            resetPrintColor();
        }
        return false;
    }
}

void *SymbolTable::Scope::lookup(std::string sym)
{
    if (symbols.find(sym) != symbols.end())
    {
        if (debugFlg)
        {
            setPrintColor(PRINTCOLOR::GREEN);
            printf("DEBUG(Scope): lookup in \"%s\" for the symbol \"%s\" and found it.\n", name.c_str(), sym.c_str());
            resetPrintColor();
        }
        return symbols[sym];
    }
    else
    {
        if (debugFlg)
        {
            setPrintColor(PRINTCOLOR::GREEN);
            printf("DEBUG(Scope): lookup in \"%s\" for the symbol \"%s\" and did NOT find it.\n", name.c_str(), sym.c_str());
            resetPrintColor();
        }
        return NULL;
    }
}

bool SymbolTable::Scope::debugFlg;

// // // // // // // // // // // // // // // // // // // //
//
// Class: SymbolTable
//
//  This is a stack of scopes that represents a symbol table
//

SymbolTable::SymbolTable()
{
    debugFlg = false;
    enter((std::string) "Global");
}

void SymbolTable::debug(bool state)
{
    debugFlg = state;
}

bool SymbolTable::getDebugState()
{
    if(debugFlg)
        return true;
    return false;
}

// Returns the number of scopes in the symbol table
int SymbolTable::depth()
{
    return stack.size();
}

// print all scopes using data printing func
void SymbolTable::print(void (*printData)(void *))
{
    printf("===========  Symbol Table  ===========\n");
    for (std::vector<Scope *>::iterator it = stack.begin(); it != stack.end(); it++)
    {
        (*it)->print(printData);
    }
    printf("===========  ============  ===========\n");
}

// Enter a scope
void SymbolTable::enter(std::string name)
{
    if (debugFlg)
    {
        setPrintColor(PRINTCOLOR::GREEN);
        printf("DEBUG(SymbolTable): enter scope \"%s\".\n", name.c_str());
        resetPrintColor();
    }
    stack.push_back(new Scope(name));
}

// Leave a scope (not allowed to leave global)
void SymbolTable::leave()
{
    if (debugFlg)
    {
        setPrintColor(PRINTCOLOR::GREEN);
        printf("DEBUG(SymbolTable): leave scope \"%s\".\n", (stack.back()->scopeName()).c_str());
        resetPrintColor();
    }
    if (stack.size() > 1)
    {
        delete stack.back();
        stack.pop_back();
    }
    else
    {
        printf("ERROR(SymbolTable): You cannot leave global scope.  Number of scopes: %d.\n", (int)stack.size());
    }
}

// Lookup a symbol anywhere in the stack of scopes
// Returns NULL if symbol not found, otherwise it returns the stored void * associated with the symbol
void *SymbolTable::lookup(std::string sym)
{
    void *data;
    std::string name;

    data = NULL; // set even though the scope stack should never be empty
    for (std::vector<Scope *>::reverse_iterator it = stack.rbegin(); it != stack.rend(); it++)
    {
        data = (*it)->lookup(sym);
        name = (*it)->scopeName();
        if (data != NULL)
            break;
    }

    if (debugFlg)
    {
        setPrintColor(PRINTCOLOR::GREEN);
        printf("DEBUG(SymbolTable): lookup the symbol \"%s\" and ", sym.c_str());
        if (data)
            printf("found it in the scope named \"%s\".\n", name.c_str());
        else
            printf("did NOT find it!\n");
        resetPrintColor();
    }

    return data;
}

// Lookup a symbol in the global scope
// returns NULL if symbol not found, otherwise it returns the stored void * associated with the symbol
void *SymbolTable::lookupGlobal(std::string sym)
{
    void *data;

    data = stack[0]->lookup(sym);
    if (debugFlg)
    {
        setPrintColor(PRINTCOLOR::GREEN);
        printf("DEBUG(SymbolTable): lookup the symbol \"%s\" in the Globals and %s.\n", sym.c_str(), (data ? "found it" : "did NOT find it"));
        resetPrintColor();
    }

    return data;
}

// Insert a symbol into the most recent scope
// Returns true if insert was successful and false if symbol already in the most recent scope
bool SymbolTable::insert(std::string sym, void *ptr)
{
    if (debugFlg)
    {
        setPrintColor(PRINTCOLOR::GREEN);
        printf("DEBUG(symbolTable): insert in scope \"%s\" the symbol \"%s\"",
               (stack.back()->scopeName()).c_str(), sym.c_str());
        if (ptr == NULL)
            printf(" WARNING: The inserted pointer is NULL!!");
        printf("\n");
        resetPrintColor();
    }

    return (stack.back())->insert(sym, ptr);
}

// Insert a symbol into the global scope
// Returns true is insert was successful and false if symbol already in the global scope
bool SymbolTable::insertGlobal(std::string sym, void *ptr)
{
    if (debugFlg)
    {
        setPrintColor(PRINTCOLOR::GREEN);
        printf("DEBUG(Scope): insert the global symbol \"%s\"", sym.c_str());
        if (ptr == NULL)
            printf(" WARNING: The inserted pointer is NULL!!");
        printf("\n");
        resetPrintColor();
    }

    return stack[0]->insert(sym, ptr);
}

// Apply function to each simple in the local scope.   The function gets both the
// string and the associated pointer.
void SymbolTable::applyToAll(void (*action)(std::string, void *))
{
    stack[stack.size() - 1]->applyToAll(action);
}

// Apply function to each simple in the global scope.   The function gets both the
// string and the associated pointer.
void SymbolTable::applyToAllGlobal(void (*action)(std::string, void *))
{
    stack[0]->applyToAll(action);
}

// // // // // // // // // // // // // // // // // // // //
//
// Some tests
//

std::string words[] = {"alfa", "bravo", "charlie", "dog", "echo", "foxtrot", "golf"};
int wordsLen = 7;

int counter;
void countSymbols(std::string sym, void *ptr)
{
    counter++;
    printf("%d %20s: ", counter, sym.c_str());
    pointerPrintAddr(ptr);
    printf("\n");
}

bool SymbolTable::test()
{
    Scope s("global");

    s.debug(true);
    s.insert("dog", (char *)"woof");
    s.insert("cat", (char *)"meow");
    printf("%s\n", (char *)(s.lookup("cat")));
    printf("%s\n", (char *)(s.lookup("dog")));

    if (s.lookup("fox") == NULL)
        printf("not found\n");
    else
        printf("found\n");
    s.print(pointerPrintAddr);
    s.print(pointerPrintStr);

    SymbolTable st;
    st.debug(true);

    printf("Print symbol table.\n");
    st.print(pointerPrintStr);
    st.insert("alfa", (char *)"ant");
    st.insert("bravo", (char *)"bat");
    st.insert("charlie", (char *)"cob");

    st.enter("First");
    st.insert("charlie", (char *)"cow");
    st.enter((std::string) "Second");
    st.insert("delta", (char *)"dog");
    st.insert("delta", (char *)"cow");
    st.insertGlobal("echo", (char *)"elk");

    printf("Print symbol table.\n");
    st.print(pointerPrintStr);

    printf("This is how you might use insert and lookup in your compiler.\n");
    st.leave(); // second no longer exists
    st.enter((std::string) "Third");
    if (st.insert("charlie", (char *)"cat"))
        printf("success\n");
    else
        printf("FAIL\n");
    if (st.insert("charlie", (char *)"pig"))
        printf("success\n");
    else
        printf("FAIL\n");
    if (st.lookup("charlie"))
        printf("success\n");
    else
        printf("FAIL\n");
    if (st.lookup("mirage"))
        printf("success\n");
    else
        printf("FAIL\n");

    printf("Print symbol table.\n");
    st.print(pointerPrintStr);
    fflush(stdout);

    printf("\nGeneral Lookup\n");
    for (int i = 0; i < wordsLen; i++)
    {
        void *data;

        if ((data = st.lookup(words[i])) == NULL)
            printf("%s: %s\n", words[i].c_str(), (char *)"NULL");
        else
            printf("%s: %s\n", words[i].c_str(), (char *)data);
    }

    printf("\nGlobal Lookup\n");
    for (int i = 0; i < wordsLen; i++)
    {
        void *data;

        if ((data = st.lookupGlobal(words[i])) == NULL)
            printf("%s: %s\n", words[i].c_str(), (char *)"NULL");
        else
            printf("%s: %s\n", words[i].c_str(), (char *)data);
    }

    printf("Test that apply works.\n");
    counter = 0;
    st.applyToAllGlobal(countSymbols);
    printf("Number of global symbols: %d\n", counter);

    st.insert((char *)"gnu", (char *)"goat");
    st.lookup((char *)"gnu");

    st.insertGlobal((char *)"gnu", (char *)"grebe");
    st.lookup((char *)"gnu");
    st.lookupGlobal((char *)"gnu");

    return true;
}

// int main()
// {
//     printf("Test\n\n\n");
//     SymbolTable st;
//     st.test();
// }