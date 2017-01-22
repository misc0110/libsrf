#ifndef YDKJEXTRACTOR_IMAGE_H
#define YDKJEXTRACTOR_IMAGE_H

#include <stdint.h>

typedef struct {
    uint32_t offset;
    uint16_t version;
    uint16_t block_word_size;
    uint16_t entries;
} __attribute__((packed)) ImageHeader;

typedef struct {
    uint16_t share_offset;
    int16_t left;
    int16_t top;
    int16_t right;
    int16_t bottom;
    uint16_t count;
} __attribute__((packed)) ImageDisplayDefinition;

typedef struct {
    uint8_t number;
    uint8_t val;
    int16_t left;
    int16_t top;
    int16_t right;
    int16_t bottom;
    uint16_t index;
} __attribute__((packed)) ImageDefinition;

typedef struct {
    uint16_t count1;
    uint16_t count2;
} __attribute__((packed)) ImageList;

typedef struct {
    uint32_t offset;
    uint16_t width;
    uint16_t height;
} __attribute__((packed)) ImageDataHeader;

#endif //YDKJEXTRACTOR_IMAGE_H
