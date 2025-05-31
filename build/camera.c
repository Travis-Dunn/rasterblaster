#include "math.h"
#include "camera.h"


void UpdateFrustum(Camera* cam){
    float tanHalfFov = tanf(cam->fovRads * 0.5f);
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
