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
    uint32_t material;
} plane;

typedef struct {
    v3 p;
    float r;
    uint32_t material;
} sphere;

typedef struct {
    uint32_t material_count;
    material *materials;
    uint32_t plane_count;
    plane *planes;
    uint32_t sphere_count;
    sphere *spheres;
} world;

static v3 cast(const world *w, v3 origin, v3 dir) {
    v3 result = w->materials[0].color; // default background color
    float hit_dist = MAXFLOAT;

    float tolerance = 0.0001f;

    for (uint32_t plane_idx = 0; plane_idx < w->plane_count; ++plane_idx) {
        plane *p = &w->planes[plane_idx];

        float denominator = v3_dot(p->n, dir);
        if (denominator < -tolerance || denominator > tolerance) {
            float t = (-p->d - v3_dot(p->n, origin)) / denominator;

            if (t > 0 && t < hit_dist) {
                result = w->materials[p->material].color;
                hit_dist = t;
            }
        }
    }

    for (uint32_t sphere_idx = 0; sphere_idx < w->sphere_count; ++sphere_idx) {
        sphere *s = &w->spheres[sphere_idx];

        v3 sphere_relative_origin = v3_sub(origin, s->p);
        float a = v3_dot(dir, dir);
        float b = 2.0f * v3_dot(dir, sphere_relative_origin);
        float c = v3_dot(sphere_relative_origin, sphere_relative_origin) - s->r;

        float denominator = 2.0f * a;
        float root_term = sqrtf(b * b - 4.0f * a * c);
        if (root_term > tolerance) {
            float tp = (-b + root_term) / denominator;
            float tn = (-b - root_term) / denominator;

            float t = tp;
            if (tn > 0 && tn < tp) {
                t = tn;
            }

            if (t > 0 && t < hit_dist) {
                result = w->materials[s->material].color;
                hit_dist = t;
            }

        }
    }

    return result;
}

int main() {

    material materials[] = {
            {0.1f, 0.1f, 0.1f},
            {1,    0,    0},
            {0,    1,    0},
            {0,    0,    1},
    };

    plane p = {
            .n = {0, 0, 1},
            .d = 0,
            .material = 1,
    };

    sphere s[] = {
            {
                    .p = {0, 0, 0},
                    .r = 1.0f,
                    .material = 2,
            },
            {
                    .p = {3, -3, 0},
                    .r = 3.0f,
                    .material = 3,
            }
    };

    world w = {
            .material_count = 2,
            .materials = materials,
            .plane_count = 1,
            .planes = &p,
            .sphere_count = 2,
            .spheres = s,
    };

    v3 camera_point = {0, -10, 1};
    v3 camera_z = v3_normalize(camera_point); // z axis points from origin to the camera; we look down -z axisjj
    v3 camera_x = v3_normalize(v3_cross((v3){0, 0, 1}, camera_z));
    v3 camera_y = v3_normalize(v3_cross(camera_z, camera_x));

    image *img = image_new(1280, 720);

    float viewport_dist = 1.0f;
    float viewport_w = 1.0f;
    float viewport_h = 1.0f;
    if (img->width > img->height) {
        viewport_h = viewport_w * ((float)img->height / img->width);
    } else if (img->height > img->width) {
        viewport_w = viewport_h * ((float)img->width / img->height);
    }

    // position our viewport 'plate' 1 unit in front of the camera
    v3 viewport_center = v3_sub(camera_point, v3_mulf(camera_z, viewport_dist));
    uint32_t *pos = img->pixels;
    for (uint32_t y = 0; y < img->height; ++y) {
        float viewport_y = -1.0f + 2.0f * ((float)y / (float)img->height);
        for (uint32_t x = 0; x < img->width; ++x) {
            float viewport_x = -1.0f + 2.0f * ((float)x / (float)img->width);
            v3 movealongx = v3_mulf(camera_x, viewport_x * 0.5f * viewport_w);
            v3 movealongy = v3_mulf(camera_y, viewport_y * 0.5f * viewport_h);
            v3 viewpoint_p = v3_add(viewport_center, movealongx);
            viewpoint_p = v3_add(viewpoint_p, movealongy);

            v3 ray_p = camera_point;
            v3 ray_dir = v3_normalize(v3_sub(viewpoint_p, camera_point));

            v3 color = cast(&w, ray_p, ray_dir);

            uint32_t bmp_pixel = (((uint32_t)(255) << 24) |
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
