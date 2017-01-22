#include "plugin.h"
#include <string.h>
#include <stdio.h>

// ---------------------------------------------------------------------------
int libsrf_find_plugin(char* type) {
    int h;
    for (h = 0; h < libsrf_plugin_count; h++) {
        if (!strncmp(type, libsrf_plugin[h].type, strlen(libsrf_plugin[h].type))) {
            return h;
        }
    }
    return -1;
}

int libsrf_plugin_count = 0;
PluginHandler libsrf_plugin[MAX_PLUGINS];
