#include "demo.h"
#include "renderer.h"
#include "arithmetic.h"
#include "timer.h"
#include "wavefront.h"
#include "texture.h"

static float modelPitch = 0.f;
static float modelYaw = 0.f;
static float modelRoll = 0.f;
static Mesh* mesh;
static Texture* cubeTex;
static float modelXPos = 0.f;
static float modelYPos = 0.f;
static float modelZPos = -5.f;
static float modelScaleX = 15.f;
static float modelScaleY = 15.f;
static float modelScaleZ = 15.f;

static Matrix scaleMatrix;
static Matrix pitchMatrix;
static Matrix yawMatrix;
static Matrix rollMatrix;
static Matrix rotMatrix;
static Matrix transMatrix;
static Matrix modelMatrix;
static Matrix viewMatrix;
static Matrix perspectiveProjMatrix;

void Init(){
    InitTimer(1024);

    printf("width: %d\n", renderer.framebuffer.w);
    printf("height: %d\n", renderer.framebuffer.h);
    if (!InitDepthBuffer()){
        printf("couldn't allocate for depth buffer\n");
    }

    mesh = loadOBJ("models/carp.obj");
    cubeTex =LoadBimg("textures/carp.bimg");

    viewMatrix = MatIdentity();
    perspectiveProjMatrix = MatPerspective(0.96f /* 55 degrees in rads */
            , (float)renderer.framebuffer.w / renderer.framebuffer.h
            , 0.1f, 100.f);

}

void Render(){
    /* clear to grey */
    ClearScreen(22);
    ClearDepthBuffer();

    /* set up a magenta color 
    int c = RGBA_INT(212, 44, 162, 255);
    */

    /* spin the cube in place */
    modelPitch += 0.5f * timer.dt;
    modelYaw += -0.3f * timer.dt;
    modelRoll += 0.1f * timer.dt;

    scaleMatrix = MatScale(modelScaleX, modelScaleY, modelScaleZ);
    pitchMatrix = MatPitch(modelPitch);
    yawMatrix = MatYaw(modelYaw);
    rollMatrix = MatRoll(modelRoll);
    transMatrix = MatTranslate(modelXPos, modelYPos, modelZPos);
    rotMatrix = MatMatMul(&rotMatrix, &rollMatrix);
    rotMatrix = MatMatMul(&yawMatrix, &pitchMatrix);
    modelMatrix = MatMatMul(&rotMatrix, &scaleMatrix);
    modelMatrix = MatMatMul(&transMatrix, &modelMatrix);

    /* each vertex is a position, texcoords, and normal index, x3 per tri */
    int numTris = mesh->indexCount / 9;
    
    int i;
    for (i = 0; i < numTris; i++) {
        /* get indices for pos (first 3), and texcoords (second 3) */
        int i0, i1, i2, i3, i4, i5;

        /* disregarding the normals, as I can't use them yet */
        i0 = mesh->indices[i * 9];
        i1 = mesh->indices[i * 9 + 3];
        i2 = mesh->indices[i * 9 + 6];
        i3 = mesh->indices[i * 9 + 1];
        i4 = mesh->indices[i * 9 + 4];
        i5 = mesh->indices[i * 9 + 7];
        /* get pos for all three verts */ 
        Vec4 v0, v1, v2;
        v0.x = mesh->positions[i0 * 3];
        v0.y = mesh->positions[i0 * 3 + 1];
        v0.z = mesh->positions[i0 * 3 + 2];
        v0.w = 1.f;
        v1.x = mesh->positions[i1 * 3];
        v1.y = mesh->positions[i1 * 3 + 1];
        v1.z = mesh->positions[i1 * 3 + 2];
        v1.w = 1.f;
        v2.x = mesh->positions[i2 * 3];
        v2.y = mesh->positions[i2 * 3 + 1];
        v2.z = mesh->positions[i2 * 3 + 2];
        v2.w = 1.f;

        float tu0, tv0, tu1, tv1, tu2, tv2;
        tu0 = mesh->texcoords[i3 * 2];
        tv0 = mesh->texcoords[i3 * 2 + 1];
        tu1 = mesh->texcoords[i4 * 2];
        tv1 = mesh->texcoords[i4 * 2 + 1];
        tu2 = mesh->texcoords[i5 * 2];
        tv2 = mesh->texcoords[i5 * 2 + 1];
        
        /* set up matrices */

        /* set up ints for the screen space triangle coordinates */
        int sx0, sy0, sx1, sy1, sx2, sy2;

        /* model -> world */
        v0 = MatVertMul(&modelMatrix, v0);
        v1 = MatVertMul(&modelMatrix, v1);
        v2 = MatVertMul(&modelMatrix, v2);

        /* world -> view */
        v0 = MatVertMul(&viewMatrix, v0);
        v1 = MatVertMul(&viewMatrix, v1);
        v2 = MatVertMul(&viewMatrix, v2);

        /* view -> clip */
        v0 = MatVertMul(&perspectiveProjMatrix, v0);
        v1 = MatVertMul(&perspectiveProjMatrix, v1);
        v2 = MatVertMul(&perspectiveProjMatrix, v2);

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
        sx0 = (int)((v0.x * 0.5f + 0.5f) * renderer.framebuffer.w);
        sy0 = (int)((v0.y * 0.5f + 0.5f) * renderer.framebuffer.h);
        sx1 = (int)((v1.x * 0.5f + 0.5f) * renderer.framebuffer.w);
        sy1 = (int)((v1.y * 0.5f + 0.5f) * renderer.framebuffer.h);
        sx2 = (int)((v2.x * 0.5f + 0.5f) * renderer.framebuffer.w);
        sy2 = (int)((v2.y * 0.5f + 0.5f) * renderer.framebuffer.h);

        /*FilledTri(sx0, sy0, sx1, sy1, sx2, sy2, c);*/
        TexturedTri(cubeTex, sx0, sy0, v0.z, tu0, tv0,
                            sx1, sy1, v1.z, tu1, tv1,
                            sx2, sy2, v2.z, tu2, tv2);
    }
}

void Update(){
    UpdateTimer();
}
