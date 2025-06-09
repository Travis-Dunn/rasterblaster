#ifndef CAMERA_H
#define CAMERA_H

#include "arithmetic.h"

typedef struct {
/**************************    Internal parameters    *************************/
    float ar;
    float fov;                  /* Radians */
    float fovMin, fovMax;       /* Radians */
    float nearClip, farClip;
    Vec3 gRight, gUp, gForward; /* global reference frame */
    Vec3 pos;
    Vec3 right, up, forward;    /* local reference frame */
    Quat rot;                   /* Radians */
    Vec3 dPosGlobal, dPosLocal;
    Vec3 dRotGlobal, dRotLocal; /* Euler degrees */
    float dFov;                 /* degrees */
    int viewDirty, projDirty;
    Mat4 view, proj;
/*********************    Derived, read only parameters    ********************/
    float pitch, yaw, roll;     /* Degrees */
    float fovDegrees;
/***************************    Tuning parameters    **************************/
    float sensitivity;          /* Unitless scalar */
    float translateFactor;      /* Unitless scalar */
    float rotateFactor;         /* Unitless scalar */
    float zoomFactor;           /* Unitless scalar */
/*********    Miscellaneous cached values used by other components    *********/
    float* dt;                  /* delta time seconds */
    Vec3 inverseDir;
    Vec3 frustum[8]; /* world space: ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr */
} Camera;

/* all other aspects of the camera such as fov limits are set to default values
 * #defined in camera.c */
int CameraMakePerspectiveRH(Camera* cam, float ar, float fovDeg, float* dt);
void TranslateGlobalLeft(Camera* cam);
void UpdateCamera(Camera* cam);
void UpdateFrustum(Camera* cam);

#endif /* CAMERA_H */
