#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "settings.h"

// ---------------------------------------------------------------------------
void libsrf_setting_cleanup(libsrf_t* session) {
    size_t i;
    for(i = 0; i < session->settings.count; i++) {
        free(session->settings.settings[i].key);
        if(session->settings.settings[i].type == STRING) {
            free(session->settings.settings[i].value_str);
        }
    }
    free(session->settings.settings);
    session->settings.settings = NULL;
}


// ---------------------------------------------------------------------------
static void libsrf_setting_reserve(libsrf_t* session, int count) {
    size_t old = session->settings.capacity;
    while (session->settings.count + count >= session->settings.capacity) {
        if (!session->settings.capacity) {
            session->settings.capacity++;
        }
        session->settings.capacity *= 2;
    }
    if (session->settings.capacity != old) {
        session->settings.settings = (libsrf_setting_t *) realloc(session->settings.settings,
                                                       session->settings.capacity * sizeof(libsrf_setting_t));
    }
}

// ---------------------------------------------------------------------------
void libsrf_setting_set(libsrf_t* session, const char *key, const char *value) {
    libsrf_setting_reserve(session, 1);
    session->settings.settings[session->settings.count].key = strdup(key);
    session->settings.settings[session->settings.count].value_str = strdup(value);
    session->settings.settings[session->settings.count].type = STRING;
    session->settings.count++;
}

// ---------------------------------------------------------------------------
void libsrf_setting_set_int(libsrf_t* session, const char *key, int value) {
    libsrf_setting_reserve(session, 1);
    session->settings.settings[session->settings.count].key = strdup(key);
    session->settings.settings[session->settings.count].value_int = value;
    session->settings.settings[session->settings.count].type = INT;
    session->settings.count++;
}

// ---------------------------------------------------------------------------
void libsrf_setting_set_float(libsrf_t* session, const char *key, double value) {
    libsrf_setting_reserve(session, 1);
    session->settings.settings[session->settings.count].key = strdup(key);
    session->settings.settings[session->settings.count].value_float = value;
    session->settings.settings[session->settings.count].type = FLOAT;
    session->settings.count++;
}

// ---------------------------------------------------------------------------
const libsrf_setting_t* libsrf_setting_get(libsrf_t* session, const char *key) {
    size_t i;
    for(i = 0; i < session->settings.count; i++) {
        if(strcmp(session->settings.settings[i].key, key) == 0) {
            return &(session->settings.settings[i]);
        }
    }
    return NULL;
}
