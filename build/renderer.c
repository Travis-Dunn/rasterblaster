#include "renderer.h"

Framebuffer framebuffer = {0};
Renderer renderer = {0};

void PutPixel(int x, int y){
    unsigned char* pixel = (unsigned char*)renderer.framebuffer.buf +
        (y * renderer.framebuffer.w * 4 + x * 4);
    pixel[0] = 255;
    pixel[1] = 255;
    pixel[2] = 255;
    pixel[3] = 255;
}
