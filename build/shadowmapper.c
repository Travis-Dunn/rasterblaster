#include "math.h"
#include "shadowmapper.h"

#define FLT_MAX 1e37

/* light dir must not be parallel to global up */
void UpdateShadowMapper(ShadowMapper* sm){
    Vec3 centre = Vec3Make(0.f, 0.f, 0.f);
    for (int i = 0; i < 8; i++) centre = Vec3Add(centre, sm->cam->frustum[i]);
    centre = Vec3Scale(centre, 1.f / 8.f);
    Vec3 eye = Vec3Sub(centre, sm->lightDir);
    Mat4 matLightView = Mat4LookAt(eye, centre, sm->cam->gUp);

    Vec3 ls[8]; /* view frustum in light space */
    for (int i = 0; i < 8; i++){
        Vec4 p = Vec4Make(sm->cam->frustum[i].x, sm->cam->frustum[i].y,
                sm->cam->frustum[i].z, 1.f);
        Vec4 q = MatVertMul(&matLightView, p);
        ls[i] = Vec3Make(q.x, q.y, q.z);
    }
    float minX, maxX, minY, maxY, minZ, maxZ;
    minX = minY = minZ = FLT_MAX;
    maxX = maxY = maxZ = -FLT_MAX;
    for (int i = 0; i < 8; i++){
        minX = fminf(minX, ls[i].x); maxX = fmaxf(maxX, ls[i].x);
        minY = fminf(minY, ls[i].y); maxY = fmaxf(maxY, ls[i].y);
        minZ = fminf(minZ, ls[i].z); maxZ = fmaxf(maxZ, ls[i].z);
    }
    Mat4 matLightProj = MatIdentity();
    float nearZ = minZ - sm->bias;
    float farZ = maxZ + sm->bias;
    matLightProj.m[0][0] = 2.f / (maxX - minX);
    matLightProj.m[1][1] = 2.f / (maxY - minY);
    matLightProj.m[2][2] = -2.f / (farZ - nearZ);
    matLightProj.m[0][3] = -(maxX + minX) / (maxX - minX);
    matLightProj.m[1][3] = -(maxY + minY) / (maxY - minY);
    matLightProj.m[2][3] = -(farZ + nearZ) / (farZ - nearZ);
    sm->matTransform = MatMatMul(&matLightProj, &matLightView);
}
