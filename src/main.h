#pragma once

#include "ecl.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct camera {
    v3 origin;
    v3 x, y, z;
    v3 viewport_lower_left;
    f32 viewport_width, viewport_height;
};

struct camera* camera_init(struct camera *cam, v3 lookFrom, v3 lookAt, f32 aspect) {
    assert(aspect > 1.0f); // width > height only, please

    if (cam) {
        cam->origin = v3_sub(lookFrom, lookAt);
        cam->z = v3_normalize(cam->origin); // z axis points from origin to the camera; we look down -z axis
        cam->x = v3_normalize(v3_cross((v3){0, 0, 1}, cam->z));
        cam->y = v3_normalize(v3_cross(cam->z, cam->x));

        // position our viewport 'plate' 1 unit in front of the camera
        cam->viewport_height = 1.0f;
        cam->viewport_width = cam->viewport_height * aspect;
        cam->viewport_lower_left = v3_sub(cam->origin, v3_mulf(cam->z, 1.0f));
        cam->viewport_lower_left = v3_sub(cam->viewport_lower_left, v3_mulf(cam->y, 0.5f * cam->viewport_height));
        cam->viewport_lower_left = v3_sub(cam->viewport_lower_left, v3_mulf(cam->x, 0.5f * cam->viewport_width));
    }

    return cam;
}

struct material {
    v3 emit_color;
    v3 reflect_color;
};

struct plane {
    v3 n;
    f32 d;
    u32 material;
};

struct sphere {
    v3 p;
    f32 r;
    u32 material;
};

struct world {
    u32 material_count;
    struct material *materials;
    u32 plane_count;
    struct plane *planes;
    u32 sphere_count;
    struct sphere *spheres;
};

/* BMP Image Writing */
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

// https://entropymine.com/imageworsener/srgbformula/
f32 linear_to_srgb(f32 x) {
    if (x < 0.0f) {
        return 0.0f;
    } else if (x > 1.0f) {
        return 1.0f;
    } else if (x > 0.0031308f) {
        return 1.055f*powf(x, 1.0f/2.4f) - 0.055f;
    } else {
        return x * 12.92f;
    }
}
