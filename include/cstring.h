#ifndef YDKJEXTRACTOR_SIMPLESTRING_H
#define YDKJEXTRACTOR_SIMPLESTRING_H

#define CSTRING_MIN_CAPACITY 16

char *libsrf_cstring_create();

void libsrf_cstring_destroy(char *str);

size_t libsrf_cstring_get_capacity(char *str);

char* libsrf_cstring_set_capacity(char *str, size_t capacity);

char* libsrf_cstring_append(char *orig, char *append);

char* libsrf_cstring_append_char(char *orig, char c);

char* libsrf_cstring_append_unicode(char *orig, uint16_t c);

char* libsrf_cstring_to_char(char *str);

char* libsrf_cstring_move_to_char(char *str);

#endif //YDKJEXTRACTOR_SIMPLESTRING_H
