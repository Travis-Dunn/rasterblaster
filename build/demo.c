#include "demo.h"
#include "renderer.h"
#include "arithmetic.h"
#include "timer.h"
#include "wavefront.h"
#include "texture.h"

static float cubePitch = 0.f;
static float cubeYaw = 0.f;
static float cubeRoll = 0.f;
static Mesh* mesh;
static Texture* cubeTex;
static float cubeXPos = 0.f;
static float cubeYPos = 0.f;
static float cubeZPos = -5.f;
static float cubeScaleX = 1.f;
static float cubeScaleY = 1.f;
static float cubeScaleZ = 1.f;



void Init(){
    InitTimer(1024);

    printf("width: %d\n", renderer.framebuffer.w);
    printf("height: %d\n", renderer.framebuffer.h);
    if (!InitDepthBuffer()){
        printf("couldn't allocate for depth buffer\n");
    }

    mesh = loadOBJ("cube.obj");
    cubeTex =LoadBimg("marble.bimg");
    int i;
    for (i = 0; i < mesh->indexCount; i += 9){
       int i0 = mesh->indices[i] + 1;
       int i1 = mesh->indices[i + 1] + 1;
       int i2 = mesh->indices[i + 2] + 1;
       int i3 = mesh->indices[i + 3] + 1;
       int i4 = mesh->indices[i + 4] + 1;
       int i5 = mesh->indices[i + 5] + 1;
       int i6 = mesh->indices[i + 6] + 1;
       int i7 = mesh->indices[i + 7] + 1;
       int i8 = mesh->indices[i + 8] + 1;
       printf("f %d/%d/%d %d/%d/%d %d/%d/%d\n", i0, i1, i2, i3, i4, i5, i6, i7, i8);
    }
}

void Render(){
    /* clear to grey */
    ClearScreen(22);
    ClearDepthBuffer();

    /* set up a magenta color */
    int c = RGBA_INT(212, 44, 162, 255);

    /* spin the cube in place */
    cubePitch += 0.5f * timer.dt;
    cubeYaw += -0.3f * timer.dt;
    cubeRoll += 0.1f * timer.dt;

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
        Matrix scaleMatrix = MatScale(cubeScaleX, cubeScaleY, cubeScaleZ);
        Matrix pitchMatrix = MatPitch(cubePitch);
        Matrix yawMatrix = MatYaw(cubeYaw);
        Matrix rollMatrix = MatRoll(cubeRoll);
        Matrix transMatrix = MatTranslate(cubeXPos, cubeYPos, cubeZPos);

        Matrix rotMatrix = MatMatMul(&yawMatrix, &pitchMatrix);
        rotMatrix = MatMatMul(&rotMatrix, &rollMatrix);
        Matrix cubeModelMatrix = MatMatMul(&rotMatrix, &scaleMatrix);
        cubeModelMatrix = MatMatMul(&transMatrix, &cubeModelMatrix);

        Matrix viewMatrix = MatIdentity();
        Matrix perspectiveProjMatrix = MatPerspective(0.96f /* 55 degrees in rads */
            , (float)renderer.framebuffer.w / renderer.framebuffer.h
            , 0.1f, 100.f);

        /* set up ints for the screen space triangle coordinates */
        int sx0, sy0, sx1, sy1, sx2, sy2;

        /* model -> world */
        v0 = MatVertMul(&cubeModelMatrix, v0);
        v1 = MatVertMul(&cubeModelMatrix, v1);
        v2 = MatVertMul(&cubeModelMatrix, v2);

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
