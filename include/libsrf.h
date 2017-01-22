#ifndef YDKJEXTRACTOR_SRF_H
#define YDKJEXTRACTOR_SRF_H
#include <stdio.h>

typedef struct {
    FILE *file;
    size_t entries_to_read;
    char current_type[5];
    size_t srf_size;
} libsrf_t;

typedef struct {
    char type[5];
    size_t entries;
} libsrf_section_t;

typedef struct {
    size_t id;
    size_t offset;
    size_t size;
    size_t plugin_size;
    char filetype[8];
} libsrf_entry_t;

libsrf_t *libsrf_open(const char *file);

int libsrf_is_valid(libsrf_t *session);

void libsrf_close(libsrf_t *session);

int libsrf_read_section(libsrf_t *session, libsrf_section_t *header);

int libsrf_read_entry(libsrf_t* session, libsrf_entry_t* entry);

char* libsrf_get_raw_entry(libsrf_t* session, libsrf_entry_t* entry);

int libsrf_section_is_supported(libsrf_section_t* section);

char* libsrf_get_entry(libsrf_t* session, libsrf_entry_t* entry);

char* libsrf_entry_get_filetype(libsrf_t* session, libsrf_entry_t* entry);

size_t libsrf_entry_get_size(libsrf_t* session, libsrf_entry_t* entry);

size_t libsrf_entry_get_id(libsrf_t* session, libsrf_entry_t* entry);

size_t libsrf_section_get_entries(libsrf_t* session, libsrf_section_t* header);

const char* libsrf_section_get_type(libsrf_t* session, libsrf_section_t* header);


#endif //YDKJEXTRACTOR_SRF_H
