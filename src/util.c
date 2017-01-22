#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------
uint32_t libsrf_swap32(uint32_t num) {
 return ((num>>24)&0xff) | 
            ((num<<8)&0xff0000) | 
            ((num>>8)&0xff00) | 
            ((num<<24)&0xff000000); 
}

// ---------------------------------------------------------------------------
uint16_t libsrf_swap16(uint16_t num) {
  return (num>>8) | (num<<8);   
}

// ---------------------------------------------------------------------------
float libsrf_swap_float(float inFloat) {
   float retVal;
   char *floatToConvert = (char*)&inFloat;
   char *returnFloat = (char*)&retVal;

   // swap the bytes into a temporary buffer
   int i;
   for(i = 0; i < sizeof(float); i++) {
    returnFloat[i] = floatToConvert[sizeof(float) - i - 1];
   }

   return retVal;
}

#if 0
// ---------------------------------------------------------------------------
char* readEntry(FileEntry* entry, FILE* f) {
    size_t cur = ftell(f);
    fseek(f, swap32(entry->offset), SEEK_SET);
    char* res = (char*)malloc(swap32(entry->length));
    fread(res, sizeof(char), swap32(entry->length), f);
    fseek(f, cur, SEEK_SET);
    return res;
}

// ---------------------------------------------------------------------------
void dumpRaw(FileEntry* entry, FILE* f, char* name, char* output) {
    char filename[128];
    char *content = readEntry(entry, f);
    sprintf(filename, "%s/%s%d.txt", output, name, swap32(entry->id));
    FILE *o = fopen(filename, "wb");
    fwrite(content, sizeof(char), swap32(entry->length), o);
    fclose(o);
    free(content);
}

#endif
