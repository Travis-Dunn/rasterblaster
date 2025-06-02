#ifndef SHADOWMAPPER_H
#define SHADOWMAPPER_H

#include "arithmetic.h"
#include "camera.h"
#include "depthbuffer.h"
#include "obj3d.h"

typedef struct {
    int init;
    int w, h;
    int sampleCount;
    Mat4 matTransform;
    float bias;
    Camera* cam;
    Vec3 lightDir;
    DepthBuffer db;
    float* buf;
} ShadowMapper;

void               ShadowMapperUpdate   (ShadowMapper* sm);
int                ShadowMapperInit     (ShadowMapper* sm, int w, int h,
        float bias, Camera* cam, Vec3 lightDir);
int                ShadowMapperFree     (ShadowMapper* sm);
void               ShadowMapperClear    (ShadowMapper* sm, float val);
void               ShadowMapperRender   (ShadowMapper* sm, Obj3D* obj);
static inline void RasterizeTri_        (ShadowMapper* sm, int x0, int y0,
        float z0, int x1, int y1, float z1, int x2, int y2, float z2);

#define FLT_MAX 1e37
#define MAX_W 3840
#define MAX_H 2160

#endif /* SHADOWMAPPER_H */
