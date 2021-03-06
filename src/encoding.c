#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "encoding.h"
#include "cstring.h"


static const uint32_t upper_encoding[] = {
        0xC384, 0xC385, 0xC387, 0xC389, 0xC391, 0xC396, 0xC39C, 0xC3A1, 0xC3A0, 0xC3A2, 0xC3A4, 0xC3A3, 0xC3A5, 0xC3A7,
        0xC3A9, 0xC3A8, 0xC3AA, 0xC3AB, 0xC3AD, 0xC3AC, 0xC3AE, 0xC3AF, 0xC3B1, 0xC3B3, 0xC3B2, 0xC3B4, 0xC3B6, 0xC3B5,
        0xC3BA, 0xC3B9, 0xC3BB, 0xC3BC, 0xE280A0, 0xC2B0, 0xC2AE, 0xC2A3, 0xC2A7, 0xE280A2, 0xC2B6, 0xC39F, 0xC2AE,
        0xC2A9, 0xE284A2, 0xC2B4, 0xC2A8, 0xE289A0, 0xC386, 0xC398, 0xE2889E, 0xC2B1, 0xE289A4, 0xE289A5, 0xC2A5,
        0xC2B5, 0xE28882, 0xC39F, 0xE2888F, 0xCF80, 0xE288AB, 0xC2AA, 0xC2BA, 0xCEA9, 0xC3A6, 0xC3B8, 0xC2BF, 0xC2A1,
        0xC2AC, 0xE2889A, 0xC692, 0xE28988, 0xCE94, 0xC2AB, 0xC2BB, 0xE280A6, 0xC2A0, 0xC380, 0xC383, 0xC395, 0xC592,
        0xC593, 0xE28093, 0xE28094, 0xE2809C, 0xE2809D, 0xE28098, 0xE28099, 0xC3B7, 0xE2978A, 0xC3BF, 0xC5B8, 0xE28184,
        0xE282AC, 0xE280B9, 0xE280BA, 0xEFAC81, 0xEFAC82, 0xE280A1, 0xC2B7, 0xE2809A, 0xE2809E, 0xE280B0, 0xC382,
        0xC38A, 0xC381, 0xC38B, 0xC388, 0xC38D, 0xC38E, 0xC38F, 0xC38C, 0xC393, 0xC394, 0xEE809E, 0xC392, 0xC39A,
        0xC39B, 0xC399, 0xC4B1, 0xCB86, 0xCB9C, 0xC2AF, 0xCB98, 0xCB99, 0xCB9A, 0xC2B8, 0xCB9D, 0xCB9B, 0xCB87};


// ---------------------------------------------------------------------------
char *libsrf_encoding_to_utf8(char *string) {
    unsigned char *str = (unsigned char *) string;
    char *utf = libsrf_cstring_create();
/*
    utf = libsrf_cstring_append_char(utf, 0xef);
    utf = libsrf_cstring_append_char(utf, 0xbb);
    utf = libsrf_cstring_append_char(utf, 0xbf);
*/
    int i, len = strlen(string);
    for (i = 0; i < len; i++) {
        if (str[i] == 1) {
            utf = libsrf_cstring_append_char(utf, 0xc2);
            utf = libsrf_cstring_append_char(utf, 0xa0);
            continue;
        }
        if (str[i] >= 0x80) {
            uint32_t utf8 = upper_encoding[str[i] - 0x80];
            int idx = (utf8 > 0xffff) ? 3 : 2;
            while (idx) {
                idx--;
                utf = libsrf_cstring_append_char(utf, (utf8 >> (idx * 8)) & 0xff);
            }
        } else {
            utf = libsrf_cstring_append_char(utf, str[i]);
        }
    }

    return libsrf_cstring_move_to_char(utf);
}

// ---------------------------------------------------------------------------
char *libsrf_encoding_move_to_utf8(char *str) {
    char *utf = libsrf_encoding_to_utf8(str);
    free(str);
    return utf;
}

// ---------------------------------------------------------------------------
char *libsrf_encoding_add_bom(char *str) {
    char* utf = (char*)malloc(strlen(str) + 4);
    utf[0] = 0xef;
    utf[1] = 0xbb;
    utf[2] = 0xbf;
    strcpy(utf + 3, str);
    return utf;
}