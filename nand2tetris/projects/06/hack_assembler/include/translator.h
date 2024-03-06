/**
 * @file sym_tbl.h
 * @author Vincent Marias <vmarias@mines.edu>
 * @date 03/05/2024
 *
 * @brief This file is part of the HackAssember program, an assembler for the
 * Hack architecture, as described in "The Elements of Computing Systems", 2nd
 * Ed. by Nisan and Schocken. This module handles translating instruction
 * op-codes and value fields to their binary equivalents in the form of
 * character strings of the ASCII characters '0' and '1'.
 *
 * @copyright Vincent Marias, 2024
 */

#ifndef HACK_ASSEMBLER_TRANSLATOR_H
#define HACK_ASSEMBLER_TRANSLATOR_H

#define _POSIX_C_SOURCE 200809L
#include <stdint.h> // for uint16_t

char* translate_dest(const char* const dest);

char* translate_comp(const char* const comp);

char* translate_jump(const char* const jump);

void translate_val(const uint16_t val, char str[16]);

#endif // HACK_ASSEMBLER_TRANSLATOR_H
