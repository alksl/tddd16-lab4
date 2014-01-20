%{
#include <symtab.hh>
#include <ast.hh>
#include <parser.hh>

/* --- Your code here ---
 *
 * This is where C declarations go. If you need to include
 * files for the actions below, this is where you do it.
 */

/* --- End your code --- */

extern void yyerror(char *);
%}

%option yylineno
%option noyywrap
%option 8bit
%option caseless
%option nodefault

/* --- Your code here ---
 *
 * This is where Flex declarations go. If you need to include
 * any such declarations, this is where you do it.
 */

%x comment

/* --- End your code --- */

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



<<EOF>>                             yyterminate();
.                                   return yytext[0];

%%
