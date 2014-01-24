#include <iostream>
#include <iomanip>

#include <ast.hh>
#include <symtab.hh>
#include <codegen.hh>


long QuadsList::labelCounter;

#define USEQ { QuadsList *xyzzy = &q; xyzzy=xyzzy; }


/*
 * ASTNode::GenerateCodeAndJump
 *
 * GenerateCodeAndJump is supposed to generate quads for part of the
 * parse tree, but ensure that all exits from the code end up at a
 * particular label. This is easy to do for most parts of the parse
 * tree: just generate the code in the usual manner, then jump to the
 * label.
 */


VariableInformation *ASTNode::GenerateCodeAndJump(QuadsList& q,
                                                  long label)
{
    VariableInformation *info;

    info = this->GenerateCode(q);
    q += new Quad(jump, label,
                  (SymbolInformation *)NULL, (SymbolInformation *)NULL);

    return info;
}


/* ElseIfStatement::GenerateCodeAndJump
 *
 * Write the function that generates code for the elseif branches of
 * an if statement. There are a couple of things to watch out for.
 * The conditions must be tested in the correct order, or the program
 * might misbehave. Once a condition tests true, do not test any more
 * conditions (that's why we use GenerateCodeAndJump for this type
 * of statement.)
 */

VariableInformation *ElseIfList::GenerateCodeAndJump(QuadsList &q,
                                                     long lbl)
{
    long                     next;
    VariableInformation     *info;

    /* --- Your code here --- */

    /* --- End your code --- */

    return NULL;

}


/*
 * ArrayReference::GenerateAssignment
 *
 * Generate assignment code for an array reference. See the comment
 * before Assignment::GenerateCode for more information.
 */

void ArrayReference::GenerateAssignment(QuadsList& q,
                                        VariableInformation *val)
{
  /* --- Your code here --- */
  if(id->type->elementType == kIntegerType) {
    q += new Quad(iassign, val, NULL, id);
  } else if(id->type->elementType == kRealType) {
    q += new Quad(rassign, val, NULL, id);
  }
  /* --- End your code --- */
}

/*
 * Identifier::GenerateAssignment
 *
 * Generate assignment code for an identifier. See the comment
 * before Assignment::GenerateCode for more information.
 */

void Identifier::GenerateAssignment(QuadsList& q, VariableInformation *val)
{
    if (val->type == NULL || id->type == NULL)
    {
        std::cerr << "Bug: you created an untyped variable.\n";
        abort();
    }
    if (id->type == kIntegerType)
    {
        q += new Quad(iassign,
		      dynamic_cast<SymbolInformation*>(val),
		      static_cast<SymbolInformation*>(NULL),
		      dynamic_cast<SymbolInformation*>(id));
    }
    else if (id->type == kRealType)
    {
        q += new Quad(rassign,
		      dynamic_cast<SymbolInformation*>(val),
		      static_cast<SymbolInformation*>(NULL),
		      dynamic_cast<SymbolInformation*>(id));
    }
    else if (id->type == val->type)
    {
        q += new Quad(aassign, val, val->type->arrayDimensions, id);
    }
}


/*
 * StatementList::GenerateCode
 *
 * Generate code for a list of statements. Make sure the code comes
 * out in the right order.
 *
 * Note: The impelementation here is absolutely ridiculous since it
 * uses stack space proportional to the number of statements in the
 * list. A more sensible definition would simply iterate down the
 * list. This code is an example of what happens when someone with a
 * degree in computer science is allowed to write code without regard
 * for efficiency...
 */

VariableInformation *StatementList::GenerateCode(QuadsList &q)
{
    if (precedingStatements != NULL)
    {
        precedingStatements->GenerateCode(q);
    }
    return statement->GenerateCode(q);
}

