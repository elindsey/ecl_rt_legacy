#include "main.h"
#include "ecl.h"

static v3 cast(const struct world *w, v3 origin, v3 dir, u32 bounces) {
    u32 hit_material = 0;
    f32 hit_dist = F32_MAX;
    v3 hit_normal = {0};

    for (u32 sphere_idx = 0; sphere_idx < w->sphere_count; ++sphere_idx) {
        // can I get away only checking one root due to camera orientation?
        // TODO add an assert that dir is unit vector; allows dropping a term
        struct sphere *s = &w->spheres[sphere_idx];

        v3 sphere_relative_origin = v3_sub(origin, s->p);
        f32 b = v3_dot(dir, sphere_relative_origin);
        f32 c = v3_dot(sphere_relative_origin, sphere_relative_origin) - s->r*s->r;
        f32 discr = b * b - c;
        if (discr > 0) {
            f32 root_term = ecl_sqrtf(discr);
            if (root_term > 0.0001f) { // tolerance; revisit this
                f32 tp = (-b + root_term);
                f32 tn = (-b - root_term);

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
    }

    struct material m = w->materials[hit_material];
    if (hit_material) {
        if (bounces > 0) {
            origin = v3_add(origin, v3_mulf(origin, hit_dist));
            f32 rand_x = xorshift32() / (f32)U32_MAX;
            f32 rand_y = xorshift32() / (f32)U32_MAX;
            f32 rand_z = xorshift32() / (f32)U32_MAX;
            dir = v3_add(hit_normal, (v3){rand_x, rand_y, rand_z});
            // perfect reflection; this is more marble-like
            //dir = v3_reflect(dir, hit_normal);
            return v3_add(m.emit_color, v3_mul(m.reflect_color, cast(w, origin, dir, --bounces)));
        } else {
            return m.emit_color;
        }
    } else {
        // we've hit the sky/background
        return m.emit_color;
    }
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
                    .emit_color = {0.4f, 2.8f, 0.9f},
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

    struct sphere spheres[] = {
            {
                // is it faster if the biggest spheres are first?
                    .p = {0, 0, -100},
                    .r = 100,
                    .material = 1,
            },
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
                    .r = 2.0f,
                    .material = 4,
            }
    };

    struct world w = {
            .material_count = sizeof(materials)/sizeof(struct material),
            .materials = materials,
            .sphere_count = sizeof(spheres)/sizeof(struct sphere),
            .spheres = spheres,
    };

    //struct image *img = image_new(1280, 720);
    struct image *img = image_new(480, 234);

    struct camera cam;
    camera_init(&cam, (v3){0, -10, 1}, (v3){0, 0, 0}, (f32)img->width / img->height);

    u32 *pixel = img->pixels;
    for (u32 image_y = 0; image_y < img->height; ++image_y) {
        for (u32 image_x = 0; image_x < img->width; ++image_x) {

            // rays per pixel
            u32 samples = 100;
            v3 color = {0, 0, 0};
            for (u32 rcount = 0; rcount < samples; ++rcount) {
                // calculate ratio we've moved along the image (y/height), step proportionally within the viewport
                f32 rand_x = xorshift32() / (f32)U32_MAX; // this bounds limiting could be more efficient, and might have an off by one
                f32 rand_y = xorshift32() / (f32)U32_MAX;
                //f32 rand_x = rand() / (f32)RAND_MAX;
                v3 viewport_y = v3_mulf(cam.y, cam.viewport_height * (image_y + rand_y) / (img->height-1.0f));
                v3 viewport_x = v3_mulf(cam.x, cam.viewport_width * (image_x + rand_x) / (img->width-1.0f));
                v3 viewport_p = v3_add(v3_add(cam.viewport_lower_left, viewport_y), viewport_x);
                // remember that a pixel in float-space is a _range_. We want to send multiple rays within that range
                // to do this we take the start of that range (what we calculated as the image projecting onto our viewport),
                // then add a random [0,1) float

                v3 ray_p = cam.origin;
                v3 ray_dir = v3_normalize(v3_sub(viewport_p, cam.origin));

                v3 rcolor = cast(&w, ray_p, ray_dir, 8);
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
