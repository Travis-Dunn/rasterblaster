#ifndef SHADOWMAPPER_H
#define SHADOWMAPPER_H

#include "arithmetic.h"
#include "camera.h"

typedef struct {
    int init;
    int w, h;
    int sampleCount;
    Mat4 matTransform;
    float bias;
    Camera* cam;
    Vec3 lightDir;
    float* buf;
} ShadowMapper;

void UpdateShadowMapper(ShadowMapper* sm);

#endif /* SHADOWMAPPER_H */
