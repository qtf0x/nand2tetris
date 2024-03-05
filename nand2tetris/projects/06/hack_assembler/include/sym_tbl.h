#ifndef HACK_ASSEMBLER_SYM_TBL_H
#define HACK_ASSEMBLER_SYM_TBL_H

#include <stdbool.h> // for bool
#include <stdint.h>  // for uint16_t

extern const uint16_t SYM_TBL_NPOS;

typedef struct sym_tbl sym_tbl;

sym_tbl* sym_tbl_alloc();

void sym_tbl_free(sym_tbl* tbl);

bool sym_tbl_insert(sym_tbl* const tbl, const char* const sym,
                    const uint16_t addr);

uint16_t sym_tbl_lookup(const sym_tbl* const tbl, const char* const sym);

#endif // HACK_ASSEMBLER_SYM_TBL_
