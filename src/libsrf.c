#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "util.h"
#include "libsrf.h"
#include "plugin.h"

// ---------------------------------------------------------------------------
typedef struct {
    char magic[4];
    uint32_t size;
    uint32_t length;
} __attribute__((packed)) SRFHeader;

typedef struct {
    char type[4];
    uint32_t count;
} __attribute__((packed)) FileHeader;

typedef struct {
    uint32_t id;
    uint32_t offset;
    uint32_t length;
} __attribute__((packed)) FileEntry;



// ---------------------------------------------------------------------------
libsrf_t *libsrf_open(const char *file) {
    SRFHeader hd;
    libsrf_t *session = (libsrf_t *) malloc(sizeof(libsrf_t));

    session->file = fopen(file, "rb");
    if (session->file == NULL) {
        goto err;
    }

    fread(&hd, sizeof(SRFHeader), 1, session->file);
    if (memcmp(hd.magic, "srf1", 4) != 0) {
        goto err_close;
    }

    session->entries_to_read = 0;
    session->srf_size = libsrf_swap32(hd.length);
    return session;

    err_close:
    fclose(session->file);

    err:
    session->file = NULL;
    return session;
}

// ---------------------------------------------------------------------------
int libsrf_is_valid(libsrf_t *session) {
    return (session != NULL && session->file != NULL);
}

// ---------------------------------------------------------------------------
void libsrf_close(libsrf_t *session) {
    if(session->file) {
        fclose(session->file);
    }
    session->file = NULL;
    free(session);
}

// ---------------------------------------------------------------------------
int libsrf_read_section(libsrf_t* session, libsrf_section_t* header) {
    // skip remaining entries
    fseek(session->file, session->entries_to_read * sizeof(FileEntry), SEEK_CUR);
    session->entries_to_read = 0;

    if(ftell(session->file) >= session->srf_size) return 0;

    FileHeader hdr;
    if (fread(&hdr, sizeof(FileHeader), 1, session->file) != 1) {
        return 0;
    }
    memcpy(header->type, hdr.type, 4);
    header->type[4] = 0;
    header->entries = libsrf_swap32(hdr.count);
    memcpy(session->current_type, header->type, 5);
    session->entries_to_read = header->entries;
    return 1;
}


// ---------------------------------------------------------------------------
int libsrf_read_entry(libsrf_t* session, libsrf_entry_t* entry) {
    FileEntry e;
    if(session->entries_to_read == 0) return 0;
    session->entries_to_read--;
    fread(&e, sizeof(FileEntry), 1, session->file);
    entry->id = libsrf_swap32(e.id);
    entry->size = libsrf_swap32(e.length);
    entry->plugin_size = entry->size;
    entry->offset = libsrf_swap32(e.offset);
    strcpy(entry->filetype, "raw");
    return 1;
}

// ---------------------------------------------------------------------------
char* libsrf_get_raw_entry(libsrf_t* session, libsrf_entry_t* entry) {
    ssize_t cur = ftell(session->file);
    fseek(session->file, entry->offset, SEEK_SET);
    char* res = (char*)malloc(entry->size + 1);
    fread(res, sizeof(char), entry->size, session->file);
    fseek(session->file, cur, SEEK_SET);
    return res;
}

// ---------------------------------------------------------------------------
char* libsrf_get_entry(libsrf_t* session, libsrf_entry_t* entry) {
    int plugin = libsrf_find_plugin(session->current_type);
    if(plugin == -1) return NULL;
    return libsrf_plugin[plugin].handler(session, entry);
}

// ---------------------------------------------------------------------------
int libsrf_section_is_supported(libsrf_section_t* section) {
    return libsrf_find_plugin(section->type) != -1;
}

// ---------------------------------------------------------------------------
char* libsrf_entry_get_filetype(libsrf_t* session, libsrf_entry_t* entry) {
    return entry->filetype;
}

// ---------------------------------------------------------------------------
size_t libsrf_entry_get_size(libsrf_t* session, libsrf_entry_t* entry) {
    return entry->plugin_size;
}

// ---------------------------------------------------------------------------
size_t libsrf_entry_get_id(libsrf_t* session, libsrf_entry_t* entry) {
    return entry->id;
}

// ---------------------------------------------------------------------------
size_t libsrf_section_get_entries(libsrf_t* session, libsrf_section_t* header) {
    return header->entries;
}

// ---------------------------------------------------------------------------
const char* libsrf_section_get_type(libsrf_t* session, libsrf_section_t* header) {
    return header->type;
}