void GenerateConditionalStatements(QuadsList& q, Condition* condition, StatementList* statements, long endLabel) {
  long endStatementsLabel = q.NextLabel();
  VariableInformation* cond = condition->GenerateCode(q);

  q += new Quad(jfalse, endStatementsLabel, cond, NULL);
  statements->GenerateCode(q);
  q += new Quad(jump, endLabel, NULL, NULL);
  q += new Quad(clabel, endStatementsLabel, NULL, NULL);
}



void GenerateElseIfCode(QuadsList& q, long endLabel, ElseIfList* elseIf) {
  if(elseIf == NULL) {
    return;
  }

  GenerateElseIfCode(q, endLabel, elseIf->preceding);
  GenerateConditionalStatements(q, elseIf->condition, elseIf->body, endLabel);
}


/*
 * IfStatement::GenerateCode
 *
 * If statements, particularly those with elseif branches are actually
 * quite involved. They tend to use a lot of jumps. For the elseif
 * branches you'll probably want to use the GenerateCodeAndJump method
 * of ElseIfStatement (which you're also supposed to write.)
 */

VariableInformation *IfStatement::GenerateCode(QuadsList& q)
{
  /* --- Your code here ---*/
  long endIfLabel = q.NextLabel();
  VariableInformation* cond = condition->GenerateCode(q);

  GenerateConditionalStatements(q, condition, thenStatements, endIfLabel);
  GenerateElseIfCode(q, endIfLabel, elseIfList);

  elseStatements->GenerateCode(q);
  q += new Quad(clabel, endIfLabel, NULL, NULL);

  /* --- End your code --- */
  return NULL;;
}


/*
 * ElseIfList::GenerateCode
 *
 * You might figure out a way to write IfStatement::GenerateCode
 * without relying on ElseIfStatement::GenerateCodeAndJump. If so,
 * this method is where you should generate the code for the elseif
 * part. Just remove the error message and call to abort.
 */

VariableInformation *ElseIfList::GenerateCode(QuadsList& q)
{
  USEQ;
  /* --- Your code here --- */
  /* --- End your code --- */

  return NULL;
}


/*
 * WhileStatement::GenerateCode
 *
 * Generate code for a while statement. This is pretty easy.
 * Insert a label "entry", generate code for the condition, jump to
 * label "exit" if the condition is false, insert the body and jump
 * back to "entry". Finally insert the label "exit".
 */

VariableInformation *WhileStatement::GenerateCode(QuadsList &q)
{
    long                 loopLabel, endLabel;
    VariableInformation *info;

    loopLabel = q.NextLabel();
    endLabel = q.NextLabel();
    q += new Quad(clabel, loopLabel, NULL, NULL);
    info = condition->GenerateCode(q);
    q += new Quad(jfalse, endLabel, info, NULL);
    body->GenerateCodeAndJump(q, loopLabel);
    q += new Quad(clabel, endLabel, NULL, NULL);

    return NULL;
}


/*
 * IntegerConstant::GenerateCode
 * RealConstant::GenerateCode
 * BooleanConstant::GenerateCode
 *
 * Simply uses the iconst/rconst quads to load the constant (which is
 * stored in the parse tree into a temporary register.
 */

VariableInformation *IntegerConstant::GenerateCode(QuadsList& q)
{
    VariableInformation *info =
        currentFunction->TemporaryVariable(kIntegerType);

    q += new Quad(iconst, value, NULL, info);
    return info;
}

VariableInformation *RealConstant::GenerateCode(QuadsList& q)
{
    VariableInformation *info =
        currentFunction->TemporaryVariable(kRealType);

    q += new Quad(rconst, value, NULL, info);
    return info;
}

VariableInformation *BooleanConstant::GenerateCode(QuadsList& q)
{
    VariableInformation *info =
        currentFunction->TemporaryVariable(kIntegerType);

    q += new Quad(iconst, value ? 1L : 0L, NULL, info);
    return info;
}


