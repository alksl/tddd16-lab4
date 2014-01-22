%{
#include <stdlib.h>
#include <iostream>
#include <string.hh>
#include <ast.hh>
#include <symtab.hh>

extern char                    *yytext;
extern int                      yylineno, errorCount, warningCount;
extern FunctionInformation     *currentFunction;

extern int yylex(void);
extern void yyerror(char *);
extern char CheckCompatibleTypes(Expression **, Expression **);
extern char CheckAssignmentTypes(LeftValue **, Expression **);
extern char CheckFunctionParameters(FunctionInformation *,
                                    VariableInformation *,
                                    ExpressionList      *);
char CheckReturnType(Expression **, TypeInformation *);
extern std::ostream& error(void);
extern std::ostream& warning(void);

#define YYDEBUG 1
%}

/*
 * We have multiple semantic types. The first couple of rules return
 * various kinds of symbol table information. The rules for the
 * program statements return nodes in the abstract syntax tree.
 *
 * The %union declaration declares all the kinds of data that
 * can be return. %type declarations later on will specify which
 * rules return what.
 */

%union
{
    ASTNode                 *ast;
    Expression              *expression;
    ExpressionList          *expressionList;
    Statement               *statement;
    StatementList           *statementList;
    Condition               *condition;
    ArrayReference          *aref;
    FunctionCall            *call;
    LeftValue               *lvalue;
    ElseIfList              *elseIfList;

    VariableInformation     *variable;
    TypeInformation         *type;
    FunctionInformation     *function;

    string                  *id;
    int                      integer;
    double                   real;
    void                    *null;
}

%type <expression>      expression term factor base
%type <expressionList>  expressions expressionz
%type <statement>       ifstmt whilestmt returnstmt callstmt assignstmt
%type <statement>       statement
%type <statementList>   statements block elsepart
%type <condition>       condition
%type <aref>            aref
%type <call>            call
%type <lvalue>          lvalue
%type <type>            type
%type <id>              id
%type <integer>         integer
%type <real>            real
%type <function>        funcname
%type <variable>        variable
%type <elseIfList>      elseifpart

/*
 * Normally tokens would have semantic types, but we've decided to
 * use the yytext variable for communicating data from the scanner
 * to the parser, so in this parser, none of the tokens have a
 * semantic type.
 */

%token FUNCTION ID DECLARE ARRAY INTEGER OF REAL XBEGIN XEND IF THEN
%token ELSE WHILE DO ASSIGN RETURN GE LE EQ NE ARRAY TRUE FALSE PROGRAM
%token ELSEIF


/* --- Your code here ---
 *
 * For expressions and conditions you'll have to specify
 * precedence and associativity (unless you factor the
 * rules for expressions and conditions.) This is where
 * the precedence and associativity declarations go.
 */

%token AND OR NOT
/* --- End your code --- */

/*
 * Here we define the start symbol of the grammar. We could have done
 * without this, since the first rule in the grammar is a rule for
 * program, but declaring the start symbol is still good form.
 */

%start program

%%


/*
 * A program is simply a list of variables, functions and
 * a code block. Very similar to a function really.
 */

program     :   variables functions block ';'
            {
                if (errorCount == 0)
                {
                    currentFunction->SetBody($3);
                    /* currentFunction->GenerateCode(); */
                    std::cout << currentFunction;
                }
            }
            ;

/*
 * We use this rule for all variable declarations.
 * Although parameters look almost the same, they
 * behave differently, so it's practical to have
 * separate rules for them.
 */

variables   :   DECLARE declarations
            |   error declarations
            |   /* Empty */
            ;

declarations:   declarations declaration
            |   declaration
            ;

declaration :   id ':' type ';'
            {
                if (currentFunction->OkToAddSymbol(*($1)))
                {
                    if ($3 != NULL)
                        currentFunction->AddVariable(*($1), $3);
                }
                else
                {
                    error() << *($1) << " is already declared\n" << std::flush;
                }
            }
            | functions
            |   error ';'
            {
                yyerrok;
            }
            ;


/*
 * This rule represents a list of functions. It uses the
 * function production which is expected to return a
 * pointer to something of type Function.
 */

functions   :   functions function
            |   /* Empty */
            ;


/* --- Your code here ---
 *
 * Write the function production. Take care to enter and exit
 * scope correctly. You'll need to understand how shift-reduce
 * parsing works and when actions are run to do this.
 *
 * Solutions that rely on shift-time actions will not be
 * acceptable. You should be able to solve the problem
 * using actions at reduce time only.
 *
 * In lab 4 you also need to generate code for functions after parsing
 * them. Just calling GeneratCode in the function should do the trick.
 */

