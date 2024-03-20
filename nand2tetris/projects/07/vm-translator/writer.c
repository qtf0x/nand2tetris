/**
 * @file writer.c
 * @author Vincent Marias <vmarias@mines.edu>
 * @date 03/19/2024
 *
 * @desc This file is part of the VMTranslator program. See `writer.h` for more
 * details.
 *
 * @copyright Vincent Marias 2024
 */

/* standard library headers */
#define _POSIX_C_SOURCE 200809L
#include <stddef.h> /* for NULL */
#include <stdint.h> /* for int16_t */

/* project-specific modules */
#include "parser.h" /* for cmd_t, C_PUSH, C_POP */
#include "writer.h"

/* >>>>>>>>>>>>>>>>>>> */
/* Types and Constants */
/* <<<<<<<<<<<<<<<<<<< */

struct writer { /* TODO */
};

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
/* (Public) Subroutine Definitions */
/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

struct writer* writer_alloc(const char* const fname) { /* TODO */
    return NULL;
}

void writer_free(struct writer* const wtr) { /* TODO */
}

void writer_put_al(struct writer* const wtr, const char* const cmd) { /* TODO */
}

void writer_put_so(struct writer* const wtr, const enum cmd_t cmd_type,
                   const char* const segment, const int16_t idx) { /* TODO */
}
