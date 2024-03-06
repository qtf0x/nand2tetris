/**
 * @file parser.h
 * @author Vincent Marias <vmarias@mines.edu>
 * @date 03/05/2024
 *
 * @brief This file is part of the HackAssember program, an assembler for the
 * Hack architecture, as described in "The Elements of Computing Systems", 2nd
 * Ed. by Nisan and Schocken. This module handles parsing instructions into
 * their constiuent fields, as well as resolving sybmolic references to memory
 * addresses.
 *
 * @copyright Vincent Marias, 2024
 */

#ifndef HACK_ASSEMBLER_PARSER_H
#define HACK_ASSEMBLER_PARSER_H

#define _POSIX_C_SOURCE 200809L
#include <regex.h>   // for regmatch_t, regex_t, regcomp, regerror, regexec
#include <stdbool.h> // for bool
#include <stdint.h>  // for uint16_t

#include "sym_tbl.h"

typedef enum { A_INSTR, C_INSTR, L_INSTR, COMMENT_IGNORE } instr_t;

typedef struct instruction {
    instr_t type;

    union {
        /* A- and L-instructions */
        struct {
            bool resolved;
            union {
                uint16_t addr;
                char* symbol;
            };
        };

        /* C-instuctions */
        struct {
            char* dest;
            char* comp;
            char* jump;
        };
    };

    uint16_t line_number;     /* line number in source file */
    struct instruction* next; /* next instruction in queue */
} instruction;

/**
 * @brief Parses an instruction (string in source file) into it's constiuent
 * fields.
 *
 * @param[in] line a line from the source file, including endline character(s)
 * @param[in] line_num line of source the instuction appears on
 * @return heap-allocated instuction struct on success, NULL on failure (syntax
 * error)
 */
instruction* parse_instr(const char* const line, const uint16_t line_num);

/**
 * @brief Attempts to resolve a symbolic reference in an A-instuction to a
 * memory address using the symbol table. Variable symbols being encountered for
 * the first time are inserted into the table and the variable counter is
 * updated.
 *
 * @param[in,out] instr an A-instuction with a symbolic reference
 * @param[in,out] tbl the symbol table for the current program
 * @param[in,out] nvars the variable counter; gives number of *next* variable to
 * be seen
 * @return symbolic reference was/was not able to be resolved to an address
 */
bool resolve_reference(instruction* const instr, sym_tbl* const tbl,
                       uint16_t* const nvars);

/**
 * @brief Attempts to match a string to a regular expression using the
 * POSIX-extended syntax.
 *
 * @param[in] str string to match
 * @param[in] ptrn pattern to match against
 * @param[out] nsub number of captured subexpressions
 * @return heap-allocated regmatch_t structure array written to by regex engine,
 * NULL on failure to match (will emit an error if something else went wrong)
 */
regmatch_t* match_regex(const char* const str, const char* const ptrn,
                        size_t* nsub);

#endif // HACK_ASSEMBLER_PARSER_H
