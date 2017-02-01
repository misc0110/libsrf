#ifndef YDKJEXTRACTOR_SETTINGS_H
#define YDKJEXTRACTOR_SETTINGS_H

typedef enum {
    STRING, INT, FLOAT
} libsrf_setting_type_t;

typedef struct {
    char *key;
    libsrf_setting_type_t type;
    union {
        char *value_str;
        int value_int;
        double value_float;
    };
} libsrf_setting_t;

void libsrf_setting_set(const char *key, const char *value);
void libsrf_setting_set_int(const char *key, int value);
void libsrf_setting_set_float(const char *key, double value);
const libsrf_setting_t* libsrf_setting_get(const char *key);


#endif //YDKJEXTRACTOR_SETTINGS_H
