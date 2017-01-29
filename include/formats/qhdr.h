#ifndef YDKJEXTRACTOR_QHDR_H
#define YDKJEXTRACTOR_QHDR_H

#include <stdint.h>

typedef struct {
    uint16_t length;
    char name[4];
    uint8_t unk1;
    uint8_t unk2;
    uint8_t value;
    uint8_t type;
    uint8_t unk3;
    uint8_t subtype;
    uint32_t unk4;
    char category[64];
    char file[66];
    char correct;
    char corr2;
} __attribute__((packed)) QHDR;


#endif //YDKJEXTRACTOR_QHDR_H
