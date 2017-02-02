#ifndef YDKJEXTRACTOR_SETTINGS_H
#define YDKJEXTRACTOR_SETTINGS_H

#include "libsrf.h"
#include "properties.h"


void libsrf_setting_set(libsrf_t *session, const char *key, const char *value);

void libsrf_setting_set_int(libsrf_t *session, const char *key, int value);

void libsrf_setting_set_float(libsrf_t *session, const char *key, double value);

const libsrf_setting_t *libsrf_setting_get(libsrf_t *session, const char *key);

void libsrf_setting_cleanup(libsrf_t *session);

#define IS_SETTING(session, key, val) (libsrf_setting_get(session, LIBSRF_PROPERTY_STRING[(size_t)key]) && strcmp(libsrf_setting_get(session, LIBSRF_PROPERTY_STRING[(size_t)key])->value_str, LIBSRF_PROPERTY_VALUE_STRING[(size_t)val]) == 0)
#define IS_SETTING_OR_NONE(session, key, val) (!libsrf_setting_get(session, LIBSRF_PROPERTY_STRING[(size_t)key]) || (libsrf_setting_get(session, LIBSRF_PROPERTY_STRING[(size_t)key]) && strcmp(libsrf_setting_get(session, LIBSRF_PROPERTY_STRING[(size_t)key])->value_str, LIBSRF_PROPERTY_VALUE_STRING[(size_t)val]) == 0))

#endif //YDKJEXTRACTOR_SETTINGS_H
