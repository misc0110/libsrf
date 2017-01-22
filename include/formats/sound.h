#ifndef _SOUND_H_
#define _SOUND_H_

#include <stdio.h>
#include <stdint.h>

typedef struct {
    char magic[4];
    uint32_t size;
    char aifc[4];
    char aifc_ver[4];
    uint32_t version;
    uint32_t aifc1_version;
    char comm[4];
    uint32_t comm_size;
    uint16_t channels;
    uint32_t frames;
    uint16_t sample_size;
    char sample_rate[10];
    char ima4[4];
} __attribute__((packed)) AIFCForm;

typedef struct {
    char magic[4];
    uint32_t size;
    uint32_t offset;
    uint32_t block_size;
} __attribute__((packed)) AIFCSnd;

typedef struct {
    char riff[4];
    uint32_t filesize;
    char wave[4];
    char fmt[4];
    uint32_t length;
    uint16_t format;
    uint16_t channels;
    uint32_t sample_rate;
    uint32_t rate_bps_chan;
    uint16_t bps_chan;
    uint16_t bps;
    char data[4];
    uint32_t size;
} __attribute__((packed)) WAVE;


#endif

