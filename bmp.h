#pragma once

#include <stdint.h>
#include <stdio.h>

// https://en.wikipedia.org/wiki/BMP_file_format#Bitmap_file_header
#pragma pack(push, 1)
typedef struct {
    uint16_t file_type;
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t bitmap_offset;
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression_method;
    uint32_t bitmap_size;
    int32_t horiz_resolution;
    int32_t vert_resolution;
    uint32_t num_colors;
    uint32_t num_important_colors;
} bmp_header;
#pragma pack(pop)

typedef struct {
    uint32_t width;
    uint32_t height;
    uint32_t *pixels;
} image;

void write_image(image img, const char* filename) {
    uint32_t img_size = img.width * img.height * sizeof(*img.pixels);

    bmp_header hdr = {};
    hdr.file_type = 0x4D42;
    hdr.file_size = sizeof(hdr) + img_size;
    hdr.bitmap_offset = sizeof(hdr);
    hdr.header_size = sizeof(hdr) - 14;
    hdr.width = img.width;
    hdr.height = img.height;
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
        fwrite(img.pixels, img_size, 1, out);
        fclose(out);
    } else {
        printf("Error writing to file: %s\n", filename);
    }
}
