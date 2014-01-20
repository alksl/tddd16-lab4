#include "ast.hh"


int  ASTNode::indentLevel = 0;
bool ASTNode::branches[10000];


void ASTNode::beginChild(ostream& o)
{
    //    indent(o);
    //    o << "|\n";
    indent(o);
    o << "+-";
    branches[indentLevel] = true;
    indentMore();
}

void ASTNode::endChild(ostream& o)
{
    o << "";                    // Suppress warning
    indentLess();
    if (branches[indentLevel])
    {
        branches[indentLevel] = false;
    }
    else
    {
        //        o << '\n';
        //        indent(o);
    }
}

void ASTNode::lastChild(ostream& o)
{
    //    indent(o);
    //    o << "|\n";
    indent(o);
    o << "+-";
    branches[indentLevel] = false;
    indentMore();
}

void ASTNode::print(ostream& o)
{
    o << "ASTNode";
}

void ASTNode::xprint(ostream& o, char *cls)
{
    o << "ASTNode (" << cls << ")";
}

void ASTNode::indent(ostream& o)
{
    int i;

    for (i = 0; i < indentLevel; i++)
    {
        if (branches[i])
            o << '|';
        else
            o << ' ';
    }
}

void ASTNode::indentMore(void)
{
    indentLevel += 2;
}

void ASTNode::indentLess(void)
{
    indentLevel -= 2;
}

void StatementList::print(ostream& o)
{
    o << "StatementList (statement, preceding)\n";
    beginChild(o);
    o << statement << '\n';
    endChild(o);
    lastChild(o);
    o << precedingStatements;
    endChild(o);
}

void Statement::print(ostream& o)
{
    o << "Statement";
}

void ElseIfList::print(ostream& o)
{
    o << "ElseIfList (preceding, condition, body)\n";
    beginChild(o);
    o << preceding << '\n';
    endChild(o);
    beginChild(o);
    o << condition << '\n';
    endChild(o);
    lastChild(o);
    o << body;
    endChild(o);
}

void IfStatement::print(ostream& o)
{
    o << "IfStatement (condition, then, elseif, else)\n";
    beginChild(o);
    o << condition << '\n';
    endChild(o);
    beginChild(o);
    o << thenStatements << '\n';
    endChild(o);
    beginChild(o);
    o << elseIfList << '\n';
    endChild(o);
    lastChild(o);
    o << elseStatements;
    endChild(o);
}

void Assignment::print(ostream& o)
{
    o << "Assignment (left, right)\n";
    beginChild(o);
    o << target << '\n';
    endChild(o);
    lastChild(o);
    o << value;
    endChild(o);
}

void CallStatement::print(ostream& o)
{
    o << "CallStatement (call)\n";
    lastChild(o);
    o << call;
    endChild(o);
}

void ReturnStatement::print(ostream& o)
{
    o << "ReturnStatement (value)\n";
    lastChild(o);
    o << value;
    endChild(o);
}

void WhileStatement::print(ostream& o)
{
    o << "WhileStatement (condition, body)\n";
    beginChild(o);
    o << condition << '\n';
    endChild(o);
    lastChild(o);
    o << body;
    endChild(o);
}

void Expression::print(ostream& o)
{
    o << "Expression ["
      << ShortSymbols << valueType << LongSymbols
      << "]\n";
}

void ExpressionList::print(ostream& o)
{
    o << "ExpressionList (expression, preceding)\n";
    beginChild(o);
    o << expression << '\n';
    endChild(o);
    lastChild(o);
    o << precedingExpressions;
    endChild(o);
}

void FunctionCall::print(ostream& o)
{
    o << "FunctionCall (function, arguments) ["
      << ShortSymbols << valueType << LongSymbols
      << "]\n";
    beginChild(o);
    o << ShortSymbols <<function << LongSymbols << '\n';
    endChild(o);
    lastChild(o);
    o << arguments;
    endChild(o);
}

void IntegerToReal::print(ostream& o)
{
    o << "IntegerToReal (value) ["
      << ShortSymbols << valueType << LongSymbols
      << "]\n";
    lastChild(o);
    o << value;
    endChild(o);
}

