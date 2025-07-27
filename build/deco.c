#include "assert.h"
#include "stdlib.h"
#include "deco.h"

#define SUCCESS              0
#define ILLEGAL_COUNT       -1
#define ALLOCATION_FAILURE  -2
#define DECO_CAP_EXCEEDED   -3
#define UNINITIALIZED_MODL  -4

#define MAX_DECOS 0xFFFF

static Deco* decos;
static int decoCap = 0;
static int decoCount = 0;
static int decoInit = 0;

static HeightMap* heightMap;

int DecoInit(int cap, HeightMap* hm) {
    assert(!decoInit); assert(hm);
    heightMap = hm;
    decoCap = (cap > 0 && cap <= MAX_DECOS) ? cap : 0;
    if (!decoCap) return ILLEGAL_COUNT;
    decos = (Deco*)malloc(decoCap * sizeof(Deco));
    if (!decos) {
        decoCap = 0;
        return ALLOCATION_FAILURE;
    }
    decoInit = 1;
    return SUCCESS;
}

int DecoMake(ModlEntry e, float scale, float rx, float ry, float rz, float x,
        float y, float z, int tileX, int tileZ) {
    assert(decoInit);
    if (decoCount >= decoCap) return DECO_CAP_EXCEEDED;
    decos[decoCount].modl = ModlGet(e);
    if (!(decos[decoCount].modl.flags & FLAG_INIT)) return UNINITIALIZED_MODL;
    decos[decoCount].scale = scale;
    decos[decoCount].rot = Vec3Make(rx, ry, rz);
    decos[decoCount].offset = Vec3Make(x, y, z);
    decos[decoCount].tile = (tileX << 16) | (tileZ & 0xFFFF);

    MatModelMake(&decos[decoCount]);

    decoCount++;

    return SUCCESS;
}

void DecoDraw(Camera* cam) {
    assert(cam); assert(decoInit); assert(decoCount > 0);

    for (int i  = 0; i < decoCount; i++) {
        

        switch(decos[i].modl.flags & FLAG_MTYPE_MASK) {
        case FLAG_MTYPE_COL: {
            ModelColDraw(cam, decos[i].modl.m.mCol, &decos[i].matModel);
        } break;
        }
    }
}

static inline Mat4 MatModelMake(Deco* d) {
    Mat4 scale = MatScale(d->scale, d->scale, d->scale);
    Mat4 pitch = MatPitch(d->rot.x);
    Mat4 yaw = MatYaw(d->rot.y);
    Mat4 roll = MatRoll(d->rot.z);
    Mat4 rot = MatMatMul(&roll, &pitch);
    rot = MatMatMul(&yaw, &rot);
    Mat4 m = MatMatMul(&rot, &scale);

    int x = (d->tile >> 16);
    int z = (d->tile & 0xFFFF);
    float height = heightMap->m[(z + 16) * 32 + (x + 16)];

    Mat4 trans = MatTranslate(d->offset.x + x + 0.5f, d->offset.y + height,
            d->offset.z + z + 0.5f);
    d->matModel = MatMatMul(&trans, &m);
}
