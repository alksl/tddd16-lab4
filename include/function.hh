#ifndef __KOMP_FUNCTION__
#define __KOMP_FUNCTION__

/*
 * SymbolInformationType is used to tag object subclassed from
 * SymbolInformation. The value of SymbolInformation's type field
 * specified which subclass the object belongs to.
 */

typedef enum
{
    kFunctionInformation,
    kVariableInformation,
    kTypeInformation,
} SymbolInformationType;


/*
 * SymbolInformation is the base class for all information about
 * symbols. It is never used directly; use the subclasses instead
 */

class SymbolInformation
{
public:
    SymbolInformationType       type;
    string                      id;
    SymbolTable                *table;

    SymbolInformation(SymbolInformationType t) : type(t) {};
};


/*
 * FunctionInformation represents information stored about a function
 * in the symbol table. It contains the return type of the function, a
 * pointer to the functions's first parameter and a pointer to the
 * symbol table for the function.
 */

class FunctionInformation : SymbolInformation
{
public:
    TypeInformation             *returnType;
    VariableInformation         *firstParam;
    SymbolTable                 *symbolTable;


    FunctionInformation() : SymbolInformationType(kFunctionInformation) {};

    void SetParent(FunctionInformation *);
    void SetReturnType(TypeInformation *);
    void SetName(string&);

    char AddParameter(string&, TypeInformation *);
    char AddVariable(string&, TypeInformation *);
    char AddArrayType(TypeInformation *, int);
};


/*
 * VariableInformation represents information stored about a variable
 * in the symbol table. It contains a type field which specifies
 * the type of the variable and a next field which is used to link
 * together parameters and local variables in a symbol table.
 */

class VariableInformation : SymbolInformation
{
public:
    TypeInformation             *type;
    VariableInformation         *next;

    VariableInformation() : SymbolInformation(kVariableInformation) {};
};

class TypeInformation : SymbolInformation
{
public:
    TypeInformation             *elementType;
    int                          arrayDimensions;

    TypeInformation() : SymbolInformation(kTypeInformation) {};
}




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
public:
    SymbolTableElement     **table;
    static int               nextTemporary;

    void AddSymbol(SymbolInformation *);
    SymbolInformation *LookupSymbol(string&);

    VariableInformation *GenTemp(TypeInformation *);
};

