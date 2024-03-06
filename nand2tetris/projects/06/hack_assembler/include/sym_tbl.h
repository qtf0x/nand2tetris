/**
 * @file sym_tbl.h
 * @author Vincent Marias <vmarias@mines.edu>
 * @date 03/05/2024
 *
 * @brief This file is part of the HackAssember program, an assembler for the
 * Hack architecture, as described in "The Elements of Computing Systems", 2nd
 * Ed. by Nisan and Schocken. This module defines a data structure to be used as
 * the symbol table during parsing, along with subroutines for inserting new
 * (key, value) pairs into the table and performing lookups into the table.
 *
 * @copyright Vincent Marias, 2024
 */

#ifndef HACK_ASSEMBLER_SYM_TBL_H
#define HACK_ASSEMBLER_SYM_TBL_H

#define _POSIX_C_SOURCE 200809L
#include <stdbool.h> // for bool
#include <stdint.h>  // for uint16_t

/* indicates a failed table lookup */
extern const uint16_t SYM_TBL_NPOS;

typedef struct sym_tbl sym_tbl;

sym_tbl* sym_tbl_alloc();

void sym_tbl_free(sym_tbl* tbl);

bool sym_tbl_insert(sym_tbl* const tbl, const char* const sym,
                    const uint16_t addr);

uint16_t sym_tbl_lookup(const sym_tbl* const tbl, const char* const sym);

#endif // HACK_ASSEMBLER_SYM_TBL_
