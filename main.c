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

int main() {
    image img = image_new(1280, 720);

    uint32_t *pos = img.pixels;
    for (uint32_t y = 0; y < img.height; y++) {
        for (uint32_t x = 0; x < img.width; x++) {
            *pos = y < 32 ? 0xFFFF0000 : 0xFF0000FF;
            pos++;
        }
    }
    write_image(img, "test.bmp");
    printf("Fin.\n");
    return 0;
}
