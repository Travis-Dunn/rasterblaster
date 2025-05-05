#ifndef RENDERER_H
#define RENDERER_H

#include "config.h"

typedef struct {
    int w, h;
    void* buf;
} Framebuffer;

typedef struct {
    int running;
    Framebuffer framebuffer;
} Renderer;

extern Renderer renderer;

void PutPixel_(int x, int y);
void PutPixel_safe(int x, int y);
extern void PutPixel_ASM(int x, int y);
void DrawLine_(int x0, int y0, int x1, int y1);

#ifdef USE_SAFE_PIXEL_FUNCTIONS
#define PutPixel PutPixel_safe
#else
#define PutPixel PutPixel_
#endif

#define DrawLine DrawLine_
#endif /* RENDERER_H */
