#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "settings.h"

// TODO: free memory

static size_t libsrf_settings_count = 0;
static size_t libsrf_settings_capacity = 0;
static libsrf_setting_t *libsrf_settings = NULL;


// ---------------------------------------------------------------------------
static void libsrf_setting_cleanup(void) {
    size_t i;
    for(i = 0; i < libsrf_settings_count; i++) {
        free(libsrf_settings[i].key);
        if(libsrf_settings[i].type == STRING) {
            free(libsrf_settings[i].value_str);
        }
    }
    free(libsrf_settings);
}


// ---------------------------------------------------------------------------
static void libsrf_setting_reserve(int count) {
    if(!libsrf_settings) {
        atexit(libsrf_setting_cleanup);
    }
    size_t old = libsrf_settings_capacity;
    while (libsrf_settings_count + count >= libsrf_settings_capacity) {
        if (!libsrf_settings_capacity) {
            libsrf_settings_capacity++;
        }
        libsrf_settings_capacity *= 2;
    }
    if (libsrf_settings_capacity != old) {
        libsrf_settings = (libsrf_setting_t *) realloc(libsrf_settings,
                                                       libsrf_settings_capacity * sizeof(libsrf_setting_t));
    }
}

// ---------------------------------------------------------------------------
void libsrf_setting_set(const char *key, const char *value) {
    libsrf_setting_reserve(1);
    libsrf_settings[libsrf_settings_count].key = strdup(key);
    libsrf_settings[libsrf_settings_count].value_str = strdup(value);
    libsrf_settings[libsrf_settings_count].type = STRING;
    libsrf_settings_count++;
}

// ---------------------------------------------------------------------------
void libsrf_setting_set_int(const char *key, int value) {
    libsrf_setting_reserve(1);
    libsrf_settings[libsrf_settings_count].key = strdup(key);
    libsrf_settings[libsrf_settings_count].value_int = value;
    libsrf_settings[libsrf_settings_count].type = INT;
    libsrf_settings_count++;
}

// ---------------------------------------------------------------------------
void libsrf_setting_set_float(const char *key, double value) {
    libsrf_setting_reserve(1);
    libsrf_settings[libsrf_settings_count].key = strdup(key);
    libsrf_settings[libsrf_settings_count].value_float = value;
    libsrf_settings[libsrf_settings_count].type = FLOAT;
    libsrf_settings_count++;
}

// ---------------------------------------------------------------------------
const libsrf_setting_t* libsrf_setting_get(const char *key) {
    size_t i;
    for(i = 0; i < libsrf_settings_count; i++) {
        if(strcmp(libsrf_settings[i].key, key) == 0) {
            return &(libsrf_settings[i]);
        }
    }
    return NULL;
}
