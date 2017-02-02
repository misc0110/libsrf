/* 
 * Copyright (c) 2017 Michael Schwarz.
 * 
 * This program is free software: you can redistribute it and/or modify  
 * it under the terms of the GNU Lesser General Public License as   
 * published by the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 * Lesser General Lesser Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include "util.h"
#include "libsrf.h"
#include "plugin.h"
#include "properties.h"
#include "settings.h"

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
libsrf_t *libsrf_new() {
    libsrf_t *session = (libsrf_t *) calloc(1, sizeof(libsrf_t));
    if (!session) {
        return NULL;
    }
    return session;
}

// ---------------------------------------------------------------------------
int libsrf_open(libsrf_t *session, const char *file) {
    if (!session) {
        return 0;
    }
    SRFHeader hd;

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
    return 1;

    err_close:
    fclose(session->file);

    err:
    session->file = NULL;
    return 0;
}

// ---------------------------------------------------------------------------
int libsrf_is_valid(libsrf_t *session) {
    return (session != NULL && session->file != NULL);
}

// ---------------------------------------------------------------------------
void libsrf_close(libsrf_t *session) {
    if (session->file) {
        fclose(session->file);
    }
    session->file = NULL;
}

// ---------------------------------------------------------------------------
void libsrf_destroy(libsrf_t *session) {
    libsrf_setting_cleanup(session);
    free(session);
}

// ---------------------------------------------------------------------------
int libsrf_read_section(libsrf_t *session, libsrf_section_t *header) {
    // skip remaining entries
    fseek(session->file, session->entries_to_read * sizeof(FileEntry), SEEK_CUR);
    session->entries_to_read = 0;

    if (ftell(session->file) >= session->srf_size) return 0;

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
int libsrf_read_entry(libsrf_t *session, libsrf_entry_t *entry) {
    FileEntry e;
    if (session->entries_to_read == 0) return 0;
    session->entries_to_read--;
    fread(&e, sizeof(FileEntry), 1, session->file);
    entry->id = libsrf_swap32(e.id);
    entry->size = libsrf_swap32(e.length);
    entry->offset = libsrf_swap32(e.offset);
    return 1;
}

// ---------------------------------------------------------------------------
char *libsrf_get_raw_entry(libsrf_t *session, libsrf_entry_t *entry) {
    ssize_t cur = ftell(session->file);
    fseek(session->file, entry->offset, SEEK_SET);
    char *res = (char *) malloc(entry->size + 1);
    fread(res, sizeof(char), entry->size, session->file);
    fseek(session->file, cur, SEEK_SET);
    return res;
}

// ---------------------------------------------------------------------------
libsrf_files_t *libsrf_get_entry(libsrf_t *session, libsrf_entry_t *entry) {
    int plugin = libsrf_find_plugin(session->current_type);
    if (plugin == -1) return NULL;
    return libsrf_plugin[plugin].handler(session, entry);
}

// ---------------------------------------------------------------------------
size_t libsrf_files_count(libsrf_t *session, libsrf_files_t *files) {
    if (!files) return 0;
    return files->count;
}

// ---------------------------------------------------------------------------
void libsrf_entry_iterator(libsrf_iterator_t *it, libsrf_files_t *files) {
    if (!it) {
        return;
    }
    it->valid = 0;
    if (!files) {
        return;
    }
    it->begin = files->files;
    it->current = files->files;
    it->valid = 1;
    it->element_size = sizeof(libsrf_file_t);
    it->end = files->files + files->count;
}

// ---------------------------------------------------------------------------
libsrf_file_t *libsrf_get_file(libsrf_iterator_t *iterator) {
    if (!iterator) return NULL;
    return (libsrf_file_t *) iterator->current;
}

// ---------------------------------------------------------------------------
int libsrf_iterator_is_valid(libsrf_iterator_t *iterator) {
    if (!iterator) return 0;
    return iterator->valid;
}

// ---------------------------------------------------------------------------
int libsrf_iterator_next(libsrf_iterator_t *iterator) {
    if (!iterator) return 0;
    iterator->current = (void *) (((char *) iterator->current) + iterator->element_size);
    if (iterator->current >= iterator->end) {
        iterator->valid = 0;
    }
    return iterator->valid;
}

// ---------------------------------------------------------------------------
size_t libsrf_file_get_size(libsrf_t *session, libsrf_file_t *file) {
    if (!file) return 0;
    return file->size;
}

// ---------------------------------------------------------------------------
char *libsrf_file_get_data(libsrf_t *session, libsrf_file_t *file) {
    if (!file) return NULL;
    return file->data;
}


// ---------------------------------------------------------------------------
const char *libsrf_file_get_type(libsrf_t *session, libsrf_file_t *file) {
    if (!file) return NULL;
    return file->filetype;
}

// ---------------------------------------------------------------------------
void libsrf_files_free(libsrf_t *session, libsrf_files_t *files) {
    size_t i;
    for (i = 0; i < files->count; i++) {
        free(files->files[i].data);
    }
    free(files->files);
    free(files);
}

// ---------------------------------------------------------------------------
int libsrf_section_is_supported(libsrf_t *session, libsrf_section_t *section) {
    if (!section) return 0;
    return libsrf_find_plugin(section->type) != -1;
}

// ---------------------------------------------------------------------------
size_t libsrf_entry_get_size(libsrf_t *session, libsrf_entry_t *entry) {
    if (!entry) return 0;
    return entry->size;
}

// ---------------------------------------------------------------------------
size_t libsrf_entry_get_id(libsrf_t *session, libsrf_entry_t *entry) {
    if (!entry) return 0;
    return entry->id;
}

// ---------------------------------------------------------------------------
size_t libsrf_section_get_entries(libsrf_t *session, libsrf_section_t *header) {
    return header->entries;
}

// ---------------------------------------------------------------------------
const char *libsrf_section_get_type(libsrf_t *session, libsrf_section_t *header) {
    return header->type;
}

// ---------------------------------------------------------------------------
void libsrf_set_property(libsrf_t *session, libsrf_property_t property, libsrf_property_value_t value) {
    if (property >= _LAST_PROPERTY || value >= _LAST_PROPVAL) {
        return;
    }
    libsrf_setting_set(session, LIBSRF_PROPERTY_STRING[(size_t) property],
                       LIBSRF_PROPERTY_VALUE_STRING[(size_t) value]);
}

// ---------------------------------------------------------------------------
void libsrf_set_property_string(libsrf_t *session, const char *property, const char *value) {
    libsrf_setting_set(session, property, value);
}
