/**
 * @file parser.h
 * @author Vincent Marias <vmarias@mines.edu>
 * @date 03/19/2024
 *
 * @desc This file is part of the VMTranslator program This module handles the
 * parsing of a single .vm file. The Parser provides servies for reading a VM
 * command, unpacking the command into its various components, and providing
 * convenient access to these components.
 *
 * @copyright Vincent Marias 2024
 */

#ifndef VM_TRANSLATOR_PARSER_H
#define VM_TRANSLATOR_PARSER_H

/* standard library headers */
#define _POSIX_C_SOURCE 200809L
#include <stdbool.h> /* for bool */
#include <stdint.h>  /* for int16_t */

/* >>>>>>>>>>>>>>>>>>> */
/* Types and Constants */
/* <<<<<<<<<<<<<<<<<<< */

/* handles the memory associated with a single open input stream */
struct parser;

/* represents the type of a VM command */
enum cmd_t {
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL
};

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
/* (Public) Subroutine Declarations */
/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

/**
 * @desc Creates a new Parser by opening the given file.
 *
 * @param[in] fname path to the file to be opened
 * @return pointer to newly allocated Parser, or NULL on error
 *
 * @note The argument to fname can be a regular file or a stream.
 * @note The returned Parser should be freed with parser_free by the caller.
 */
struct parser* parser_alloc(const char* const fname);

/**
 * @desc Frees the memory associated with a Parser. Additionally closes the
 * associated file if it's still open.
 *
 * @param[out] psr pointer to a Parser previously allocated using parser_alloc
 *
 * @note The memory pointed to by psr should not be accessed after a call to
 * this routine.
 */
void parser_free(struct parser* const psr);

/**
 * @desc Queries whether there are any lines left to be parsed in the input.
 *
 * @param[in] psr pointer to a Parser to query
 * @return true if there more lines to parse, else false
 */
bool parser_more_lines(const struct parser* const psr);

/**
 * @desc Reads the next command from the input and makes it the currect command.
 *
 * @param[in,out] psr pointer to a Parser to advance
 *
 * @note This routine should be called only if parser_more_lines returns true.
 * @note Initially there is no current command.
 */
void parser_advance(struct parser* const psr);

/**
 * @desc Queries the types of the current command.
 *
 * @param[in] psr pointer to a Parser to query
 * @return enum representing the type of the current command
 *
 * @note If the current command is an arithmetic-logical command, returns
 * C_ARITHMETIC.
 */
enum cmd_t parser_command_type(const struct parser* const psr);

/**
 * @desc Queries the first argument of the current command.
 *
 * @param[in] psr pointer to a Parser to query
 * @return a string representing the first argument of the current command
 *
 * @note In the case of C_ARITHMETIC, the command itself ("add", "sub", etc.) is
 * returned.
 * @note Should not be called if the current command is C_RETURN.
 * @note No guarantees are made about the lifetime of the returned memory after
 * subsequent calls to Parser routines. If the information is needed after such
 * calls, the caller should first make a copy.
 */
const char* parser_arg1(const struct parser* const psr);

/**
 * @desc Queries the second argument of the current command.
 *
 * @param[in] psr pointer to a Parser to query
 * @return an integer representing the second argument of the current command
 *
 * @note Should be called only if the current command is C_PUSH, C_POP,
 * C_FUNCTION, or C_CALL.
 */
int16_t parser_arg2(const struct parser* const psr);

#endif /* VM_TRANSLATOR_PARSER_H */
