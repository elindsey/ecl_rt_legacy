#include "main.h"
#include "ecl.h"
#include <stdio.h>

static v3 cast(const struct world *w, v3 origin, v3 dir) {
    v3 result = w->materials[0].color; // default background color
    f32 hit_dist = F32_MAX;

    f32 tolerance = 0.0001f;

    for (u32 plane_idx = 0; plane_idx < w->plane_count; ++plane_idx) {
        struct plane *p = &w->planes[plane_idx];

        f32 denominator = v3_dot(p->n, dir);
        if (denominator < -tolerance || denominator > tolerance) {
            f32 t = (-p->d - v3_dot(p->n, origin)) / denominator;

            if (t > 0 && t < hit_dist) {
                result = w->materials[p->material].color;
                hit_dist = t;
            }
        }
    }

    for (u32 sphere_idx = 0; sphere_idx < w->sphere_count; ++sphere_idx) {
        struct sphere *s = &w->spheres[sphere_idx];

        v3 sphere_relative_origin = v3_sub(origin, s->p);
        f32 a = v3_dot(dir, dir);
        f32 b = 2.0f * v3_dot(dir, sphere_relative_origin);
        f32 c = v3_dot(sphere_relative_origin, sphere_relative_origin) - s->r;

        f32 denominator = 2.0f * a;
        f32 root_term = ecl_sqrt(b * b - 4.0f * a * c);
        if (root_term > tolerance) {
            f32 tp = (-b + root_term) / denominator;
            f32 tn = (-b - root_term) / denominator;

            f32 t = tp;
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

    struct material materials[] = {
            {0.1f, 0.1f, 0.1f},
            {1,    0,    0},
            {0,    1,    0},
            {0,    0,    1},
    };

    struct plane p = {
            .n = {0, 0, 1},
            .d = 0,
            .material = 1,
    };

    struct sphere s[] = {
            {
                    .p = {0, 0, 1},
                    .r = 1.0f,
                    .material = 2,
            },
            {
                    .p = {3, -3, 0},
                    .r = 3.0f,
                    .material = 3,
            }
    };

    struct world w = {
            .material_count = 2,
            .materials = materials,
            .plane_count = 1,
            .planes = &p,
            .sphere_count = 2,
            .spheres = s,
    };

    struct image *img = image_new(1280, 720);

    struct camera cam;
    camera_init(&cam, (v3){0, -10, 1}, (v3){0, 0, 0}, (f32)img->width / img->height);

    u32 *pixel = img->pixels;
    for (u32 image_y = 0; image_y < img->height; ++image_y) {
        // calculate ratio we've moved along the image (y/height), step proportionally within the viewport
        v3 viewport_y = v3_mulf(cam.y, cam.viewport_height * image_y / img->height);
        for (u32 image_x = 0; image_x < img->width; ++image_x) {
            v3 viewport_x = v3_mulf(cam.x, cam.viewport_width * image_x / img->width);

            v3 viewport_p = v3_add(v3_add(cam.viewport_lower_left, viewport_y), viewport_x);

            v3 ray_p = cam.origin;
            v3 ray_dir = v3_normalize(v3_sub(viewport_p, cam.origin));

            v3 color = cast(&w, ray_p, ray_dir);

            u32 bmp_pixel = (((u32)(255) << 24) |
                             ((u32)(255.0f * color.r + 0.05f) << 16) |
                             ((u32)(255.0f * color.g + 0.05f) << 8) |
                             ((u32)(255.0f * color.b + 0.05f) << 0));
            *pixel++ = bmp_pixel;
        }
    }
    write_image(img, "out.bmp");

    printf("Fin.\n");
    return 0;
}
