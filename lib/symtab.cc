#include <stdlib.h>
#include "symtab.hh"
#include "ast.hh"
#include "string.hh"

/*
 * Global variables
 */

FunctionInformation *currentFunction;
TypeInformation     *kIntegerType;
TypeInformation     *kRealType;
FunctionInformation *kFPrintFunction;
FunctionInformation *kIPrintFunction;
FunctionInformation *kFReadFunction;
FunctionInformation *kIReadFunction;

SymbolInformation::tFormatType SymbolInformation::outputFormat =
        SymbolInformation::kFullFormat;


/*
 * FunctionInformation methods
 */

std::ostream& SymbolInformation::print(std::ostream& o)
{
    switch (outputFormat)
    {
    case kFullFormat:
        o << "SymbolInformation @ " << (void*)this << '\n';
        o << "  Tag:   " << tag << '\n';
        o << "  ID:    " << id << '\n';
        o << "  Table: " << (void*)table << '\n';
        break;
    case kSummaryFormat:
        o << id << ' ' << tag;
        break;
    case kShortFormat:
        o << id;
        break;
    default:
        o << "Bad output format\n";
        abort();
    }

    return o;
}
std::ostream& SummarySymbols(std::ostream&);

std::ostream& TypeInformation::print(std::ostream& o)
{
    switch (outputFormat)
    {
    case kFullFormat:
        o << "TypeInformation @ " << (void*)this << '\n';
        o << "  Tag:   " << tag << '\n';
        o << "  ID:    " << id << '\n';
        o << "  Table: " << (void*)table << '\n';
        o << "  Element type: 0x" << (void*)elementType << ' ';
        if (elementType) o << SummarySymbols << elementType << LongSymbols;
        o << '\n';
        o << "  Dimensions:  " << arrayDimensions << '\n';
        o << "  Size:  " << size << '\n';

    case kSummaryFormat:
        o << (void*)this << ' ';
        if (elementType != NULL)
        {
            o << '\'' << id << '\'' << ' ';
            o << "array " << arrayDimensions
              << " of "
              << ShortSymbols << elementType << SummarySymbols;
        }
        else
        {
            o << id;
        }

        o << " [" << size << "]";
        break;

    case kShortFormat:
        if (elementType != NULL)
        {
            o << "array " << arrayDimensions
              << " of "
              << elementType;
        }
        else
        {
            o << id;
        }
        break;


    default:
        o << "Bad output format\n";
        abort();
    }
    return o;
}

std::ostream& VariableInformation::print(std::ostream& o)
{
    switch (outputFormat)
    {
    case kFullFormat:
        o << "VariableInformation @ " << (void*)this << '\n';
        o << "  Tag:   " << tag << '\n';
        o << "  ID:    " << id << '\n';
        o << "  Table: " << (void*)table  << '\n';
        o << "  Type:  " << (void*)type << ' ';
        if (type) o << SummarySymbols << type << LongSymbols;
        o << '\n';
        o << "  Next:  " << (void*)prev << ' ';
        if (prev) o << SummarySymbols << prev << LongSymbols;
        o << '\n';
        break;

    case kSummaryFormat:
        o << id << " : ";
        o << type;
        if (prev != NULL)
        {
            o << " --> " << (void*)prev << ' ' << prev->id;
        }
        break;

    case kShortFormat:
        o << id;
        break;

    default:
        o << "Bad output format\n";
        abort();
    }

    return o;
}

