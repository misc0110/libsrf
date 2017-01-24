#include "formats/binary.h"
#include <stdlib.h>
#include <string.h>
#include "libsrf.h"
#include "plugin.h"
#include "util.h"
#include "cstring.h"

// TODO: qheaders

// ---------------------------------------------------------------------------
static libsrf_files_t *handlerAns(libsrf_t *session, libsrf_entry_t *entry) {
    char* json = libsrf_cstring_create();
    json = libsrf_cstring_append(json, "[");

    char *content = libsrf_get_raw_entry(session, entry);
    int count = libsrf_swap32(*(uint32_t*)content);
    int i;
    for(i = 0; i < count; i++) {
        char nr[4];
        sprintf(nr, "%d", content[i + 4]);
        json = libsrf_cstring_append(json, nr);
        if(i != count - 1) {
            json = libsrf_cstring_append(json, ", ");
        }
    }

    json = libsrf_cstring_append(json, "]");
    free(content);
    content = libsrf_cstring_to_char(json);
    libsrf_cstring_destroy(json);
    return libsrf_to_single_file(content, strlen(content) + 1, "json");
}

PLUGIN("ANS#", handlerAns);

