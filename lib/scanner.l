%{

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <symtab.hh>
#include <ast.hh>
#include <parser.hh>

std::vector<std::string> scanner_warnings;

void warning(std::string msg) {
  std::stringstream ss;
  ss << "Warning (line ";
  ss << yylineno;
  ss << "): ";
  ss << msg;
  ss << std::endl;
  scanner_warnings.push_back(msg);
  std::cerr << ss.str();
}

%}

%option yylineno
%option noyywrap
%option 8bit
%option caseless
%option nodefault

digit                             [0-9]
integer                           {digit}+
sign                              [+-]
exponent                          [Ee]({sign})?{integer}
real                              (\.{integer}|{integer}\.({integer})?)({exponent})?
integer_with_exponent             {integer}{exponent}

identifier                        [a-zA-Z][a-zA-Z0-9_]*

cpp_comment                       \/\/.*\n

whitespace                        [ \t\n]*

%x comment
%%

if                                  return IF;
then                                return THEN;
elseif                              return ELSEIF;
else                                return ELSE;
begin                               return XBEGIN;
end                                 return XEND;
while                               return WHILE;
function                            return FUNCTION;
program                             return PROGRAM;
return                              return RETURN;
declare                             return DECLARE;
do                                  return DO;
and                                 return AND;
or                                  return OR;
not                                 return NOT;
:=                                  return ASSIGN;
">="                                return GE;
"<="                                return LE;
==                                  return EQ;
"<>"                                return NE;
array                               return ARRAY;
of                                  return OF;

{identifier}                        return ID;

{real}                              return REAL;
{integer_with_exponent}             return REAL;
{integer}                           return INTEGER;

{cpp_comment}                       { }

"/*"                                BEGIN(comment);
<comment>"/*"                       { warning("Starting comment inside another comment!!!"); }
<comment>"/"                        { }
<comment>[^/*\n]*                   { }
<comment>"*"+[^*/\n]*               { }
<comment>\n                         { }
<comment>"*/"                       BEGIN(INITIAL);


{whitespace}                        { }

.                                   return yytext[0];
<<EOF>>                             yyterminate();
%%
