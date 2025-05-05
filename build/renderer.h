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

typedef struct {
    int x, y;
} Point2D;

extern Renderer renderer;

extern void PutPixel_ASM(int x, int y);
static inline void PutPixel_new(int x, int y, int c);
void PutPixel_external(int x, int y, int c);
void PutPixel_external_safe(int x, int y, int c);

#ifdef USE_SAFE_PIXEL_FUNCTIONS
#define PutPixel PutPixel_external_safe
#else
#define PutPixel PutPixel_external
#endif

#define DrawLine DrawLine_
#define DrawRect DrawRect_
#define DrawFilledRect DrawFilledRect_
#define RGBA_INT(R, G, B, A) ((A << 24) | (R << 16) | (G << 8) | B)
#endif /* RENDERER_H */
