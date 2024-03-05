#ifndef HACK_ASSEMBLER_TRANSLATOR_H
#define HACK_ASSEMBLER_TRANSLATOR_H

#include <stdint.h> // for uint16_t

char* translate_dest(const char* const dest);

char* translate_comp(const char* const comp);

char* translate_jump(const char* const jump);

void translate_val(const uint16_t val, char str[16]);

#endif // HACK_ASSEMBLER_TRANSLATOR_H
