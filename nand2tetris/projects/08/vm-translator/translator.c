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
#include <dirent.h> /* for opendir, readdir */
#include <stddef.h> /* for NULL */
#include <stdio.h>  /* for fprintf, stderr */
#include <stdlib.h> /* for EXIT_FAILURE, EXIT_SUCCESS, calloc, free */
#include <string.h> /* for strrchr, strcmp, strlen, strcpy */
#include <unistd.h> /* for get_current_dir_name */

/* POSIX headers */
#include <sys/stat.h>  /* for stat, S_ISDIR */
#include <sys/types.h> /* for DIR */

/* Linux headers */
#include <linux/limits.h>

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

    /* could have trailing /, or not */
    if (*(argv[1] + strlen(argv[1]) - 1) == '/') {
        argv[1][strlen(argv[1]) - 1] = '\0';
    }

    struct stat sb;

    if (stat(argv[1], &sb) == -1) {
        perror("[ERROR] stat");
        EXIT_STATUS = EXIT_FAILURE;
        goto EXIT;
    }

    /* need the relative path prefix because readdir sucks ass */
    char* rel_path_prefix =
        calloc(strlen(argv[1]) + 2, sizeof(*rel_path_prefix));
    strcpy(rel_path_prefix, argv[1]);
    strcat(rel_path_prefix, "/");

    bool input_dir = false;
    DIR* dirfd = NULL;

    if (S_ISDIR(sb.st_mode)) { /* it's a directory */
        input_dir = true;
        dirfd = opendir(argv[1]);
    }

    /* --------------------------------------------- */
    /* Create Writer for Translation/Output Services */
    /* --------------------------------------------- */

    char* ofname = NULL;

    if (input_dir) {
        char dirname[PATH_MAX + 1] = {0};

        if (!strcmp(argv[1], ".") || !strcmp(argv[1], "..")) {
            getcwd(dirname, PATH_MAX);
        } else {
            strcpy(dirname, argv[1]);
        }

        char* ifname = strrchr(dirname, '/');

        if (ifname) {
            ++ifname;
        } else {
            ifname = dirname;
        }

        ofname = calloc(strlen(dirname) + strlen(ifname) + strlen(OUT_EXT) + 3,
                        sizeof(*ofname));
        strcpy(ofname, dirname);

        strcat(ofname, "/");
        strcat(ofname, ifname);
        strcat(ofname, ".");
        strcat(ofname, OUT_EXT);
    } else {
        /* one byte for '.', one for NUL */
        ofname = calloc(strlen(argv[1]) + strlen(OUT_EXT) + 2, sizeof(*ofname));
        strcpy(ofname, argv[1]);
        strcpy(strrchr(ofname, '.') + 1,
               OUT_EXT); /* overwrite file extension */
    }

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

    struct dirent* next_file = NULL;
    char* next_file_name = NULL;

    /* if only doing a single file, jump into the loop */
    if (!input_dir) {
        next_file_name = argv[1];
        goto PARSE_FILE;
    }

    while (dirfd && (next_file = readdir(dirfd))) {
        free(next_file_name);
        next_file_name = NULL;

        next_file_name =
            calloc(strlen(rel_path_prefix) + strlen(next_file->d_name) + 1,
                   sizeof(*next_file_name));
        strcpy(next_file_name, rel_path_prefix);
        strcat(next_file_name, next_file->d_name);

    PARSE_FILE:
        /* only parse actual vm files */
        if (!strrchr(next_file_name, '.') ||
            strcmp(strrchr(next_file_name, '.') + 1, IN_EXT)) {
            continue;
        }

        /* tell the writer that we're parsing a different file now */
        writer_set_fname(wtr, next_file_name);

        /* begin parsing */
        parser_free(psr);
        psr = NULL;
        psr = parser_alloc(next_file_name);
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
                    fprintf(
                        stderr,
                        "[ERROR] Could not write arithmetic-logical command\n");
                    EXIT_STATUS = EXIT_FAILURE;
                    goto EXIT;
                }
                break;
            case C_PUSH:
            case C_POP:
                if (!writer_put_so(wtr, cmd, parser_arg1(psr).segment,
                                   parser_arg2(psr))) {
                    fprintf(
                        stderr,
                        "[ERROR] Could not write arithmetic-logical command\n");
                    EXIT_STATUS = EXIT_FAILURE;
                    goto EXIT;
                }
                break;
            case C_LABEL:
            case C_GOTO:
            case C_IF:
                if (!writer_put_branch(wtr, cmd, parser_arg1(psr).label)) {
                    fprintf(stderr,
                            "[ERROR] Could not write branching command\n");
                    EXIT_STATUS = EXIT_FAILURE;
                    goto EXIT;
                }
                break;
            case C_FUNCTION:
                if (!writer_put_func(wtr, parser_arg1(psr).label,
                                     parser_arg2(psr))) {
                    fprintf(stderr,
                            "[ERROR] Could not write function command\n");
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
            case C_CALL:
                if (!writer_put_call(wtr, parser_arg1(psr).label,
                                     parser_arg2(psr))) {
                    fprintf(stderr, "[ERROR] Could not write call command\n");
                    EXIT_STATUS = EXIT_FAILURE;
                    goto EXIT;
                }
                break;
            default:
                fprintf(stderr, "[ERROR] I wasn't expecting that command type "
                                "just yet :/\n");
                EXIT_STATUS = EXIT_FAILURE;
                goto EXIT;
            }
        }
    }

EXIT:
    writer_free(wtr);
    parser_free(psr);

    return EXIT_STATUS;
}
