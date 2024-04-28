%code requires {
    /* defintions */
    #include <cstddef>
    #include <cstdint>
    #include <string>
    #include <iostream>
}

%define api.value.type variant

/* constants */
%token<int16_t> TOK_INTEGER
%token<std::string> TOK_STRING
%token<std::string> TOK_IDENT

/* keywords */
%token
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
%token
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

/* types */
%nterm<std::string> className

/* rules */
%%

class:
    TOK_CLASS className TOK_LBRACE TOK_RBRACE { std::cout << "CLASS NAME: " << $2 << '\n'; }
;

className:
    TOK_IDENT { $$ = $1; }
;

%%

int main(void) {
    yyparse();

    return EXIT_SUCCESS;
}