function : FUNCTION id
  {
    FunctionInformation* newFunction = new FunctionInformation(*($2));

    newFunction->SetParent(currentFunction);
    currentFunction->AddFunction(*($2), newFunction);
    currentFunction = newFunction;
  }
  parameters ':' type DECLARE declarations block ';'
	{
    currentFunction->SetReturnType($6);
    currentFunction->SetBody($9);
    std::cout << currentFunction << std::endl;
    currentFunction = currentFunction->GetParent();
	}
	;

/* --- End your code --- */


/*
 * Parameter lists are defined by the following three
 * productions. Most of the work is done by the AddParameter
 * function in the Function class
 */

parameters  :   '(' paramlist ')'
            |   '(' error ')'
            |   /* Empty */
            ;

paramlist   :   paramlist ';' parameter
            |   parameter
            ;

parameter   :   id ':' type
            {
                if (currentFunction->OkToAddSymbol(*($1)))
                {
                    currentFunction->AddParameter(*($1), $3);
                }
                else
                {
                    error() << *($1) << " already defined\n" << std::flush;
                    currentFunction->AddParameter(*($1), $3);
                }
            }
            ;

/*
 * Types are rather important. We have two different types.
 * First there are the primitive types, integer and real. Then
 * we have arrays.
 *
 * We want types to be considered equivalent if they have the
 * same definition. We do this by creating temporary types for
 * arrays and giving them names that encode all the information
 * in the type. We do this my concatenating the element type
 * with the dimension in angle brackets, e.g. INTEGER<10>. This
 * is safe since such names are not legal in the source code.
 *
 */

type        :   id
            {
                SymbolInformation       *info;
                TypeInformation         *typeInfo;

                info = currentFunction->LookupIdentifier(*($1));
                if (info == NULL)
                {
                    error() << "undefined type " << *($1) << "\n" << std::flush;
                    $$ = NULL;
                }
                else
                {
                    typeInfo = info->SymbolAsType();

                    if (typeInfo == NULL)
                    {
                        error() << *($1) << " is not a type" << "\n" <<std::flush;
                        $$ = NULL;
                    }
                    else
                    {
                        $$ = typeInfo;
                    }
                }
            }
            |   ARRAY integer OF type
            {
                if ($4 == NULL)
                {
                    error() << "can't create arrays of invalid tpyes\n"
                            << std::flush;
                    $$ = NULL;
                }
                else if ($4->elementType != NULL)
                {
                    error() << "can't do arrays of arrays\n" << std::flush;
                    $$ = NULL;
                }
                else
                {
                    $$ = currentFunction->AddArrayType($4, $2);
                }
            }
            ;



/*
 * The rest of the grammar deals with statements and such
 */


block       :   XBEGIN statements XEND
            {
                $$ = $2;
            }
            ;

statements  :   statements statement
            {
                if ($2 == NULL)
                    $$ = NULL;
                else
                    $$ = new StatementList($1, $2)
            }
            |   /* Empty */
            {
                $$ = NULL;
            }
            ;

statement   :   ifstmt ';'
            |   assignstmt ';'
            |   callstmt ';'
            |   returnstmt ';'
            |   whilestmt ';'
            |   error ';' { yyerrok; $$ = NULL; }
            ;


ifstmt      :   IF condition THEN block elseifpart elsepart
            {
                if ($2 == NULL || $4 == NULL)
                    $$ = NULL;
                else
                    $$ = new IfStatement($2, $4, $5, $6);
            }
            ;


elseifpart  :   elseifpart ELSEIF condition THEN block
            {
                if ($3 == NULL || $5 == NULL)
                    $$ = NULL;
                else
                    $$ = new ElseIfList($1, $3, $5);
            }
            |   /* Empty */
            {
                $$ = NULL;
            }
            ;


elsepart    :   ELSE block IF
            {
                $$ = $2;
            }
            |   IF
            {
                $$ = NULL;
            }
            ;


assignstmt  :   lvalue ASSIGN expression
            {
                LeftValue       *left;
                Expression      *right;

                left = $1;
                right = $3;
                if (left == NULL || right == NULL)
                {
                    $$ = NULL;
                }
                else if (!CheckAssignmentTypes(&left, &right))
                {
                    error() << "Incompatible types in assignment.\n"
                            << std::flush;
                    $$ = NULL;
                }
                else
                {
                    $$ = new Assignment(left, right);
                }
            }
            ;


callstmt    :   call
            {
                if ($1 == NULL)
                    $$ = NULL;
                else
                    $$ = new CallStatement($1);
            }
            ;


returnstmt  :   RETURN expression
            {
                if ($2 == NULL)
                    $$ = NULL;
                else
                {
                    Expression      *expr;

                    expr = $2;
                    if (!CheckReturnType(&expr,
                                         currentFunction->GetReturnType()))
                    {
                        error() << "incompatible return type in "
                                << currentFunction->id << '\n';
                        error() << "  attempt to return "
                                << ShortSymbols << expr->valueType << '\n';
                        error() << " in function declared to return "
                                << ShortSymbols
                                << currentFunction->GetReturnType()
                                << LongSymbols << '\n';
                        $$ = NULL;
                    }
                    else
                    {
                        $$ = new ReturnStatement(expr);
                    }
                }
            }
            ;


