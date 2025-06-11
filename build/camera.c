#include "math.h"
#include "camera.h"
#include "stdio.h"

#define DEFAULT_FOV_MIN_DEG     15.f
#define DEFAULT_FOV_MAX_DEG     175.f
#define DEFAULT_NEARCLIP        0.1f
#define DEFAULT_FARCLIP         100.f
#define DEFAULT_SENSITIVITY     0.04f
#define DEFAULT_TRANSLATEFACTOR 1.5f
#define DEFAULT_ROTATEFACTOR    32.f
#define DEFAULT_ZOOMFACTOR      0.04f

int CameraMakePerspectiveRH(Camera* cam, float ar, float fovDeg, float* dt){
    cam->ar =               ar;
    cam->fov =              FRADS(fovDeg);
    cam->fovMin =           FRADS(DEFAULT_FOV_MIN_DEG);
    cam->fovMax =           FRADS(DEFAULT_FOV_MAX_DEG);
    cam->nearClip =         DEFAULT_NEARCLIP;
    cam->farClip =          DEFAULT_FARCLIP;
    cam->gRight =           Vec3Make(1.f,   0.f,    0.f);
    cam->gUp =              Vec3Make(0.f,   1.f,    0.f);
    cam->gForward =         Vec3Make(0.f,   0.f,   -1.f);
    cam->right =            cam->gRight;
    cam->up =               cam->gUp;
    cam->forward =          cam->gForward;
    cam->rot =              QuatMake(1.f, 0.f, 0.f, 0.f);
    cam->dPosGlobal =       Vec3Make(0.f, 0.f, 0.f);
    cam->dPosLocal =        Vec3Make(0.f, 0.f, 0.f);
    cam->dRotGlobal =       Vec3Make(0.f, 0.f, 0.f);
    cam->dRotLocal =        Vec3Make(0.f, 0.f, 0.f);
    cam->dFov =             0.f;
    cam->viewDirty =        1;
    cam->projDirty =        1;
    cam->view =             MatIdentity();
    cam->proj =             MatIdentity();
    cam->pitch =            0.f;
    cam->yaw =              0.f;
    cam->roll =             0.f;
    cam->fovDegrees =       0.f;
    cam->sensitivity =      DEFAULT_SENSITIVITY;
    cam->translateFactor =  DEFAULT_TRANSLATEFACTOR;
    cam->rotateFactor =     DEFAULT_ROTATEFACTOR;
    cam->zoomFactor =       DEFAULT_ZOOMFACTOR;
    cam->dt =               dt;
    cam->inverseDir =       Vec3Make(0.f, 0.f, 1.f);
    UpdateFrustum(cam);
}

