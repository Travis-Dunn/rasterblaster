#include "stdlib.h"
#include "stdio.h"
#include "math.h"
#include "shadowmapper.h"
#include "renderer.h"


int ShadowMapperInit(ShadowMapper* sm, int w, int h, float bias, Camera* cam,
        Vec3 lightDir){
    if (!sm){
        /* TODO */
        printf("can't pass null to ShadowMapperInit\n");
        return 1;
    }
    if (!cam){
        /* this one as well */
        printf("can't pass null Camera* to ShadowMapperInit\n");
        return 1;
    }
    if (sm->init){
        /* TODO: some sort of warning or something, print statement is temp */
        printf("What a buffoon! Init ShadowMapper once, not twice. \n");
        return 1;
    }
    if (!(w > 0 && w < MAX_W && h > 0 && h < MAX_H)){
        /* TODO: probably just exit with error */
        printf("ShadowMapper size invalid. W: %d, H: %d\n", sm->w, sm->h);
        return 1;
    }
    sm->w = w; sm->h = h; sm->sampleCount = h * w;
    sm->buf = malloc(sizeof(float) * sm->w * sm->h);
    if (!sm->buf){
        /* TODO: probably just exit with error */
        printf("Failed to allocate ShadowMapper\n");
        return 1;
    }
    if (fabsf(Vec3Dot(lightDir, cam->gUp)) > 0.98f){
        /* here too */
        printf("ShadowMapper init problem\n");
        printf("lightDir was parallel to global up, which isn't allowed\n");
        return 1;
    }
    sm->cam = cam;
    sm->init = 1;
    sm->bias = bias;
    sm->lightDir = lightDir;
    DepthBufferInit(&sm->db, sm->w, sm->h);
    return 0;
}

int ShadowMapperFree(ShadowMapper* sm){
    if (!sm){
        /* TODO */
        printf("can't pass null to ShadowMapperFree\n");
        return 1;
    }
    if (!sm->buf){
        /* TODO: A warning or exit or something */
        printf("Tried to free ShadowMapper.buf, but it was already null.\n");
        printf("You probably freed it manually and then called \
                ShadowMapperFree().\n");
        return 1;
    }
    if (!sm->init){
        /* A warning or exit or seomthing */
        printf("Tried to free ShadowMapper, but it was never properly \
                initialized.\n");
        printf("You need to initialize it with ShadowMapperInit().\n");
        return 1;
    }
    free(sm->buf);
    sm->buf = 0;
    sm->init = 0;
    sm->w = sm->h = sm->sampleCount = 0;
    sm->bias = 0.f;
    sm->cam = 0;
    sm->lightDir = Vec3Make(0.f, 0.f, 0.f);
    DepthBufferFree(&sm->db);
    return 0;
}

void ShadowMapperClear(ShadowMapper* sm, float val){
    if (!sm || !sm->init || !sm->buf){
        /*TODO: error handling or something */
        printf("ShadowMapper not ready to use\n");
        return;
    }
    for (int i = 0; i < sm->sampleCount; i++){
        sm->buf[i] = val;
    }
    DepthBufferClear(&sm->db, 1.0f);
}

/* light dir must not be parallel to global up */
void ShadowMapperUpdate(ShadowMapper* sm){
    Vec3 centre = Vec3Make(0.f, 0.f, 0.f);
    for (int i = 0; i < 8; i++) centre = Vec3Add(centre, sm->cam->frustum[i]);
    centre = Vec3Scale(centre, 1.f / 8.f);

    sm->lightDir = Vec3Norm(sm->lightDir);   
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
    sm->matTransform = matLightView;
    sm->matTransformProj = matLightProj;
}

