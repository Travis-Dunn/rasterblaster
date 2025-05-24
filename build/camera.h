#ifndef CAMERA_H
#define CAMERA_H

#include "arithmetic.h"

typedef struct {
    float ar;
    float fovRads;
    float nearClip, farClip;
    Vec3 gRight, gUp, gForward;
    Vec3 pos;
    Vec3 right, up, forward;
    Quat rot;
    Mat4 view, proj;

    Vec3 inverseDir;
} Camera;

#endif /* CAMERA_H */
