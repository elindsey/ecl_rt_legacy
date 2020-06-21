#include "image.h"
#include "v3.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    v3 color;
} material;

typedef struct {
    v3 n;
    float d;
    int material;
} plane;

typedef struct {
    int material_count;
    material *materials;
    int plane_count;
    plane *planes;
} world;

static v3 cast(const world *ptr, v3 p, v3 dir) {
    v3 result;
    result.r = 1;
    result.b = 1;
    result.g = 1;
    return result;
}

int main() {

    material materials[2] = {
            {0, 0, 0},
            {1, 0, 0}
    };

    plane p = {
            .n = {0, 0, 1},
            .d = 0,
            .material = 2
    };

    world w = {
        .material_count = 2,
        .materials = materials,
        .plane_count = 1,
        .planes = &p
    };

    v3 camera_point = {0, 10, 1};
    v3 camera_z = v3_normalize(camera_point); // z axis points from origin to the camera; we look down -z axisjj
    v3 camera_x = v3_normalize(v3_cross(camera_z, (v3){0, 0, 1}));
    v3 camera_y = v3_normalize(v3_cross(camera_z, camera_x));

    image img = image_new(1280, 720);

    float viewport_dist = 1.0f;
    float viewport_w = 2.0f;
    float viewport_h = 1.0f;
    // position our viewport 'plate' 1 unit in front of the camera
    v3 viewport_center = v3_sub(camera_point, v3_mulf(camera_z, viewport_dist));
    // make a viewport from -2 to 2 width, -1 to 1 height
    // is it going to be distorted since our viewport doesn't match the output image aspect ratio?
    uint32_t *pos = img.pixels;
    for (uint32_t y = 0; y < img.height; y++) {
        float viewport_y = -1.0f + 2.0f * ((float)y / (float)img.height);
        for (uint32_t x = 0; x < img.width; x++) {
            float viewport_x = -2.0f + 4.0f * ((float)x / (float)img.width);
            v3 movealongx = v3_mulf(camera_x, viewport_x * 0.5f * viewport_w);
            v3 movealongy = v3_mulf(camera_y, viewport_y * 0.5f * viewport_h);
            v3 viewpoint_p = v3_add(viewport_center, movealongx);
            viewpoint_p = v3_add(viewpoint_p, movealongy);

            v3 ray_p = camera_point;
            v3 ray_dir = v3_normalize(v3_sub(viewpoint_p, camera_point));

            v3 color = cast(&w, ray_p, ray_dir);

            uint32_t bmp_pixel = ((255 << 24) |
                                  ((uint32_t)(255.0f * color.r + 0.05f) << 16) |
                                  ((uint32_t)(255.0f * color.g + 0.05f) << 8) |
                                  ((uint32_t)(255.0f * color.b + 0.05f) << 0));
            *pos++ = bmp_pixel;
        }
    }
    write_image(img, "out.bmp");

    printf("Fin.\n");
    return 0;
}
