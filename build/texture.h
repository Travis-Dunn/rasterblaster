#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
    int w, h;
    int* buf;
} Texture;

Texture*            LoadBimg        (char* filename);
void                FreeTexture     (Texture* t);
int                 SampleTex       (Texture* t, float u, float v);
static inline int   WrapRepeatI_    (int i, int n);

#endif /* TEXTURE_H */
