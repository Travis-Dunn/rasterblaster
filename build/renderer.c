#include "renderer.h"
#include "stdio.h"

Framebuffer framebuffer = {0};
Renderer renderer = {0};

void PutPixel_(int x, int y){
    unsigned char* pixel = (unsigned char*)renderer.framebuffer.buf +
        (y * renderer.framebuffer.w * 4 + x * 4);
    pixel[0] = 255;
    pixel[1] = 255;
    pixel[2] = 255;
    pixel[3] = 255;
}

void PutPixel_safe(int x, int y){
    printf("using safe pixel func");
    if (x >= renderer.framebuffer.w || x < 0 ||
        y >= renderer.framebuffer.h || y < 0) return;
    unsigned char* pixel = (unsigned char*)renderer.framebuffer.buf +
        (y * renderer.framebuffer.w * 4 + x * 4);
    pixel[0] = 255;
    pixel[1] = 255;
    pixel[2] = 255;
    pixel[3] = 255;
}
