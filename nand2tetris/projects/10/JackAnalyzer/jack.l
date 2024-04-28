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
%}

%%

%{
/* Rules -------------------------------------------------------------------- */
%}

%{
/* whitespace and comments */
%}

[ \n]           /* gobble whitespace */
"//".*$         /* gobble line-end comments */
"/*"(?s:.)*"*/" /* gobble multi-line comments */

%{
/* symbols */
%}

%{/* for grouping arithmetic expressions and enclosing argument-lists (in
   * subroutine calls) and parameter-lists (in subroutine declarations */
%}
"("|")"                             |
"["|"]"                             | /* for array indexing */
"{"|"}"                             | /* for grouping program units, statements */
","                                 | /* variable-list separator */
";"                                 | /* statement terminator */
"="                                 | /* assignment and comparison operator */
"."                                 | /* class membership */
"+"|"-"|"*"|"/"|"&"|"|"|"~"|"<"|">" /* operators */ {
                                        std::cout << "<symbol>\t" << yytext << "\t</symbol>\n";
                                    }

%{
/* reserved words */
%}

class|constructor|method|function | /* program components */
int|boolean|char|void             | /* primitive types */
var|static|field                  | /* variable declarations */
let|do|if|else|while|return       | /* statements */
true|false|null                   | /* constant values */
this                              /* object reference */ {
                                      std::cout << "<keyword>\t" << yytext << "\t</keyword>\n";
                                  }

%{
/* constants */
%}

[[:digit:]]+    /* integer constants are values in the range [0, 32767] */ {
                    int val = std::stoi(yytext);
                    if (val >= 0 && val <= 32767) {
                        std::cout << "<intConst>\t" << yytext << "\t</intConst>\n";
                    } else {
                        std::cout << "<ERROR>\t" << yytext << "\t</ERROR>\n";
                    }
                }
\"[^"\n]*\"     /* string constants can't contain double quotes or newlines */ {
                    std::cout << "<strConst>\t" << yytext << "\t</strConst>\n";
                }

%{
/* identifiers */
%}

[_[:alpha:]][_[:alnum:]]*   std::cout << "<identifier>\t" << yytext << "\t</identifier>\n";

%{
/* anything else is a lexical error */
%}

.   std::cout << "<ERROR>\t" << yytext << "\t</ERROR>\n";

%%

/* User Code ---------------------------------------------------------------- */

int main(void) {
    yylex();

    return EXIT_SUCCESS;
}
