/**
 * @file writer.h
 * @author Vincent Marias <vmarias@mines.edu>
 * @date 03/19/2024
 *
 * @desc This file is part of the VMTranslator program This module translates a
 * parsed VM command into Hack assembly code.
 *
 * @copyright Vincent Marias 2024
 */

#ifndef VM_TRANSLATOR_WRITER_H
#define VM_TRANSLATOR_WRITER_H

/* standard library headers */
#define _POSIX_C_SOURCE 200809L
#include <stdbool.h> /* for bool */
#include <stdint.h>  /* for int16_t */

/* project-specific modules */
#include "parser.h"

/* >>>>>>>>>>>>>>>>>>> */
/* Types and Constants */
/* <<<<<<<<<<<<<<<<<<< */

/* handles the memory associated with a single open input stream */
struct writer;

/* >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> */
/* (Public) Subroutine Declarations */
/* <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */

/**
 * @desc Creates a new Writer by opening the given file.
 *
 * @param[in] fpath path to the file to be opened
 * @return pointer to newly allocated Writer, or NULL on error
 *
 * @note The argument to fpath can be a regular file or a stream.
 * @note The returned Writer should be freed with writer_free by the caller.
 */
struct writer* writer_alloc(const char* const fpath);

/**
 * @desc Frees the memory associated with a Writer. Additionally closes the
 * associated file if it's still open.
 *
 * @param[out] wtr pointer to a Writer previously allocated using writer_alloc
 *
 * @note The memory pointed to by wtr should not be accessed after a call to
 * this routine.
 */
void writer_free(struct writer* const wtr);

/**
 * @desc Writes to the output file the assembly code that implements the given
 * arithmetic-logical command.
 *
 * @param[out] wtr pointer to a Writer previously allocated using writer_alloc
 * @param[in] op the arithmetic-logical operation to effect
 * @return true on success, false on error
 */
bool writer_put_al(struct writer* const wtr, const enum op_t op);

/**
 * @desc Writes to the output file the assembly code that implements the given
 * push or pop command.
 *
 * @param[out] wtr pointer to a Writer previously allocated using writer_alloc
 * @param[in] cmd_type either C_PUSH or C_POP
 * @param[in] seg the memory segment to operate on
 * @param[in] idx index into the given memory segment
 * @return true on success, false on error
 */
bool writer_put_so(struct writer* const wtr, const enum cmd_t cmd_type,
                   const enum seg_t seg, const int16_t idx);

#endif /* VM_TRANSLATOR_WRITER_H */
