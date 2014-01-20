#ifndef __KOMP_SYMTAB__
#define __KOMP_SYMTAB__

#include <string.hh>

class StatementList;
class QuadsList;

class SymbolInformation;
class VariableInformation;
class FunctionInformation;
class TypeInformation;
class SymbolTableElement;
class SymbolTable;

extern FunctionInformation *currentFunction;

extern FunctionInformation *kFPrintFunction;
extern FunctionInformation *kIPrintFunction;
extern FunctionInformation *kFReadFunction;
extern FunctionInformation *kIReadFunction;
extern TypeInformation *kRealType;
extern TypeInformation *kIntegerType;



/*
 * SymbolTable is a symbol table. You'll never really use this
 * directly. Instead, use the methods in the FunctionInformation
 * class for adding and looking up variables in the symbol table
 */


class SymbolTableElement
{
public:
    SymbolInformation       *info;
    SymbolTableElement      *next;
};

class SymbolTable
{
protected:
    virtual std::ostream& print(std::ostream&);

public:
    SymbolTableElement     **table;
    int                      tableSize;
    static int               nextTemporary;

    SymbolTable();

    void AddSymbol(SymbolInformation *);
    SymbolInformation *LookupSymbol(const string&);

    friend std::ostream& operator<<(std::ostream&, SymbolTable &);
    friend std::ostream& operator<<(std::ostream&, SymbolTable *);
};





/*
 * SymbolInformationType is used to tag object subclassed from
 * SymbolInformation. The value of SymbolInformation's type field
 * specified which subclass the object belongs to.
 */

typedef enum
{
    kFunctionInformation,
    kVariableInformation,
    kTypeInformation
} SymbolInformationType;


/*
 * SymbolInformation is the base class for all information about
 * symbols. It is never used directly; use the subclasses instead
 */

class SymbolInformation
{
protected:
    virtual std::ostream& print(std::ostream&);
    friend class SymbolTable;
    friend std::ostream& LongSymbols(std::ostream&);
    friend std::ostream& SummarySymbols(std::ostream&);
    friend std::ostream& ShortSymbols(std::ostream&);

    typedef enum { kFullFormat, kSummaryFormat, kShortFormat } tFormatType;

    static tFormatType outputFormat;

public:
    SymbolInformationType       tag;
    string                      id;
    SymbolTable                *table;

    SymbolInformation(SymbolInformationType t, const string &i) :
        tag(t),
        id(i) {};
    virtual ~SymbolInformation() {}

    virtual FunctionInformation *SymbolAsFunction(void) { return NULL; };
    virtual VariableInformation *SymbolAsVariable(void) { return NULL; };
    virtual TypeInformation     *SymbolAsType(void)     { return NULL; };

    friend std::ostream& operator<<(std::ostream&, SymbolInformation&);
    friend std::ostream& operator<<(std::ostream&, SymbolInformation*);
};


/*
 * FunctionInformation represents information stored about a function
 * in the symbol table. It contains the return type of the function, a
 * pointer to the functions's last parameter and a pointer to the
 * symbol table for the function.
 */

class FunctionInformation : public SymbolInformation
{
protected:
    virtual std::ostream& print(std::ostream&);
    friend class SymbolTable;

    long temporaryCount;

private:
    FunctionInformation         *parent;
    TypeInformation             *returnType;
    VariableInformation         *lastParam;
    VariableInformation         *lastLocal;
    SymbolTable                  symbolTable;

    StatementList               *body;
    QuadsList                   *quads;

public:

    FunctionInformation(const string& i) :
        SymbolInformation(kFunctionInformation, i),
        parent(NULL),
        returnType(NULL),
        lastParam(NULL),
        lastLocal(NULL),
        body(NULL),
        quads(NULL) {};

    virtual FunctionInformation *SymbolAsFunction(void) { return this; };

    void SetParent(FunctionInformation *);
    void SetReturnType(TypeInformation *);
    void SetBody(StatementList *);
    void SetQuads(QuadsList *);

    FunctionInformation *GetParent(void);
    TypeInformation     *GetReturnType(void);
    VariableInformation *GetLastParam(void);
    StatementList       *GetBody(void);
    QuadsList           *GetQuads(void);

    FunctionInformation *AddFunction(const string&, FunctionInformation *);
    VariableInformation *AddParameter(const string&, TypeInformation *);
    VariableInformation *AddVariable(const string&, TypeInformation *);
    SymbolInformation   *AddSymbol(SymbolInformation *);
    TypeInformation     *AddArrayType(TypeInformation *, int);

    VariableInformation *TemporaryVariable(TypeInformation *type);

    void GenerateCode(void);

    char OkToAddSymbol(const string&);

    SymbolInformation *LookupIdentifier(const string&);
};


/*
 * VariableInformation represents information stored about a variable
 * in the symbol table. It contains a type field which specifies
 * the type of the variable and a next field which is used to link
 * together parameters and local variables in a symbol table.
 */

class VariableInformation : public SymbolInformation
{
protected:
    virtual std::ostream& print(std::ostream&);

public:
    TypeInformation             *type;
    VariableInformation         *prev;

    virtual VariableInformation *SymbolAsVariable(void) { return this; };

    VariableInformation(const string& i) :
        SymbolInformation(kVariableInformation, i) {};
    VariableInformation(const string& i, TypeInformation *t) :
        SymbolInformation(kVariableInformation, i),
        type(t) {};

};

class TypeInformation : public SymbolInformation
{
protected:
    virtual std::ostream& print(std::ostream&);
    friend class SymbolTable;
    friend class FunctionInformation;
    friend class VariableInformation;

public:
    TypeInformation             *elementType;
    int                          arrayDimensions;
    unsigned long                size;

    virtual TypeInformation     *SymbolAsType(void)      { return this; };

    TypeInformation(const string& i, unsigned long s) :
        SymbolInformation(kTypeInformation, i),
        size(s) {};
};


/*
 * I/O manipulators for better output
 */

std::ostream& ShortSymbols(std::ostream& o);
std::ostream& LongSymbols(std::ostream& o);


#endif
