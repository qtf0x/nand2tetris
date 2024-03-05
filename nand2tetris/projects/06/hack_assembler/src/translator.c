#include <stddef.h> // for NULL
#include <string.h> // for strlen, size_t

#include "translator.h"

char* translate_dest(const char* const dest) {
    if (!dest) {
        return NULL;
    }

    /* default for empty string */
    if (strlen(dest) == 0) {
        return "000";
    }

    switch (strlen(dest)) {
    case 1:
        switch (dest[0]) {
        case 'A':
            return "100";
        case 'D':
            return "010";
        case 'M':
            return "001";
        }
        break;
    case 2:
        switch (dest[0]) {
        case 'A':
            switch (dest[1]) {
            case 'M':
                return "101";
            case 'D':
                return "110";
            }
            break;
        case 'D':
            return "011";
        }
        break;
    case 3:
        return "111";
    case 4:
        return "000";
    }

    return NULL;
}

char* translate_comp(const char* const comp) {
    if (!comp || strlen(comp) == 0) {
        return NULL;
    }

    switch (strlen(comp)) {
    case 1:
        switch (comp[0]) {
        case '0':
            return "0101010";
        case '1':
            return "0111111";
        case 'D':
            return "0001100";
        case 'A':
            return "0110000";
        case 'M':
            return "1110000";
        }
        break;
    case 2:
        switch (comp[0]) {
        case '-':
            switch (comp[1]) {
            case '1':
                return "0111010";
            case 'D':
                return "0001111";
            case 'A':
                return "0110011";
            case 'M':
                return "1110011";
            }
            break;
        case '!':
            switch (comp[1]) {
            case 'D':
                return "0001101";
            case 'A':
                return "0110001";
            case 'M':
                return "1110001";
            }
        }
        break;
    case 3:
        switch (comp[0]) {
        case 'D':
            switch (comp[1]) {
            case '+':
                switch (comp[2]) {
                case '1':
                    return "0011111";
                case 'A':
                    return "0000010";
                case 'M':
                    return "1000010";
                }
                break;
            case '-':
                switch (comp[2]) {
                case '1':
                    return "0001110";
                case 'A':
                    return "0010011";
                case 'M':
                    return "1010011";
                }
                break;
            case '&':
                switch (comp[2]) {
                case 'A':
                    return "0000000";
                case 'M':
                    return "1000000";
                }
                break;
            case '|':
                switch (comp[2]) {
                case 'A':
                    return "0010101";
                case 'M':
                    return "1010101";
                }
            }
            break;
        case 'A':
            switch (comp[1]) {
            case '+':
                return "0110111";
            case '-':
                switch (comp[2]) {
                case '1':
                    return "0110010";
                case 'D':
                    return "0000111";
                }
            }
            break;
        case 'M':
            switch (comp[1]) {
            case '+':
                return "1110111";
            case '-':
                switch (comp[2]) {
                case '1':
                    return "1110010";
                case 'D':
                    return "1000111";
                }
            }
        }
    }

    return NULL;
}

char* translate_jump(const char* const jump) {
    if (!jump) {
        return NULL;
    }

    /* default for empty string */
    if (strlen(jump) == 0) {
        return "000";
    }

    switch (jump[0]) {
    case 'n':
        return "000";
    default:
        switch (jump[1]) {
        case 'G':
            switch (jump[2]) {
            case 'T':
                return "001";
            case 'E':
                return "011";
            }
            break;
        case 'E':
            return "010";
        case 'L':
            switch (jump[2]) {
            case 'T':
                return "100";
            case 'E':
                return "110";
            }
            break;
        case 'N':
            return "101";
        case 'M':
            return "111";
        }
    }

    return NULL;
}

void translate_val(const uint16_t val, char str[16]) {
    uint16_t rest = val;

    for (size_t i = 0; i < 16; ++i) {
        size_t j = 15 - i; /* bits do be backward tho */
        str[j] = (rest % 2 ? '1' : '0');
        rest /= 2;
    }
}
