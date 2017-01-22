#include <stdlib.h>
#include <string.h>
#include "libsrf.h"

#include "formats/sound.h"
#include "formats/aifc.h"
#include "util.h"
#include "libsrf.h"
#include "plugin.h"

// ---------------------------------------------------------------------------
static char *readWAVEEntry(libsrf_t* session, libsrf_entry_t *entry) {
    char *content = libsrf_get_raw_entry(session, entry);
    char *sound = (char *) calloc(entry->size + 100, sizeof(char));
    char *ptr = sound;
    WAVE *wave = (WAVE *) ptr;
    ptr += sizeof(WAVE);

    memcpy(wave->riff, "RIFF", 4);
    wave->filesize = (uint32_t)entry->size;
    memcpy(wave->wave, "WAVE", 4);
    memcpy(wave->fmt, "fmt ", 4);
    wave->length = 16; //swap32(entry->length);
    wave->format = 1;
    wave->channels = 1;
    wave->sample_rate = 22500;
    wave->rate_bps_chan = 45000;
    wave->bps_chan = 2;
    wave->bps = 16;
    memcpy(wave->data, "data", 4);
    wave->size = (uint32_t)entry->size;

    memcpy(ptr, content + 85, entry->size);
    free(content);
    strcpy(entry->filetype, "wav");
    return sound;
}

// ---------------------------------------------------------------------------
static char *readAIFCEntry(libsrf_t* session, libsrf_entry_t *entry) {
    char *content = libsrf_get_raw_entry(session, entry);
    char *sound = (char *) calloc(entry->size + 100, sizeof(char));
    char *ptr = sound;
    AIFCForm *form = (AIFCForm *) ptr;
    ptr += sizeof(AIFCForm);
    memcpy(form->magic, "FORM", 4);
    form->size = libsrf_swap32(entry->size + 8);
    memcpy(form->aifc, "AIFC", 4);
    memcpy(form->aifc_ver, "FVER", 4);
    form->version = libsrf_swap32(4);
    form->aifc1_version = libsrf_swap32(0xa2805140);
    memcpy(form->comm, "COMM", 4);
    form->comm_size = libsrf_swap32(0x16);
    form->channels = libsrf_swap16(1);
    form->frames = libsrf_swap32(0x30f);
    form->sample_size = libsrf_swap16(0x10);
    memcpy(form->sample_rate, "\x40\x0d\xaf\xc8\x00\x00\x00\x00\x00\x00", 10);
    memcpy(form->ima4, "ima4", 4);

    AIFCSnd *snd = (AIFCSnd *) ptr;
    ptr += sizeof(AIFCSnd);
    memcpy(snd->magic, "SSND", 4);
    snd->size = libsrf_swap32(entry->size - 46);
    snd->offset = libsrf_swap32(0);
    snd->block_size = libsrf_swap32(0);

    memcpy(ptr, content + 84, entry->size - 84);
    free(content);

    int wav_len = 0;
    char* wav = libsrf_aifc2wav(sound, entry->size, &wav_len);
    free(sound);
    strcpy(entry->filetype, "wav");
    entry->plugin_size = wav_len;
    return wav;
}

// ---------------------------------------------------------------------------
static char* handlerSND(libsrf_t* session, libsrf_entry_t* entry) {
    return readAIFCEntry(session, entry);
}

// ---------------------------------------------------------------------------
static char* handlerMJ18(libsrf_t* session, libsrf_entry_t* entry) {
    return readWAVEEntry(session, entry);
}

// ---------------------------------------------------------------------------
static char* handlerMJ19(libsrf_t* session, libsrf_entry_t* entry) {
    return readWAVEEntry(session, entry);
}

// ---------------------------------------------------------------------------
static char* handlerMQ(libsrf_t* session, libsrf_entry_t* entry) {
    return readAIFCEntry(session, entry);
}


PLUGIN("snd", handlerSND);

PLUGIN("Mj18", handlerMJ18);

PLUGIN("Mj19", handlerMJ19);

PLUGIN("Mq", handlerMQ);