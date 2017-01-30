#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "formats/qhdr.h"
#include "formats/json.h"
#include "libsrf.h"
#include "plugin.h"
#include "encoding.h"
#include "util.h"

// ---------------------------------------------------------------------------
static libsrf_files_t *handlerQhdr(libsrf_t *session, libsrf_entry_t *entry) {
    const char *typename[] = {
            "normal", "n/a", "gibberish", "disordat", "jackattack", "fiber", "n/a", "n/a", "n/a", "n/a", "n/a", "n/a",
            "threeway", "bingo", "roadkill", "n/a", "n/a", "rideintro"
    };

    char *content = libsrf_get_raw_entry(session, entry);
    QHDR *qhdr = (QHDR *) content;
    char *utf = NULL;

    libsrf_json_t *json = libsrf_json_create();
    libsrf_json_begin_object(json);
    libsrf_json_key(json, "name");
    utf = libsrf_encoding_to_utf8(qhdr->name);
    libsrf_json_write_string(json, utf);
    free(utf);
    libsrf_json_key(json, "category");
    utf = libsrf_encoding_to_utf8(qhdr->category);
    libsrf_json_write_string(json, utf);
    free(utf);
    libsrf_json_key(json, "file");
    utf = libsrf_encoding_to_utf8(qhdr->file);
    libsrf_json_write_string(json, utf);
    free(utf);
    libsrf_json_key(json, "value");
    libsrf_json_write_int(json, qhdr->value * 1000);
    libsrf_json_key(json, "type");
    libsrf_json_write_int(json, qhdr->type);
    libsrf_json_key(json, "subtype");
    libsrf_json_write_int(json, qhdr->subtype);
    libsrf_json_key(json, "typename");
    if (qhdr->type < sizeof(typename) / sizeof(typename[0])) {
        libsrf_json_write_string(json, typename[qhdr->type]);
    } else {
        libsrf_json_write_string(json, "n/a");
    }
    libsrf_json_key(json, "correct");
    libsrf_json_write_int(json, qhdr->correct);
    libsrf_json_key(json, "correct2");
    if (entry->size >= sizeof(QHDR)) {
        libsrf_json_write_int(json, qhdr->corr2);
    } else {
        libsrf_json_write_int(json, 0);
    }

    libsrf_json_end_object(json);
    char *hdr = libsrf_json_to_string(json);
    libsrf_json_destroy(json);
    free(content);
    utf = libsrf_encoding_add_bom(hdr);
    free(hdr);
    return libsrf_to_single_file(utf, strlen(utf), "json");
}

PLUGIN("qhdr", handlerQhdr);
