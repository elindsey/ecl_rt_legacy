#include "ecl_rt.h"
#include "ecl_math.h"
#include <stdlib.h>

static const struct sphere spheres[] = {
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
                .p = {-2, -3, 1.5f},
                .r = 0.3f,
                .material = 4,
        },
        {
                .p = {-3, -6, 0},
                .r = 0.3f,
                .material = 4,
        },
        {
                .p = {-3, -5, 2.0f},
                .r = 0.5f,
                .material = 3,
        },
        {
                .p = {3, -3, 0.8f},
                .r = 1.0f,
                .material = 4,
        }
};
static const u32 sphere_count = sizeof(spheres)/sizeof(struct sphere);

static const struct material materials[] = {
        { // background
                .emit_color = {0.3f, 0.4f, 0.8f},
                .reflect_color = {0, 0, 0},
        },
        { // ground
                .emit_color = {0, 0, 0},
                .reflect_color = {0.5f, 0.5f, 0.5f},
        },
        { // center
                .emit_color = {0.4f, 0.8f, 0.9f},
                .reflect_color = {0.8f, 0.8f, 0.8f},
        },
        { // red left
                .emit_color = {0, 0, 0},
                .reflect_color = {1, 0, 0},
        },
        { // right
                .emit_color = {0, 0, 0},
                .reflect_color = {0.95f, 0.95f, 0.95f},
        },
};

static v3 cast(v3 origin, v3 dir, u32 bounces) {
    u32 hit_material = 0; // background material
    f32 hit_dist = F32_MAX;
    v3 hit_normal, hit_p;
    f32 tolerance = 0.0001f;

    for (u32 sphere_idx = 0; sphere_idx < sphere_count; ++sphere_idx) {
        // can I get away only checking one root due to camera orientation?
        // TODO add an assert that dir is unit vector; allows dropping the 'a' term
        const struct sphere *s = &spheres[sphere_idx];

        v3 sphere_relative_origin = v3_sub(origin, s->p);
        f32 b = v3_dot(dir, sphere_relative_origin);
        f32 c = v3_dot(sphere_relative_origin, sphere_relative_origin) - s->r*s->r;
        f32 discr = b * b - c;
        if (discr > 0) { // at least one real root, meaning we've hit the sphere
            f32 root_term = sqrtf(discr);
            if (root_term > 0.0001f) { // tolerance; revisit this
                /*
                 * Order here matters. root_term is positive; b may be positive or negative
                 *
                 * If b is negative, -b is positive, so -b + root_term is _more_ positive than -b - root_term
                 * Thus we check -b - root_term first; if it's negative, we check -b + root_term. This is why -b - root_term
                 * must be first.
                 *
                 * Second case is less interesting
                 * If b is positive, -b is negative, so -b - root_term is more negative and we will then check -b + root_term
                 */
                f32 t = (-b - root_term); // -b minus pos
                if (t > tolerance && t < hit_dist) {
                    hit_dist = t;
                    hit_material = s->material;
                    hit_p = v3_add(origin, v3_mulf(dir, hit_dist));
                    // TODO: clean this up, reuse some of the computation from intersection math
                    // technically this could be normalized with mulf by 1/s->r, b/c length of that vector is the radius
                    hit_normal = v3_normalize(v3_sub(hit_p, s->p));
                    continue;
                }
                t = (-b + root_term); // -b plus pos
                if (t > tolerance && t < hit_dist) {
                    hit_dist = t;
                    hit_material = s->material;
                    hit_p = v3_add(origin, v3_mulf(dir, hit_dist));
                    // TODO: clean this up, reuse some of the computation from intersection math
                    // technically this could be normalized with mulf by 1/s->r, b/c length of that vector is the radius
                    hit_normal = v3_normalize(v3_sub(hit_p, s->p));
                    continue;
                }
            }
        }
    }

    const struct material *m = &materials[hit_material];
    if (hit_material) {
        if (bounces > 0) {
            f32 rand_x = randf01();
            f32 rand_y = randf01();
            f32 rand_z = randf01();
            //dir = v3_add(hit_normal, (v3){rand_x, rand_y, rand_z});
            // perfect reflection; this is more marble-like
            dir = v3_reflect(dir, hit_normal);
            return v3_add(m->emit_color, v3_mul(m->reflect_color, cast(hit_p, dir, --bounces)));
        } else {
            return m->emit_color;
        }
    } else {
        // we've hit the sky/background
        return m->emit_color;
    }
}

int main() {
    //const u32 width = 480;
    //const u32 height = 234;
    const u32 width = 1280;
    const u32 height = 720;
    u32 *pixels = malloc(width * height * sizeof(*pixels));

    // rays per pixel
    u32 samples = 100;
    struct camera cam;
    camera_init(&cam, (v3){0, -10, 1}, (v3){0, 0, 0}, (f32)width / height);

    u32 total_rays = width * height * samples;

    u32 *pixel = pixels;
    for (u32 image_y = 0; image_y < height; ++image_y) {
        printf("%.2lf%%...\n", image_y * width * samples * 1.0 / total_rays * 100.0);
        for (u32 image_x = 0; image_x < width; ++image_x) {

            v3 color = {0, 0, 0};
            for (u32 rcount = 0; rcount < samples; ++rcount) {
                // calculate ratio we've moved along the image (y/height), step proportionally within the viewport
                f32 rand_x = randf01();
                f32 rand_y = randf01();
                v3 viewport_y = v3_mulf(cam.y, cam.viewport_height * (image_y + rand_y) / (height-1.0f));
                v3 viewport_x = v3_mulf(cam.x, cam.viewport_width * (image_x + rand_x) / (width-1.0f));
                v3 viewport_p = v3_add(v3_add(cam.viewport_lower_left, viewport_y), viewport_x);
                // remember that a pixel in float-space is a _range_. We want to send multiple rays within that range
                // to do this we take the start of that range (what we calculated as the image projecting onto our viewport),
                // then add a random [0,1) float

                v3 ray_p = cam.origin;
                v3 ray_dir = v3_normalize(v3_sub(viewport_p, cam.origin));

                v3 rcolor = cast(ray_p, ray_dir, 8);
                color = v3_add(color, rcolor);
            }

            u32 bmp_pixel = (((u32)(255) << 24) |
                             ((u32)(255.0f * linear_to_srgb(color.r / samples)) << 16) |
                             ((u32)(255.0f * linear_to_srgb(color.g / samples)) << 8) |
                             ((u32)(255.0f * linear_to_srgb(color.b / samples)) << 0));
            *pixel++ = bmp_pixel;
        }
    }
    write_image(width, height, pixels, "out.bmp");

    printf("Fin.\n");
    return 0;
}