/*
 * ArrayReference::GenerateCode
 *
 * Arrays are stored in memory, but the address to the first element
 * in the array is stored in a variable (the id instance variable in
 * the ArrayReference object.) Since we don't have a quad to do this
 * for us, you'll have to use lower-level operations. Hint: the size
 * field in the TypeInformation class will come in handy.
 */

VariableInformation *ArrayReference::GenerateCode(QuadsList& q)
{
  /* --- Your code here --- */
  VariableInformation* base =
    currentFunction->TemporaryVariable(kIntegerType);
  VariableInformation* address =
    currentFunction->TemporaryVariable(kIntegerType);
  VariableInformation* variable =
    currentFunction->TemporaryVariable(id->type->elementType);
  VariableInformation* offset = index->GenerateCode(q);


  q += new Quad(iaddr, id, NULL, base);
  q += new Quad(iadd, base, offset, address);

  if(variable->type == kIntegerType) {
    q += new Quad(iload, address, NULL, variable);
  } else if(variable->type == kRealType) {
    q += new Quad(rload, address, NULL, variable);
  }

  return variable;
  /* --- End your code --- */
}

/*
 * Identifier::GenerateCode
 *
 * Generate code to get the value of an identifier. Actually, we don't
 * need to generate any code at all. We just return the identifier.
 */

VariableInformation *Identifier::GenerateCode(QuadsList& q)
{
    USEQ;

    return id;
}

/*
 * ReturnStatement::GenerateCode
 *
 * Generate code for a return statement. This is really
 * straightforward.
 */

VariableInformation *ReturnStatement::GenerateCode(QuadsList &q)
{
    VariableInformation     *info;

    info = value->GenerateCode(q);
    if (info->type != currentFunction->GetReturnType())
    {
        std::cerr << "Bug: you forgot to typecheck return statements.\n";
        abort();
    }

    q += new Quad(creturn,
		  static_cast<SymbolInformation*>(NULL),
		  static_cast<SymbolInformation*>(NULL),
		  dynamic_cast<SymbolInformation*>(info));

    return NULL;
}


/*
 * ExpressionList::GenerateCode
 *
 * Whoa! If you call this, you're doing something very wrong!
 * Expression lists are currently only used in the parameter
 * list of a function call, but we might want to use them in
 * other places too, such as in an initializer for an array.
 */

VariableInformation *ExpressionList::GenerateCode(QuadsList& q)
{
    USEQ;

    std::cerr << "Bug: can't generate code for an ExpressionList.\n";
    abort();
}

void ExpressionList::GenerateParameterList(QuadsList &q,
                                           VariableInformation *lastParam)
{
    VariableInformation *info;

    if (lastParam == NULL ||
        (lastParam->prev != NULL && precedingExpressions == NULL))
    {
        std::cerr << "Bug: type checking of function params isn't good enough.\n";
        abort();
    }

    if (precedingExpressions)
        precedingExpressions->GenerateParameterList(q, lastParam->prev);

    info = expression->GenerateCode(q);


    if (expression->valueType == lastParam->type)
    {
        q += new Quad(param,
		      dynamic_cast<SymbolInformation*>(info),
		      static_cast<SymbolInformation*>(NULL),
		      static_cast<SymbolInformation*>(NULL));
    }
    else
    {
        std::cerr << "Bug: type checking of function params isn't good enough.\n";
        abort();
    }
}





/*
 * Assignment::GenerateCode
 *
 * Assignments are kind of strange. Normally code generation is done
 * by first generating code for all the children of a node, then
 * adding some code for the node itself. In an assignment we need to
 * do things a little differently. First we generate code for the
 * value, then, since we need to generate very different code
 * depending on what the LValue is, we let the GenerateAssignment
 * method in the LeftValue subclasses generate the actual assignment
 * code.
 */

VariableInformation *Assignment::GenerateCode(QuadsList& q)
{
    VariableInformation     *valueInfo;

    valueInfo = value->GenerateCode(q);
    target->GenerateAssignment(q, valueInfo);

    return NULL;
}

