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
    int i;
    /*
    for (i = 0; i < mesh->vertexCount; i++){
        printf("vert %d: %.1f, %.1f, %.1f\n", i, mesh->vertices[i].x,
                mesh->vertices[i].y, mesh->vertices[i].z);
    }
    */
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
    
    for (int i = 0; i < 8; i++) {
        cubeVertices[i].x = mesh->positions[i * 3 + 0];
        cubeVertices[i].y = mesh->positions[i * 3 + 1];
        cubeVertices[i].z = mesh->positions[i * 3 + 2];
        cubeVertices[i].w = 1.0f; // explicitly set w since positions are xyz only
    }

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
