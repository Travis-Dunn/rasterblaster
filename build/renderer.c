#include "stdlib.h"
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
    if (x >= renderer.framebuffer.w || x < 0 ||
        y >= renderer.framebuffer.h || y < 0) return;
    unsigned char* pixel = (unsigned char*)renderer.framebuffer.buf +
        (y * renderer.framebuffer.w * 4 + x * 4);
    pixel[0] = 255;
    pixel[1] = 255;
    pixel[2] = 255;
    pixel[3] = 255;
}

void DrawLine_(int x0, int y0, int x1, int y1){
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int steep = dy > dx;
    if (steep){
        int temp = x0;
        x0 = y0;
        y0 = temp;
        temp = x1;
        x1 = y1;
        y1 = temp;
    }
    if (x0 > x1){
        int temp = x0;
        x0 = x1;
        x1 = temp;
        temp = y0;
        y0 = y1;
        y1 = temp;
    }
    dx = abs(x1 - x0);
    dy = abs(y1 - y0);
    int error = dx / 2;
    int ystep = (y0 < y1) ? 1 : -1;
    int y = y0;
    int x;
    for (x = x0; x <= x1; x++){
        if (steep){
            PutPixel(y, x);
        } else {
            PutPixel(x, y);
        }
        error -= dy;
        if (error < 0){
            y += ystep;
            error += dx;
        }
    }
}