/*
 * IntegerToReal::GenerateCode
 * TruncateReal::GenerateCode
 *
 * This is so straightforward...
 */

VariableInformation *IntegerToReal::GenerateCode(QuadsList& q)
{
    VariableInformation *info, *valueInfo;

    if (value->valueType != kIntegerType)
    {
        std::cerr << "Bug: you're trying to convert a non-integer to a real.\n";
    }

    info = currentFunction->TemporaryVariable(kRealType);
    valueInfo = value->GenerateCode(q);
    q += new Quad(itor,
		  dynamic_cast<SymbolInformation*>(valueInfo),
		  static_cast<SymbolInformation*>(NULL),
		  dynamic_cast<SymbolInformation*>(info));

    return info;
}

VariableInformation *TruncateReal::GenerateCode(QuadsList& q)
{
    VariableInformation *info, *valueInfo;

    if (value->valueType != kRealType)
    {
        std::cerr << "Bug: you're trying to truncate a non-real.\n";
    }

    info = currentFunction->TemporaryVariable(kIntegerType);
    valueInfo = value->GenerateCode(q);
    q += new Quad(rtrunc,
		  dynamic_cast<SymbolInformation*>(valueInfo),
		  static_cast<SymbolInformation*>(NULL),
		  dynamic_cast<SymbolInformation*>(info));

    return info;
}

/*
 *
 * This function is used to generate code for all kinds of binary
 * operators and relations. The arguments to the function are the
 * following:
 *
 * q        The QuadsList onto which the generated code is placed.
 * realop   The quad to generate for the operator if the arguments
 *          are of type real.
 * intop    The quad to generate for the operator if the arguments
 *          are of type integer.
 * left     The AST for the left-hand side of the operator. The
 *          value type of this AST must be the same as that of the
 *          AST for the right-hand side.
 * right    The AST for the right-hand side of the operator. The
 *          value type of this AST must be the same as that of the
 *          AST for the left-hand side.
 * node     The operator node itself. You probably won't need this
 *          unless you want to print an error message.
 * type     If not NULL, this is the type of the result. This is
 *          used for relations, where the type of the result is
 *          always integer, even if the operands are real. If this
 *          parameter is NULL, then the type of the result is the
 *          same as the type of the operands (the parser needs to
 *          make sure that the operands have the same type.)
 *
 * See the GenerateCode methods for the binary operators for
 * examples of how this function is used.
 *
 */

static VariableInformation *BinaryGenerateCode(QuadsList& q,
                                               tQuadType realop,
                                               tQuadType intop,
                                               ASTNode *left,
                                               ASTNode *right,
                                               ASTNode *node,
                                               TypeInformation *type = NULL)
{
    VariableInformation *leftInfo, *rightInfo, *result;

    /* --- Your code here --- */

    /* --- End your code --- */
  return currentFunction->TemporaryVariable(kIntegerType);
}

/*
 * Plus::GenerateCode
 * Minus::GenerateCode
 * Divide::GenerateCode
 * Times::GenerateCode
 * Power::GenerateCode
 *
 * All these are very similar, so they share a common function defined
 * in this file.
 */

VariableInformation *Plus::GenerateCode(QuadsList& q)
{
    return BinaryGenerateCode(q, radd, iadd, left, right, this);
}

VariableInformation *Minus::GenerateCode(QuadsList& q)
{
    return BinaryGenerateCode(q, rsub, isub, left, right, this);
}

VariableInformation *Times::GenerateCode(QuadsList& q)
{
    return BinaryGenerateCode(q, rmul, imul, left, right, this);
}

VariableInformation *Divide::GenerateCode(QuadsList& q)
{
    return BinaryGenerateCode(q, rdiv, idiv, left, right, this);
}

VariableInformation *Power::GenerateCode(QuadsList& q)
{
    return BinaryGenerateCode(q, rpow, ipow, left, right, this);
}

