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

// ---------------------------------------------------------------------------
libsrf_files_t* libsrf_to_single_file(char* data, size_t size, char* type) {
    libsrf_files_t* files = (libsrf_files_t*)malloc(sizeof(libsrf_files_t));
    files->count = 1;
    files->files = (libsrf_file_t*)malloc(sizeof(libsrf_file_t));
    files->files[0].size = size;
    files->files[0].data = data;
    strcpy(files->files[0].filetype, type);
    return files;
}

