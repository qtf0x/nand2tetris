#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"

regmatch_t* match_regex(const char* const str, const char* const ptrn,
                        size_t* nsub) {
    if (!str || !ptrn || !nsub) {
        return NULL;
    }

    regex_t regex;
    int reg_err = 0;
    regmatch_t* pmatch = NULL;

    if ((reg_err = regcomp(&regex, ptrn, REG_EXTENDED))) {
        size_t errbuf_size = regerror(reg_err, &regex, NULL, 0);
        char* errbuf = malloc(errbuf_size * sizeof(char));
        regerror(reg_err, &regex, errbuf, errbuf_size);

        fprintf(stderr,
                "[ERROR] Regular expression \"%s\" failed to compile with "
                "error:\n\t%s\n",
                ptrn, errbuf);

        free(errbuf);
        goto EXIT;
    }

    *nsub = regex.re_nsub;
    pmatch = calloc(*nsub + 1, sizeof(regmatch_t));

    if (regexec(&regex, str, *nsub + 1, pmatch, 0) == REG_NOMATCH) {
        free(pmatch);
        pmatch = NULL;
    }

EXIT:
    regfree(&regex);
    return pmatch;
}

instruction* parse_instr(const char* const line, const uint16_t line_num) {
    instruction* instr = malloc(sizeof(instruction));
    instr->line_number = line_num;
    instr->next = NULL;

    regmatch_t* pmatch = NULL;
    size_t nsub = 0;
    regoff_t off = 0, len = 0;

    /* note that the spec only declares that leading space characters are
     * ignored - I am choosing to also ignore leading and trailing space and tab
     * characters */

    if ((pmatch = match_regex(line, "^[ \t\v]*@([0-9]+)[ \t\v]*(\n|\r\n|\r)$",
                              &nsub))) {
        /* value = pmatch[1]
         * */
        if (nsub < 1) {
            goto NOMATCH_EXIT;
        }

        off = pmatch[1].rm_so;
        len = pmatch[1].rm_eo - pmatch[1].rm_so;
        char* str = calloc(len + 1, sizeof(char));
        strncpy(str, line + off, len);

        /* A-instruction with constant address value */
        instr->type = A_INSTR;
        instr->resolved = true;
        instr->addr = strtoul(str, NULL, 10);
        free(str);
    } else if ((pmatch = match_regex(
                    line,
                    "^[ \t\v]*@([a-zA-Z_\\.\\$:][a-zA-Z0-9_\\.\\$:]*)[ "
                    "\t\v]*(\n|\r\n|\r)$",
                    &nsub))) {
        /* addr = pmatch[1]
         * */
        if (nsub < 1) {
            goto NOMATCH_EXIT;
        }

        off = pmatch[1].rm_so;
        len = pmatch[1].rm_eo - pmatch[1].rm_so;

        /* A-instruction with symbol */
        instr->type = A_INSTR;
        instr->resolved = false;
        instr->symbol = calloc(len + 1, sizeof(char));
        strncpy(instr->symbol, line + off, len);
    } else if ((pmatch = match_regex(
                    line,
                    "^[ "
                    "\t\v]*((null|ADM|AD?M?|DM?|M)=)?(0|-?1|[-!]?[ADM]|[ADM]["
                    "\\+-]1|D[-\\+&\\|][AM]|[AM]-D)(;(null|J([GL][TE]|EQ|NE|MP)"
                    "))?[ \t\v]*(\n|\r\n|\r)$",
                    &nsub))) {
        /* dest = pmatch[2]
         * comp = pmatch[3]
         * jump = pmatch[5]
         */
        if (nsub < 5) {
            goto NOMATCH_EXIT;
        }

        instr->type = C_INSTR;

        /* C-istruction */
        off = pmatch[2].rm_so;
        len = pmatch[2].rm_eo - pmatch[2].rm_so;
        instr->dest = calloc(len + 1, sizeof(char));
        strncpy(instr->dest, line + off, len);

        off = pmatch[3].rm_so;
        len = pmatch[3].rm_eo - pmatch[3].rm_so;
        instr->comp = calloc(len + 1, sizeof(char));
        strncpy(instr->comp, line + off, len);

        off = pmatch[5].rm_so;
        len = pmatch[5].rm_eo - pmatch[5].rm_so;
        instr->jump = calloc(len + 1, sizeof(char));
        strncpy(instr->jump, line + off, len);
    } else if ((pmatch = match_regex(
                    line,
                    "^[ "
                    "\t\v]*\\(([a-zA-Z_\\.\\$:][a-zA-Z0-9_\\.\\$:]*)\\)[ "
                    "\t\v]*(\n|\r\n|\r)$",
                    &nsub))) {
        /* sym = pmatch[1]
         */
        if (nsub < 1) {
            goto NOMATCH_EXIT;
        }

        off = pmatch[1].rm_so;
        len = pmatch[1].rm_eo - pmatch[1].rm_so;

        /* L-instructions */
        instr->type = L_INSTR;
        instr->resolved = false;
        instr->symbol = calloc(len + 1, sizeof(char));
        strncpy(instr->symbol, line + off, len);
    } else if ((pmatch = match_regex(
                    line, "^[ \t\v]*((//.*)|([ \t\v]*))(\n|\r\n|\r)$",
                    &nsub))) {
        /* comments and "empty" lines */
        instr->type = COMMENT_IGNORE;
    } else {
        free(instr);
        instr = NULL;
    }

    free(pmatch);
    return instr;

NOMATCH_EXIT:
    fprintf(stderr,
            "[ERROR] Regex parsing failed on input line %u with incorrect "
            "number of captured subexpressions (%zu)\n",
            line_num, nsub);
    free(pmatch);
    return NULL;
}

bool resolve_reference(instruction* const instr, sym_tbl* const tbl,
                       uint16_t* const nvars) {
    /* series of checks will short-circuit before accessing undefined memory
     */
    if (!instr || !tbl || (instr->type != L_INSTR && instr->type != A_INSTR) ||
        instr->resolved) {
        return false;
    }

    /* note we're deciding to use a different alternative in the union */
    if (sym_tbl_insert(tbl, instr->symbol, *nvars)) {
        instr->addr = (*nvars)++;
    } else {
        instr->addr = sym_tbl_lookup(tbl, instr->symbol);
    }

    return (instr->resolved = true);
}