/*
 * UnaryMinus::GenerateCode
 *
 * Generate code for the value expression, then subtract it from 0
 */

VariableInformation *UnaryMinus::GenerateCode(QuadsList& q)
{
    VariableInformation *info, *result, *constInfo;

    info = right->GenerateCode(q);
    result = currentFunction->TemporaryVariable(info->type);
    constInfo = currentFunction->TemporaryVariable(info->type);

    if (info->type == kIntegerType)
    {
        q += new Quad(iconst, 0L, (SymbolInformation*)NULL, constInfo);
        q += new Quad(isub, constInfo, info, result);
    }
    else if (info->type == kRealType)
    {
        q += new Quad(rconst, 0.0, NULL, constInfo);
        q += new Quad(rsub, constInfo, info, result);
    }
    else
    {
        std::cerr << "Bug: unary minus of a non-numeric type.\n";
        abort();
    }

    return result;
}


/*
 * LessThan::GenerateCode
 * LessThanOrEqual::GenerateCode
 * GreaterThan::GenerateCode
 * GreaterThanOrEqual::GenerateCode
 * Equal::GenerateCode
 * NotEqual::GenerateCode
 *
 * To generate code for these relations, we'll use the BinaryGenerateCode
 * function defined for binary operations. We'll also add some more
 * quads for some of the operations.
 */

VariableInformation *LessThan::GenerateCode(QuadsList& q)
{
    return BinaryGenerateCode(q, rlt, ilt, left, right, this, kIntegerType);
}

VariableInformation *GreaterThan::GenerateCode(QuadsList& q)
{
    return BinaryGenerateCode(q, rgt, igt, left, right, this, kIntegerType);
}

VariableInformation *Equal::GenerateCode(QuadsList& q)
{
    return BinaryGenerateCode(q, req, ieq, left, right, this, kIntegerType);
}


VariableInformation *LessThanOrEqual::GenerateCode(QuadsList& q)
{
    VariableInformation     *r0, *r1;

    r0 = BinaryGenerateCode(q, rlt, ilt, left, right, this, kIntegerType);
    r1 = BinaryGenerateCode(q, req, ieq, left, right, this, kIntegerType);
    q += new Quad(ior, r0, r1, r1);

    return r1;
}

VariableInformation *GreaterThanOrEqual::GenerateCode(QuadsList& q)
{
    VariableInformation     *r0, *r1;

    r0 = BinaryGenerateCode(q, rgt, igt, left, right, this, kIntegerType);
    r1 = BinaryGenerateCode(q, req, ieq, left, right, this, kIntegerType);
    q += new Quad(ior, r0, r1, r1);

    return r1;
}

VariableInformation *NotEqual::GenerateCode(QuadsList& q)
{
    VariableInformation *r0;

    r0 = BinaryGenerateCode(q, req, ieq, left, right, this, kIntegerType);
    q += new Quad(inot,
		  dynamic_cast<SymbolInformation*>(r0),
		  static_cast<SymbolInformation*>(NULL),
		  dynamic_cast<SymbolInformation*>(r0));
    return r0;
}


/*
 * And::GenerateCode
 * Or::GenerateCode
 * Not::Generatecode
 *
 * The current versions of these functions do not short-circuit the
 * logical connectives and and or. Rewrite them so that the test
 * is aborted as soon as the result is known (for or, as soon as one
 * of the conditions evaluates to true, and for and as soon as one
 * condition evaluates to false.)
 */


VariableInformation *And::GenerateCode(QuadsList& q)
{
    return BinaryGenerateCode(q, hcf, iand, left, right, this, kIntegerType);
}

VariableInformation *Or::GenerateCode(QuadsList& q)
{
    return BinaryGenerateCode(q, hcf, ior, left, right, this, kIntegerType);
}

