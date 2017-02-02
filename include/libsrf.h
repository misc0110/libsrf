#ifndef YDKJEXTRACTOR_SRF_H
#define YDKJEXTRACTOR_SRF_H
#include <stdio.h>
#include "properties.h"


typedef struct {
    FILE *file;
    size_t entries_to_read;
    char current_type[5];
    size_t srf_size;
    libsrf_settings_t settings;
} libsrf_t;

typedef struct {
    char type[5];
    size_t entries;
} libsrf_section_t;

typedef struct {
    size_t id;
    size_t offset;
    size_t size;
    //char filetype[8];
} libsrf_entry_t;

typedef struct {
    char filetype[8];
    size_t size;
    char* data;
} libsrf_file_t;

typedef struct {
    size_t count;
    libsrf_file_t* files;
} libsrf_files_t;

libsrf_t* libsrf_new();

void libsrf_destroy(libsrf_t* session);

int libsrf_open(libsrf_t* session, const char *file);

int libsrf_is_valid(libsrf_t *session);

void libsrf_close(libsrf_t *session);

int libsrf_read_section(libsrf_t *session, libsrf_section_t *header);

int libsrf_read_entry(libsrf_t* session, libsrf_entry_t* entry);

char* libsrf_get_raw_entry(libsrf_t* session, libsrf_entry_t* entry);

int libsrf_section_is_supported(libsrf_section_t* section);

libsrf_files_t* libsrf_get_entry(libsrf_t* session, libsrf_entry_t* entry);

// TODO: iterator and getter

size_t libsrf_entry_get_size(libsrf_t* session, libsrf_entry_t* entry);

size_t libsrf_entry_get_id(libsrf_t* session, libsrf_entry_t* entry);

size_t libsrf_section_get_entries(libsrf_t* session, libsrf_section_t* header);

const char* libsrf_section_get_type(libsrf_t* session, libsrf_section_t* header);

void libsrf_set_property(libsrf_t* session, libsrf_property_t property, libsrf_property_value_t value);

void libsrf_set_property_string(libsrf_t* session, const char* property, const char* value);

#endif //YDKJEXTRACTOR_SRF_H
