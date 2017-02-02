#ifndef YDKJEXTRACTOR_PROPERTIES_H
#define YDKJEXTRACTOR_PROPERTIES_H

#include <stdio.h>

#define FOREACH_PROPERTY(PROPERTY) \
        PROPERTY(IMAGE_FORMAT)   \
        PROPERTY(IMAGE_PALETTE)  \
        PROPERTY(_LAST_PROPERTY) \

#define FOREACH_PROPERTY_VALUE(PROPVAL) \
    PROPVAL(IMAGE_FORMAT_BMP)  \
    PROPVAL(IMAGE_FORMAT_PNG)   \
    PROPVAL(IMAGE_PALETTE_1) \
    PROPVAL(IMAGE_PALETTE_2) \
    PROPVAL(IMAGE_PALETTE_3) \
    PROPVAL(IMAGE_PALETTE_4) \
    PROPVAL(IMAGE_PALETTE_5) \
    PROPVAL(IMAGE_PALETTE_6) \
    PROPVAL(IMAGE_PALETTE_7) \
    PROPVAL(IMAGE_PALETTE_8) \
    PROPVAL(_LAST_PROPVAL) \

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

typedef enum {
    FOREACH_PROPERTY(GENERATE_ENUM)
} libsrf_property_t;


typedef enum {
    FOREACH_PROPERTY_VALUE(GENERATE_ENUM)
} libsrf_property_value_t;

extern const char *LIBSRF_PROPERTY_STRING[];
extern const char *LIBSRF_PROPERTY_VALUE_STRING[];


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

typedef struct {
    size_t count;
    size_t capacity;
    libsrf_setting_t *settings;
} libsrf_settings_t;


#endif //YDKJEXTRACTOR_PROPERTIES_H
