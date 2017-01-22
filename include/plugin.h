#ifndef YDKJEXTRACTOR_PLUGIN_H
#define YDKJEXTRACTOR_PLUGIN_H

#include "libsrf.h"

#define MAX_PLUGINS 32

// ---------------------------------------------------------------------------
extern int libsrf_plugin_count;
typedef struct {
    const char *type;

    char* (*handler)(libsrf_t *, libsrf_entry_t*);
} PluginHandler;
extern PluginHandler libsrf_plugin[MAX_PLUGINS];

#define PLUGIN(str, fnc) __attribute__((constructor)) void _libsrf_register_plugin_##fnc(void) { \
    libsrf_plugin[libsrf_plugin_count].type = str; \
    libsrf_plugin[libsrf_plugin_count++].handler = fnc; \
}

int libsrf_find_plugin(char* type);


#endif //YDKJEXTRACTOR_PLUGIN_H
