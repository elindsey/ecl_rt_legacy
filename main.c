#include "bmp.h"
#include "v3.h"
#include <stdio.h>

int main() {
    printf("Hello, World!\n");
    uint32_t px[3] = {100, 0, 55};
    image img = {1, 3, px};
    write_image(img, "test.bmp");
    return 0;
}
