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
    char* curr_func;
    size_t label_count;
};

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
/* (Private) Subroutine Definitions */
/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

static void pop_D(struct writer* const wtr) {
    fprintf(wtr->fout, "@SP\nM=M-1\nA=M\nD=M\n");
}

static void push_D(struct writer* const wtr) {
    fprintf(wtr->fout, "@SP\nM=M+1\nA=M-1\nM=D\n");
}

static void write_arithmetic(struct writer* const wtr, const enum op_t op) {
    pop_D(wtr);
    fprintf(wtr->fout, "@R13\nM=D\n");
    pop_D(wtr);
    fprintf(wtr->fout, "@R13\n");

    switch (op) {
    case O_ADD:
        fprintf(wtr->fout, "D=D+M\n");
        break;
    case O_SUB:
        fprintf(wtr->fout, "D=D-M\n");
        break;
    case O_AND:
        fprintf(wtr->fout, "D=D&M\n");
        break;
    case O_OR:
        fprintf(wtr->fout, "D=D|M\n");
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
    fprintf(wtr->fout, "@R13\nM=D\n");
    pop_D(wtr);
    fprintf(wtr->fout, "@R13\nD=D-M\n");

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

    fprintf(wtr->fout, "%s", label_1_load);

    switch (op) {
    case O_EQ:
        fprintf(wtr->fout, "D;JEQ\n");
        break;
    case O_LT:
        fprintf(wtr->fout, "D;JLT\n");
        break;
    case O_GT:
        fprintf(wtr->fout, "D;JGT\n");
        break;
    default:
        fprintf(stderr,
                "[WARNING] Calling %s with incompatible op-code, no operation "
                "performed\n",
                __func__);
        return;
    }

    fprintf(wtr->fout, "D=0\n");
    fprintf(wtr->fout, "%s", label_2_load);
    fprintf(wtr->fout, "0;JMP\n");
    fprintf(wtr->fout, "%s", label_1);
    fprintf(wtr->fout, "D=-1\n");
    fprintf(wtr->fout, "%s", label_2);

    free(label_1);
    free(label_2);
    free(label_1_load);
    free(label_2_load);
}

static void write_unary(struct writer* const wtr, const enum op_t op) {
    pop_D(wtr);

    switch (op) {
    case O_NEG:
        fprintf(wtr->fout, "D=-D\n");
        break;
    case O_NOT:
        fprintf(wtr->fout, "D=!D\n");
        break;
    default:
        fprintf(stderr,
                "[WARNING] Calling %s with incompatible op-code, no operation "
                "performed\n",
                __func__);
        return;
    }
}

static void access_segment(struct writer* const wtr, const enum seg_t seg) {
    switch (seg) {
    case S_LOCAL:
        fprintf(wtr->fout, "@LCL\n");
        break;
    case S_ARGUMENT:
        fprintf(wtr->fout, "@ARG\n");
        break;
    case S_THIS:
        fprintf(wtr->fout, "@THIS\n");
        break;
    case S_THAT:
        fprintf(wtr->fout, "@THAT\n");
        break;
    case S_TEMP:
        fprintf(wtr->fout, "@5\n");
        break;
    case S_CONSTANT:
        /* we never literally access the purely virtual constant segment */
        return;
    default:
        fprintf(stderr, "[ERROR] Calling %s with incompatible memory segment\n",
                __func__);
        return;
    }
}

static void access_static(struct writer* const wtr, const int16_t idx) {
    fprintf(wtr->fout, "@%s.%d\n", wtr->fname, idx);
}

static void push_pointer(struct writer* const wtr, const enum seg_t seg,
                         const int16_t idx) {
    switch (seg) {
    case S_POINTER:
        /* offset is 0 for pointer segment */
        fprintf(wtr->fout, "@0\nD=A\n");

        switch (idx) {
        case 0:
            access_segment(wtr, S_THIS);
            break;
        case 1:
            access_segment(wtr, S_THAT);
        }
        break;
    default:
        fprintf(wtr->fout, "@%d\nD=A\n", idx);
        access_segment(wtr, seg);
    }

    /* don't need any more instructions for constant segment */
    if (seg == S_CONSTANT) {
        return;
    }

    if (seg == S_TEMP || seg == S_POINTER) {
        fprintf(wtr->fout, "A=D+A\n");
    } else {
        fprintf(wtr->fout, "A=D+M\n");
    }

    fprintf(wtr->fout, "D=M\n");
}

static void push_static(struct writer* const wtr, const int16_t idx) {
    access_static(wtr, idx);

    fprintf(wtr->fout, "D=M\n");
}

static void push(struct writer* const wtr, const enum seg_t seg,
                 const int16_t idx) {
    switch (seg) {
    case S_LOCAL:
    case S_ARGUMENT:
    case S_THIS:
    case S_THAT:
    case S_TEMP:
    case S_POINTER:
    case S_CONSTANT:
        push_pointer(wtr, seg, idx);
        break;
    case S_STATIC:
        push_static(wtr, idx);
        break;
    default:
        fprintf(stderr, "[WARNING] Calling %s with error-type memory segment\n",
                __func__);
        return;
    }

    push_D(wtr);
}

static void pop_pointer(struct writer* const wtr, const enum seg_t seg,
                        const int16_t idx) {
    fprintf(wtr->fout, "@R14\nM=D\n");

    switch (seg) {
    case S_POINTER:
        /* offset is 0 for pointer segment */
        fprintf(wtr->fout, "@0\nD=A\n");

        switch (idx) {
        case 0:
            access_segment(wtr, S_THIS);
            break;
        case 1:
            access_segment(wtr, S_THAT);
        }
        break;
    default:
        fprintf(wtr->fout, "@%d\nD=A\n", idx);
        access_segment(wtr, seg);
    }

    if (seg == S_TEMP || seg == S_POINTER) {
        fprintf(wtr->fout, "D=D+A\n");
    } else {
        fprintf(wtr->fout, "D=D+M\n");
    }

    fprintf(wtr->fout, "@R15\nM=D\n@R14\nD=M\n@R15\nA=M\nM=D\n");
}

static void pop_static(struct writer* const wtr, const int16_t idx) {
    access_static(wtr, idx);

    fprintf(wtr->fout, "M=D\n");
}

static void pop(struct writer* const wtr, const enum seg_t seg,
                const int16_t idx) {
    pop_D(wtr);

    switch (seg) {
    case S_LOCAL:
    case S_ARGUMENT:
    case S_THIS:
    case S_THAT:
    case S_TEMP:
    case S_POINTER:
        pop_pointer(wtr, seg, idx);
        break;
    case S_STATIC:
        pop_static(wtr, idx);
        break;
    default:
        fprintf(stderr, "[WARNING] Calling %s with error-type memory segment\n",
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

    /* set default function name */
    wtr->curr_func = calloc(strlen(wtr->fname) + 1, sizeof(*wtr->curr_func));
    strcpy(wtr->curr_func, wtr->fname);

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
                   const enum seg_t seg, const int16_t idx) {
    if (!wtr || !wtr->fout) {
        fprintf(stderr,
                "[WARNING] Calling %s with NULL argument(s), no operation "
                "performed\n",
                __func__);
        return false;
    }

    /* make sure index is nonegative */
    if (idx < 0) {
        fprintf(stderr, "[ERROR] Stack operation with negative index\n");
        return false;
    }

    if (cmd_type != C_PUSH && cmd_type != C_POP) {
        fprintf(stderr,
                "[ERROR] Calling %s with command type that is not a "
                "stack operation\n",
                __func__);
        return false;
    }

    switch (cmd_type) {
    case C_PUSH:
        push(wtr, seg, idx);
        break;
    case C_POP:
        pop(wtr, seg, idx);
        break;
    default:
        fprintf(stderr,
                "[WARNING] Calling %s with incompatible command type, no "
                "operation performed\n",
                __func__);
        return false;
    }

    return true;
}

bool writer_put_branch(struct writer* const wtr, const enum cmd_t cmd_type,
                       const char* const label) {
    if (!wtr || !wtr->fout) {
        fprintf(stderr,
                "[WARNING] Calling %s with NULL argument(s), no operation "
                "performed\n",
                __func__);
        return false;
    }

    switch (cmd_type) {
    case C_LABEL:
        fprintf(wtr->fout, "(%s$%s)\n", wtr->curr_func, label);
        break;
    case C_GOTO:
        fprintf(wtr->fout, "@%s$%s\n0;JMP\n", wtr->curr_func, label);
        break;
    case C_IF:
        pop_D(wtr);
        fprintf(wtr->fout, "@%s$%s\nD;JNE\n", wtr->curr_func, label);
        break;
    default:
        fprintf(
            stderr,
            "[ERROR] Unknown branching command type at %s:%s with label %s\n",
            wtr->fname, wtr->curr_func, label);
        return false;
    }

    return true;
}

bool writer_put_func(struct writer* const wtr, const char* const label,
                     const int16_t nvars) {
    if (!wtr || !wtr->fout) {
        fprintf(stderr,
                "[WARNING] Calling %s with NULL argument(s), no operation "
                "performed\n",
                __func__);
        return false;
    }

    /* inject function entry label into code */
    fprintf(wtr->fout, "(%s)\n", label);

    /* initialize local variables */
    for (int16_t i = 0; i < nvars; ++i) {
        push(wtr, S_CONSTANT, 0);
    }

    /* update current function for use in local label generation */
    free(wtr->curr_func);
    wtr->curr_func = calloc(strlen(label) + 1, sizeof(*wtr->curr_func));
    strcpy(wtr->curr_func, label);

    return true;
}

bool writer_put_return(struct writer* const wtr) {
    if (!wtr || !wtr->fout) {
        fprintf(stderr,
                "[WARNING] Calling %s with NULL argument(s), no operation "
                "performed\n",
                __func__);
        return false;
    }

    /* reposition the return value for the caller */
    pop_D(wtr);
    fprintf(wtr->fout, "@ARG\nA=M\nM=D\n");

    /* reposition SP for the caller */
    fprintf(wtr->fout, "@ARG\nD=M+1\n@SP\nM=D\n");

    /* restore segment pointers from stack frame */
    fprintf(wtr->fout, "@LCL\nD=M\n@R13\nM=D-1\nA=M\nD=M\n@THAT\nM=D\n");
    fprintf(wtr->fout, "@R13\nM=M-1\nA=M\nD=M\n@THIS\nM=D\n");
    fprintf(wtr->fout, "@R13\nM=M-1\nA=M\nD=M\n@ARG\nM=D\n");
    fprintf(wtr->fout, "@R13\nM=M-1\nA=M\nD=M\n@LCL\nM=D\n");

    /* go to the return address */
    fprintf(wtr->fout, "@R13\nM=M-1\nA=M\nA=M\n0;JMP\n");

    return true;
}
