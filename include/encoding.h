#ifndef YDKJEXTRACTOR_ENCODING_H
#define YDKJEXTRACTOR_ENCODING_H

char* libsrf_encoding_to_utf8(char* str);
char* libsrf_encoding_move_to_utf8(char* str);
char *libsrf_encoding_add_bom(char *str);

#endif //YDKJEXTRACTOR_ENCODING_H
