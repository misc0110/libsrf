#ifndef YDKJEXTRACTOR_JSON_H
#define YDKJEXTRACTOR_JSON_H
#include <stdio.h>

typedef struct {
    char* data;
    size_t capacity;
    size_t pos;
    size_t flags;

    int requires_sep;
} libsrf_json_t;

#define LIBSRF_JSON_FLAG_PRETTY 1


libsrf_json_t* libsrf_json_create();
void libsrf_json_destroy(libsrf_json_t* json);

void libsrf_json_begin_object(libsrf_json_t* json);
void libsrf_json_end_object(libsrf_json_t* json);
void libsrf_json_begin_array(libsrf_json_t* json);
void libsrf_json_end_array(libsrf_json_t* json);
void libsrf_json_key(libsrf_json_t* json, const char* key);
void libsrf_json_key_int(libsrf_json_t* json, ssize_t key);

void libsrf_json_write_int(libsrf_json_t* json, ssize_t val);
void libsrf_json_write_uint(libsrf_json_t* json, size_t val);
void libsrf_json_write_float(libsrf_json_t* json, double val);
void libsrf_json_write_string(libsrf_json_t* json, const char* val);

char* libsrf_json_to_string(libsrf_json_t* json);

void libsrf_json_dump(libsrf_json_t* json);

#endif //YDKJEXTRACTOR_JSON_H
