%{
    /* defintions */
    #include <stddef.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <iostream>
    #include <fstream>
    #include <memory>
    #include <string>
    #include <string.h>

    extern "C" int yylex();
    extern "C" int yyerror(const char*);
    extern "C" void xmlKeyword (char*);
    extern "C" void xmlSymbol (char*);
    extern "C" void xmlInteger (int16_t);
    extern "C" void xmlString (char*);
    extern "C" void xmlIdent (char*);
    extern "C" void xmlOpenTag (std::string);
    extern "C" void xmlCloseTag (std::string);

    std::shared_ptr<std::ostream> out(&std::cout, [](...) {});
%}

%glr-parser

%union {
    int16_t intVal;
    char* strVal;
}

/* constants */
%token<intVal> TOK_INTEGER
%token<strVal> TOK_STRING
%token<strVal> TOK_IDENT

/* keywords */
%token <strVal>
    TOK_CLASS
    TOK_CONSTRUCTOR
    TOK_FUNCTION
    TOK_METHOD
    TOK_FIELD
    TOK_STATIC
    TOK_VAR
    TOK_INT
    TOK_CHAR
    TOK_BOOLEAN
    TOK_VOID
    TOK_TRUE
    TOK_FALSE
    TOK_NULL
    TOK_THIS
    TOK_LET
    TOK_DO
    TOK_IF
    TOK_ELSE
    TOK_WHILE
    TOK_RETURN
;

/* symbols */
%token <strVal>
    TOK_LBRACE
    TOK_RBRACE
    TOK_LPAREN
    TOK_RPAREN
    TOK_LBRACKET
    TOK_RBRACKET
    TOK_DOT
    TOK_COMMA
    TOK_SEMICOLON
    TOK_PLUS
    TOK_MINUS
    TOK_ASTERISK
    TOK_FSLASH
    TOK_AMPERSAND
    TOK_PIPE
    TOK_LANGLE
    TOK_RANGLE
    TOK_EQUALS
    TOK_TILDE
;

//%nterm<strVal> statements statement letStatement ifStatement whileStatement

/* rules */
%%

statements: 
    %empty          { }
    | statements    { xmlOpenTag("statements"); }
    statement       { xmlCloseTag("statements"); }
;

statement:
    letStatement
    | ifStatement
    | whileStatement
;

letStatement:
        { xmlOpenTag("letStatement"); }
    let varName equals expression semicolon
        { xmlCloseTag("letStatement"); }
;

ifStatement:
        { xmlOpenTag("ifStatement"); }
    if lparen expression rparen
    lbrace
        statements
    rbrace
        { xmlCloseTag("ifStatement"); }
;

whileStatement:
        { xmlOpenTag("whileStatement"); }
    while lparen expression rparen
    lbrace
        statements
    rbrace
        { xmlCloseTag("whileStatement"); }
;

expression:
        { xmlOpenTag("expression"); }
    term
        { xmlCloseTag("expression"); }

    |   { xmlOpenTag("expression"); }
    term op term
        { xmlCloseTag("expression"); }
;

term:
        { xmlOpenTag("term"); }
    varName
        { xmlCloseTag("term"); }

    |   { xmlOpenTag("term"); }
    constant
        { xmlCloseTag("term"); }
;

varName:
        { xmlOpenTag("varName"); }
    ident
        { xmlCloseTag("varName"); }
;

constant:
    integer
;

op:
        { xmlOpenTag("op"); }
    plus
        { xmlCloseTag("op"); }

    |   { xmlOpenTag("op"); }
    minus
        { xmlCloseTag("op"); }

    |   { xmlOpenTag("op"); }
    equals
        { xmlCloseTag("op"); }


    |   { xmlOpenTag("op"); }
    rangle
        { xmlCloseTag("op"); }

    |   { xmlOpenTag("op"); }
    langle
        { xmlCloseTag("op"); }
;

lbrace:     TOK_LBRACE      { xmlSymbol($1); }
rbrace:     TOK_RBRACE      { xmlSymbol($1); }
lparen:     TOK_LPAREN      { xmlSymbol($1); }
rparen:     TOK_RPAREN      { xmlSymbol($1); }
lbracket:   TOK_LBRACKET    { xmlSymbol($1); }
rbracket:   TOK_RBRACKET    { xmlSymbol($1); }
dot:        TOK_DOT         { xmlSymbol($1); }
comma:      TOK_COMMA       { xmlSymbol($1); }
semicolon:  TOK_SEMICOLON   { xmlSymbol($1); }
plus:       TOK_PLUS        { xmlSymbol($1); }
minus:      TOK_MINUS       { xmlSymbol($1); }
asterisk:   TOK_ASTERISK    { xmlSymbol($1); }
fslash:     TOK_FSLASH      { xmlSymbol($1); }
ampersand:  TOK_AMPERSAND   { xmlSymbol($1); }
pipe:       TOK_PIPE        { xmlSymbol($1); }
langle:     TOK_LANGLE      { xmlSymbol($1); }
rangle:     TOK_RANGLE      { xmlSymbol($1); }
equals:     TOK_EQUALS      { xmlSymbol($1); }
tilde:      TOK_TILDE       { xmlSymbol($1); }

class:          TOK_CLASS       { xmlKeyword($1); }
constructor:    TOK_CONSTRUCTOR { xmlKeyword($1); }
function:       TOK_FUNCTION    { xmlKeyword($1); }
method:         TOK_METHOD      { xmlKeyword($1); }
field:          TOK_FIELD       { xmlKeyword($1); }
static:         TOK_STATIC      { xmlKeyword($1); }
var:            TOK_VAR         { xmlKeyword($1); }
int:            TOK_INT         { xmlKeyword($1); }
char:           TOK_CHAR        { xmlKeyword($1); }
boolean:        TOK_BOOLEAN     { xmlKeyword($1); }
void:           TOK_VOID        { xmlKeyword($1); }
true:           TOK_TRUE        { xmlKeyword($1); }
false:          TOK_FALSE       { xmlKeyword($1); }
null:           TOK_NULL        { xmlKeyword($1); }
this:           TOK_THIS        { xmlKeyword($1); }
let:            TOK_LET         { xmlKeyword($1); }
do:             TOK_DO          { xmlKeyword($1); }
if:             TOK_IF          { xmlKeyword($1); }
else:           TOK_ELSE        { xmlKeyword($1); }
while:          TOK_WHILE       { xmlKeyword($1); }
return:         TOK_RETURN      { xmlKeyword($1); }

integer:    TOK_INTEGER { xmlInteger($1); }
string:     TOK_STRING  { xmlString($1); }
ident:      TOK_IDENT   { xmlIdent($1); }

%%

int main(int argc, char** argv) {
    //out.reset(&std::cout);

    yyparse();

    return EXIT_SUCCESS;
}

int yyerror(const char* s) {
    fprintf(stderr, "error: %s\n", s);

    return 0;
}

void xmlKeyword (char* s) {
    *out << "<keyword> " << s << " </keyword>\n";
}

void xmlSymbol (char* s) {
    *out << "<symbol> " << s << " </symbol>\n";
}

void xmlInteger (int16_t i) {
    *out << "<integerConstant> " << i << " </integerConstant>\n";
}

void xmlString (char* s) {
    *out << "<stringConstant> " << s << " </stringConstant>\n";
}

void xmlIdent (char* s) {
    *out << "<identifier> " << s << " </identifier>\n";
}

void xmlOpenTag (std::string s) {
    *out << "<" << s << ">\n";
}

void xmlCloseTag (std::string s) {
    *out << "</" << s << ">\n";
}
