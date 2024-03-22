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
#include <stdbool.h> /* for bool, true, false */
#include <stddef.h>  /* for NULL, size_t */
#include <stdint.h>  /* for int16_t */
#include <stdio.h> /* for FILE, fopen, perror, fclose, getline, fprintf, stderr*/
#include <stdlib.h> /* for malloc, free */
#include <string.h> /* for strlen, strcpy, strcmp, strtok */

/* POSIX headers */
#include <sys/types.h> /* for ssize_t */

/* project-specific modules */
#include "parser.h"

/* >>>>>>>>>>>>>>>>>>> */
/* Types and Constants */
/* <<<<<<<<<<<<<<<<<<< */

struct command {
    enum cmd_t command;
    union arg_t arg1;
    int16_t arg2; /* optional */
};

struct parser {
    FILE* fin;
    struct command curr_cmd, next_cmd;
    bool has_lines;
};

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
/* (Private) Subroutine Definitions */
/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

static enum cmd_t get_command_type(const char* const str) {
    if (!str) {
        return C_ERROR;
    }

    if (!strcmp(str, "push")) {
        return C_PUSH;
    }

    if (!strcmp(str, "pop")) {
        return C_POP;
    }

    if (!strcmp(str, "add") || !strcmp(str, "sub") || !strcmp(str, "neg") ||
        !strcmp(str, "eq") || !strcmp(str, "gt") || !strcmp(str, "lt") ||
        !strcmp(str, "and") || !strcmp(str, "or") || !strcmp(str, "not")) {
        return C_ARITHMETIC;
    }

    return C_ERROR;
}

static enum op_t get_op(const char* const str) {
    if (!str) {
        return O_ERROR;
    }

    if (!strcmp(str, "add")) {
        return O_ADD;
    }
    if (!strcmp(str, "sub")) {
        return O_SUB;
    }
    if (!strcmp(str, "neg")) {
        return O_NEG;
    }
    if (!strcmp(str, "eq")) {
        return O_EQ;
    }
    if (!strcmp(str, "gt")) {
        return O_GT;
    }
    if (!strcmp(str, "lt")) {
        return O_LT;
    }
    if (!strcmp(str, "and")) {
        return O_AND;
    }
    if (!strcmp(str, "or")) {
        return O_OR;
    }
    if (!strcmp(str, "not")) {
        return O_NOT;
    }

    return O_ERROR;
}

static enum seg_t get_seg(const char* const str) {
    if (!str) {
        return S_ERROR;
    }

    if (!strcmp(str, "argument")) {
        return S_ARGUMENT;
    }
    if (!strcmp(str, "local")) {
        return S_LOCAL;
    }
    if (!strcmp(str, "static")) {
        return S_STATIC;
    }
    if (!strcmp(str, "constant")) {
        return S_CONSTANT;
    }
    if (!strcmp(str, "this")) {
        return S_THIS;
    }
    if (!strcmp(str, "that")) {
        return S_THAT;
    }
    if (!strcmp(str, "pointer")) {
        return S_POINTER;
    }
    if (!strcmp(str, "temp")) {
        return S_TEMP;
    }

    return S_ERROR;
}