VariableInformation *Not::GenerateCode(QuadsList& q)
{
    VariableInformation *info, *result;

    info = right->GenerateCode(q);
    if (info->type != kIntegerType)
    {
        std::cerr << "Bug: not operator applied to a non-integer.\n";
        abort();
    }

    result = currentFunction->TemporaryVariable(kIntegerType);
    q += new Quad(inot,
		  dynamic_cast<SymbolInformation*>(info),
		  static_cast<SymbolInformation*>(NULL),
		  dynamic_cast<SymbolInformation*>(result));

    return result;
}

/*
 * CallStatement::GenerateCode
 * FunctionCall::GenerateCode
 *
 * Generate code for the parameter list by iterating over the
 * actual arguments, then generate the function call itself.
 */

VariableInformation *CallStatement::GenerateCode(QuadsList &q)
{
    return call->GenerateCode(q);
}

VariableInformation *FunctionCall::GenerateCode(QuadsList& q)
{
    VariableInformation *info;

    if (arguments)
        arguments->GenerateParameterList(q, function->GetLastParam());
    info = currentFunction->TemporaryVariable(function->GetReturnType());
    q += new Quad(call,
		  dynamic_cast<SymbolInformation*>(function),
		  static_cast<SymbolInformation*>(NULL),
		  dynamic_cast<SymbolInformation*>(info));

    return info;
}


/* ======================================================================
 * Quads and Quads Lists
 */

QuadsList& QuadsList::operator+=(Quad *q)
{
    if (head == NULL)
    {
        head = new QuadsListElement(q, NULL);
        tail = head;
    }
    else
    {
        tail->next = new QuadsListElement(q, NULL);
        tail = tail->next;
    }

    return *this;
}

std::ostream& QuadsList::print(std::ostream& o)
{
    QuadsListElement        *elem;

    o << "    QuadsList @ " << (void *)this << "\n";
    o << ShortSymbols;

    elem = head;
    while (elem)
    {
        o << elem->data << '\n';
        elem = elem->next;
    }

    o << LongSymbols;
    return o;
}

