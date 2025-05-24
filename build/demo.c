#include "demo.h"
#include "renderer.h"
#include "arithmetic.h"
#include "timer.h"
#include "wavefront.h"
#include "texture.h"
#include "model.h"
#include "camera.h"

static Mat4 modelMatrix;
static Model model;
static Camera cam;
static Light light;

void Init(){
    InitTimer(1024);

    printf("width: %d\n", renderer.framebuffer.w);
    printf("height: %d\n", renderer.framebuffer.h);
    if (!InitDepthBuffer()){
        printf("couldn't allocate for depth buffer\n");
    }

    cam.fovRads = 0.96f;
    cam.nearClip = .1f;
    cam.farClip = 100.f;
    Vec3 eye = Vec3Make(0, 0, 0);
    Vec3 tgt = Vec3Make(0, 0, -1);
    Vec3 up = Vec3Make(0, 1, 0);
    cam.view = Mat4LookAt(eye, tgt, up);
    cam.proj = MatPerspective(cam.fovRads,
            (float)renderer.framebuffer.w / renderer.framebuffer.h
            , cam.nearClip, cam.farClip);
    light = MakeAmbient(224, 160, 90);
    model.scale = Vec3Make(15.f, 15.f, 15.f);
    model.rot = Vec3Make(0.f, 0.f, 0.f);
    model.pos = Vec3Make(0.f, 0.f, -5.f);
    model.mesh = loadOBJ("models/carp.obj");
    model.tex = LoadBimg("textures/carp.bimg");
}

void Render(){
    /* clear to grey */
    ClearScreen(22);
    ClearDepthBuffer();

    /* spin the cube in place */
    model.rot.x += 0.5f * timer.dt;
    model.rot.y += -0.3f * timer.dt;
    model.rot.z += 0.1f * timer.dt;

    modelMatrix = ModelMatrix(&model);
    DrawModelLambert(&cam, &model, &renderer.framebuffer, &light, 1,
                modelMatrix);
}

void Update(){
    UpdateTimer();
}
