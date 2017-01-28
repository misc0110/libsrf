#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "libsrf.h"
#include "formats/json.h"

// ---------------------------------------------------------------------------
libsrf_json_t* libsrf_json_create() {
    libsrf_json_t* json = (libsrf_json_t*)malloc(sizeof(libsrf_json_t));
    json->flags = 0; // LIBSRF_JSON_FLAG_PRETTY;
    json->pos = 0;
    json->capacity = 256;
    json->data = (char*)calloc(json->capacity, 1);
    json->requires_sep = 0;
    return json;
}

// ---------------------------------------------------------------------------
void libsrf_json_destroy(libsrf_json_t* json) {
    free(json->data);
    free(json);
}

// ---------------------------------------------------------------------------
static void libsrf_json_reserve_capacity(libsrf_json_t* json, size_t len) {
    size_t old_capacity = json->capacity;
    while(len + json->pos + 1 >= json->capacity) {
        json->capacity *= 2;
    }
    if(json->capacity > old_capacity) {
        json->data = (char*)realloc(json->data, json->capacity);
        memset(json->data + old_capacity, 0, json->capacity - old_capacity);
    }
}


// ---------------------------------------------------------------------------
static void libsrf_json_append_string(libsrf_json_t* json, const char* string) {
    char scratch[128];
    char* tmp = scratch;
    size_t len = strlen(string);
    if(len >= 128) {
        tmp = (char*)calloc(2 * len + 1, 1);
    }
    size_t index = 0;
    int quoted = 0;
    size_t i;
    for(i = 0; i < len; i++) {
        if((string[i] == '\n' && (i && string[i - 1] != '\r')) || string[i] == '\r') {
            tmp[index++] = '\\';
            tmp[index++] = 'n';
            continue;
        }
        if(string[i] == '\\') {
            quoted = !quoted;
            tmp[index++] = string[i];
            continue;
        }
        if(!quoted && (string[i] == '"')) {
            tmp[index++] = '\\';
            tmp[index++] = '"';
        } else {
            tmp[index++] = string[i];
        }
        quoted = 0;
    }
    tmp[index] = 0;

    libsrf_json_reserve_capacity(json, index + 1);
    memcpy(json->data + json->pos, tmp, index + 1);
    json->pos += index;

    if(tmp != scratch) {
        free(tmp);
    }
}

// ---------------------------------------------------------------------------
static void libsrf_json_append_quote(libsrf_json_t* json) {
    libsrf_json_reserve_capacity(json, 2);
    json->data[json->pos++] = '"';
    json->data[json->pos] = 0;
}

// ---------------------------------------------------------------------------
static void libsrf_json_append(libsrf_json_t* json, const char* str) {
    size_t len = strlen(str);
    libsrf_json_reserve_capacity(json, len + 1);
    memcpy(json->data + json->pos, str, len + 1);
    json->pos += len;
}


// ---------------------------------------------------------------------------
static void libsrf_json_append_quoted_string(libsrf_json_t* json, const char* str) {
    libsrf_json_append_quote(json);
    libsrf_json_append_string(json, str);
    libsrf_json_append_quote(json);
}


// ---------------------------------------------------------------------------
static void libsrf_json_check_sep(libsrf_json_t* json) {
    if(json->requires_sep) {
        libsrf_json_append(json, (json->flags & LIBSRF_JSON_FLAG_PRETTY) ? ", " : ",");
        json->requires_sep = 0;
    }
}

// ---------------------------------------------------------------------------
void libsrf_json_begin_object(libsrf_json_t* json) {
    libsrf_json_check_sep(json);
    libsrf_json_append(json, (json->flags & LIBSRF_JSON_FLAG_PRETTY) ? "{\n" : "{");
    json->requires_sep = 0;
}

// ---------------------------------------------------------------------------
void libsrf_json_end_object(libsrf_json_t* json) {
    libsrf_json_append(json, (json->flags & LIBSRF_JSON_FLAG_PRETTY) ? "\n}\n" : "}");
    json->requires_sep = 1;
}

// ---------------------------------------------------------------------------
void libsrf_json_begin_array(libsrf_json_t* json) {
    libsrf_json_check_sep(json);
    libsrf_json_append(json, (json->flags & LIBSRF_JSON_FLAG_PRETTY) ? "[ " : "[");
    json->requires_sep = 0;
}

// ---------------------------------------------------------------------------
void libsrf_json_end_array(libsrf_json_t* json) {
    libsrf_json_append(json, (json->flags & LIBSRF_JSON_FLAG_PRETTY) ? " ]" : "]");
    json->requires_sep = 1;
}

// ---------------------------------------------------------------------------
void libsrf_json_dump(libsrf_json_t* json) {
    printf("%s\n", json->data);
}

// ---------------------------------------------------------------------------
void libsrf_json_key(libsrf_json_t* json, const char* key) {
    libsrf_json_check_sep(json);
    libsrf_json_append_quoted_string(json, key);
    libsrf_json_append_string(json, (json->flags & LIBSRF_JSON_FLAG_PRETTY) ? ": " : ":");
    json->requires_sep = 0;
}

// ---------------------------------------------------------------------------
void libsrf_json_key_int(libsrf_json_t* json, ssize_t key) {
    char key_str[32];
    libsrf_json_check_sep(json);
    sprintf(key_str, "%zd", key);
    libsrf_json_append_string(json, key_str);
    libsrf_json_append_string(json, (json->flags & LIBSRF_JSON_FLAG_PRETTY) ? ": " : ":");
    json->requires_sep = 0;
}

// ---------------------------------------------------------------------------
void libsrf_json_write_int(libsrf_json_t* json, ssize_t val) {
    char val_str[32];
    libsrf_json_check_sep(json);
    sprintf(val_str, "%zd", val);
    libsrf_json_append_string(json, val_str);
    json->requires_sep = 1;
}

// ---------------------------------------------------------------------------
void libsrf_json_write_uint(libsrf_json_t* json, size_t val) {
    char val_str[32];
    libsrf_json_check_sep(json);
    sprintf(val_str, "%zu", val);
    libsrf_json_append_string(json, val_str);
    json->requires_sep = 1;
}

// ---------------------------------------------------------------------------
void libsrf_json_write_float(libsrf_json_t* json, double val) {
    char val_str[32];
    libsrf_json_check_sep(json);
    sprintf(val_str, "%f", val);
    libsrf_json_append_string(json, val_str);
    json->requires_sep = 1;
}

// ---------------------------------------------------------------------------
void libsrf_json_write_string(libsrf_json_t* json, const char* val) {
    libsrf_json_check_sep(json);
    libsrf_json_append_quoted_string(json, val);
    json->requires_sep = 1;
}

// ---------------------------------------------------------------------------
char* libsrf_json_to_string(libsrf_json_t* json) {
    return strdup(json->data);
}
