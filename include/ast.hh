#ifndef __KOMP_AST__
#define __KOMP_AST__

#include <symtab.hh>
#include <codegen.hh>

class ASTNode;                  // X
class StatementList;            // X
class Statement;                // X
class ElseIfList;               // X
class IfStatement;              // X
class Assignment;               // X
class CallStatement;            // X
class ReturnStatement;          // X
class WhileStatement;           // X
class Expression;               // X
class ExpressionList;           // X
class FunctionCall;             // X
class IntegerToReal;            // X
class TruncateReal;             // X
class IntegerConstant;          // X
class RealConstant;             // X
class BinaryOperation;          // X
class Plus;                     // X
class Minus;                    // X
class Times;                    // X
class Divide;                   // X
class Power;                    // X
class UnaryMinus;               // X
class LeftValue;                // X
class ArrayReference;           // X
class Identifier;               // X
class Condition;                // X
class BinaryRelation;           // X
class BinaryCondition;          // X
class LessThan;                 // X
class GreaterThan;              // X
class GreaterThanOrEqual;       // X
class LessThanOrEqual;          // X
class Equal;                    // X
class NotEqual;                 // X
class BooleanConstant;          // X

typedef enum
{
} ASTNodeType;


class ASTNode
{
protected:
    static int  indentLevel;
    static bool branches[10000];

    void indent(std::ostream& o);
    void indentMore(void);
    void indentLess(void);
    void beginChild(std::ostream& o);
    void endChild(std::ostream& o);
    void lastChild(std::ostream& o);
    virtual void print(std::ostream& o);
    virtual void xprint(std::ostream& o, char* cls);

public:
    virtual VariableInformation *GenerateCode(QuadsList &q) = 0;
    virtual VariableInformation *GenerateCodeAndJump(QuadsList &q,
                                                     long label);

    friend std::ostream& operator<<(std::ostream&, ASTNode&);
    friend std::ostream& operator<<(std::ostream&, ASTNode*);
};

std::ostream& operator<<(std::ostream&, ASTNode&);
std::ostream& operator<<(std::ostream&, ASTNode*);


class StatementList :  public ASTNode
{
protected:
    virtual void print(std::ostream& o);
public:
    Statement       *statement;
    StatementList   *precedingStatements;

    StatementList(StatementList *l, Statement *s) :
        statement(s),
        precedingStatements(l) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class Statement :  public ASTNode
{
protected:
    virtual void print(std::ostream& o);
public:
    virtual VariableInformation *GenerateCode(QuadsList &q) = 0;
};

class ElseIfList  :  public ASTNode
{
protected:
    virtual void print(std::ostream& o);

public:
    ElseIfList      *preceding;
    Condition       *condition;
    StatementList   *body;

    ElseIfList(ElseIfList *p, Condition *c, StatementList *b) :
        preceding(p),
        condition(c),
        body (b) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
    virtual VariableInformation *GenerateCodeAndJump(QuadsList& q,
                                                     long lbl);
};

class IfStatement :  public Statement
{
protected:
    virtual void print(std::ostream& o);

public:
    Condition       *condition;
    StatementList   *thenStatements;
    ElseIfList      *elseIfList;
    StatementList   *elseStatements;