static int parse_line(const char* const line, struct command* const cmd) {
    if (!line || !cmd) {
        fprintf(
            stderr,
            "[WARNING] %s called with NULL argument, no operation performed\n",
            __func__);
        return false;
    }

    int result = 1;

    char* line_cpy = calloc(strlen(line) + 1, sizeof(*line_cpy));
    if (!line_cpy) {
        perror("[ERROR] calloc");
        result = -1;
        goto EXIT;
    }
    strcpy(line_cpy, line);

    const char* token = strtok(line_cpy, " \f\n\r\t\v");

    /* check for empty lines and comments */
    if (!token || !strncmp(token, "//", strlen("//"))) {
        result = 0;
        goto EXIT;
    }

    enum cmd_t cmd_type = get_command_type(token);
    if (cmd_type == C_ERROR) {
        fprintf(stderr,
                "[ERROR] Syntax error: Command type \"%s\" not recognized\n",
                token);
        result = -1;
        goto EXIT;
    }

    cmd->command = cmd_type;

    switch (cmd->command) {
    case C_ARITHMETIC: {
        enum op_t op_type = get_op(token);
        if (op_type == O_ERROR) {
            fprintf(
                stderr,
                "[ERROR] Syntax error: Operation type \"%s\" not recognized\n",
                token);
            result = -1;
            goto EXIT;
        }

        cmd->arg1.operation = op_type;
        break;
    }
    case C_PUSH:
    case C_POP: {
        token = strtok(NULL, " \f\n\r\t\v");
        enum seg_t seg_type = get_seg(token);
        if (seg_type == S_ERROR) {
            fprintf(
                stderr,
                "[ERROR] Syntax error: Memory segment \"%s\" not recognized\n",
                token);
            result = -1;
            goto EXIT;
        }

        cmd->arg1.segment = seg_type;

        token = strtok(NULL, " \f\n\r\t\v");
        if (!token) {
            fprintf(
                stderr,
                "[ERROR] Syntax error: Invalid/missing memory index \"%s\"\n",
                token);
            result = -1;
            goto EXIT;
        }

        cmd->arg2 = atoi(token);
        break;
    }
    default:
        fprintf(stderr, "[ERROR] Command type unimplemented\n");
        result = -1;
        goto EXIT;
    }

EXIT:
    free(line_cpy);
    return result;
}

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
/* (Public) Subroutine Definitions */
/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

struct parser* parser_alloc(const char* const fname) {
    if (!fname) {
        return NULL;
    }

    /* attempt to open the file */
    FILE* fin = fopen(fname, "r");
    if (!fin) {
        perror("[ERROR] fopen");
        return NULL;
    }

    /* attempt to create the Parser */
    struct parser* psr = malloc(sizeof(*psr));
    if (!psr) {
        perror("[ERROR] malloc");
        if (fclose(fin)) {
            perror("[ERROR] fclose");
        }
        return NULL;
    }

    /* assign fields, get first line from file */
    psr->fin = fin;
    psr->has_lines = true;
    parser_advance(psr);

    return psr;
}

void parser_free(struct parser* const psr) {
    if (!psr) {
        return;
    }

    /* attempt to close the file if it's open */
    if (psr->fin && fclose(psr->fin)) {
        perror("[ERROR] fclose");
        psr->fin = NULL;
    }

    /* just in case :) */
    psr->has_lines = false;

    free(psr);
}

bool parser_has_lines(const struct parser* const psr) {
    if (!psr) {
        return false;
    }

    return psr->has_lines;
}

void parser_advance(struct parser* const psr) {
    if (!psr) {
        fprintf(
            stderr,
            "[WARNING] %s called with NULL argument, no operation performed\n",
            __func__);
        return;
    }

    struct command next_cmd;

    char* line = NULL;
    size_t line_sz = 0;
    ssize_t gl_return = 0;
    int pl_return = 0;

    while ((gl_return = getline(&line, &line_sz, psr->fin)) != -1) {
        pl_return = parse_line(line, &next_cmd);
        free(line);
        line = NULL;

        if (pl_return == 1) {
            psr->curr_cmd = psr->next_cmd;
            psr->next_cmd = next_cmd;
            break;
        } else if (pl_return == -1) {
            break;
        }
    }

    /* memory allocated by getline must be freed even on failure */
    free(line);

    if (gl_return == -1 || pl_return == -1) {
        /* do final swap */
        psr->curr_cmd = psr->next_cmd;
        psr->has_lines = false;
    }
}

enum cmd_t parser_command_type(const struct parser* const psr) {
    if (!psr) {
        fprintf(stderr, "[WARNING] %s called with NULL argument, returning 0\n",
                __func__);
        return 0;
    }

    return psr->curr_cmd.command;
}

union arg_t parser_arg1(const struct parser* const psr) {
    if (!psr) {
        fprintf(stderr,
                "[WARNING] %s called with NULL argument, returning O_ADD\n",
                __func__);
        return (union arg_t){.operation = O_ADD};
    }

    return psr->curr_cmd.arg1;
}

int16_t parser_arg2(const struct parser* const psr) {
    if (!psr) {
        fprintf(stderr, "[WARNING] %s called with NULL argument, returning 0\n",
                __func__);
        return 0;
    }

    return psr->curr_cmd.arg2;
}