std::ostream& FunctionInformation::print(std::ostream& o)
{
    VariableInformation *tmp;

    switch (outputFormat)
    {
    case kFullFormat:
        o << "FunctionInformation @ " << (void*)this << '\n';
        o << "  Tag:     " << tag << '\n';
        o << "  ID:      " << id << '\n';
        o << "  Table:   " << (void*)table << '\n';
        o << "  Parent:  " << (void*)parent << ' ';
        if (parent) o << ShortSymbols << parent << LongSymbols;
        o << '\n';
        o << "  Returns: " << (void*)returnType << ' ';
        if (returnType) o << ShortSymbols << returnType << LongSymbols;
        o << '\n';

        if (lastParam != NULL)
        {
            o << "  Parameters:\n" << ShortSymbols;
            tmp = lastParam;
            while (tmp != NULL)
            {
                o << "    " << (void*)tmp << ' ' << tmp << '\n';
                tmp = tmp->prev;
            }
            o << LongSymbols;
        }
        else
        {
            o << "  Parameters: none\n";
        }

        if (lastLocal)
        {
            o << "  Locals:\n" << ShortSymbols;
            tmp = lastLocal;
            while (tmp != NULL)
            {
                o << "    " << (void*)tmp << ' ' << tmp << '\n';
                tmp = tmp->prev;
            }
            o << LongSymbols;
        }
        else
        {
            o << "  Locals: none\n";
        }

        o << "  Body:  " << (void*)body << '\n';
        if (body) o << body;
        o << '\n';

        o << "  Quads: " << (void*)quads << '\n';
        if (quads) o << quads;
        o << '\n';

        o << symbolTable;
        break;

    case kSummaryFormat:
        o << id << '(';

        tmp = lastParam;
        while (tmp != NULL)
        {
            o << ShortSymbols << tmp << SummarySymbols;
            tmp = tmp->prev;
            if (tmp != NULL)
                o << "; ";
        }
        o << ") -> ";
        if (returnType) o << ShortSymbols << returnType << SummarySymbols;
        else
            o << "no return type";
        break;

    case kShortFormat:
        o << id;
        break;

    default:
        o << "Bad output format.\n";
        abort();
    }

    return o;
}

void FunctionInformation::SetParent(FunctionInformation *newParent)
{
    parent = newParent;
}

FunctionInformation *FunctionInformation::GetParent(void)
{
    return parent;
}

void FunctionInformation::SetBody(StatementList *b)
{
    body = b;
}

StatementList *FunctionInformation::GetBody(void)
{
    return body;
}

void FunctionInformation::SetReturnType(TypeInformation *newReturnType)
{
    returnType = newReturnType;
}

TypeInformation *FunctionInformation::GetReturnType(void)
{
    return returnType;
}

VariableInformation *FunctionInformation::GetLastParam(void)
{
    return lastParam;
}


SymbolInformation *FunctionInformation::LookupIdentifier(const string& name)
{
    SymbolInformation *info;

    info = symbolTable.LookupSymbol(name);

    if (info != NULL)
        return info;
    else if (parent == NULL)
        return NULL;
    else
        return parent->LookupIdentifier(name);
}

VariableInformation *FunctionInformation::AddParameter(const string& name,
                                                       TypeInformation *type)
{
    VariableInformation *info;

    info = new VariableInformation(name, type);
    symbolTable.AddSymbol(info);

    info->prev = lastParam;
    lastParam = info;

    return info;
}

SymbolInformation *FunctionInformation::AddSymbol(SymbolInformation *i)
{
    symbolTable.AddSymbol(i);
    return i;
}

VariableInformation *FunctionInformation::AddVariable(const string& name,
                                                      TypeInformation *type)
{
    VariableInformation *info;

    info = new VariableInformation(name, type);
    symbolTable.AddSymbol(info);

    info->prev = lastLocal;
    lastLocal = info;

    return info;
}

TypeInformation *FunctionInformation::AddArrayType(TypeInformation *elemType,
                                                   int dimensions)
{
    SymbolInformation *xinfo;
    TypeInformation *info;
    string           name;

    if (parent != NULL)
        return parent->AddArrayType(elemType, dimensions);
    else
    {

        name = elemType->id + "<" + dimensions + ">.";
        xinfo = symbolTable.LookupSymbol(name);
        if (xinfo == NULL)
        {
            info = new TypeInformation(name, elemType->size * dimensions);
            info->elementType = elemType;
            info->arrayDimensions = dimensions;
            symbolTable.AddSymbol(info);
        }
        else
        {
            info = xinfo->SymbolAsType();

            if (info == NULL)
            {
                std::cerr << "Bug: Name collision creating array (can't happen.)\n"
                     << std::flush;
                abort();
            }
        }
        return info;
    }
}

