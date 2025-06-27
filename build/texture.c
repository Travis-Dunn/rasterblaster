#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "texture.h"

Texture* LoadBimg(char* filename){
    FILE* f = fopen(filename, "rb");
    if (!f) return 0;
    Texture* t = (Texture*)calloc(1, sizeof(Texture));
    if (!t) {
        fclose(f);
        return 0;
    }
    fread(&t->w, 4, 1, f);
    fread(&t->h, 4, 1, f);
    int bufSize = t->w * t->h * 4; /* only works with 32bpp! */
    t->buf = (int*)malloc(bufSize);
    if (!t->buf){
        free(t);
        fclose(f);
        return 0;
    }
    fread(t->buf, 1, bufSize, f);
    fclose(f);
    return t;
}

void FreeTexture(Texture* t){
    if (t){
        free(t->buf);
        free(t);
    }
}

static inline int WrapRepeatI_(int i, int n){
    i %= n;
    return (i <0) ? i + n : i;
}

int SampleTex(Texture* t, float u, float v){
    u = u - floorf(u);
    v = 1.f - (v - floorf(v));
    int x = (int)(u * t->w);
    int y = (int)(v * t->h);
    x = WrapRepeatI_(x, t->w);
    y = WrapRepeatI_(y, t->h);
    return t->buf[y * t->w + x];
}
