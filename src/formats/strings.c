#include <stdlib.h>
#include <string.h>
#include "libsrf.h"
#include "strings.h"
#include "util.h"
#include "plugin.h"
#include "cstring.h"
#include "encoding.h"

// ---------------------------------------------------------------------------
static char *handleString(libsrf_t *session, libsrf_entry_t *entry, int correction) {
    char *content = libsrf_get_raw_entry(session, entry);
    content[entry->size - correction] = 0;
    return content;
}

// ---------------------------------------------------------------------------
static libsrf_files_t *handleUTF8String(libsrf_t *session, libsrf_entry_t *entry, int correction) {
    char *content = libsrf_get_raw_entry(session, entry);
    content[entry->size - correction] = 0;
    char* utf = libsrf_encoding_move_to_utf8(content);
    return libsrf_to_single_file(utf, strlen(utf) + 1, "txt");
}

// ---------------------------------------------------------------------------
static libsrf_files_t* handleStringArray(libsrf_t* session, libsrf_entry_t* entry) {
    char* content = handleString(session, entry, 0);
    char* str = content;
    char* json = libsrf_cstring_create();
    json = libsrf_cstring_append(json, "{[");
    while(strlen(str)) {
        json = libsrf_cstring_append(json, "\"");
        json = libsrf_cstring_append(json, str);
        json = libsrf_cstring_append(json, "\"");
        str += strlen(str) + 1;
        if(str - content >= libsrf_swap32((uint32_t)entry->size)) {
            break;
        }
        if(strlen(str)) {
            json = libsrf_cstring_append(json, ", ");
        }

    }
    json = libsrf_cstring_append(json, "]}");
    free(content);
    content = libsrf_cstring_move_to_char(json);
    char* utf = libsrf_encoding_move_to_utf8(content);
    return libsrf_to_single_file(utf, strlen(utf) + 1, "json");
}

// ---------------------------------------------------------------------------
static libsrf_files_t *handlerSTR(libsrf_t *session, libsrf_entry_t *entry) {
    return handleUTF8String(session, entry, 1);
}

// ---------------------------------------------------------------------------
static libsrf_files_t *handlerRoot(libsrf_t *session, libsrf_entry_t *entry) {
    return handleStringArray(session, entry);
}

// ---------------------------------------------------------------------------
static libsrf_files_t *handlerMtch(libsrf_t *session, libsrf_entry_t *entry) {
    return handleStringArray(session, entry);
}

// ---------------------------------------------------------------------------
static libsrf_files_t *handlerDcoy(libsrf_t *session, libsrf_entry_t *entry) {
    return handleStringArray(session, entry);
}

// ---------------------------------------------------------------------------
static libsrf_files_t *handlerStrl(libsrf_t *session, libsrf_entry_t *entry) {
    return handleStringArray(session, entry);
}

// ---------------------------------------------------------------------------
static libsrf_files_t *handlerGibr(libsrf_t *session, libsrf_entry_t *entry) {
    return handleUTF8String(session, entry, 0);
}

// ---------------------------------------------------------------------------
static libsrf_files_t *handlerHnt(libsrf_t *session, libsrf_entry_t *entry) {
    return handleUTF8String(session, entry, 0);
}

// ---------------------------------------------------------------------------
static libsrf_files_t *handlerGory(libsrf_t *session, libsrf_entry_t *entry) {
    return handleUTF8String(session, entry, 0);
}

// ---------------------------------------------------------------------------
static libsrf_files_t *handlerAnsr(libsrf_t *session, libsrf_entry_t *entry) {
    return handleUTF8String(session, entry, 0);
}

// ---------------------------------------------------------------------------
static libsrf_files_t *handlerWrds(libsrf_t *session, libsrf_entry_t *entry) {
    int i;
    char *content = handleString(session, entry, 0);
    int count = content[0];
    char *str = content + 1;

    char *json = libsrf_cstring_create();
    json = libsrf_cstring_append(json, "{");

    for (i = 0; i < count; i++) {
        json = libsrf_cstring_append(json, "{");
        int words = *str;
        str++;

        json = libsrf_cstring_append(json, "\"");
        json = libsrf_cstring_append(json, str);
        json = libsrf_cstring_append(json, "\": [");

        str += strlen(str) + 1;

        while (--words) {
            json = libsrf_cstring_append(json, "\"");
            json = libsrf_cstring_append(json, str);
            json = libsrf_cstring_append(json, "\"");

            str += strlen(str) + 1;
            if (str - content >= entry->size) break;
            if (words > 1) {
                json = libsrf_cstring_append(json, ", ");
            }
        }
        json = libsrf_cstring_append(json, "]}");
        if (i != count - 1) {
            json = libsrf_cstring_append(json, ",\n");
        }

    }

    json = libsrf_cstring_append(json, "}");
    free(content);
    content = libsrf_cstring_move_to_char(json);
    char* utf = libsrf_encoding_move_to_utf8(content);
    return libsrf_to_single_file(utf, strlen(utf) + 1, "json");
}

// ---------------------------------------------------------------------------
static libsrf_files_t *handlerMultiStr(libsrf_t *session, libsrf_entry_t *entry) {

    char *content = handleString(session, entry, 0);
    int count = libsrf_swap16(*(uint16_t *) content);
    int i, pos = 2;
    if (count == 0) {
        count = libsrf_swap32(*(uint32_t *) content);
        pos += 2;
    }

    char *json = libsrf_cstring_create();
    json = libsrf_cstring_append(json, "{[");

    for (i = 0; i < count; i++) {
        int len = content[pos];
        int c;
        char *str = libsrf_cstring_create();
        str = libsrf_cstring_set_capacity(str, len + 1);
        for (c = 0; c < len; c++) {
            str[c] = content[pos + 1 + c];
        }
        str[len] = 0;

        json = libsrf_cstring_append(json, "\"");
        json = libsrf_cstring_append(json, str);
        json = libsrf_cstring_append(json, "\"");
        if (i != count - 1) {
            json = libsrf_cstring_append(json, ", ");
        }

        pos += len + 1;

        libsrf_cstring_destroy(str);
    }

    json = libsrf_cstring_append(json, "]}");
    free(content);
    content = libsrf_cstring_move_to_char(json);
    char* utf = libsrf_encoding_move_to_utf8(content);
    return libsrf_to_single_file(utf, strlen(utf) + 1, "json");
}

PLUGIN("STR ", handlerSTR);

PLUGIN("Root", handlerRoot);

PLUGIN("Mtch", handlerMtch);

PLUGIN("Dcoy", handlerDcoy);


PLUGIN("Gibr", handlerGibr);

PLUGIN("Wrds", handlerWrds);

PLUGIN("Hnt", handlerHnt);

PLUGIN("Gory", handlerGory);

PLUGIN("Ansr", handlerAnsr);

PLUGIN("STR#", handlerMultiStr);

PLUGIN("STRL", handlerStrl);