FunctionInformation *FunctionInformation::AddFunction(const string& name,
                                                      FunctionInformation *fn)
{
    SymbolInformation *xinfo;

    xinfo = LookupIdentifier(name);
    if (xinfo != NULL && xinfo->tag == kTypeInformation)
    {
        std::cerr << "Bug: you tried to create a function that's also a type\n";
	abort();
    }

    xinfo = symbolTable.LookupSymbol(name);
    if (xinfo != NULL)
    {
        std::cerr << "Bug: you tried to create a function with a name "
	     << "that's already in use\n";
    }

    fn->id = name;
    symbolTable.AddSymbol(fn);

    return fn;
}

VariableInformation *FunctionInformation::TemporaryVariable(TypeInformation *type)
{
    VariableInformation   *info;

    temporaryCount += 1;

    info = new VariableInformation(string("T:") + (int)temporaryCount, type);
    info->prev = NULL;
    AddSymbol(info);

    return info;
}


char FunctionInformation::OkToAddSymbol(const string& name)
{
    SymbolInformation *info;

    info = symbolTable.LookupSymbol(name);
    if (info != NULL)
        return 0;

    info = symbolTable.LookupSymbol(name);
    if (info != NULL && info->tag == kTypeInformation)
        return 0;
    else
        return 1;
}


void FunctionInformation::GenerateCode(void)
{
    if (body)
    {
        quads = new QuadsList;
        body->GenerateCode(*quads);
    }
}



/*
 * SymbolTable methods
 */


int SymbolTable::nextTemporary;

SymbolTable::SymbolTable()
{
    int i;

    tableSize = 1023;
    table = new SymbolTableElement*[1023];

    for (i = 0; i < tableSize; i++)
        table[i] = NULL;
}

void SymbolTable::AddSymbol(SymbolInformation *info)
{
    int                 index;
    SymbolTableElement *elem;
    string              tmp = info->id;

    info->table = this;
    tmp.upcase();
    index = tmp.hash();
    index = (index < 0 ? -index : index) % tableSize;
    if (table[index] == NULL)
    {
        table[index] = new SymbolTableElement;
        table[index]->info = info;
        table[index]->next = NULL;
    }
    else
    {
        elem = table[index];
        while (elem->next)
        {
            elem = elem->next;
        }
        elem->next = new SymbolTableElement;
        elem->next->info = info;
        elem->next->next = NULL;
    }
}

SymbolInformation *SymbolTable::LookupSymbol(const string& id)
{
    int                  index;
    SymbolTableElement  *elem;
    string               tmp = id;

    tmp.upcase();
    index = tmp.hash();
    index = (index < 0 ? -index : index) % tableSize;
    elem = table[index];

    while (elem)
    {
        if (elem->info->id == id)
            return elem->info;
        else
            elem = elem->next;
    }

    return NULL;
}

std::ostream& SymbolTable::print(std::ostream& o)
{
    int                      i;
    SymbolTableElement      *elem;

    o << "------------------------------------------------"
      << "-------------------------------\n";
    o << "SymbolTable @ " << (void*)this << '\n';
    o << "------------------------------------------------"
      << "-------------------------------\n";

    o << SummarySymbols;
    for (i = 0; i < tableSize; i++)
    {
        if (table[i] != NULL)
        {
            elem = table[i];
            while (elem != NULL)
            {
                o << i << '\t' << (void*)elem->info << ' ' <<elem->info<< '\n';
                elem = elem->next;
            }
        }
    }
    o << LongSymbols;

    o << "------------------------------------------------"
      << "-------------------------------\n";

    return o;
}


std::ostream& operator<<(std::ostream& o, SymbolTable& t)
{
    return t.print(o);
}

std::ostream& operator<<(std::ostream& o, SymbolTable *t)
{
    return t->print(o);
}

std::ostream& operator<<(std::ostream& o, SymbolInformation& i)
{
    return i.print(o);
}

std::ostream& operator<<(std::ostream& o, SymbolInformation *i)
{
    if (i == NULL)
        return o << "<SymbolInformation @ 0x0>";
    else
        return i->print(o);
}

std::ostream& ShortSymbols(std::ostream& o)
{
    SymbolInformation::outputFormat = SymbolInformation::kShortFormat;
    return o;
}

std::ostream& LongSymbols(std::ostream& o)
{
    SymbolInformation::outputFormat = SymbolInformation::kFullFormat;
    return o;
}

std::ostream& SummarySymbols(std::ostream& o)
{
    SymbolInformation::outputFormat = SymbolInformation::kSummaryFormat;
    return o;
}
