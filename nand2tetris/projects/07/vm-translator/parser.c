/**
 * @file parser.c
 * @author Vincent Marias <vmarias@mines.edu>
 * @date 03/19/2024
 *
 * @desc This file is part of the VMTranslator program. See `parser.h` for more
 * details.
 *
 * @copyright Vincent Marias 2024
 */

/* standard library headers */
#define _POSIX_C_SOURCE 200809L
#include <stdbool.h> /* for bool, false */
#include <stddef.h>  /* for NULL */
#include <stdint.h>  /* for int16_t */

/* project-specific modules */
#include "parser.h"

/* >>>>>>>>>>>>>>>>>>> */
/* Types and Constants */
/* <<<<<<<<<<<<<<<<<<< */

struct parser { /* TODO */
};

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
/* (Public) Subroutine Definitions */
/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

struct parser* parser_alloc(const char* const fname) { /* TODO */
    return NULL;
}

void parser_free(struct parser* const psr) { /* TODO */
}

bool parser_more_lines(const struct parser* const psr) { /* TODO */
    return false;
}

void parser_advance(struct parser* const psr) { /* TODO */
}

enum cmd_t parser_command_type(const struct parser* const psr) { /* TODO */
    return 0;
}

const char* parser_arg1(const struct parser* const psr) { /* TODO */
    return NULL;
}

int16_t parser_arg2(const struct parser* const psr) { /* TODO */
    return 0;
}
