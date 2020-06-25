#pragma once

#include "ecl.h"
#include <stdio.h>
#include <stdlib.h>

// https://en.wikipedia.org/wiki/BMP_file_format#Bitmap_file_header
#pragma pack(push, 1)
typedef struct {
    u16 file_type;
    u32 file_size;
    u16 reserved1;
    u16 reserved2;
    u32 bitmap_offset;
    u32 header_size;
    s32 width;
    s32 height;
    u16 planes;
    u16 bits_per_pixel;
    u32 compression_method;
    u32 bitmap_size;
    s32 horiz_resolution;
    s32 vert_resolution;
    u32 num_colors;
    u32 num_important_colors;
} bmp_header;
#pragma pack(pop)

struct image {
    u32 width;
    u32 height;
    u32 pixels[];
};

struct image *image_new(u32 width, u32 height) {
    struct image *img;
    img = malloc(sizeof(struct image) + width * height * sizeof(*img->pixels));
    img->width = width;
    img->height = height;
    return img;
}

void image_free(struct image *img) {
    free(img);
}

void write_image(struct image *img, const char *filename) {
    u32 img_size = img->height * img->width * sizeof(*img->pixels);

    bmp_header hdr = {};
    hdr.file_type = 0x4D42;
    hdr.file_size = sizeof(hdr) + img_size;
    hdr.bitmap_offset = sizeof(hdr);
    hdr.header_size = sizeof(hdr) - 14;
    hdr.width = img->width;
    hdr.height = img->height;
    hdr.planes = 1;
    hdr.bits_per_pixel = 32;
    hdr.compression_method = 0;
    hdr.bitmap_size = img_size;
    hdr.horiz_resolution = 0;
    hdr.vert_resolution = 0;
    hdr.num_colors = 0;
    hdr.num_important_colors = 0;

    FILE *out = fopen(filename, "wb");
    if (out) {
        fwrite(&hdr, sizeof(hdr), 1, out);
        fwrite(img->pixels, img_size, 1, out);
        fclose(out);
    } else {
        printf("Error writing to file: %s\n", filename);
    }
}
