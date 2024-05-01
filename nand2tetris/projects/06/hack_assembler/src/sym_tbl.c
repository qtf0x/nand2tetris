/**
 * @file sym_tbl.c
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

#define _POSIX_C_SOURCE 200809L
#include <stdint.h> // for UINT16_MAX
#include <stdlib.h> // for calloc, size_t
#include <string.h> // for strdup, strlen

#include "sym_tbl.h"

const uint16_t SYM_TBL_NPOS = UINT16_MAX;

/* the maximum number of symbols we might need to map */
static const uint16_t TBL_CAP = (1 << 14) + 7;

typedef struct bucket {
    char* symbol;
    uint16_t address;
    struct bucket* next;
} bucket;

struct sym_tbl {
    bucket** buckets;
    bool initialized;
    uint16_t nvars;
};

sym_tbl* sym_tbl_alloc() {
    sym_tbl* tbl = (sym_tbl*)malloc(sizeof(sym_tbl));

    /* note that calloc zeros out the memory */
    tbl->buckets = (bucket**)calloc(TBL_CAP, sizeof(bucket*));
    tbl->initialized = true;
    tbl->nvars = 0;

    /* add predefied symbols to the table */
    sym_tbl_insert(tbl, "R0", 0);
    sym_tbl_insert(tbl, "R1", 1);
    sym_tbl_insert(tbl, "R2", 2);
    sym_tbl_insert(tbl, "R3", 3);
    sym_tbl_insert(tbl, "R4", 4);
    sym_tbl_insert(tbl, "R5", 5);
    sym_tbl_insert(tbl, "R6", 6);
    sym_tbl_insert(tbl, "R7", 7);
    sym_tbl_insert(tbl, "R8", 8);
    sym_tbl_insert(tbl, "R9", 9);
    sym_tbl_insert(tbl, "R10", 10);
    sym_tbl_insert(tbl, "R11", 11);
    sym_tbl_insert(tbl, "R12", 12);
    sym_tbl_insert(tbl, "R13", 13);
    sym_tbl_insert(tbl, "R14", 14);
    sym_tbl_insert(tbl, "R15", 15);

    sym_tbl_insert(tbl, "SP", 0);
    sym_tbl_insert(tbl, "LCL", 1);
    sym_tbl_insert(tbl, "ARG", 2);
    sym_tbl_insert(tbl, "THIS", 3);
    sym_tbl_insert(tbl, "THAT", 4);

    sym_tbl_insert(tbl, "SCREEN", 16384);
    sym_tbl_insert(tbl, "KBD", 24576);

    return tbl;
}

void sym_tbl_free(sym_tbl* tbl) {
    if (!tbl || !tbl->initialized) {
        return;
    }

    for (size_t i = 0; i < TBL_CAP; ++i) {
        /* need to free all nodes in conflict chain */
        for (bucket* next_bckt = tbl->buckets[i]; next_bckt;) {
            free(next_bckt->symbol);

            bucket* last_bckt = next_bckt;
            next_bckt = next_bckt->next;
            free(last_bckt);
        }
    }

    free(tbl->buckets);
    free(tbl);
    tbl = NULL;
}

/* why doesn't this exist in <math.h>????? */
static uint16_t powu16(const uint16_t base, const uint16_t exp) {
    uint16_t result = 1;

    for (uint16_t i = 0; i < exp; ++i) {
        result *= base;
    }

    return result;
}

static uint16_t hash(const char* const sym) {
    /* polynomial rolling hash
     * [https://cp-algorithms.com/string/string-hashing.html] */

    uint16_t hash = 0;
    const uint16_t p = 67; /* prime number closest to our alphabet size */

    for (size_t i = 0; i < strlen(sym); ++i) {
        hash += (uint16_t)(sym[i] * powu16(p, (uint16_t)i));
    }

    return hash % TBL_CAP;
}

bool sym_tbl_insert(sym_tbl* const tbl, const char* const sym,
                    const uint16_t addr) {
    /* note that we don't allow duplicate keys (obv) */
    if (!tbl || !sym || sym_tbl_lookup(tbl, sym) != SYM_TBL_NPOS) {
        return false;
    }

    /* create the new bucket */
    bucket* b = malloc(sizeof(bucket));
    b->symbol = malloc(strlen(sym) + 1);
    strcpy(b->symbol, sym);
    b->address = addr;

    /* figure out where to put it */
    uint16_t idx = hash(sym);

    /* insert into table (chain on collisions) */
    b->next = tbl->buckets[idx];
    tbl->buckets[idx] = b;

    return true;
}

uint16_t sym_tbl_lookup(const sym_tbl* const tbl, const char* const sym) {
    if (!tbl || !sym) {
        return SYM_TBL_NPOS;
    }

    uint16_t idx = hash(sym);

    /* scan linked list for matching symbol */
    bucket* tmp = tbl->buckets[idx];
    while (tmp && strcmp(tmp->symbol, sym) != 0) {
        tmp = tmp->next;
    }

    /* reached end of list before finding match */
    if (!tmp) {
        return SYM_TBL_NPOS;
    }

    return tmp->address;
}
