#include "demo.h"
#include "renderer.h"
#include "arithmetic.h"
#include "timer.h"
#include "wavefront.h"

static float cubePitch = 0.f;
static float cubeYaw = 0.f;
static float cubeRoll = 0.f;
static Mesh* mesh;

void Init(){
    InitTimer(1024);

    mesh = loadOBJ("cube.obj");
}

void Render(){
    ClearScreen(22);


    int c = RGBA_INT(212, 44, 162, 255);
    cubePitch += 0.5f * timer.dt;
    cubeYaw += -0.3f * timer.dt;
    cubeRoll += 0.1f * timer.dt;

    /*
    Vec4 cubeVertices[8] = {
        {-1.f, -1.f, -1.f, 1.f},
        { 1.f, -1.f, -1.f, 1.f},
        { 1.f,  1.f, -1.f, 1.f},
        {-1.f,  1.f, -1.f, 1.f},
        {-1.f, -1.f,  1.f, 1.f},
        { 1.f, -1.f,  1.f, 1.f},
        { 1.f,  1.f,  1.f, 1.f},
        {-1.f,  1.f,  1.f, 1.f}
    };
    */
    Vec4 cubeVertices[8];
    
    cubeVertices[0].x = mesh->vertices[0].x;
    cubeVertices[0].y = mesh->vertices[0].y;
    cubeVertices[0].z = mesh->vertices[0].z;
    cubeVertices[0].w = mesh->vertices[0].w;

    cubeVertices[1].x = mesh->vertices[1].x;
    cubeVertices[1].y = mesh->vertices[1].y;
    cubeVertices[1].z = mesh->vertices[1].z;
    cubeVertices[1].w = mesh->vertices[1].w;

    cubeVertices[2].x = mesh->vertices[2].x;
    cubeVertices[2].y = mesh->vertices[2].y;
    cubeVertices[2].z = mesh->vertices[2].z;
    cubeVertices[2].w = mesh->vertices[2].w;

    cubeVertices[3].x = mesh->vertices[3].x;
    cubeVertices[3].y = mesh->vertices[3].y;
    cubeVertices[3].z = mesh->vertices[3].z;
    cubeVertices[3].w = mesh->vertices[3].w;

    cubeVertices[4].x = mesh->vertices[4].x;
    cubeVertices[4].y = mesh->vertices[4].y;
    cubeVertices[4].z = mesh->vertices[4].z;
    cubeVertices[4].w = mesh->vertices[4].w;

    cubeVertices[5].x = mesh->vertices[5].x;
    cubeVertices[5].y = mesh->vertices[5].y;
    cubeVertices[5].z = mesh->vertices[5].z;
    cubeVertices[5].w = mesh->vertices[5].w;

    cubeVertices[6].x = mesh->vertices[6].x;
    cubeVertices[6].y = mesh->vertices[6].y;
    cubeVertices[6].z = mesh->vertices[6].z;
    cubeVertices[6].w = mesh->vertices[6].w;

    cubeVertices[7].x = mesh->vertices[7].x;
    cubeVertices[7].y = mesh->vertices[7].y;
    cubeVertices[7].z = mesh->vertices[7].z;
    cubeVertices[7].w = mesh->vertices[7].w;

    float cubeXPos = 0.f;
    float cubeYPos = 0.f;
    float cubeZPos = 5.f;
    float cubeScaleX = 1.f;
    float cubeScaleY = 1.f;
    float cubeScaleZ = 1.f;

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

    for (int i = 0; i < 8; i++){
        Vec4 v = cubeVertices[i];
        /* model -> world */
        v = MatVertMul(&cubeModelMatrix, v);
        /* world -> view */
        v = MatVertMul(&viewMatrix, v);
        /* view -> clip */
        v = MatVertMul(&perspectiveProjMatrix, v);
        /* perspective divide */
        v.x /= v.w;
        v.y /= v.w;
        v.z /= v.w;
        /* NDC -> screen */
        int screenX = (int)((v.x * 0.5f + 0.5f) * renderer.framebuffer.w);
        int screenY = (int)((-v.y * 0.5f + 0.5f) * renderer.framebuffer.h);

        PutPixel(screenX, screenY, c);
    }
}

void Update(){
    UpdateTimer();
}
