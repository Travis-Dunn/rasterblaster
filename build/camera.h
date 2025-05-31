#ifndef CAMERA_H
#define CAMERA_H

#include "arithmetic.h"

typedef struct {
    float ar;
    float fovRads;
    float nearClip, farClip;
    Vec3 gRight, gUp, gForward; /* global reference frame */
    Vec3 pos;
    Vec3 right, up, forward; /* local reference frame */
    Quat rot;
    Mat4 view, proj;

    Vec3 inverseDir;
    Vec3 frustum[8]; /* world space: ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr */
} Camera;

void UpdateFrustum(Camera* cam);

#endif /* CAMERA_H */
