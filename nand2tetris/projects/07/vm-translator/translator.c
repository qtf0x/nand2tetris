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
#include <stdio.h>  /* for fprintf, stderr */
#include <stdlib.h> /* for EXIT_FAILURE, EXIT_SUCCESS */

/* project-specific modules */
#include "parser.h"
#include "writer.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        fprintf(stderr, "[ERROR] Usage: %s <path to file>.vm\n", argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