    IfStatement(Condition *c,
                StatementList *ts,
                ElseIfList *eif,
                StatementList *es) :
        condition(c),
        thenStatements(ts),
        elseIfList(eif),
        elseStatements(es) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class Assignment :  public Statement
{
protected:
    virtual void print(std::ostream& o);

public:
    LeftValue       *target;
    Expression      *value;

    Assignment(LeftValue *l, Expression *r) :
        target(l),
        value(r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class CallStatement :  public Statement
{
protected:
    virtual void print(std::ostream& o);

public:
    FunctionCall        *call;

    CallStatement(FunctionCall *c) :
        call(c) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class ReturnStatement :  public Statement
{
protected:
    virtual void print(std::ostream& o);

public:
    Expression          *value;

    ReturnStatement() :
        value(NULL) {};
    ReturnStatement(Expression *e) :
        value(e) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class WhileStatement :  public Statement
{
protected:
    virtual void print(std::ostream& o);

public:
    Condition           *condition;
    StatementList       *body;

    WhileStatement(Condition *c, StatementList *b) :
        condition(c),
        body(b) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};


class Expression :  public ASTNode
{
protected:
    virtual void print(std::ostream& o);

public:
    TypeInformation         *valueType;

    Expression(TypeInformation *t) :
        valueType(t) {};

    virtual VariableInformation *GenerateCode(QuadsList &q) = 0;
};

class ExpressionList :  public ASTNode
{
protected:
    virtual void print(std::ostream& o);

public:
    ExpressionList          *precedingExpressions;
    Expression              *expression;

    ExpressionList(ExpressionList *pe,
                   Expression *e) :
        precedingExpressions(pe),
        expression(e) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
    virtual void GenerateParameterList(QuadsList &q,
                                       VariableInformation *firstParam);
};

class FunctionCall :  public Expression
{
protected:
    virtual void print(std::ostream& o);

public:
    FunctionInformation     *function;
    ExpressionList          *arguments;

    FunctionCall(FunctionInformation *f,
                 ExpressionList *a) :
        Expression(f->GetReturnType()),
        function(f),
        arguments(a) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class IntegerToReal :  public Expression
{
protected:
    virtual void print(std::ostream& o);

public:
    Expression              *value;

    IntegerToReal(Expression *e) :
        Expression(kRealType),
        value(e) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class TruncateReal :  public Expression
{
protected:
    virtual void print(std::ostream& o);

public:
    Expression              *value;

    TruncateReal(Expression *e) :
        Expression(kIntegerType),
        value(e) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class IntegerConstant :  public Expression
{
protected:
    virtual void print(std::ostream& o);

public:
    long int                 value;

    IntegerConstant(long int v) :
      Expression(kIntegerType),
      value(v) {}

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class RealConstant :  public Expression
{
protected:
    virtual void print(std::ostream& o);

public:
    double                  value;

    RealConstant(double v) :
        Expression(kRealType),
        value(v) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class BinaryOperation :  public Expression
{
protected:
    virtual void print(std::ostream& o);
    virtual void xprint(std::ostream& o, char *);

public:
    Expression          *left, *right;

    BinaryOperation(Expression *l, Expression *r) :
        Expression(l->valueType),
        left(l),
        right(r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q) = 0;
};

class Plus :  public BinaryOperation
{
protected:
    virtual void print(std::ostream& o);

public:
    Plus(Expression *l, Expression *r) :
        BinaryOperation(l, r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};


class Minus :  public BinaryOperation
{
protected:
    virtual void print(std::ostream& o);

public:
    Minus(Expression *l, Expression *r) :
        BinaryOperation(l, r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};


class Times :  public BinaryOperation
{
protected:
    virtual void print(std::ostream& o);

public:
    Times(Expression *l, Expression *r) :
        BinaryOperation(l, r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};


class Divide :  public BinaryOperation
{
protected:
    virtual void print(std::ostream& o);

public:
    Divide(Expression *l, Expression *r) :
        BinaryOperation(l, r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};


class Power :  public BinaryOperation
{
protected:
    virtual void print(std::ostream& o);

public:
    Power(Expression *l, Expression *r) :
        BinaryOperation(l, r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};


class UnaryMinus :  public Expression
{
protected:
    virtual void print(std::ostream& o);

public:
    Expression          *right;

    UnaryMinus(Expression *e) :
        Expression(e->valueType),
        right(e) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};



class LeftValue :  public Expression
{
protected:
    virtual void print(std::ostream& o);

public:
    LeftValue(TypeInformation *t) :
        Expression(t) {};

    virtual void GenerateAssignment(QuadsList& q,
                                    VariableInformation *val) = 0;
    virtual VariableInformation *GenerateCode(QuadsList &q) = 0;
};

class ArrayReference :  public LeftValue
{
protected:
    virtual void print(std::ostream& o);

public:
    VariableInformation     *id;
    Expression              *index;

    ArrayReference(VariableInformation *i,
                   Expression *x) :
        LeftValue(i->type->elementType),
        id(i),
        index(x) {};

    virtual void GenerateAssignment(QuadsList& q,
                                    VariableInformation *val);
    virtual VariableInformation *GenerateCode(QuadsList &q);
};


class Identifier :  public LeftValue
{
protected:
    virtual void print(std::ostream& o);

public:
    VariableInformation     *id;

    Identifier(VariableInformation *i) :
        LeftValue(i->type),
        id(i) {};

    virtual void GenerateAssignment(QuadsList& q,
                                    VariableInformation *val);
    virtual VariableInformation *GenerateCode(QuadsList &q);
};



class Condition :  public ASTNode
{
protected:
    virtual void print(std::ostream& o);

public:
    virtual VariableInformation *GenerateCode(QuadsList &q) = 0;
};

class BinaryRelation :  public Condition
{
protected:
    virtual void print(std::ostream& o);
    virtual void xprint(std::ostream& o, char *cls);

public:
    Expression      *left;
    Expression      *right;

    BinaryRelation(Expression *l, Expression *r) :
        left(l),
        right(r) {};
    virtual VariableInformation *GenerateCode(QuadsList &q) = 0;
};

class LessThan :  public BinaryRelation
{
protected:
    virtual void print(std::ostream& o);
public:
    LessThan(Expression *l, Expression *r) :
        BinaryRelation(l, r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class GreaterThan :  public BinaryRelation
{
protected:
    virtual void print(std::ostream& o);
public:
    GreaterThan(Expression *l, Expression *r) :
        BinaryRelation(l, r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class GreaterThanOrEqual :  public BinaryRelation
{
protected:
    virtual void print(std::ostream& o);
public:
    GreaterThanOrEqual(Expression *l, Expression *r) :
        BinaryRelation(l, r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class LessThanOrEqual :  public BinaryRelation
{
protected:
    virtual void print(std::ostream& o);
public:
    LessThanOrEqual(Expression *l, Expression *r) :
        BinaryRelation(l, r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class Equal :  public BinaryRelation
{
protected:
    virtual void print(std::ostream& o);
public:
    Equal(Expression *l, Expression *r) :
        BinaryRelation(l, r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class NotEqual :  public BinaryRelation
{
protected:
    virtual void print(std::ostream& o);
public:
    NotEqual(Expression *l, Expression *r) :
        BinaryRelation(l, r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};


class BinaryCondition : public Condition
{
protected:
    virtual void print(std::ostream& o);
    virtual void xprint(std::ostream&o, char *cls);

public:
    Condition *left, *right;

    BinaryCondition(Condition *l, Condition *r) :
        left(l),
        right(r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q) = 0;
};

class And : public BinaryCondition
{
protected:
    virtual void print(std::ostream& o);
public:
    And(Condition *l, Condition *r) :
        BinaryCondition(l, r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class Or : public BinaryCondition
{
protected:
    virtual void print(std::ostream& o);
public:
    Or(Condition *l, Condition *r) :
        BinaryCondition(l, r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class Not : public Condition
{
protected:
    virtual void print(std::ostream& o);
public:
    Condition *right;

    Not(Condition *r) :
        right(r) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

class BooleanConstant : public Condition
{
protected:
    virtual void print(std::ostream& o);
public:
    bool         value;

    BooleanConstant(int v) :
        value(v?true:false) {};

    virtual VariableInformation *GenerateCode(QuadsList &q);
};

#endif
