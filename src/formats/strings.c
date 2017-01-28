#include <stdlib.h>
#include <string.h>
#include "libsrf.h"
#include "strings.h"
#include "util.h"
#include "plugin.h"
#include "cstring.h"
#include "encoding.h"
#include "formats/json.h"

// TODO: correct JSON handler (-> quotation marks)

// TODO: utf-8 header

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
    size_t i;
    for(i = 0; i < entry->size; i++) {
        if(content[i] == 0x7b) {
            // fix for strange encoding
            content[i] = 1;
        }
    }
    char* utf = libsrf_encoding_move_to_utf8(content);
    return libsrf_to_single_file(utf, strlen(utf), "txt");
}

// ---------------------------------------------------------------------------
static libsrf_files_t* handleStringArray(libsrf_t* session, libsrf_entry_t* entry) {
    char* content = handleString(session, entry, 0);
    char* str = content;
    libsrf_json_t* json = libsrf_json_create();
    libsrf_json_begin_array(json);
    while(strlen(str)) {
        libsrf_json_write_string(json, str);
        str += strlen(str) + 1;
        if(str - content >= libsrf_swap32((uint32_t)entry->size)) {
            break;
        }
    }
    libsrf_json_end_array(json);
    free(content);
    content = libsrf_json_to_string(json);
    libsrf_json_destroy(json);
    char* utf = libsrf_encoding_move_to_utf8(content);
    return libsrf_to_single_file(utf, strlen(utf), "json");
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

    libsrf_json_t *json = libsrf_json_create();
    libsrf_json_begin_object(json);

    for (i = 0; i < count; i++) {
        int words = *str;
        str++;

        libsrf_json_key(json, str);
        libsrf_json_begin_array(json);

        str += strlen(str) + 1;

        while (--words) {
            libsrf_json_write_string(json, str);

            str += strlen(str) + 1;
            if (str - content >= entry->size) break;
        }
        libsrf_json_end_array(json);
    }

    libsrf_json_end_object(json);

    free(content);
    content = libsrf_json_to_string(json);
    libsrf_json_destroy(json);
    char* utf = libsrf_encoding_move_to_utf8(content);
    return libsrf_to_single_file(utf, strlen(utf), "json");
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

    libsrf_json_t *json = libsrf_json_create();
    libsrf_json_begin_object(json);
    libsrf_json_begin_array(json);

    for (i = 0; i < count; i++) {
        int len = content[pos];
        int c;
        char *str = libsrf_cstring_create();
        str = libsrf_cstring_set_capacity(str, len + 1);
        for (c = 0; c < len; c++) {
            str[c] = content[pos + 1 + c];
        }
        str[len] = 0;

        libsrf_json_write_string(json, str);
        pos += len + 1;

        libsrf_cstring_destroy(str);
    }

    libsrf_json_end_array(json);
    libsrf_json_end_object(json);
    free(content);
    content = libsrf_json_to_string(json);
    libsrf_json_destroy(json);
    char* utf = libsrf_encoding_move_to_utf8(content);
    return libsrf_to_single_file(utf, strlen(utf), "json");
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