void CameraTransGlobalXMinus(Camera* cam){
    cam->dPosGlobal.x = -cam->translateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraTransGlobalXPlus(Camera* cam){
    cam->dPosGlobal.x = cam->translateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraTransGlobalYMinus(Camera* cam){
    cam->dPosGlobal.y = -cam->translateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraTransGlobalYPlus(Camera* cam){
    cam->dPosGlobal.y = cam->translateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraTransGlobalZMinus(Camera* cam){
    cam->dPosGlobal.z = -cam->translateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraTransGlobalZPlus(Camera* cam){
    cam->dPosGlobal.z = cam->translateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraTransLocalXMinus(Camera* cam){
    cam->dPosLocal.x = -cam->translateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraTransLocalXPlus(Camera* cam){
    cam->dPosLocal.x = cam->translateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraTransLocalYMinus(Camera* cam){
    cam->dPosLocal.y = -cam->translateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraTransLocalYPlus(Camera* cam){
    cam->dPosLocal.y = cam->translateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraTransLocalZMinus(Camera* cam){
    cam->dPosLocal.z = -cam->translateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraTransLocalZPlus(Camera* cam){
    cam->dPosLocal.z = cam->translateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraRotGlobalXMinus(Camera* cam){
    cam->dRotGlobal.x = -cam->rotateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraRotGlobalXPlus(Camera* cam){
    cam->dRotGlobal.x = cam->rotateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraRotGlobalYMinus(Camera* cam){
    cam->dRotGlobal.y = -cam->rotateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraRotGlobalYPlus(Camera* cam){
    cam->dRotGlobal.y = cam->rotateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraRotGlobalZMinus(Camera* cam){
    cam->dRotGlobal.z = -cam->rotateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraRotGlobalZPlus(Camera* cam){
    cam->dRotGlobal.z = cam->rotateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraRotLocalXMinus(Camera* cam){
    cam->dRotLocal.x = -cam->rotateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraRotLocalXPlus(Camera* cam){
    cam->dRotLocal.x = cam->rotateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraRotLocalYMinus(Camera* cam){
    cam->dRotLocal.y = -cam->rotateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraRotLocalYPlus(Camera* cam){
    cam->dRotLocal.y = cam->rotateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraRotLocalZMinus(Camera* cam){
    cam->dRotLocal.z = -cam->rotateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void CameraRotLocalZPlus(Camera* cam){
    cam->dRotLocal.z = cam->rotateFactor * *cam->dt;
    cam->viewDirty = 1;
}

void UpdateCamera(Camera* cam){
    int frustumDirty = cam->viewDirty || cam->projDirty;

    if (!cam->viewDirty) goto updateProj;

    /* calculate local translation, apply it and global translation */
    Vec3 dx = Vec3Scale(cam->right,      cam->dPosLocal.x);
    Vec3 dy = Vec3Scale(cam->up,         cam->dPosLocal.y);
    Vec3 dz = Vec3Scale(cam->forward,   -cam->dPosLocal.z);
    cam->pos = Vec3Add(cam->pos, Vec3Add(dx, Vec3Add(dy,
                Vec3Add(dz, cam->dPosGlobal))));
    /* calculate and apply global and local rotation */
    Quat dGPitchQ = QuatFromAxisAngle(cam->gRight,   FRADS(cam->dRotGlobal.x));
    Quat dGYawQ =   QuatFromAxisAngle(cam->gUp,      FRADS(cam->dRotGlobal.y));
    Quat dGRollQ =  QuatFromAxisAngle(cam->gForward, FRADS(cam->dRotGlobal.z));
    Quat gQ = QuatMul(dGPitchQ, QuatMul(dGYawQ, dGRollQ));
    Quat dLPitchQ = QuatFromAxisAngle(cam->right,    FRADS(cam->dRotLocal.x));
    Quat dLYawQ =   QuatFromAxisAngle(cam->up,       FRADS(cam->dRotLocal.y));
    Quat dLRollQ =  QuatFromAxisAngle(cam->forward,  FRADS(cam->dRotLocal.z));
    Quat lQ = QuatMul(dLPitchQ, QuatMul(dLYawQ, dLRollQ));
    cam->rot = QuatMul(gQ, QuatMul(lQ, cam->rot));
    /* calculate local reference frame */
    cam->forward =  QuatRotateVec3(cam->rot, cam->gForward);
    cam->up =       QuatRotateVec3(cam->rot, cam->gUp);
    cam->right =    QuatRotateVec3(cam->rot, cam->gRight);
    /* update view matrix */
    cam->view = Mat4LookAt(cam->pos, Vec3Add(cam->pos, cam->forward), cam->up);
    /* reset flag */
    cam->viewDirty = 0;

    /* reset deltas */
    cam->dPosGlobal = Vec3Make(0.f, 0.f, 0.f);
    cam->dPosLocal  = Vec3Make(0.f, 0.f, 0.f);
    cam->dRotGlobal = Vec3Make(0.f, 0.f, 0.f);
    cam->dRotLocal  = Vec3Make(0.f, 0.f, 0.f);

    /* update read only values TODO: needs function to decompose quat->euler */
    /*
    cam->pitch      = ;
    cam->yaw        = ;
    cam->roll       = ;
    */

    /* update misc cached values */
    cam->inverseDir = Vec3Make(-cam->forward.x, -cam->forward.y,
            -cam->forward.z);

updateProj:
    if (!cam->projDirty) goto updateFrustum;

    /* convert to rads and bounds check */
    float newFov = FRADS(cam->dFov) + cam->fov;
    newFov = newFov > cam->fovMax ? cam->fovMax : newFov;
    newFov = newFov < cam->fovMin ? cam->fovMin : newFov;
    cam->fov = newFov;
    /* calculate new projection matrix */
    cam->proj = Mat4Perspective(cam->fov, cam->ar, cam->nearClip, cam->farClip);
    /* reset deltas */
    cam->dFov = 0.f;
    /* update read only values */
    cam->fovDegrees = FDEGS(cam->fov);
    /* reset flag */
    cam->projDirty = 0;

updateFrustum:
    if (!frustumDirty) return;

    UpdateFrustum(cam);
}

void UpdateFrustum(Camera* cam){
    float tanHalfFov = tanf(cam->fov * 0.5f);
    float hN = tanHalfFov * cam->nearClip;
    float wN = hN * cam->ar;
    float hF = tanHalfFov * cam->farClip;
    float wF = hF * cam->ar;

    Vec3 nCenter = Vec3Add(cam->pos, Vec3Scale(cam->forward, cam->nearClip));
    Vec3 fCenter = Vec3Add(cam->pos, Vec3Scale(cam->forward, cam->farClip));

    cam->frustum[0] = Vec3Add(Vec3Add(nCenter, Vec3Scale(cam->up,  hN))
        , Vec3Scale(cam->right, -wN));
    cam->frustum[1] = Vec3Add(Vec3Add(nCenter, Vec3Scale(cam->up,  hN))
        , Vec3Scale(cam->right,  wN));
    cam->frustum[2] = Vec3Add(Vec3Add(nCenter, Vec3Scale(cam->up, -hN))
        , Vec3Scale(cam->right, -wN));
    cam->frustum[3] = Vec3Add(Vec3Add(nCenter, Vec3Scale(cam->up, -hN))
        , Vec3Scale(cam->right,  wN));
    cam->frustum[4] = Vec3Add(Vec3Add(fCenter, Vec3Scale(cam->up,  hF))
        , Vec3Scale(cam->right, -wF));
    cam->frustum[5] = Vec3Add(Vec3Add(fCenter, Vec3Scale(cam->up,  hF))
        , Vec3Scale(cam->right,  wF));
    cam->frustum[6] = Vec3Add(Vec3Add(fCenter, Vec3Scale(cam->up, -hF))
        , Vec3Scale(cam->right, -wF));
    cam->frustum[7] = Vec3Add(Vec3Add(fCenter, Vec3Scale(cam->up, -hF))
        , Vec3Scale(cam->right,  wF));
}