whilestmt   :   WHILE condition DO block WHILE
            {
                if ($2 == NULL || $4 == NULL)
                    $$ = NULL;
                else
                    $$ = new WhileStatement($2, $4);
            }
            ;


lvalue      :   variable
            {
                if ($1 == NULL)
                    $$ = NULL;
                else
                    $$ = new Identifier($1);
            }
            |   aref
            {
                $$ = $1;
            }
            ;


variable    :   id
            {
                SymbolInformation       *info;
                VariableInformation     *varInfo;

                info = currentFunction->LookupIdentifier(*($1));
                if (info == NULL)
                {
                    error()
                        << "undeclared variable: "
                        << *($1)
                        << "\n"
                        << std::flush;

                    $$ = NULL;
                }
                else
                {
                    varInfo = info->SymbolAsVariable();

                    if (varInfo == NULL)
                    {
                        error()
                            << "identifier "
                            << *($1)
                            << " is not a variable\n"
                            << std::flush;
                        $$ = NULL;
                    }
                    else
                    {
                        $$ = varInfo;
                    }
                }
            }


funcname    :   id
            {
                SymbolInformation       *info;
                FunctionInformation     *funcInfo;

                info = currentFunction->LookupIdentifier(*($1));
                if (info == NULL)
                {
                    error() << *($1) << " is not defined\n" << std::flush;
                    $$ = NULL;
                }
                else
                {
                    funcInfo = info->SymbolAsFunction();

                    if (funcInfo == NULL)
                    {
                        error() << *($1) << " is not a function\n" << std::flush;
                        $$ = NULL;
                    }
                    else
                    {
                        $$ = funcInfo;
                    }
                }
            }


aref        :   variable '[' expression ']'
            {
                if ($1 == NULL || $3 == NULL)
                    $$ = NULL;
                else
                    $$ = new ArrayReference($1, $3);
            }
            |   variable '[' error ']'
            {
                $$ = NULL;
            }
            ;


call        :   funcname '(' expressions ')'
            {
                if ($1 == NULL)
                    $$ = NULL;
                else
                {
                    if (CheckFunctionParameters($1, $1->GetLastParam(), $3))
                    {
                        $$ = new FunctionCall($1, $3);
                    }
                    else
                    {
                        $$ = NULL;
                    }
                }
            }
            |   funcname '(' error ')'
            {
                $$ = NULL;
            }
            ;


id          :   ID
            {
                $$ = new string(yytext);
            }
            ;


integer     :   INTEGER
            {
                $$ = atoi(yytext);
            }
            ;


real        :   REAL
            {
                $$ = atof(yytext);
            }
            ;

/* --- Your code here ---
 *
 * Insert the expression grammar here
 * The start symbol of the expression grammar is
 * expression. This is important since it's used
 * in a number of other places.
 *
 * Make sure that your code creates itor nodes in the
 * AST wherever necessary and that it only created
 * trees for expressions with compatible types!
 */

expression : term '+' expression { $$ = new Plus($1, $3);  }
           | term '-' expression { $$ = new Minus($1, $3); }
           | term
           ;

term       : factor '*' expression { $$ = new Times($1, $3);  }
           | factor '/' expression { $$ = new Divide($1, $3); }
           | factor
           ;

factor     : expression '^' base { $$ = new Power($1, $3); }
           | base
           ;
base       : '-' expression { $$ = new UnaryMinus($2); }
           | id
           {
              SymbolInformation* symbol = currentFunction->LookupIdentifier(*($1));
              if(symbol != NULL) {
                $$ = new Identifier(symbol->SymbolAsVariable());
              } else {
                error() << "Unable to find variable: " << *($1) << std::endl;
              }
           }
           | integer { $$ = new IntegerConstant($1) }
           | real { $$ = new RealConstant($1) }
           | call
           ;

/* --- End your code --- */


expressions : expressionz
            {
                $$ = $1;
            }
            | /* Empty */
            {
                $$ = NULL;
            }
            ;


expressionz : expressionz ',' expression
            {
                if ($3 == NULL)
                    $$ = NULL;
                else
                    $$ = new ExpressionList($1, $3);
            }
            | expression
            {
                if ($1 == NULL)
                    $$ = NULL;
                else
                    $$ = new ExpressionList(NULL, $1);
            }
            ;


/* --- Your code here ---
 *
 * Insert the condition grammar here
 * The start symbol is condition. It's used
 * elsewhere, so make sure you get it right.
 */

