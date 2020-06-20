#pragma once

#include <stdint.h>

// https://en.wikipedia.org/wiki/BMP_file_format#Bitmap_file_header
#pragma pack(push, 1)
typedef struct {
    uint16_t file_type; // 0x42 0x4D
    uint32_t file_size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t bitmap_offset; // offset where pixel data starts
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t planes; // should be 1
    uint16_t bits_per_pixel;
    uint32_t compression_method;
    uint32_t bitmap_size;
    int32_t horiz_resolution;
    int32_t vert_resolution;
    uint32_t num_colors;
    uint32_t num_important_colors;
} bmp_header;
#pragma pack(pop)
