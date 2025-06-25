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
    /* world space view frustum verts: ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr */
    Vec3 frustum[8]; 
    Frustum viewFrustum;
} Camera;

/* all other aspects of the camera such as fov limits are set to default values
 * #defined in camera.c */
int CameraMakePerspectiveRH(Camera* cam, float ar, float fovDeg, float* dt);
void CameraTransGlobalXMinus(Camera* cam);
void CameraTransGlobalXPlus(Camera* cam);
void CameraTransGlobalYMinus(Camera* cam);
void CameraTransGlobalYPlus(Camera* cam);
void CameraTransGlobalZMinus(Camera* cam);
void CameraTransGlobalZPlus(Camera* cam);
void CameraTransLocalXMinus(Camera* cam);
void CameraTransLocalXPlus(Camera* cam);
void CameraTransLocalYMinus(Camera* cam);
void CameraTransLocalYPlus(Camera* cam);
void CameraTransLocalZMinus(Camera* cam);
void CameraTransLocalZPlus(Camera* cam);
void CameraRotGlobalXMinus(Camera* cam);
void CameraRotGlobalXPlus(Camera* cam);
void CameraRotGlobalYMinus(Camera* cam);
void CameraRotGlobalYPlus(Camera* cam);
void CameraRotGlobalZMinus(Camera* cam);
void CameraRotGlobalZPlus(Camera* cam);
void CameraRotLocalXMinus(Camera* cam);
void CameraRotLocalXPlus(Camera* cam);
void CameraRotLocalYMinus(Camera* cam);
void CameraRotLocalYPlus(Camera* cam);
void CameraRotLocalZMinus(Camera* cam);
void CameraRotLocalZPlus(Camera* cam);
void CameraRotLocalXFloat(Camera* cam, float dx);
void CameraRotLocalYFloat(Camera* cam, float dy);
void UpdateCamera(Camera* cam);
void UpdateFrustum(Camera* cam); /* for the array of Vec3 for shadow mapping */
void UpdateViewFrustum(Camera* cam); /* for the struct of planes for clipping */
void CameraPrint(Camera* cam);
void CameraRotSnapLocalYMinus(Camera* cam, float deg);

#endif /* CAMERA_H */