std::ostream& Quad::print(std::ostream& o)
{
    o << "    ";
    switch(opcode)
    {
    case iconst:
        o << std::setw(8) << "iconst  "
          << std::setw(8) << int1
          << std::setw(8) <<"-"
          << std::setw(8) <<sym3;
        break;
    case rconst:
        o << std::setw(8) <<"rconst  "
          << std::setw(8) <<real1
          << std::setw(8) <<"-"
          << std::setw(8) <<sym3;
        break;
    case iaddr:
        o << std::setw(8) <<"iaddr   "
          << std::setw(8) <<sym1
          << std::setw(8) <<"-"
          << std::setw(8) <<sym3;
        break;
    case itor:
        o << std::setw(8) <<"itor    "
          << std::setw(8) <<sym1
          << std::setw(8) <<"-"
          << std::setw(8) <<sym3;
        break;
    case rtrunc:
        o << std::setw(8) <<"rtrunc  "
          << std::setw(8) <<sym1
          << std::setw(8) <<"-"
          << std::setw(8) <<sym3;
        break;
    case iadd:
        o << std::setw(8) << "iadd    "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case isub:
        o << std::setw(8) << "isub    "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case imul:
        o << std::setw(8) << "imul    "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case idiv:
        o << std::setw(8) << "idiv    "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case ipow:
        o << std::setw(8) << "ipow    "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case radd:
        o << std::setw(8) << "radd    "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case rsub:
        o << std::setw(8) << "rsub    "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case rmul:
        o << std::setw(8) << "rmul    "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case rdiv:
        o << std::setw(8) << "rdiv    "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case rpow:
        o << std::setw(8) << "rpow    "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case igt:
        o << std::setw(8) << "igt     "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case ilt:
        o << std::setw(8) << "ilt     "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case ieq:
        o << std::setw(8) << "ieq     "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case rgt:
        o << std::setw(8) << "rgt     "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case rlt:
        o << std::setw(8) << "rlt     "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case req:
        o << std::setw(8) << "req     "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case iand:
        o << std::setw(8) << "iand    "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case ior:
        o << std::setw(8) << "ior     "
          << std::setw(8) << sym1
          << std::setw(8) << sym2
          << std::setw(8) << sym3;
        break;
    case inot:
        o << std::setw(8) << "inot    "
          << std::setw(8) << sym1
          << std::setw(8) << "-"
          << std::setw(8) << sym3;
        break;
    case jtrue:
        o << std::setw(8) << "jtrue   "
          << std::setw(8) << int1
          << std::setw(8) << sym2
          << std::setw(8) << "-";
        break;
    case jfalse:
        o << std::setw(8) << "jfalse  "
          << std::setw(8) << int1
          << std::setw(8) << sym2
          << std::setw(8) << "-";
        break;
    case jump:
        o << std::setw(8) << "jump    "
          << std::setw(8) << int1
          << std::setw(8) << "-"
          << std::setw(8) << "-";
        break;
    case clabel:
        o << std::setw(8) << "clabel  "
          << std::setw(8) << int1
          << std::setw(8) << "-"
          << std::setw(8) << "-";
        break;
    case istore:
        o << std::setw(8) << "istore  "
          << std::setw(8) << sym1
          << std::setw(8) << "-"
          << std::setw(8) << sym3;
        break;
    case iload:
        o << std::setw(8) << "iload   "
          << std::setw(8) << sym1
          << std::setw(8) << "-"
          << std::setw(8) << sym3;
        break;
    case rstore:
        o << std::setw(8) << "rstore  "
          << std::setw(8) << sym1
          << std::setw(8) << "-"
          << std::setw(8) << sym3;
        break;
    case rload:
        o << std::setw(8) << "rload   "
          << std::setw(8) << sym1
          << std::setw(8) << "-"
          << std::setw(8) << sym3;
        break;
    case creturn:
        o << std::setw(8) << "creturn "
          << std::setw(8) << "-"
          << std::setw(8) << "-"
          << std::setw(8) << sym3;
        break;
    case param:
        o << std::setw(8) << "param   "
          << std::setw(8) << sym1
          << std::setw(8) << "-"
          << std::setw(8) << "-";
        break;
    case call:
        o << std::setw(8) << "call    "
          << std::setw(8) << sym1
          << std::setw(8) << "-"
          << std::setw(8) << sym3;
        break;
    case iassign:
        o << std::setw(8) << "iassign "
          << std::setw(8) << sym1
          << std::setw(8) << "-"
          << std::setw(8) << sym3;
        break;
    case rassign:
        o << std::setw(8) << "rassign "
          << std::setw(8) << sym1
          << std::setw(8) << "-"
          << std::setw(8) << sym3;
        break;
    case aassign:
        o << std::setw(8) << "aassign "
          << std::setw(8) << sym1
          << std::setw(8) << int1
          << std::setw(8) << sym3;
        break;
    case hcf:
        o << std::setw(8) << "hcf     "
          << std::setw(8) << "-"
          << std::setw(8) << "-"
          << std::setw(8) << "-";
        break;
    case nop:
        o << std::setw(8) << "nop     "
          << std::setw(8) << "-"
          << std::setw(8) << "-"
          << std::setw(8) << "-";
        break;
    default:
        o << "unknown (" << opcode << ")";
        break;
    }

    return o;
}


std::ostream& operator<<(std::ostream& o, QuadsList *q)
{
    if (q != NULL)
        return q->print(o);
    else
        return o << "    QuadsList @ 0x0\n";
}

std::ostream& operator<<(std::ostream& o, QuadsList& q)
{
    return q.print(o);
}

std::ostream& operator<<(std::ostream& o, Quad *q)
{
    if (q != NULL)
        return q->print(o);
    else
        return o << "    Quad @ 0x0";
}

std::ostream& operator<<(std::ostream& o, Quad& q)
{
    return q.print(o);
}
