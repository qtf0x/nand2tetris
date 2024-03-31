/**
 * @file translator.c
 * @author Vincent Marias <vmarias@mines.edu>
 * @date 03/19/2024
 *
 * @desc This file is part of the VMTranslator program, a VM-assembly
 * translator and effective backend for the Jack language on the Hack
 * architecture, as described in "The Elements of Computing Systems", 2nd Ed. by
 * Nisan and Schocken. This module drives the translation process, using the
 * APIs provided by the Parser and Writer modules.
 *
 * @copyright Vincent Marias 2024
 */

/* standard library headers */
#define _POSIX_C_SOURCE 200809L
#include <ctype.h>  /* for isupper */
#include <stddef.h> /* for NULL */
#include <stdio.h>  /* for fprintf, stderr */
#include <stdlib.h> /* for EXIT_FAILURE, EXIT_SUCCESS, calloc, free */
#include <string.h> /* for strrchr, strcmp, strlen, strcpy */

/* project-specific modules */
#include "parser.h"
#include "writer.h"

/* >>>>>>>>>>>>>>>>>>> */
/* Types and Constants */
/* <<<<<<<<<<<<<<<<<<< */

const char *const IN_EXT = "vm", *const OUT_EXT = "asm";

/* >>>>>>>>>>>>>>>>>>> */
/* Program Entry Point */
/* <<<<<<<<<<<<<<<<<<< */

int main(int argc, char** argv) {
    struct parser* psr = NULL;
    struct writer* wtr = NULL;
    int EXIT_STATUS = EXIT_SUCCESS;

    if (argc != 2) {
        fprintf(stderr, "[ERROR] Usage: %s <path to file>.vm\n", argv[0]);
        EXIT_STATUS = EXIT_FAILURE;
        goto EXIT;
    }

    /* ------------------- */
    /* Validate Input File */
    /* ------------------- */

    const char* const dot_loc = strrchr(argv[1], '.');
    const char* ifname = strrchr(argv[1], '/');
    if (!ifname) {
        ifname = argv[1];
    } else {
        ++ifname;
    }

    if (!dot_loc || strcmp(dot_loc + 1, IN_EXT) || !isupper(ifname[0])) {
        fprintf(stderr,
                "[ERROR] File path \"%s\" invalid; filename must start with "
                "uppercase letter and have extension \".%s\"\n",
                argv[1], IN_EXT);
        EXIT_STATUS = EXIT_FAILURE;
        goto EXIT;
    }

    /* --------------------------------------------- */
    /* Create Writer for Translation/Output Services */
    /* --------------------------------------------- */

    /* one byte for '.', one for NUL */
    char* ofname =
        calloc(strlen(argv[1]) + strlen(OUT_EXT) + 2, sizeof(*ofname));
    strcpy(ofname, argv[1]);
    strcpy(strrchr(ofname, '.') + 1, OUT_EXT); /* overwrite file extension */

    wtr = writer_alloc(ofname);

    free(ofname);
    ofname = NULL;

    if (!wtr) {
        fprintf(stderr, "[ERROR] Could not create Writer\n");
        EXIT_STATUS = EXIT_FAILURE;
        goto EXIT;
    }

    /* --------------------------- */
    /* Parse the File Line-by-Line */
    /* --------------------------- */

    psr = parser_alloc(argv[1]);
    if (!psr) {
        fprintf(stderr, "[ERROR] Could not create Parser\n");
        EXIT_STATUS = EXIT_FAILURE;
        goto EXIT;
    }

    while (parser_has_lines(psr)) {
        parser_advance(psr);
        enum cmd_t cmd = parser_command_type(psr);

        switch (cmd) {
        case C_ARITHMETIC:
            if (!writer_put_al(wtr, parser_arg1(psr).operation)) {
                fprintf(stderr,
                        "[ERROR] Could not write arithmetic-logical command\n");
                EXIT_STATUS = EXIT_FAILURE;
                goto EXIT;
            }
            break;
        case C_PUSH:
        case C_POP:
            if (!writer_put_so(wtr, cmd, parser_arg1(psr).segment,
                               parser_arg2(psr))) {
                fprintf(stderr,
                        "[ERROR] Could not write arithmetic-logical command\n");
                EXIT_STATUS = EXIT_FAILURE;
                goto EXIT;
            }
            break;
        case C_LABEL:
        case C_GOTO:
        case C_IF:
            if (!writer_put_branch(wtr, cmd, parser_arg1(psr).label)) {
                fprintf(stderr, "[ERROR] Could not write branching command\n");
                EXIT_STATUS = EXIT_FAILURE;
                goto EXIT;
            }
            break;
        case C_FUNCTION:
            if (!writer_put_func(wtr, parser_arg1(psr).label,
                                 parser_arg2(psr))) {
                fprintf(stderr, "[ERROR] Could not write function command\n");
                EXIT_STATUS = EXIT_FAILURE;
                goto EXIT;
            }
            break;
        case C_RETURN:
            if (!writer_put_return(wtr)) {
                fprintf(stderr, "[ERROR] Could not write return command\n");
                EXIT_STATUS = EXIT_FAILURE;
                goto EXIT;
            }
            break;
        default:
            fprintf(
                stderr,
                "[ERROR] I wasn't expecting that command type just yet :/\n");
            EXIT_STATUS = EXIT_FAILURE;
            goto EXIT;
        }
    }

EXIT:
    writer_free(wtr);
    parser_free(psr);

    return EXIT_STATUS;
}
