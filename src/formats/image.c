#include <stdlib.h>
#include <string.h>
#include "formats/image.h"
#include "plugin.h"
#include "libsrf.h"
#include "util.h"
#include "formats/bmp.h"

// ---------------------------------------------------------------------------
static uint16_t* convert_to_raw(unsigned char* data, size_t width, size_t height, size_t len) {
    int bpos = 0, pos = 0, i;
    unsigned char* bitmap = (unsigned char*)calloc(width * height * 2, 1);
    uint16_t* result = (uint16_t*)calloc(width * height * 2, sizeof(uint16_t));
    unsigned int extra = data[0] - 2;
    int next = data[0] + data[1];
    int bglen = -1;
    int repeat = 0;

    for(i = 0; i < 3; i++) {
        bitmap[bpos++] = data[pos++];
    }

    while(bpos + extra < width * height && pos < len) {
        int widebf = (((int)data[pos++]) << 1) | 1;
        if(pos >= len) break;
        while((widebf & 0xff) != 0) {
            char status = (char)((widebf & 0x100) >> 8);
            widebf <<= 1;
            if(status == 0) { // new pixel
                if((bpos == next - extra) || (bglen != -1)) {
                    if(bglen == -1) {
                        bglen = data[pos];
                    } else {
                        extra += bglen - 2;
                        next += bglen + data[pos];
                        bglen = -1;
                    }
                }
                bitmap[bpos++] = data[pos++];
                if(pos >= len) break;
            } else { // repetition
                int info[2];
                info[0] = data[pos++];
                info[1] = data[pos++];
                if(pos >= len) break;
                if((info[1] & 0xf0) == 0) { // repeat - 3 byte syntax
                    repeat = data[pos++] + 0x10;
                } else { // repeat - 2 byte syntax
                    repeat = ((unsigned char)(info[1] & 0xf0)) >> 4;
                }
                if(pos >= len) break;
                int offset = bpos - (info[0] + 0x100 * (info[1] & 0x0f));
                for(i = 0; i < repeat + 2; i++) {
                    if((bpos == next - extra) || (bglen != -1)) {
                        if(bglen == -1) {
                            bglen = bitmap[offset];
                        } else {
                            extra += bglen - 2;
                            next += bglen + bitmap[offset];
                            bglen = -1;
                        }
                    }
                    bitmap[bpos++] = bitmap[offset++];
                    if(pos >= len) break;
                }
                if(pos >= len) break;
            }
        }
    }

    next = 0;
    bglen = -1;
    pos = 0;
    for(i = 0; i < bpos; i++) {
        if(bglen != -1) {
            next += bglen + bitmap[i];
            while(bglen--) {
                result[pos++] = 255 << 8; // transparent
                if(pos >= width * height) break;
            }
            bglen = -1;
        } else if(pos == next) {
            bglen = bitmap[i];
        } else {
            result[pos++] = bitmap[i];
            if(pos >= width * height) break;
        }
    }
    free(bitmap);
    return (uint16_t*)result;
}


// ---------------------------------------------------------------------------
static libsrf_files_t *handlerOff4(libsrf_t *session, libsrf_entry_t *entry) {
    char *content = libsrf_get_raw_entry(session, entry);
    char *ptr = content;

    ImageHeader *hdr = (ImageHeader *) ptr;
    printf("Offset: %u\nVersion: %u\nBlock Word Size: %u\nEntries: %u\n", libsrf_swap32(hdr->offset),
           libsrf_swap16(hdr->version), libsrf_swap16(hdr->block_word_size), libsrf_swap16(hdr->entries));
    ptr += sizeof(ImageHeader);

    int i, j;
    uint16_t entries = libsrf_swap16(hdr->entries);
    libsrf_files_t* files = (libsrf_files_t*)malloc(sizeof(libsrf_files_t));
    files->count = 0;
    files->files = NULL;

    // image indices
    char *header_end = ptr + entries * 2;

    // TODO: save to json
    for (i = 0; i < entries; i++) {
        uint16_t offset = libsrf_swap16(*(uint16_t *) ptr);
        //printf("%d\n", offset);
        if (offset) {
            // image definitions
            //printf("-- Entry: %d\n", i);

            ImageDisplayDefinition *def = (ImageDisplayDefinition *) (header_end +
                                                                      (offset - 1) * sizeof(ImageDisplayDefinition));
            /*printf("off: %d\nleft: %d\ntop: %d\nright: %d\nbottom: %d\ncount: %d\n\n", libsrf_swap16(def->share_offset),
                   libsrf_swap16(def->left), libsrf_swap16(def->top), libsrf_swap16(def->right),
                   libsrf_swap16(def->bottom), libsrf_swap16(def->count));
            */
            ImageDefinition *idef = NULL;
            uint16_t off = libsrf_swap16(def->share_offset);
            if (off == 0) {
                // own image definition
                idef = (ImageDefinition *) (def + 1);
            } else {
                // reuse image definition of given offset
                idef = (ImageDefinition *) (header_end + off * sizeof(ImageDefinition));
            }
            for (j = 0; j < libsrf_swap16(def->count); j++) {
                /*
                printf("  num: %d\n  val: %d\n  left: %d\n  top: %d\n  right: %d\n  bottom: %d\n  index: %d\n\n",
                       idef->number, idef->val,
                       libsrf_swap16(idef->left), libsrf_swap16(idef->top), libsrf_swap16(idef->right),
                       libsrf_swap16(idef->bottom), libsrf_swap16(idef->index));
                       */
                idef++;
            }
        }

        ptr += 2;
    }


    ImageList* list = (ImageList*)(content + libsrf_swap32(hdr->offset));
    size_t images = libsrf_swap16(list->count1);
    printf("Images: %zu | %u\n", images, libsrf_swap16(list->count2));

    ImageDataHeader* img = (ImageDataHeader*)(list + 1);
    for(i = 0; i < images; i++) {
        printf("[%d] Offset: %u, width: %u, height: %u\n", i, libsrf_swap32(img->offset), libsrf_swap16(img->width),
               libsrf_swap16(img->height));

        unsigned char* img_data = (unsigned char*)(content + libsrf_swap32(img->offset));
        uint16_t* raw = convert_to_raw(img_data, libsrf_swap16(img->width), libsrf_swap16(img->height), entry->size - libsrf_swap32(img->offset));
        size_t bmp_size = 0;
        char* bmp = libsrf_raw_to_bmp(raw, libsrf_swap16(img->width), libsrf_swap16(img->height), &bmp_size);
        free(raw);
        files->count++;
        files->files = realloc(files->files, files->count * sizeof(libsrf_file_t));
        files->files[files->count - 1].data = bmp;
        files->files[files->count - 1].size = bmp_size;
        strcpy(files->files[files->count - 1].filetype, "bmp");
        img++;
    }

    free(content);
    return files;
}

PLUGIN("off4", handlerOff4);