void TruncateReal::print(ostream& o)
{
    o << "TruncateReal (value) ["
      << ShortSymbols << valueType
      << "]\n";
    lastChild(o);
    o << value;
    endChild(o);
}

void IntegerConstant::print(ostream& o)
{
    o << "IntegerConstant (" << value << ") ["
      << ShortSymbols << valueType << LongSymbols
      << "]";
}

void RealConstant::print(ostream& o)
{
    o << "RealConstant (" << value << ") ["
      << ShortSymbols << valueType << LongSymbols
      << "]";
}

void BinaryOperation::print(ostream& o)
{
    xprint(o, "BinaryOperation");
}

void BinaryOperation::xprint(ostream& o, char *cls)
{
    o << cls << " (left, right) ["
      << ShortSymbols << valueType << LongSymbols
      << "]\n";
    beginChild(o);
    o << left << '\n';
    endChild(o);
    lastChild(o);
    o << right;
    endChild(o);
}

void Plus::print(ostream& o) { xprint(o, "Plus"); } // 
void Minus::print(ostream& o) { xprint(o, "Minus"); }
void Times::print(ostream& o) { xprint(o, "Times"); }
void Divide::print(ostream& o) {xprint(o, "Divide"); }
void Power::print(ostream& o) { xprint(o, "Power"); }

void UnaryMinus::print(ostream& o)
{
    o << "UnaryMinus (right) ["
      << ShortSymbols << valueType << LongSymbols
      << "]\n";
    lastChild(o);
    o << right;
    endChild(o);
}

void LeftValue::print(ostream& o)
{
    o << "LeftValue";
}

void ArrayReference::print(ostream& o)
{
    o << "ArrayReference (id, index)\n";
    beginChild(o);
    o << ShortSymbols << id << LongSymbols << '\n';
    endChild(o);
    lastChild(o);
    o << index;
    endChild(o);
}

void Identifier::print(ostream& o)
{
    o << "Identifier (";
    if (id)
        o << ShortSymbols << id << LongSymbols;
    else
        o << (void*)id;
    o << ")";
}

void Condition::print(ostream& o)
{
    o << "Condition";
}

void BinaryRelation::print(ostream& o)
{
    xprint(o, "BinaryRelation");
}

void BinaryRelation::xprint(ostream& o, char *cls)
{
    o << cls << " (left, right)\n";
    beginChild(o);
    o << left << '\n';
    endChild(o);
    lastChild(o);
    o << right;
    endChild(o);
}

void LessThan::print(ostream& o) { xprint(o, "LessThan"); }
void GreaterThan::print(ostream& o) { xprint(o, "GreaterThan"); }
void LessThanOrEqual::print(ostream& o) { xprint(o, "LessThanOrEqual"); }
void GreaterThanOrEqual::print(ostream& o) { xprint(o, "GreaterThanOrEqual"); }
void Equal::print(ostream& o) { xprint(o, "Equal"); }
void NotEqual::print(ostream& o) { xprint(o, "NotEqual"); }

void BinaryCondition::print(ostream& o)
{
    xprint(o, "BinaryCondition");
}

void BinaryCondition::xprint(ostream& o, char *cls)
{
    o << cls << " (left, right)\n";
    beginChild(o);
    o << left << '\n';
    endChild(o);
    lastChild(o);
    o << right;
    endChild(o);
}

void And::print(ostream& o) { xprint(o, "And"); }
void Or::print(ostream& o) { xprint(o, "Or"); } // 

void Not::print(ostream& o)
{
    o << "Not (right)\n";
    lastChild(o);
    o << right;
    endChild(o);
}

void BooleanConstant::print(ostream& o)
{
    o << "BooleanConstant (" << (value?"TRUE":"FALSE") << ")";
}



ostream& operator<<(ostream& o, ASTNode *node)
{
    if (node == NULL)
        o << (void*)node;
    else
        node->print(o);
    return o;
}

ostream& operator<<(ostream& o, ASTNode& node)
{
    node.print(o);
    return o;
}
