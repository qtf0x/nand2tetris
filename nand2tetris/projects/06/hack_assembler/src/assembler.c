/**
 * @file assembler.c
 * @author Vincent Marias <vmarias@mines.edu>
 * @date 03/05/2024
 *
 * @brief This file is part of the HackAssember program, an assembler for the
 * Hack architecture, as described in "The Elements of Computing Systems", 2nd
 * Ed. by Nisan and Schocken. This module handles file I/O, constructs the
 * symbol table, passes instructions to the parser and later to the translator,
 * and catenates the binary opcodes returned by the translator.
 *
 * @copyright Vincent Marias, 2024
 */

// standard library headers
#define _POSIX_C_SOURCE 200809L
#include <regex.h>  // for regmatch_t, size_t, regoff_t
#include <stdint.h> // for uint16_t
#include <stdio.h>  // for NULL, fprintf, stderr, fopen, size_t, getline
#include <stdlib.h> // for EXIT_FAILURE, calloc, free, EXIT_SUCCESS
#include <string.h> // for strncmp, memcpy, strncpy, strlen, strcat

// project-specific modules
#include "parser.h"
#include "sym_tbl.h"
#include "translator.h"

static const char* const OUT_EXT = ".hack";

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "[ERROR] Usage: %s <path to file>.asm\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * FILE I/O
     * parse input into [path][name][extension]
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

    regmatch_t* pmatch = NULL;
    size_t nsub = 0;
    // regoff_t off = 0, len = 0;
    regoff_t path_no_ext_off = 0, path_no_ext_len = 0;
    regoff_t name_off = 0, name_len = 0;
    regoff_t ext_off = 0, ext_len = 0;

    if ((pmatch = match_regex(argv[1], "^(.*/)?([^/\\]*)\\.(.*)$", &nsub))) {
        /* extract filepath without extension */
        if (pmatch[1].rm_so < 0) {
            /* match empty path prefix */
            path_no_ext_off = pmatch[2].rm_so;
            path_no_ext_len = pmatch[2].rm_eo - pmatch[2].rm_so;
        } else {
            path_no_ext_off = pmatch[1].rm_so;
            path_no_ext_len = pmatch[2].rm_eo - pmatch[1].rm_so;
        }

        /* extract filename without extension */
        name_off = pmatch[2].rm_so;
        name_len = pmatch[3].rm_eo - pmatch[2].rm_so;

        /* extract file extension */
        ext_off = pmatch[3].rm_so;
        ext_len = pmatch[3].rm_eo - pmatch[3].rm_so;

        /* clean up after yourself, you slob */
        free(pmatch);
        pmatch = NULL;
    } else {
        fprintf(stderr,
                "[ERROR] Input filepath \"%s\" invalid - check you spelling!\n",
                argv[1]);
        return EXIT_FAILURE;
    }

    /* validate file extension as .asm */
    if (strncmp(argv[1] + ext_off, "asm", (size_t)ext_len)) {
        fprintf(stderr,
                "[ERROR] File extension \"%.*s\" invalid - use <path to "
                "file>.asm\n",
                ext_len, argv[1] + ext_off);
        return EXIT_FAILURE;
    }

    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * PARSING
     * process input stream line-by-line
     * pass lines to the parser, get back instruction struct
     * enqueue A- and C-instructions, use L-instructions to update symbol table
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

    sym_tbl* tbl = sym_tbl_alloc();

    /* used to count instructions in program */
    uint16_t pc = 0;

    /* set up instruction queue */
    instruction* first_instr = NULL;
    instruction* curr_instr = NULL;

    char* line = NULL;
    FILE* fin = fopen(argv[1], "r");

    if (!fin) {
        fprintf(stderr, "[ERROR] Failed to open source file \"%s\"\n", argv[1]);
        return EXIT_FAILURE;
    }

    size_t glen = 0;
    for (uint16_t nline = 1; getline(&line, &glen, fin) != -1; ++nline) {
        instruction* instr = parse_instr(line, nline);

        if (!instr) {
            fprintf(stderr,
                    "[ERROR] Syntax error at %.*s:%u for instruction\n\t%s\n",
                    name_len, argv[1] + name_off, nline, line);
            sym_tbl_free(tbl);
            return EXIT_FAILURE;
        }

        if (instr->type == L_INSTR) {
            /* update symbol table */
            sym_tbl_insert(tbl, instr->symbol, pc);
            free(instr->symbol);
            free(instr);
        } else if (instr->type == A_INSTR || instr->type == C_INSTR) {
            if (!first_instr) {
                first_instr = instr;
            }

            /* add the new instruction to the back of the queue */
            if (curr_instr) {
                curr_instr->next = instr;
            }
            curr_instr = instr;

            ++pc; /* only count instructions that generate code */
        } else {
            free(instr);
        }
    }

    fclose(fin);
    free(line);
    line = NULL;

    /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     * TRANSLATION
     * iterate trough instruction queue
     * pass instruction, symbol table to parser for symbol resolution
     * pass resolved instruction fields to translator for binary translation
     * catenate binary fields together to form complete binary instruction
     * write out binary instruction
     * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

    uint16_t nvars = 16; /* used to count variables in program */

    /* output filepath is same as input w/ extension changed to .hack */
    char* filename_out =
        calloc((size_t)path_no_ext_len + strlen(OUT_EXT) + 1, sizeof(char));
    strncpy(filename_out, argv[1] + path_no_ext_off, (size_t)path_no_ext_len);
    strcat(filename_out, OUT_EXT);

    FILE* fout = fopen(filename_out, "w");

    if (!fout) {
        fprintf(stderr, "[ERROR] Failed to open otuput file \"%s\"\n",
                filename_out);
        return EXIT_FAILURE;
    }

    free(filename_out);
    filename_out = NULL;

    for (instruction* instr = first_instr; instr;) {
        /* if symbol needs to be resolved, attempt to resolve to an address */
        if ((instr->type == A_INSTR || instr->type == L_INSTR) &&
            !instr->resolved && !resolve_reference(instr, tbl, &nvars)) {
            fprintf(stderr,
                    "[ERROR] Failed to resolve reference \"%s\" at %.*s:%u\n",
                    instr->symbol, name_len, argv[1] + name_off,
                    instr->line_number);
            fclose(fout);
            sym_tbl_free(tbl);
            return EXIT_FAILURE;
        }

        /* the actual "bits" in the translated instruction */
        char instr_bits[16] = {'0'};

        if (instr->type == A_INSTR) {
            /* A-instructions just need a decimal->binary value translation */
            translate_val(instr->addr, instr_bits);
        } else if (instr->type == C_INSTR) {
            /* all C-instructions start with 111, then catenate other fields */
            memcpy(instr_bits, "111", 3);
            memcpy(instr_bits + 3, translate_comp(instr->comp), 7);
            memcpy(instr_bits + 10, translate_dest(instr->dest), 3);
            memcpy(instr_bits + 13, translate_jump(instr->jump), 3);

            free(instr->comp);
            free(instr->dest);
            free(instr->jump);
        } else {
            /* L-instructions and comments/blank lines don't generate code */
            continue;
        }

        fwrite(instr_bits, sizeof(char), 16, fout);
        fwrite("\n", sizeof(char), 1, fout);

        instruction* last_instr = instr;
        instr = instr->next;
        free(last_instr);
    }

    fclose(fout);
    sym_tbl_free(tbl);

    return EXIT_SUCCESS;
}
