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
#include <stdio.h>  /* for FILE, fopen, perror, fclose, fprintf, stderr*/
#include <stdlib.h> /* for malloc, free */

#include <string.h>

/* project-specific modules */
#include "parser.h" /* for cmd_t, C_PUSH, C_POP */
#include "writer.h"

/* >>>>>>>>>>>>>>>>>>> */
/* Types and Constants */
/* <<<<<<<<<<<<<<<<<<< */

struct writer {
    FILE* fout;
};

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
/* (Public) Subroutine Definitions */
/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

struct writer* writer_alloc(const char* const fname) {
    if (!fname) {
        return NULL;
    }

    /* attempt to open the file */
    FILE* fout = fopen(fname, "w");
    if (!fout) {
        perror("[ERROR] fopen");
        return NULL;
    }

    /* attempt to create the Writer */
    struct writer* wtr = malloc(sizeof(*wtr));
    if (!wtr) {
        perror("[ERROR] malloc");
        if (fclose(fout)) {
            perror("[ERROR] fclose");
        }
        return NULL;
    }

    wtr->fout = fout;

    return wtr;
}

void writer_free(struct writer* const wtr) {
    if (!wtr) {
        return;
    }

    /* attempt to close the file if it's open */
    if (wtr->fout && fclose(wtr->fout)) {
        perror("[ERROR] fclose");
        wtr->fout = NULL;
    }

    free(wtr);
}

bool writer_put_al(struct writer* const wtr, const enum op_t op) { /* TODO */
    if (!wtr) {
        return false;
    }

    return true;
}

bool writer_put_so(struct writer* const wtr, const enum cmd_t cmd_type,
                   const enum seg_t seg, const int16_t idx) { /* TODO */
    if (!wtr) {
        return false;
    }

    return true;
}
