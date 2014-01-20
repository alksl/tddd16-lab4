#ifndef __KOMP_CODEGEN__
#define __KOMP_CODEGEN__


//
// Quad types
//
// Quads that start with an `i' take integer arguments and return
// integer results, with a few exceptions. Operations that start with
// an `r' take real arguments and return real results.
//
//

typedef enum
{
    // Constants and stuff

    iconst,     // Set register to integer constant: iconst <c>   - <reg>
    rconst,     // Set register to real constant   : rconst <c>   - <reg>
    iaddr,      // Load base addr of a into reg    : iaddr  <a>   - <reg>
    itor,       // Convert integer in src to real  : itor   <src> - <reg>
    rtrunc,     // Truncate real in src            : rtrunc <src> - <reg>

    // Arithmetic operations

    iadd,       // Add integers a, b giving int r  : iadd <a> <b> <r>
    isub,       // Subtract b from a giving int r  : isub <a> <b> <r>
    imul,       // Multiply a by b giving int r    : imul <a> <b> <r>
    idiv,       // Divide a by b and truncate reslt: idiv <a> <b> <r>
    ipow,       // Raise x to the power of y (ints): ipow <x> <y> <r>
    radd,       // Add reals a, b giving real r    : radd <a> <b> <r>
    rsub,       // Subtract b from a giving real r : rsub <a> <b> <r>
    rmul,       // Multiply a by b giving real r   : rmul <a> <b> <r>
    rdiv,       // Divide a by b giving real r     : rdiv <a> <b> <r>
    rpow,       // Raise x to y (reals)            : rpow <x> <y> <r>

    // Comparisons

    igt,        // If a > b, then r = 1, else r = 0: igt <a> <b> <r>
    ilt,        // If a < b, then r = 1, else r = 0: ilt <a> <b> <r>
    ieq,        // If a = b, then r = 1, else r = 0: ieq <a> <b> <r>
    rgt,        // If a > b, then r = 1, else r = 0: rgt <a> <b> <r>
    rlt,        // If a < b, then r = 1, else r = 0: rlt <a> <b> <r>
    req,        // If a = b, then r = 1, else r = 0: req <a> <b> <r>

    // Conjunctions

    iand,       // If a && b then r = 1, else r = 0: iand <a> <b> <r>
    ior,        // If a || b then r = 1, else r = 0: ior  <a> <b> <r>
    inot,       // If !a then r = 1, else r = 0    : inot <a>  -  <r>

    // Jumps

    jtrue,      // Jump to label l if r is nonzero : jtrue  <l> <r> -
    jfalse,     // Jump to label l if r is zero    : jfalse <l> <r> -
    jump,       // Jump to label l                 : jump   <l>  -  -
    clabel,     // Label l                         : clabel <l>  -  -

    // Memory operations

    istore,     // Store r to memory location a    : istore <r> - <a>
    iload,      // Load memory location a to r     : iload  <a> - <r>
    rstore,     // Store r to memory location a    : istore <r> - <a>
    rload,      // Load memory location a to r     : iload  <a> - <r>

    // Parameters and stuff

    creturn,    // Exit function and return r      : return   -  - <r>
    param,      // Push parameter p                : param   <p> -  -
    call,       // Call function f, return in r    : call    <f> - <r>

    // Assignments

    iassign,    // Assign integer to register p    : iassign <r>  -  <p>
    rassign,    // Assign real to register p       : rassign <r>  -  <p>
    aassign,    // Assign n-elem array from r to p : aassign <r> <n> <p>

    // Just end the enum

    hcf,        // Crash. If this is generated, you've got a bug.
    nop         // Do nothing                      : nop - - -

} tQuadType;


class Quad
{
private:
    std::ostream& print(std::ostream&);

public:
    tQuadType        opcode;

    //
    // Arguments. Make sure you initialize the right ones!
    //

    SymbolInformation *sym1;
    SymbolInformation *sym2;
    SymbolInformation *sym3;
    long               int1;
    long               int2;
    long               int3;
    double             real1;
    double             real2;
    double             real3;


    Quad(tQuadType o,
         SymbolInformation *a, SymbolInformation *b, SymbolInformation *c) :
        opcode(o),
        sym1(a),
        sym2(b),
        sym3(c)
        {};

    Quad(tQuadType o, long a, SymbolInformation* b, SymbolInformation* c) :
        opcode(o),
        sym2(b),
        sym3(c),
        int1(a)
        {};

    Quad(tQuadType o, SymbolInformation *a, long b, SymbolInformation *c) :
        opcode(o),
        sym1(a),
        sym3(c),
        int2(b)
        {};


    Quad(tQuadType o,
         double a, SymbolInformation *b, SymbolInformation *c) :
        opcode(o),
        sym2(b),
        sym3(c),
        real1(a)
        {};

    friend std::ostream& operator<<(std::ostream&, Quad*);
    friend std::ostream& operator<<(std::ostream&, Quad&);
};


class QuadsListIterator;

class QuadsList
{
    class QuadsListElement
    {
    public:
        Quad                *data;
        QuadsListElement    *next;

        QuadsListElement(Quad *d, QuadsListElement *n) :
            data(d),
            next(n) {};
        ~QuadsListElement() { delete data; next = NULL; }
    };

    QuadsListElement        *head, *tail;
    static long              labelCounter;

    std::ostream& print(std::ostream&);

public:
    QuadsList() :
        head(NULL),
        tail(NULL) {};

    QuadsList& operator+=(Quad *q);
    long       NextLabel(void) { return (labelCounter += 1); };

    friend class QuadsListIterator;
    friend std::ostream& operator<<(std::ostream&, QuadsList*);
    friend std::ostream& operator<<(std::ostream&, QuadsList&);
};

class QuadsListIterator
{
    QuadsList::QuadsListElement     *elem;

public:
    QuadsListIterator(QuadsList *ql) :
        elem(ql->head) {};

    Quad *Next(void) {
        elem = elem ? elem->next : (QuadsList::QuadsListElement*)NULL;
        return elem ? (Quad*)NULL : elem->data;
    };
};


#endif
