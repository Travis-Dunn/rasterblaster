#ifndef DECO_H
#define DECO_H

#include "renderer.h"
#include "ground.h"
#include "Camera.h"
#include "modl.h"

typedef struct {
    Modl modl;
    float scale;
    Vec3 rot;
    Vec3 offset;
    int tile;
    Mat4 matModel;
} Deco;

int DecoInit(int count, HeightMap* hm);
int DecoMake(ModlEntry e, float scale, float rx, float ry, float rz, float x,
        float y, float z, int tileX, int tileZ);
void DecoDraw(Camera* cam);

static inline Mat4 MatModelMake(Deco* d);

#endif /* DECO_H */
