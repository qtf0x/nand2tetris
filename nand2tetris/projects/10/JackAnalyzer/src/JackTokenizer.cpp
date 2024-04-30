#include "JackTokenizer.hpp"

bool JackTokenizer::hasMoreTokens(void) const { return this->stream.good(); }

void JackTokenizer::advance(void) {
    if (!this->hasMoreTokens()) {
        return;
    }

    while(true) {
        char c = this->stream.get();

        if (
    }
}

tok_t JackTokenizer::tokenType(void) const { return this->curr_tok.type; }

kw_t JackTokenizer::keyWord(void) const {
    if (this->curr_tok.type != TOK_KEYWORD) {
        return KW_NONE;
    }

    return this->kw_map.at(this->curr_tok.strVal);
}

char JackTokenizer::symbol(void) const {
    if (this->curr_tok.type != TOK_SYMBOL) {
        return '\0';
    }

    return this->curr_tok.charVal;
}

std::string JackTokenizer::identifier(void) const {
    if (this->curr_tok.type != TOK_IDENTIFIER) {
        return "";
    }

    return this->curr_tok.strVal;
}

int16_t JackTokenizer::intVal(void) const {
    if (this->curr_tok.type != TOK_INT_CONST) {
        return 0;
    }

    return this->curr_tok.intVal;
}

std::string JackTokenizer::stringVal(void) const {
    if (this->curr_tok.type != TOK_STRING_CONST) {
        return "";
    }

    return this->curr_tok.strVal;
}
