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
static inline void PutPixel_(int x, int y, int c);
void PutPixel_external(int x, int y, int c);
void PutPixel_external_safe(int x, int y, int c);
static inline void DrawLineOct0_(int x0, int y0, int dx, int dy, int xdir, 
        int c);
static inline void DrawLineOct1_(int x0, int y0, int dx, int dy, int xdir,
        int c);
static inline void DrawHorizontalLine_(int x0, int y0, int x1, int c);
static inline void DrawVeritcalLine_(int x0, int y0, int y1, int c);
static inline void DrawSlopeOneLine_(int x0, int y0, int y1, int c);
static inline void DrawSlopeMinusOneLine_(int x0, int y0, int y1, int c);
void DrawLine_(int x0, int y0, int x1, int y1, int c);
void DrawTri_(int x0, int y0, int x1, int y1, int x2, int y2, int c);

void ClearScreen_(unsigned char grey);

#ifdef USE_SAFE_PIXEL_FUNCTIONS
#define PutPixel PutPixel_external_safe
#else
#define PutPixel PutPixel_external
#endif

#define DrawLine DrawLine_
#define DrawTri DrawTri_
#define ClearScreen ClearScreen_
#define RGBA_INT(R, G, B, A) ((A << 24) | (R << 16) | (G << 8) | B)
#endif /* RENDERER_H */
