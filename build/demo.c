#include "demo.h"
#include "renderer.h"
#include "arithmetic.h"
#include "timer.h"
#include "wavefront.h"
#include "texture.h"
#include "model.h"
#include "camera.h"
#include "mouse.h"
#include "obj3d.h"

static Model model;
static Obj3D carp;
static Camera cam;
static Light light[8];
static Depthbuffer depthbuf;

void Init(){
    InitTimer(1024);
    InitPickbuf(renderer.framebuffer.w, renderer.framebuffer.h);
    (void)DepthbufferInit(&depthbuf, renderer.framebuffer.w,
            renderer.framebuffer.h);

    printf("width: %d\n", renderer.framebuffer.w);
    printf("height: %d\n", renderer.framebuffer.h);

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
    cam.inverseDir = Vec3Norm(Vec3Make(0, 0, 1));
    light[0] = MakeDirectional(192, 192, 192, Vec3Norm(Vec3Make(1, -.5f, -1)));
    light[1] = MakeAmbient(64, 64, 64);
    model.mesh = loadOBJ("models/carp.obj");
    model.tex = LoadBimg("textures/carp.bimg");
    carp.model = &model;
    carp.scale = Vec3Make(15.f, 15.f, 15.f);
    carp.rot = Vec3Make(0.f, 0.f, 0.f);
    carp.pos = Vec3Make(0.f, 0.f, -5.f);
    carp.id = 12;
}

void Render(){
    /* clear to grey */
    ClearScreen(22);
    DepthbufferClear(&depthbuf, 1.f);
    ClearPickbuf();

    /* some spinning */
    carp.rot.x += 0.5f * timer.dt;
    carp.rot.y += -0.3f * timer.dt;
    carp.rot.z += 0.1f * timer.dt;

    DrawObj3DLambert(&cam, &carp, &renderer.framebuffer, &light[0], 2, 
            &depthbuf);
}

void Update(){
    UpdateTimer();
    UpdateObj3DModelMatrix(&carp);
}
