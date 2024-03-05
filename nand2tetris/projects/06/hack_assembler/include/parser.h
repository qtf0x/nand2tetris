#ifndef HACK_ASSEMBLER_PARSER_H
#define HACK_ASSEMBLER_PARSER_H

#include <regex.h>   // for regular expression parsing
#include <stdbool.h> // for bool
#include <stdint.h>  // for uint16_t

#include "sym_tbl.h"

typedef enum { A_INSTR, C_INSTR, L_INSTR, COMMENT_IGNORE } instr_t;

typedef struct instruction {
    instr_t type;

    union {
        struct {
            bool resolved;
            union {
                uint16_t addr;
                char* symbol;
            };
        };

        struct {
            char* dest;
            char* comp;
            char* jump;
        };
    };

    uint16_t line_number;
    struct instruction* next;
} instruction;

instruction* parse_instr(const char* const line, const uint16_t line_num);

bool resolve_reference(instruction* const instr, sym_tbl* const tbl,
                       uint16_t* const nvars);

regmatch_t* match_regex(const char* const str, const char* const ptrn,
                        size_t* nsub);

#endif // HACK_ASSEMBLER_PARSER_H
