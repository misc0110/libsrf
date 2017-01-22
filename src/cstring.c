#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "cstring.h"

typedef struct {
    size_t capacity;
    char data[];
} cstring;

#define TO_CSTR(x) ((cstring*)((size_t)(x) - sizeof(cstring)))
#define TO_CHAR(x) ((char*)(((size_t)(x)) + sizeof(cstring)))

// ---------------------------------------------------------------------------
char* libsrf_cstring_create() {
    cstring* str = (cstring*)malloc(sizeof(cstring) + CSTRING_MIN_CAPACITY);
    str->capacity = CSTRING_MIN_CAPACITY;
    memset(str->data, 0, CSTRING_MIN_CAPACITY);
    return TO_CHAR(str);
}

// ---------------------------------------------------------------------------
void libsrf_cstring_destroy(char *str) {
    free(TO_CSTR(str));
}

// ---------------------------------------------------------------------------
size_t libsrf_cstring_get_capacity(char *str) {
    return TO_CSTR(str)->capacity;
}

// ---------------------------------------------------------------------------
char* libsrf_cstring_set_capacity(char *str, size_t capacity) {
    cstring* c = TO_CSTR(str);
    if(capacity <= c->capacity) return str;
    c = (cstring*)realloc(c, capacity + sizeof(cstring));
    c->capacity = capacity;
    return TO_CHAR(c);
}

// ---------------------------------------------------------------------------
char* libsrf_cstring_append(char *orig, char *append) {
    size_t new_capacity = strlen(orig) + strlen(append) + 1;
    orig = libsrf_cstring_set_capacity(orig, new_capacity);
    strcpy(orig + strlen(orig), append);
    return orig;
}

// ---------------------------------------------------------------------------
char* libsrf_cstring_append_unicode(char *orig, uint16_t c) {
    size_t len = strlen(orig);
    size_t new_capacity = len + 2 + 1;
    orig = libsrf_cstring_set_capacity(orig, new_capacity);
    orig[len] = (char)(c & 0xff);
    orig[len + 1] = (char)((c >> 8) & 0xff);
    orig[len + 2] = 0;
    return orig;
}

// ---------------------------------------------------------------------------
char* libsrf_cstring_append_char(char *orig, char c) {
    size_t len = strlen(orig);
    size_t new_capacity = len + 1 + 1;
    orig = libsrf_cstring_set_capacity(orig, new_capacity);
    orig[len] = c;
    orig[len + 1] = 0;
    return orig;
}

// ---------------------------------------------------------------------------
char* libsrf_cstring_to_char(char *str) {
    char* content = malloc(strlen(str) + 1);
    memcpy(content, str, strlen(str) + 1);
    return content;
}

// ---------------------------------------------------------------------------
char* libsrf_cstring_move_to_char(char *str) {
    char* content = libsrf_cstring_to_char(str);
    libsrf_cstring_destroy(str);
    return content;
}