void ShadowMapperRender(ShadowMapper* sm, Obj3D* obj){
    Mat4 mvp = MatMatMul(&sm->matTransform, &obj->matModel);
    int numTris = obj->model->mesh->indexCount / 9;

    int i;
    for (i = 0; i < numTris; i++) {
        /* get indices in format pos/pos/pos/tex/tex/tex/normal/normal/normal */
        int i0, i1, i2, i3, i4, i5, i6, i7, i8;
        GetTriIndices(obj->model->mesh, i, &i0, &i1, &i2, &i3, &i4, &i5, &i6, &i7,
                &i8);

        /* use indices to get data */ 
        Vec4 v0, v1, v2, n0, n1, n2;
        float tu0, tv0, tu1, tv1, tu2, tv2;

        GetVertex(obj->model->mesh, i0, i3, i6, &v0, &tu0, &tv0, &n0);
        GetVertex(obj->model->mesh, i1, i4, i7, &v1, &tu1, &tv1, &n1);
        GetVertex(obj->model->mesh, i2, i5, i8, &v2, &tu2, &tv2, &n2);
        
        /* set up ints for the screen space triangle coordinates */
        int sx0, sy0, sx1, sy1, sx2, sy2;

        /* model -> light clip */
        v0 = MatVertMul(&mvp, v0);
        v1 = MatVertMul(&mvp, v1);
        v2 = MatVertMul(&mvp, v2);

        /* clip -> NDC (clipping not yet implemented) */

        v0.x /= v0.w;
        v0.y /= v0.w;
        v0.z /= v0.w;
        v1.x /= v1.w;
        v1.y /= v1.w;
        v1.z /= v1.w;
        v2.x /= v2.w;
        v2.y /= v2.w;
        v2.z /= v2.w;

        /* NDC -> screen */
        sx0 = (int)((v0.x * 0.5f + 0.5f) * sm->w);
        sy0 = (int)((0.5f - v0.y * 0.5f) * sm->h);
        sx1 = (int)((v1.x * 0.5f + 0.5f) * sm->w);
        sy1 = (int)((0.5f - v1.y * 0.5f) * sm->h);
        sx2 = (int)((v2.x * 0.5f + 0.5f) * sm->w);
        sy2 = (int)((0.5f - v2.y * 0.5f) * sm->h);

        float sz0, sz1, sz2;
        sz0 = v0.z * 0.5f + 0.5f;
        sz1 = v1.z * 0.5f + 0.5f;
        sz2 = v2.z * 0.5f + 0.5f;
        RasterizeTri_(sm, sx0, sy0, sz0, sx1, sy1, sz1, sx2, sy2, sz2);
    }
}

static inline void RasterizeTri_(ShadowMapper* sm, int x0, int y0,
        float z0, int x1, int y1, float z1, int x2, int y2, float z2){
    /* 1. Bounding‑box, clamped to framebuffer */
    int minX = (x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2));
    int minY = (y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2));
    int maxX = (x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2));
    int maxY = (y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2));

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX >= sm->w) maxX = sm->w - 1;
    if (maxY >= sm->h) maxY = sm->h - 1;

    /* 2. Pre‑compute denominator and edge deltas */
    float denom = (float)((y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2));
    if (denom == 0.0f) return;          /* Degenerate triangle */

    /* accumulate contribution from ambient lights */
    float invDen = 1.0f / denom;

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            /* 3. Barycentric weights (affine) */
            float l0 = ((y1 - y2) * (x - x2) + (x2 - x1) * (y - y2)) * invDen;
            float l1 = ((y2 - y0) * (x - x2) + (x0 - x2) * (y - y2)) * invDen;
            float l2 = 1.0f - l0 - l1;

            if (l0 < 0.0f || l1 < 0.0f || l2 < 0.0f) continue;

            float depth = l0 * z0 + l1 * z1 + l2 * z2;
            
            if (!DepthBufferTestWrite(&sm->db, x, y, depth)) continue;
            
            /* 5. write to shadow map */
            float* texel = (float*)sm->buf + (y * sm->w + x);
            *texel = depth;
        }
    }
}

#undef FLT_MAX
#undef MAX_W
#undef MAX_H
