#ifndef RENDERER_H
#define RENDERER_H

typedef struct {
    int w, h;
    void* buf;
} Framebuffer;

typedef struct {
    int running;
    Framebuffer framebuffer;
} Renderer;

extern Renderer renderer;

void PutPixel(int x, int y);
extern void PutPixel_ASM(int x, int y);

#endif /* RENDERER_H */
