#include "stdlib.h"
#include "string.h"
#include "renderer.h"
#include "stdio.h"

Framebuffer framebuffer = {0};
Renderer renderer = {0};

static inline void PutPixel_(int x, int y, int c){
    int* pixel = (int*)renderer.framebuffer.buf +
        (y * renderer.framebuffer.w + x);
    *pixel = c;
}

void PutPixel_external(int x, int y, int c){
    PutPixel_(x, y, c);
}

void PutPixel_external_safe(int x, int y, int c){
    if (x >= renderer.framebuffer.w || x < 0 ||
        y >= renderer.framebuffer.h || y < 0) return;
    PutPixel_(x, y, c);
}

static inline void DrawLineOct0_(int x0, int y0, int dx, int dy, int xdir, 
        int c){
    int dYx2, dYx2MinusdXx2, err;
    dYx2 = dy * 2;
    dYx2MinusdXx2 = dYx2 - (dx * 2);
    err = dYx2 - dx;
    PutPixel_(x0, y0, c);
    while (dx--){
        if (err >= 0){
            y0++;
            err += dYx2MinusdXx2;
        } else err += dYx2;
        x0 += xdir;
        PutPixel_(x0, y0, c);
    }
}

static inline void DrawLineOct1_(int x0, int y0, int dx, int dy, int xdir,
        int c){
    int dXx2, dXx2MinusdYx2, err;
    dXx2 = dx * 2;
    dXx2MinusdYx2 = dXx2 - (dy * 2);
    err = dXx2 - dy;
    PutPixel_(x0, y0, c);
    while (dy--){
        if (err >= 0){
            x0 += xdir;
            err += dXx2MinusdYx2;
        } else err += dXx2;
        y0++;
        PutPixel_(x0, y0, c);
    }
}

static inline void DrawHorizontalLine_(int x0, int y0, int x1, int c){
    if (x1 < x0){
        int temp = x0;
        x0 = x1;
        x1 = temp;
    }
    while(x0 <= x1){
        PutPixel_(x0, y0, c);
        x0++;
    }
}

static inline void DrawVerticalLine_(int x0, int y0, int y1, int c){
    while(y0 <= y1){
        PutPixel_(x0, y0, c);
        y0++;
    }
}

void DrawLine_(int x0, int y0, int x1, int y1, int c){
    int dx, dy, temp;
    if (y0 > y1){
        temp = y0;
        y0 = y1;
        y1 = temp;
        temp = x0;
        x0 = x1;
        x1 = temp;
    }
    dx = x1 - x0;
    dy = y1 - y0;
    if (dy == 0){
        DrawHorizontalLine_(x0, y0, x1, c); 
        return;
    }
    if (dx == 0){
        DrawVerticalLine_(x0, y0, y1, c);
        return;
    }
    if (dx > 0){
        if (dx > dy){
            DrawLineOct0_(x0, y0, dx, dy, 1, c);
            return;
        } else {
            DrawLineOct1_(x0, y0, dx, dy, 1, c);
            return;
        }
    } else {
        dx = -dx;
        if (dx > dy){
            DrawLineOct0_(x0, y0, dx, dy, -1, c);
            return;
        } else {
            DrawLineOct1_(x0, y0, dx, dy, -1, c);
            return;
        }
    }
}

void DrawTri_(int x0, int y0, int x1, int y1, int x2, int y2, int c){
    DrawLine_(x0, y0, x1, y1, c);
    DrawLine_(x1, y1, x2, y2, c);
    DrawLine_(x2, y2, x0, y0, c);
}

void ClearScreen_(unsigned char grey){
    (void)memset(renderer.framebuffer.buf, grey, sizeof(unsigned char) *
            renderer.framebuffer.w * renderer.framebuffer.h * 4);
}
