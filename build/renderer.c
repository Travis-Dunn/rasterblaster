#include "stdlib.h"
#include "renderer.h"
#include "stdio.h"

Framebuffer framebuffer = {0};
Renderer renderer = {0};

static inline void PutPixel_new(int x, int y, int c){
    int* pixel = (int*)renderer.framebuffer.buf +
        (y * renderer.framebuffer.w + x);
    *pixel = c;
}

void PutPixel_external(int x, int y, int c){
    PutPixel_new(x, y, c);
}

void PutPixel_external_safe(int x, int y, int c){
    if (x >= renderer.framebuffer.w || x < 0 ||
        y >= renderer.framebuffer.h || y < 0) return;
    PutPixel_new(x, y, c);
}
