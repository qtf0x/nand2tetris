%{
/* Definitions -------------------------------------------------------------- */
%}

%{
extern "C" int yylex();
extern "C" int yyerror(const char*);
#include <stddef.h>
#include <stdlib.h>
#include <string>
#include <stdio.h>
#include <iostream>

#include "parser.h"
%}

%x C_COMMENT

%%

%{
/* Rules -------------------------------------------------------------------- */
%}

%{
/* whitespace and comments */
%}

[ \t\n\r]           {} /* gobble whitespace */
"//"[^\n]*(\r)?\n   {} /* gobble line-end comments */

"/*"            { BEGIN(C_COMMENT); } /* gobble C-style comments */
<C_COMMENT>"*/" { BEGIN(INITIAL); }
<C_COMMENT>\n   { }
<C_COMMENT>.    { }

%{
/* symbols */
%}

%{/* for grouping arithmetic expressions and enclosing argument-lists (in
   * subroutine calls) and parameter-lists (in subroutine declarations */
%}
"{" { yylval.strVal = strdup(yytext); return TOK_LBRACE; }      /* for grouping program units, statements */
"}" { yylval.strVal = strdup(yytext); return TOK_RBRACE; }
"(" { yylval.strVal = strdup(yytext); return TOK_LPAREN; }      /* for grouping arithmetic expressions and */
")" { yylval.strVal = strdup(yytext); return TOK_RPAREN; }      /* enclosing argument-lists in subroutine calls */
                                                        /* and parameter-lists in subroutine declarations */
"[" { yylval.strVal = strdup(yytext); return TOK_LBRACKET; }    /* for array indexing */
"]" { yylval.strVal = strdup(yytext); return TOK_RBRACKET; }
"." { yylval.strVal = strdup(yytext); return TOK_DOT; }         /* class membership */
"," { yylval.strVal = strdup(yytext); return TOK_COMMA; }       /* variable-list separator */
";" { yylval.strVal = strdup(yytext); return TOK_SEMICOLON; }   /* statement terminator */
"+" { yylval.strVal = strdup(yytext); return TOK_PLUS; }        /* operators */
"-" { yylval.strVal = strdup(yytext); return TOK_MINUS; }
"*" { yylval.strVal = strdup(yytext); return TOK_ASTERISK; }
"/" { yylval.strVal = strdup(yytext); return TOK_FSLASH; }
"&" { yylval.strVal = strdup("&amp;"); return TOK_AMPERSAND; }
"|" { yylval.strVal = strdup(yytext); return TOK_PIPE; }
"<" { yylval.strVal = strdup("&lt;"); return TOK_LANGLE; }
">" { yylval.strVal = strdup("&gt;"); return TOK_RANGLE; }
"=" { yylval.strVal = strdup(yytext); return TOK_EQUALS; }      /* assignment and comparison operator */
"~" { yylval.strVal = strdup(yytext); return TOK_TILDE; }

%{
/* reserved words */
%}

class       { yylval.strVal = strdup(yytext); return TOK_CLASS; }   /* program components */
constructor { yylval.strVal = strdup(yytext); return TOK_CONSTRUCTOR; }
method      { yylval.strVal = strdup(yytext); return TOK_METHOD; }
function    { yylval.strVal = strdup(yytext); return TOK_FUNCTION; }
int         { yylval.strVal = strdup(yytext); return TOK_INT; }     /* primitive types */
boolean     { yylval.strVal = strdup(yytext); return TOK_BOOLEAN; }
char        { yylval.strVal = strdup(yytext); return TOK_CHAR; }
void        { yylval.strVal = strdup(yytext); return TOK_VOID; }
var         { yylval.strVal = strdup(yytext); return TOK_VAR; }     /* variable declarations */
static      { yylval.strVal = strdup(yytext); return TOK_STATIC; }
field       { yylval.strVal = strdup(yytext); return TOK_FIELD; }
let         { yylval.strVal = strdup(yytext); return TOK_LET; }     /* statements */
do          { yylval.strVal = strdup(yytext); return TOK_DO; }
if          { yylval.strVal = strdup(yytext); return TOK_IF; }
else        { yylval.strVal = strdup(yytext); return TOK_ELSE; }
while       { yylval.strVal = strdup(yytext); return TOK_WHILE; }
return      { yylval.strVal = strdup(yytext); return TOK_RETURN; }
true        { yylval.strVal = strdup(yytext); return TOK_TRUE; }    /* constant values */
false       { yylval.strVal = strdup(yytext); return TOK_FALSE; }
null        { yylval.strVal = strdup(yytext); return TOK_NULL; }
this        { yylval.strVal = strdup(yytext); return TOK_THIS; }    /* object reference */

%{
/* constants */
%}

[[:digit:]]+    /* integer constants are values in the range [0, 32767] */ {
                    yylval.intVal = atoi(yytext);
                    return TOK_INTEGER;
                }
\"[^"\n]*\"     /* string constants can't contain double quotes or newlines */ {
                    /* strip off the literal quotes */
                    std::string s(yytext);
                    yylval.strVal = strdup(s.substr(1, s.size() - 2).c_str());
                    return TOK_STRING;
                }

%{
/* identifiers */
%}

[_[:alpha:]][_[:alnum:]]*   {
                                yylval.strVal = strdup(yytext);
                                return TOK_IDENT;
                            }

%{
/* anything else is a lexical error */
%}

.   yyerror(strdup((std::string("lexical error: ") + std::string(yytext)).c_str()));
