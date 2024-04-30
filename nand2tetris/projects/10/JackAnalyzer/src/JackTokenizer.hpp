#ifndef JACK_ANALYZER_JACK_TOKENIZER_HPP
#define JACK_ANALYZER_JACK_TOKENIZER_HPP

#include <iostream> /* for istream, cin */
#include <string>   /* for string */
#include <unordered_map>

#include <cstdint> /* for int16_t */

enum tok_t {
    TOK_NONE,
    TOK_KEYWORD,
    TOK_SYMBOL,
    TOK_IDENTIFIER,
    TOK_INT_CONST,
    TOK_STRING_CONST
};

enum kw_t {
    KW_NONE,
    KW_CLASS,
    KW_METHOD,
    KW_FUNCTION,
    KW_CONSTRUCTOR,
    KW_INT,
    KW_BOOLEAN,
    KW_CHAR,
    KW_VOID,
    KW_VAR,
    KW_STATIC,
    KW_FIELD,
    KW_LET,
    KW_DO,
    KW_IF,
    KW_ELSE,
    KW_WHILE,
    KW_RETURN,
    KW_TRUE,
    KW_FALSE,
    KW_NULL,
    KW_THIS
};

class JackTokenizer {
  public:
    /**
     * Opens the input .jack file/stream and gets ready to tokenize it.
     */
    JackTokenizer(std::istream& input_src = std::cin)
        : stream{input_src.rdbuf()}, curr_tok{TOK_NONE, 0, ""} {}

    /**
     * Are there more tokens in the input?
     */
    bool hasMoreTokens(void) const;

    /**
     * Gets the next token from the input, and makes it the current token.
     * This method should be called only if JackTokenizer::hasMoreTokens is
     * true. Initially there is no current token.
     */
    void advance(void);

    /**
     * Returns the type of the current token, as a constant.
     */
    tok_t tokenType(void) const;

    /**
     * Returns the keyword which is the current token, as a constant. Should be
     * called only if JackTokenizer::tokenType is TOK_KEYWORD.
     */
    kw_t keyWord(void) const;

    /**
     * Returns the character which is the current token. Should be called only
     * if JackTokenizer::tokenType is TOK_SYMBOL.
     */
    char symbol(void) const;

    /**
     * Returns the string which is the current token. Should be called only if
     * JackTokenizer::tokenType is TOK_IDENTIFIER.
     */
    std::string identifier(void) const;

    /**
     * Returns the integer value of the current token. Should be called only if
     * JackTokenizer::tokenType is TOK_INT_CONST.
     */
    int16_t intVal(void) const;

    /**
     * Returns the string value of the current token, without the opening and
     * closing double quotes. Should be called only if JackTokenizer::tokenType
     * is TOK_STRING_CONST.
     */
    std::string stringVal(void) const;

  private:
    struct token {
        tok_t type;

        union {
            char charVal;
            int16_t intVal;
        };
        std::string strVal;
    };

    std::istream stream;
    struct token curr_tok;

    std::unordered_map<std::string, kw_t> kw_map{
        {"", KW_NONE},
        {"class", KW_CLASS},
        {"method", KW_METHOD},
        {"function", KW_FUNCTION},
        {"constructor", KW_CONSTRUCTOR},
        {"int", KW_INT},
        {"boolean", KW_BOOLEAN},
        {"char", KW_CHAR},
        {"void", KW_VOID},
        {"var", KW_VAR},
        {"static", KW_STATIC},
        {"field", KW_FIELD},
        {"let", KW_LET},
        {"do", KW_DO},
        {"if", KW_IF},
        {"else", KW_ELSE},
        {"while", KW_WHILE},
        {"return", KW_RETURN},
        {"true", KW_TRUE},
        {"false", KW_FALSE},
        {"null", KW_NULL},
        {"this", KW_THIS}};
};

#endif /* JACK_ANALYZER_JACK_TOKENIZER_HPP */
