#include "texture.h"
#include "stdio.h"
#include "stdlib.h"

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

int SampleTex(Texture* t, float u, float v){
    v = 1.f - v;
    int x = (int)(u * t->w) & (t->w - 1);
    int y = (int)(v * t->h) & (t->h - 1);
    return t->buf[y * t->w + x];
}
