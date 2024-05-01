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
    #include <filesystem>

    extern "C" int yylex();
    extern "C" int yyerror(const char*);
    extern "C" void lexrestart(FILE*);

    extern "C" void parseFile(std::filesystem::path);

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
%token<strVal>
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
%token<strVal>
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

/* rules */
%%

class:
        { xmlOpenTag("class"); }
    tok_class tok_ident tok_lbrace classVarDec subroutineDec tok_rbrace
        { xmlCloseTag("class"); }
;

classType: tok_static | tok_field;

classVarDec:
    %empty  {}

    | classVarDec   { xmlOpenTag("classVarDec"); }
    classType type identList tok_semicolon
                    { xmlCloseTag("classVarDec"); }
;

identList:
    tok_ident
    | tok_ident tok_comma identList

type: tok_int | tok_char | tok_boolean | tok_void | tok_ident;

subroutineType: tok_constructor | tok_function | tok_method;

subroutineDec:
    %empty  {}

    | subroutineDec { xmlOpenTag("subroutineDec"); }
    subroutineType type tok_ident tok_lparen parameterList tok_rparen subroutineBody
                    { xmlCloseTag("subroutineDec"); }
;

parameterList:
    %empty
        {  xmlOpenTag("parameterList"); xmlCloseTag("parameterList"); }

    |   { xmlOpenTag("parameterList"); }
    params
        { xmlCloseTag("parameterList"); }
;

params:
    type tok_ident
    | type tok_ident tok_comma params
;

subroutineBody:
        { xmlOpenTag("subroutineBody"); }
    tok_lbrace varDec statements tok_rbrace
        { xmlCloseTag("subroutineBody"); }
;

varDec:
    %empty    {}

    | varDec    { xmlOpenTag("varDec"); }
    tok_var type identList tok_semicolon
                { xmlCloseTag("varDec"); }
;

statementList:
    statement
    | statement statementList
;

statements: 
    %empty    { xmlOpenTag("statements"); xmlCloseTag("statements"); }

    |   { xmlOpenTag("statements"); }
    statementList
        { xmlCloseTag("statements"); }
;

statement:
    letStatement
    | ifStatement
    | whileStatement
    | doStatement
    | returnStatement
;

arrayAccess:
    %empty  {}

    | tok_lbracket expression tok_rbracket
;

letStatement:
        { xmlOpenTag("letStatement"); }
    tok_let tok_ident arrayAccess tok_equals expression tok_semicolon
        { xmlCloseTag("letStatement"); }
;

elseStatement:
    %empty  {}

    | tok_else tok_lbrace statements tok_rbrace
;

ifStatement:
        { xmlOpenTag("ifStatement"); }
    tok_if tok_lparen expression tok_rparen tok_lbrace statements tok_rbrace elseStatement
        { xmlCloseTag("ifStatement"); }
;

whileStatement:
        { xmlOpenTag("whileStatement"); }
    tok_while tok_lparen expression tok_rparen tok_lbrace statements tok_rbrace
        { xmlCloseTag("whileStatement"); }
;

doStatement:
        { xmlOpenTag("doStatement"); }
    tok_do subroutineCall tok_semicolon
        { xmlCloseTag("doStatement"); }
;

returnExpr:
    %empty  {}

    | expression
;

returnStatement:
        { xmlOpenTag("returnStatement"); }
    tok_return returnExpr tok_semicolon
        { xmlCloseTag("returnStatement"); }
;

termList:
    %empty  {}

    | op term termList
;

expression:
        { xmlOpenTag("expression"); }
    term termList
        { xmlCloseTag("expression"); }
;

term:
        { xmlOpenTag("term"); }
    tok_integer
        { xmlCloseTag("term"); }

    |   { xmlOpenTag("term"); }
    tok_string
        { xmlCloseTag("term"); }

    |   { xmlOpenTag("term"); }
    keywordConstant
        { xmlCloseTag("term"); }

    |   { xmlOpenTag("term"); }
    tok_ident arrayAccess
        { xmlCloseTag("term"); }

    |   { xmlOpenTag("term"); }
    tok_lparen expression tok_rparen
        { xmlCloseTag("term"); }

    |   { xmlOpenTag("term"); }
    unaryOp term
        { xmlCloseTag("term"); }

    |   { xmlOpenTag("term"); }
    subroutineCall
        { xmlCloseTag("term"); }
;

subroutineCall:
    tok_ident tok_lparen expressionList tok_rparen
    | tok_ident tok_dot tok_ident tok_lparen expressionList tok_rparen
;

expressionList:
    %empty  { xmlOpenTag("expressionList");  xmlCloseTag("expressionList"); }

    |   { xmlOpenTag("expressionList"); }
    exprs
        { xmlCloseTag("expressionList"); }
