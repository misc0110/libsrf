#ifndef YDKJEXTRACTOR_BMP_H
#define YDKJEXTRACTOR_BMP_H

#include <stdint.h>

typedef struct {
    uint8_t header[2];
    uint32_t filesize;
    uint32_t zero;
    uint32_t pos;
    uint32_t header_len;
    uint32_t width;
    uint32_t height;
    uint16_t layers;
    uint16_t depth;
    uint32_t compression;
    uint32_t data_len;
    uint32_t xpxm;
    uint32_t ypxm;
    uint32_t used_colors;
    uint32_t important_colors;
} __attribute__((packed)) BMPHeader;


char* libsrf_raw_to_bmp(uint16_t* data, size_t width, size_t height, size_t* bmp_size, int palette_idx);
char* libsrf_raw_to_png(uint16_t* data, size_t width, size_t height, size_t* png_size, int palette_idx);

#endif //YDKJEXTRACTOR_BMP_H
