%{
/* Definitions -------------------------------------------------------------- */
%}

%{
/* for inclusion in C++ translation units */
extern "C" int yylex();
%}

%{
#include <string>
#include <iostream>
#include <cstddef>

#include "parser.h"
%}

%%

%{
/* Rules -------------------------------------------------------------------- */
%}

%{
/* whitespace and comments */
%}

[ \n]           {} /* gobble whitespace */
"//".*$         {} /* gobble line-end comments */
"/*"(?s:.)*"*/" {} /* gobble multi-line comments */

%{
/* symbols */
%}

%{/* for grouping arithmetic expressions and enclosing argument-lists (in
   * subroutine calls) and parameter-lists (in subroutine declarations */
%}
"{" { return TOK_LBRACE; }      /* for grouping program units, statements */
"}" { return TOK_RBRACE; }
"(" { return TOK_LPAREN; }      /* for grouping arithmetic expressions and */
")" { return TOK_RPAREN; }      /* enclosing argument-lists in subroutine calls */
                                /* and parameter-lists in subroutine declarations */
"[" { return TOK_LBRACKET; }    /* for array indexing */
"]" { return TOK_RBRACKET; }
"." { return TOK_DOT; }         /* class membership */
"," { return TOK_COMMA; }       /* variable-list separator */
";" { return TOK_SEMICOLON; }   /* statement terminator */
"+" { return TOK_PLUS; }        /* operators */
"-" { return TOK_MINUS; }
"*" { return TOK_ASTERISK; }
"/" { return TOK_FSLASH; }
"&" { return TOK_AMPERSAND; }
"|" { return TOK_PIPE; }
"<" { return TOK_LANGLE; }
">" { return TOK_RANGLE; }
"=" { return TOK_EQUALS; }      /* assignment and comparison operator */
"~" { return TOK_TILDE; }

%{
/* reserved words */
%}

class       { return TOK_CLASS; }   /* program components */
constructor { return TOK_CONSTRUCTOR; }
method      { return TOK_METHOD; }
function    { return TOK_FUNCTION; }
int         { return TOK_INT; }     /* primitive types */
boolean     { return TOK_BOOLEAN; }
char        { return TOK_CHAR; }
void        { return TOK_VOID; }
var         { return TOK_VAR; }     /* variable declarations */
static      { return TOK_STATIC; }
field       { return TOK_FIELD; }
let         { return TOK_LET; }     /* statements */
do          { return TOK_DO; }
if          { return TOK_IF; }
else        { return TOK_ELSE; }
while       { return TOK_WHILE; }
return      { return TOK_RETURN; }
true        { return TOK_TRUE; }    /* constant values */
false       { return TOK_FALSE; }
null        { return TOK_NULL; }
this        { return TOK_THIS; }    /* object reference */

%{
/* constants */
%}

[[:digit:]]+    /* integer constants are values in the range [0, 32767] */ {
                    yylval->emplace(std::stoi(yytext));
                    return TOK_INTEGER;
                }
\"[^"\n]*\"     /* string constants can't contain double quotes or newlines */ {
                    std::string val(yytext);
                    /* strip off the literal quotes */
                    yylval->emplace(val.substr(1, val.size() - 2).c_str);
                    return TOK_STRING;
                }

%{
/* identifiers */
%}

[_[:alpha:]][_[:alnum:]]*   {
                                yylval->emplace(yytext);
                                return TOK_IDENT;
                            }

%{
/* anything else is a lexical error */
%}

.   std::cout << "<ERROR>\t" << yytext << "\t</ERROR>\n";

%%

/* User Code ---------------------------------------------------------------- */