condition : expression GE expression { $$ = new GreaterThanOrEqual($1, $3);   }
          | expression LE expression { $$ = new LessThanOrEqual($1, $3);      }
          | expression '>' expression { $$ = new GreaterThan($1, $3); }
          | expression '<' expression { $$ = new LessThan($1, $3); }
          | expression EQ expression { $$ = new Equal($1, $3); }
          | expression NE expression { $$ = new NotEqual($1, $3); }
          ;

/* --- End your code --- */


%%

int errorCount = 0;
int warningCount = 0;


/* --- Your code here ---
 *
 * Insert utility functions that you think you need here.
 */

/* It is reasonable to believe that you will need a function
 * that checks that two expressions are of compatible types,
 * and if possible makes a type conversion.
 * For your convenience a skeleton for such a function is
 * provided below. It will be very similar to CheckAssignmentTypes.
 */

/*
 * CheckCompatibleTypes checks that the expressions indirectly pointed
 * to by left and right are compatible. If type conversion is
 * necessary, the pointers left and right point to will be modified to
 * point to the node representing type conversion. That's why you have
 * to pass a pointer to pointer to Expression in these arguments.
 */

char CheckCompatibleTypes(Expression **left, Expression **right)
{
    return 0;
}

/* --- End your code --- */


/*
 * CheckAssignmentTypes is similar to CheckCompatibleTypes, but in
 * this case left is never modified since it represents an lvalue.
 */

char CheckAssignmentTypes(LeftValue **left, Expression **right)
{
    if (*left == NULL || *right == NULL)
        return 1;

    if ((*left)->valueType == (*right)->valueType)
    {
        return 1;
    }
    if ((*left)->valueType == kRealType && (*right)->valueType == kRealType)
    {
        return 1;
    }
    if ((*left)->valueType == kIntegerType &&
        (*right)->valueType == kIntegerType)
    {
        return 1;
    }
    if ((*left)->valueType == kIntegerType && (*right)->valueType == kRealType)
    {
        *right = new TruncateReal(*right);
        return 1;
    }
    if ((*left)->valueType == kRealType && (*right)->valueType == kIntegerType)
    {
        *right = new IntegerToReal(*right);
        return 1;
    }

    return 0;
}


/*
 * CheckFunctionParameters is used to check parameters passed to a
 * function. func is the function we're passing parameters to, formals
 * is a pointer to the last formal parameter we're checking against
 * and params is a pointer to the ExpressionList we're checking. If
 * type conversion is necessary, the Expressions pointed to by the
 * ExpressionList will be modified accordingly.
 *
 * This function prints it's own error messages.
 */

char CheckFunctionParameters(FunctionInformation *func,
                             VariableInformation *formals,
                             ExpressionList      *params)
{
    if (formals == NULL && params == NULL)
    {
        return 1;
    }
    else if (formals == NULL && params != NULL)
    {
        error() << "too many arguments in call to " << func->id << '\n'
                << std::flush;
        return 0;
    }
    else if (formals != NULL && params == NULL)
    {
        error() << "too few arguments in call to " << func->id << '\n'
                << std::flush;
        return 0;
    }
    else
    {
        if (CheckFunctionParameters(func, formals->prev,
                                    params->precedingExpressions))
        {
            if (formals->type == params->expression->valueType)
            {
                return 1;
            }
            else if (formals->type == kIntegerType &&
                     params->expression->valueType == kRealType)
            {
                params->expression = new TruncateReal(params->expression);
                return 1;
            }
            else if (formals->type == kRealType &&
                     params->expression->valueType == kIntegerType)
            {
                params->expression = new IntegerToReal(params->expression);
                return 1;
            }
            else
            {
                error() << "incompatible types in call to "
                        << func->id
                        << '\n'
                        << std::flush;
                error() << "  parameter "
                        << formals->id
                        << " was declared "
                        << ShortSymbols
                        << formals->type
                        << '\n'
                        << std::flush;
                error() << "  argument was of type "
                        << params->expression->valueType
                        << '\n'
                        << LongSymbols << std::flush;
                return 0;
            }
        }
    }
}


char CheckReturnType(Expression **expr, TypeInformation *info)
{
    if (info == NULL || *expr == NULL)
        return 1;

    if ((*expr)->valueType == info)
        return 1;

    if ((*expr)->valueType == kIntegerType && info == kRealType)
    {
        *expr = new IntegerToReal(*expr);
        return 1;
    }

    if ((*expr)->valueType == kRealType && info == kIntegerType)
    {
        *expr = new TruncateReal(*expr);
        return 1;
    }

    return 0;
}


void yyerror(char *message)
{
    error() << message << '\n' << std::flush;
}

std::ostream& error(void)
{
    errorCount += 1;
    return std::cerr << yylineno << " Error: ";
}

std::ostream& warning(void)
{
    warningCount += 1;
    return std::cerr << yylineno << " Warning: ";
}
