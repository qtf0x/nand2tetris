// standard library headers
#define _POSIX_C_SOURCE 200809L
#include <regex.h>
#include <stdint.h> // for uint16_t
#include <stdio.h>  // for fprintf, stderr, fopen, size_t, getline
#include <stdlib.h> // for EXIT_FAILURE, EXIT_SUCCESS
#include <string.h> // for strncmp, memcpy

// project-specific modules
#include "parser.h"
#include "sym_tbl.h"
#include "translator.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "[ERROR] Usage: .%s <path to file>.asm\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* parse input into [path][name][extension] */
    /* validate file extension as .asm */

    regmatch_t* pmatch = NULL;
    size_t nsub = 0;
    regoff_t off = 0, len = 0;

    char* path_no_ext = NULL;
    char* name = NULL;
    char* ext = NULL;

    if ((pmatch = match_regex(argv[1], "^(.*/)([^/\\]*)\\.(.*)$", &nsub))) {
        /* extract filepath without extension */
        off = pmatch[1].rm_so;
        len = pmatch[2].rm_eo - pmatch[1].rm_so;

        path_no_ext = calloc(len + 1, sizeof(char));
        strncpy(path_no_ext, argv[1] + off, len);

        /* extract filename without extension */
        off = pmatch[2].rm_so;
        len = pmatch[3].rm_eo - pmatch[2].rm_so;

        name = calloc(len + 1, sizeof(char));
        strncpy(name, argv[1] + off, len);

        /* extract file extension */
        off = pmatch[3].rm_so;
        len = pmatch[3].rm_eo - pmatch[3].rm_so;

        ext = calloc(len + 1, sizeof(char));
        strncpy(ext, argv[1] + off, len);

        /* clean up after yourself, you slob */
        free(pmatch);
        pmatch = NULL;
    } else {
        return EXIT_FAILURE;
    }

    if (strncmp(ext, "asm", len)) {
        fprintf(
            stderr,
            "[ERROR] File extension \"%s\" invalid - use <path to file>.asm\n",
            ext);
        return EXIT_FAILURE;
    }

    char* filename_out = calloc(strlen(path_no_ext) + 7, sizeof(char));
    strncpy(filename_out, path_no_ext, strlen(path_no_ext));
    strcat(filename_out, ".hack");

    free(path_no_ext);
    path_no_ext = NULL;
    free(ext);
    ext = NULL;

    /* process input stream line-by-line */
    /* pass non-empty lines to the parser, get back instruction struct */
    /* enqueue A- and C-instructions, use L-instructions to update symbol table
     */

    sym_tbl* tbl = sym_tbl_alloc();

    /* used to count instructions in program */
    uint16_t pc = 0;

    /* set up instruction queue */
    instruction* first_instr = NULL;
    instruction* curr_instr = NULL;

    FILE* fin = fopen(argv[1], "r");
    char* line = NULL;
    size_t glen = 0;

    for (uint16_t nline = 1; getline(&line, &glen, fin) != -1; ++nline) {
        /* don't bother giving parser empty lines */
        if (glen == 1) {
            continue;
        }

        instruction* instr = parse_instr(line, nline);

        if (!instr) {
            fprintf(stderr,
                    "[ERROR] Syntax error at %s:%u for instruction\n\t%s\n",
                    name, nline, line);
            return EXIT_FAILURE;
        }

        if (instr->type == L_INSTR) {
            sym_tbl_insert(tbl, instr->symbol, pc);
        } else if (instr->type == A_INSTR || instr->type == C_INSTR) {
            if (!first_instr) {
                first_instr = instr;
            }

            /* add the new instruction to the back of the queue */
            if (curr_instr) {
                curr_instr->next = instr;
            }
            curr_instr = instr;

            ++pc;
        }
    }

    free(line);
    fclose(fin);

    /* iterate trough instruction queue */
    /* pass instruction, symbol table to parser for symbol resolution */
    /* pass resolved instruction fields to translator for binary translation
     */
    /* catenate binary fields together to form complete binary instruction
     */
    /* write out binary instruction */

    FILE* fout = fopen(filename_out, "w");
    free(filename_out);
    filename_out = NULL;

    uint16_t nvars = 16; /* used to count variables in program */

    for (instruction* instr = first_instr; instr; instr = instr->next) {
        if ((instr->type == A_INSTR || instr->type == L_INSTR) &&
            !instr->resolved && !resolve_reference(instr, tbl, &nvars)) {
            fprintf(stderr,
                    "[ERROR] Failed to resolve reference \"%s\" at %s:%u\n",
                    instr->symbol, name, instr->line_number);
            return EXIT_FAILURE;
        }

        char instr_bits[16] = {'0'};

        if (instr->type == A_INSTR) {
            translate_val(instr->addr, instr_bits);
        } else if (instr->type == C_INSTR) {
            memcpy(instr_bits, "111", 3);
            memcpy(instr_bits + 3, translate_comp(instr->comp), 7);
            memcpy(instr_bits + 10, translate_dest(instr->dest), 3);
            memcpy(instr_bits + 13, translate_jump(instr->jump), 3);
        } else {
            continue;
        }

        fwrite(instr_bits, sizeof(char), 16, fout);
        fwrite("\n", sizeof(char), 1, fout);
    }

    free(name);
    fclose(fout);
    sym_tbl_free(tbl);

    return EXIT_SUCCESS;
}
