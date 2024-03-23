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
#include <stddef.h> /* for NULL, size_t */
#include <stdint.h> /* for int16_t */
#include <stdio.h>  /* for FILE, fopen, perror, fclose, fprintf, stderr*/
#include <stdlib.h> /* for malloc, free */
#include <string.h> /* for strlen, strcpy, strrchr, strtok */

/* project-specific modules */
#include "parser.h" /* for cmd_t, C_PUSH, C_POP */
#include "writer.h"

/* >>>>>>>>>>>>>>>>>>> */
/* Types and Constants */
/* <<<<<<<<<<<<<<<<<<< */

struct writer {
    FILE* fout;
    char* fname;
    size_t label_count;
};

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
/* (Private) Subroutine Definitions */
/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

static void write_instructions(struct writer* const wtr,
                               const char* const instructions) {
    fwrite(instructions, sizeof(*instructions), strlen(instructions),
           wtr->fout);
}

static void pop_D(struct writer* const wtr) {
    write_instructions(wtr, "@SP\nM=M-1\nA=M\nD=M\n");
}

static void push_D(struct writer* const wtr) {
    write_instructions(wtr, "@SP\nM=M+1\nA=M-1\nM=D\n");
}

static void write_arithmetic(struct writer* const wtr, const enum op_t op) {
    pop_D(wtr);
    write_instructions(wtr, "@R13\nM=D\n");
    pop_D(wtr);
    write_instructions(wtr, "@R13\n");

    switch (op) {
    case O_ADD:
        write_instructions(wtr, "D=D+M\n");
        break;
    case O_SUB:
        write_instructions(wtr, "D=D-M\n");
        break;
    case O_AND:
        write_instructions(wtr, "D=D&M\n");
        break;
    case O_OR:
        write_instructions(wtr, "D=D|M\n");
        break;
    default:
        fprintf(stderr,
                "[WARNING] Calling %s with incompatible op-code, no operation "
                "performed\n",
                __func__);
        break;
    }
}

static void write_comparison(struct writer* const wtr, const enum op_t op) {
    pop_D(wtr);
    write_instructions(wtr, "@R13\nM=D\n");
    pop_D(wtr);
    write_instructions(wtr, "@R13\nD=D-M\n");

    /* stupid labels ugh kill meeeeeeee */
    const int len = snprintf(NULL, 0, "%zu", wtr->label_count + 1);
    char* label_1_load =
        calloc(strlen(wtr->fname) + len + 4, sizeof(*label_1_load));
    char* label_2_load =
        calloc(strlen(wtr->fname) + len + 4, sizeof(*label_1_load));

    char* label_1 = calloc(strlen(wtr->fname) + len + 5, sizeof(*label_1));
    char* label_2 = calloc(strlen(wtr->fname) + len + 5, sizeof(*label_1));

    sprintf(label_1_load, "@%s:%zu\n", wtr->fname, wtr->label_count);
    sprintf(label_1, "(%s:%zu)\n", wtr->fname, wtr->label_count++);

    sprintf(label_2_load, "@%s:%zu\n", wtr->fname, wtr->label_count);
    sprintf(label_2, "(%s:%zu)\n", wtr->fname, wtr->label_count++);

    write_instructions(wtr, label_1_load);

    switch (op) {
    case O_EQ:
        write_instructions(wtr, "D;JEQ\n");
        break;
    case O_LT:
        write_instructions(wtr, "D;JLT\n");
        break;
    case O_GT:
        write_instructions(wtr, "D;JGT\n");
        break;
    default:
        fprintf(stderr,
                "[WARNING] Calling %s with incompatible op-code, no operation "
                "performed\n",
                __func__);
        return;
    }

    write_instructions(wtr, "D=0\n");
    write_instructions(wtr, label_2_load);
    write_instructions(wtr, "0;JMP\n");
    write_instructions(wtr, label_1);
    write_instructions(wtr, "D=-1\n");
    write_instructions(wtr, label_2);

    free(label_1);
    free(label_2);
    free(label_1_load);
    free(label_2_load);
}

static void write_unary(struct writer* const wtr, const enum op_t op) {
    pop_D(wtr);

    switch (op) {
    case O_NEG:
        write_instructions(wtr, "D=-D\n");
        break;
    case O_NOT:
        write_instructions(wtr, "D=!D\n");
        break;
    default:
        fprintf(stderr,
                "[WARNING] Calling %s with incompatible op-code, no operation "
                "performed\n",
                __func__);
        return;
    }
}

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
/* (Public) Subroutine Definitions */
/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

struct writer* writer_alloc(const char* const fpath) {
    if (!fpath) {
        return NULL;
    }

    /* attempt to open the file */
    FILE* fout = fopen(fpath, "w");
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
    wtr->label_count = 0;

    /* extract filename from path */
    char* fpath_cpy = calloc(strlen(fpath) + 1, sizeof(*fpath_cpy));
    strcpy(fpath_cpy, fpath);

    char* fname = strrchr(fpath_cpy, '/');
    if (!fname) {
        fname = fpath_cpy;
    } else {
        ++fname;
    }

    fname = strtok(fname, ".");

    wtr->fname = calloc(strlen(fname) + 1, sizeof(*wtr->fname));
    strcpy(wtr->fname, fname);

    free(fpath_cpy);

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

    free(wtr->fname);
    wtr->fname = NULL;

    free(wtr);
}

bool writer_put_al(struct writer* const wtr, const enum op_t op) {
    if (!wtr || !wtr->fout) {
        fprintf(stderr,
                "[WARNING] Calling %s with NULL argument(s), no operation "
                "performed\n",
                __func__);
        return false;
    }

    switch (op) {
    case O_ADD:
    case O_SUB:
    case O_AND:
    case O_OR:
        write_arithmetic(wtr, op);
        break;

    case O_EQ:
    case O_LT:
    case O_GT:
        write_comparison(wtr, op);
        break;

    case O_NEG:
    case O_NOT:
        write_unary(wtr, op);
        break;

    default:
        fprintf(stderr,
                "[WARNING] Calling %s with incompatible op-code, no operation "
                "performed\n",
                __func__);
        return false;
    }

    push_D(wtr);

    return true;
}

bool writer_put_so(struct writer* const wtr, const enum cmd_t cmd_type,
                   const enum seg_t seg, const int16_t idx) { /* TODO */
    if (!wtr) {
        return false;
    }

    return true;
}
