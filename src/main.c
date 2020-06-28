#include "main.h"
#include "ecl.h"

static v3 cast(const struct world *w, v3 origin, v3 dir) {

    f32 tolerance = 0.0001f;
    v3 result = {0};
    v3 attenuation = {1, 1, 1};

    for (u32 bounces = 0; bounces < 10; ++bounces) {
        u32 hit_material = 0;
        f32 hit_dist = F32_MAX;
        v3 hit_normal = {0};

        for (u32 plane_idx = 0; plane_idx < w->plane_count; ++plane_idx) {
            struct plane *p = &w->planes[plane_idx];

            f32 denominator = v3_dot(p->n, dir);
            if (denominator < -tolerance || denominator > tolerance) {
                f32 t = (-p->d - v3_dot(p->n, origin)) / denominator;

                if (t > 0 && t < hit_dist) {
                    hit_material = p->material;
                    hit_dist = t;
                    hit_normal = p->n;
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
                    hit_material = s->material;
                    hit_dist = t;
                    // TODO: clean this up, reuse some of the computation from intersection math
                    hit_normal = v3_normalize(v3_add(sphere_relative_origin, v3_mulf(dir, hit_dist)));
                }
            }
        }

        struct material m = w->materials[hit_material];
        result = v3_add(result, v3_mul(attenuation, m.emit_color));
        if (hit_material) {
            attenuation = m.reflect_color;
            origin = v3_add(origin, v3_mulf(origin, hit_dist));
            // perfect reflection; this is more marble-like
            f32 rand_x = xorshift32() / (f32)U32_MAX;
            f32 rand_y = xorshift32() / (f32)U32_MAX;
            f32 rand_z = xorshift32() / (f32)U32_MAX;
            dir = v3_add(hit_normal, (v3){rand_x, rand_y, rand_z});
            if (bounces > 1) {
                rand_x += 1;
            }
            //dir = v3_reflect(dir, hit_normal);
        } else {
            // we've hit the sky
            break;
        }
    }

    return result;
}

int main() {

    struct material materials[] = {
            {
                    .emit_color = {0.3f, 0.4f, 0.8f},
                    .reflect_color = {0, 0, 0},
            },
            {
                    .emit_color = {0, 0, 0},
                    .reflect_color = {0.5f, 0.5f, 0.5f},
            },
            {
                    .emit_color = {0.4f, 0.8f, 0.9f},
                    .reflect_color = {1, 0.8f, 0.8f},
            },
            {
                    .emit_color = {1, 0, 0},
                    .reflect_color = {0, 0, 0},
            },
            {
                    .emit_color = {0.1f, 0.1f, 0.1f},
                    .reflect_color = {0.95f, 0.95f, 0.95f},
            },
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
                    .p = {-3, -5, 1.5f},
                    .r = 1.0f,
                    .material = 3,
            },
            {
                    .p = {3, -3, 0},
                    .r = 3.0f,
                    .material = 4,
            }
    };

    struct world w = {
            .material_count = 4,
            .materials = materials,
            .plane_count = 1,
            .planes = &p,
            .sphere_count = 3,
            .spheres = s,
    };

    //struct image *img = image_new(1280, 720);
    struct image *img = image_new(480, 234);

    struct camera cam;
    camera_init(&cam, (v3){0, -10, 1}, (v3){0, 0, 0}, (f32)img->width / img->height);

    u32 *pixel = img->pixels;
    for (u32 image_y = 0; image_y < img->height; ++image_y) {
        for (u32 image_x = 0; image_x < img->width; ++image_x) {

            // rays per pixel
            u32 samples = 10;
            v3 color = {0, 0, 0};
            for (u32 rcount = 0; rcount < samples; ++rcount) {
                // calculate ratio we've moved along the image (y/height), step proportionally within the viewport
                f32 rand_x = xorshift32() / (f32)U32_MAX; // this bounds limiting could be more efficient, and might have an off by one
                f32 rand_y = xorshift32() / (f32)U32_MAX;
                v3 viewport_y = v3_mulf(cam.y, cam.viewport_height * (image_y + rand_y) / (img->height-1.0f));
                v3 viewport_x = v3_mulf(cam.x, cam.viewport_width * (image_x + rand_x) / (img->width-1.0f));
                v3 viewport_p = v3_add(v3_add(cam.viewport_lower_left, viewport_y), viewport_x);
                // remember that a pixel in float-space is a _range_. We want to send multiple rays within that range
                // to do this we take the start of that range (what we calculated as the image projecting onto our viewport),
                // then add a random [0,1) float

                v3 ray_p = cam.origin;
                v3 ray_dir = v3_normalize(v3_sub(viewport_p, cam.origin));

                v3 rcolor = cast(&w, ray_p, ray_dir);
                color = v3_add(color, rcolor);
            }

            u32 bmp_pixel = (((u32)(255) << 24) |
                             ((u32)(255.0f * linear_to_srgb(color.r / samples)) << 16) |
                             ((u32)(255.0f * linear_to_srgb(color.g / samples)) << 8) |
                             ((u32)(255.0f * linear_to_srgb(color.b / samples)) << 0));
            *pixel++ = bmp_pixel;
        }
    }
    write_image(img, "out.bmp");

    printf("Fin.\n");
    return 0;
}
