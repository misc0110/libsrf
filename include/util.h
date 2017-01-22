#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <stdint.h>


uint16_t libsrf_swap16(uint16_t num);

uint32_t libsrf_swap32(uint32_t num);

float libsrf_swap_float(float inFloat);

#if 0
char *readEntry(FileEntry *entry, FILE *f);
void dumpRaw(FileEntry* entry, FILE* f, char* name, char* output);
#endif

#endif
