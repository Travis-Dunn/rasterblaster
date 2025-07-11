#include "stdio.h"
#include "assert.h"
#include "math.h"
#include "camera.h"

#define DEFAULT_FOV_MIN_DEG     15.f
#define DEFAULT_FOV_MAX_DEG     175.f
#define DEFAULT_NEARCLIP        0.1f
#define DEFAULT_FARCLIP         100.f
#define DEFAULT_SENSITIVITY     0.04f
#define DEFAULT_TRANSLATEFACTOR 1.5f
#define DEFAULT_ROTATEFACTOR    3.f
#define DEFAULT_ZOOMFACTOR      0.04f

int CameraMakePerspectiveRH(Camera* cam, float ar, float fovDeg, float* dt){
    assert(fovDeg > DEFAULT_FOV_MIN_DEG && fovDeg < DEFAULT_FOV_MAX_DEG);
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
    cam->fovDegrees =       fovDeg;
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

void CameraRotLocalXFloat(Camera* cam, float dx){
    cam->dRotLocal.x = -cam->rotateFactor * *cam->dt * dx;
    cam->viewDirty = 1;
}

void CameraRotLocalYFloat(Camera* cam, float dy){
    cam->dRotLocal.y = -cam->rotateFactor * *cam->dt * dy;
    cam->viewDirty = 1;
}

void UpdateCamera(Camera* cam){
    int frustumDirty = cam->viewDirty || cam->projDirty;

    if (!cam->viewDirty) goto updateProj;

    Vec3 dx = Vec3Scale(cam->right,      cam->dPosLocal.x);
    Vec3 dy = Vec3Scale(cam->up,         cam->dPosLocal.y);
    Vec3 dz = Vec3Scale(cam->forward,   -cam->dPosLocal.z);
    cam->pos = Vec3Add(cam->pos, Vec3Add(dx, Vec3Add(dy,
                Vec3Add(dz, cam->dPosGlobal))));

    Quat dGPitchQ = QuatFromAxisAngle(cam->gRight,   FRADS(cam->dRotGlobal.x));
    Quat dGYawQ =   QuatFromAxisAngle(cam->gUp,      FRADS(cam->dRotGlobal.y));
    Quat dGRollQ =  QuatFromAxisAngle(cam->gForward, FRADS(cam->dRotGlobal.z));
    Quat gQ = QuatMul(dGPitchQ, QuatMul(dGYawQ, dGRollQ));
    Quat dLPitchQ = QuatFromAxisAngle(cam->right,    FRADS(cam->dRotLocal.x));
    Quat dLYawQ =   QuatFromAxisAngle(cam->up,       FRADS(cam->dRotLocal.y));
    Quat dLRollQ =  QuatFromAxisAngle(cam->forward,  FRADS(cam->dRotLocal.z));
    Quat lQ = QuatMul(dLPitchQ, QuatMul(dLYawQ, dLRollQ));
    /* is this order right?, It works... */
    cam->rot = QuatMul(cam->rot, QuatMul(lQ, gQ));
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
    /* this is also a candidate for removal */
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
    /* candidate for removal */
    UpdateViewFrustum(cam);
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

void UpdateViewFrustum(Camera* cam){
    float tanHalfFov = tanf(cam->fov * 0.5f);
    float nearHeight = tanHalfFov * cam->nearClip;
    float nearWidth = nearHeight * cam->ar;

    cam->viewFrustum.nearPlane.normal = Vec3Make(0.f, 0.f, 1.f);
    cam->viewFrustum.nearPlane.distance = cam->nearClip;
    cam->viewFrustum.farPlane.normal = Vec3Make(0.f, 0.f, -1.f);
    cam->viewFrustum.farPlane.distance = cam->farClip;
    float leftNormalX = nearHeight;
    float leftNormalZ = nearWidth;
    float leftLength = 
        sqrtf(leftNormalX * leftNormalX + leftNormalZ * leftNormalZ);
    cam->viewFrustum.leftPlane.normal = Vec3Make(leftNormalX / leftLength,
            0.f, leftNormalZ / leftLength);
    cam->viewFrustum.leftPlane.distance = 0.f;
    cam->viewFrustum.rightPlane.normal = Vec3Make(-leftNormalX / leftLength,
            0.f, leftNormalZ / leftLength);
    cam->viewFrustum.rightPlane.distance = 0.f;
    float topNormalY = nearWidth;
    float topNormalZ = nearHeight;
    float topLength = sqrtf(topNormalY * topNormalY + topNormalZ * topNormalZ);
    cam->viewFrustum.topPlane.normal = Vec3Make(0.f, -topNormalY / topLength,
            topNormalZ / topLength);
    cam->viewFrustum.topPlane.distance = 0.f;
    cam->viewFrustum.bottomPlane.normal = Vec3Make(0.f, topNormalY / topLength,
            topNormalZ / topLength);
    cam->viewFrustum.bottomPlane.distance = 0.f;
}

void CameraPrint(Camera* cam){
    printf("===================================\n");
    printf("pos: (%.2f, %.2f, %.2f)\n", cam->pos.x, cam->pos.y, cam->pos.z);
    printf("right: (%.2f, %.2f, %.2f)\n", cam->right.x, cam->right.y,
            cam->right.z);
    printf("up: (%.2f, %.2f, %.2f)\n", cam->up.x, cam->up.y, cam->up.z);
    printf("forward: (%.2f, %.2f, %.2f)\n", cam->forward.x, cam->forward.y,
            cam->forward.z);
    printf("gRight: (%.2f, %.2f, %.2f)\n", cam->gRight.x, cam->gRight.y,
            cam->gRight.z);
    printf("gUp: (%.2f, %.2f, %.2f)\n", cam->gUp.x, cam->gUp.y, cam->gUp.z);
    printf("gForward: (%.2f, %.2f, %.2f)\n", cam->gForward.x, cam->gForward.y,
            cam->gForward.z);
}

void CameraRotSnapLocalY(Camera* cam, float deg){
    cam->dRotLocal.y = deg;
    cam->viewDirty = 1;
}
