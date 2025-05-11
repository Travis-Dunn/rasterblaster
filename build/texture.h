#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
    int w, h;
    int* buf;
} Texture;

Texture* LoadBimg(char* filename);
void FreeTexture(Texture* t);
int SampleTex(Texture* t, float u, float v);

#endif /* TEXTURE_H */