;

exprs:
    expression
    | expression tok_comma exprs
;

op: tok_plus | tok_minus | tok_asterisk | tok_fslash | tok_ampersand | tok_pipe | tok_langle | tok_rangle | tok_equals;

unaryOp: tok_minus | tok_tilde;

keywordConstant: tok_true | tok_false | tok_null | tok_this;

tok_lbrace:     TOK_LBRACE      { xmlSymbol($1); }
tok_rbrace:     TOK_RBRACE      { xmlSymbol($1); }
tok_lparen:     TOK_LPAREN      { xmlSymbol($1); }
tok_rparen:     TOK_RPAREN      { xmlSymbol($1); }
tok_lbracket:   TOK_LBRACKET    { xmlSymbol($1); }
tok_rbracket:   TOK_RBRACKET    { xmlSymbol($1); }
tok_dot:        TOK_DOT         { xmlSymbol($1); }
tok_comma:      TOK_COMMA       { xmlSymbol($1); }
tok_semicolon:  TOK_SEMICOLON   { xmlSymbol($1); }
tok_plus:       TOK_PLUS        { xmlSymbol($1); }
tok_minus:      TOK_MINUS       { xmlSymbol($1); }
tok_asterisk:   TOK_ASTERISK    { xmlSymbol($1); }
tok_fslash:     TOK_FSLASH      { xmlSymbol($1); }
tok_ampersand:  TOK_AMPERSAND   { xmlSymbol($1); }
tok_pipe:       TOK_PIPE        { xmlSymbol($1); }
tok_langle:     TOK_LANGLE      { xmlSymbol($1); }
tok_rangle:     TOK_RANGLE      { xmlSymbol($1); }
tok_equals:     TOK_EQUALS      { xmlSymbol($1); }
tok_tilde:      TOK_TILDE       { xmlSymbol($1); }

tok_class:          TOK_CLASS       { xmlKeyword($1); }
tok_constructor:    TOK_CONSTRUCTOR { xmlKeyword($1); }
tok_function:       TOK_FUNCTION    { xmlKeyword($1); }
tok_method:         TOK_METHOD      { xmlKeyword($1); }
tok_field:          TOK_FIELD       { xmlKeyword($1); }
tok_static:         TOK_STATIC      { xmlKeyword($1); }
tok_var:            TOK_VAR         { xmlKeyword($1); }
tok_int:            TOK_INT         { xmlKeyword($1); }
tok_char:           TOK_CHAR        { xmlKeyword($1); }
tok_boolean:        TOK_BOOLEAN     { xmlKeyword($1); }
tok_void:           TOK_VOID        { xmlKeyword($1); }
tok_true:           TOK_TRUE        { xmlKeyword($1); }
tok_false:          TOK_FALSE       { xmlKeyword($1); }
tok_null:           TOK_NULL        { xmlKeyword($1); }
tok_this:           TOK_THIS        { xmlKeyword($1); }
tok_let:            TOK_LET         { xmlKeyword($1); }
tok_do:             TOK_DO          { xmlKeyword($1); }
tok_if:             TOK_IF          { xmlKeyword($1); }
tok_else:           TOK_ELSE        { xmlKeyword($1); }
tok_while:          TOK_WHILE       { xmlKeyword($1); }
tok_return:         TOK_RETURN      { xmlKeyword($1); }

tok_integer:    TOK_INTEGER { xmlInteger($1); }
tok_string:     TOK_STRING  { xmlString($1); }
tok_ident:      TOK_IDENT   { xmlIdent($1); }

%%

int main(int argc, char** argv) {
    std::filesystem::path argPath{argv[1]};

    if (std::filesystem::is_directory(argPath)) {
        for (const auto& entry : std::filesystem::directory_iterator(argPath)) {
            if (entry.path().extension() == ".jack") {
                parseFile(entry);
            }
        }
    } else {
        parseFile(argPath);
    }

    return EXIT_SUCCESS;
}

void parseFile(std::filesystem::path inPath) {
    const char* s = inPath.c_str();
    std::filesystem::path outExt{".xml"};

    /* set input stream */
    std::cout << "Opening " << s << " for input...\n";
    FILE* inFile = fopen(s, "r");
    lexrestart(inFile);

    /* set output stream */
    std::filesystem::path outPath = inPath;
    outPath.replace_extension(outExt);
    std::cout << "Opening " << outPath << " for output...\n";
    out.reset(new std::ofstream(outPath));

    /* do the parse */
    std::cout << "Parsing...\n\n";
    yyparse();

    fclose(inFile);
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
