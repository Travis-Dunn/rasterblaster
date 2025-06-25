#include "rasterblaster.h"
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
#include "shadowmapper.h"
#include "event.h"
#include "input.h"

static Model model;
static Model groundModel;
static Model rscHouseModel;
static Model cubeModel;
static Obj3D carp;
static Obj3D ground;
static Obj3D rscHouse;
static Obj3D cube;
static Camera cam;
static Light light[8];
static DepthBuffer depthbuf;
static ShadowMapper shadowMapper;
static EventQueue* eventQueue;
static InputSystem inputSystem;

static int debugX, debugY;
static int frameCount;

void Init(){
    if (EventQueueInit(&eventQueue, 256)){
        printf("problem setting up event queue\n");
    }
    SetPlatformEventQueue(eventQueue);
    InputInit(&inputSystem);
    InitTimer(1024);
    InitPickbuf(renderer.framebuffer.w, renderer.framebuffer.h);
    (void)DepthBufferInit(&depthbuf, renderer.framebuffer.w,
            renderer.framebuffer.h);
    (void)ShadowMapperInit(&shadowMapper, renderer.framebuffer.w,
            renderer.framebuffer.h, 0.003f, &cam, Vec3Make(1, -.5f, -1));

    printf("width: %d\n", renderer.framebuffer.w);
    printf("height: %d\n", renderer.framebuffer.h);

    /* this is deprecated after I added the CameraMakePerspectiveRH function
    cam.fov = 0.96f;
    cam.ar = (float)renderer.framebuffer.w / renderer.framebuffer.h;
    cam.nearClip = .1f;
    cam.farClip = 10.f;
    Vec3 eye = Vec3Make(0, 0.f, 0);
    Vec3 tgt = Vec3Make(0, 0, -1);
    Vec3 up = Vec3Make(0, 1, 0);
    cam.gUp = up;
    cam.up = up;
    cam.right = Vec3Make(1, 0, 0);
    cam.forward = tgt;
    cam.pos = eye;
    cam.view = Mat4LookAt(eye, tgt, up);
    cam.proj = Mat4Perspective(cam.fov,
            (float)renderer.framebuffer.w / renderer.framebuffer.h
            , cam.nearClip, cam.farClip);
    cam.inverseDir = Vec3Norm(Vec3Make(0, 0, 1));
    */
    CameraMakePerspectiveRH(&cam, (float)renderer.framebuffer.w /
            renderer.framebuffer.h, 55.f, &timer.dt);
    cam.farClip = 20.f;
    /*
    UpdateFrustum(&cam);
    */
    light[0] = MakeDirectional(192, 192, 192, Vec3Norm(Vec3Make(1, -.5f, -1)));
    light[1] = MakeAmbient(64, 64, 64);
    model.mesh = loadOBJ("models/carp.obj");
    model.tex = LoadBimg("textures/carp.bimg");
    groundModel.mesh = loadOBJ("models/ground.obj");
    groundModel.tex = LoadBimg("textures/dirt.bimg");
    rscHouseModel.mesh = loadOBJ("models/rsc house.obj");
    rscHouseModel.tex = LoadBimg("textures/rsc house tex.bimg");
    cubeModel.mesh = loadOBJ("models/cube.obj");
    cubeModel.tex = 0;
    carp.model = &model;
    carp.scale = Vec3Make(15.f, 15.f, 15.f);
    carp.rot = Vec3Make(0.f, 0.f, 0.f);
    carp.pos = Vec3Make(0.f, 0.f, -5.f);
    ground.model = &groundModel;
    ground.scale = Vec3Make(1.f, 1.f, 1.f);
    ground.rot = Vec3Make(0.f, 0.f, 0.f);
    ground.pos = Vec3Make(0.f, 0.f, -5.f);
    rscHouse.model = &rscHouseModel;
    rscHouse.scale = Vec3Make(1.f, 1.f, 1.f);
    rscHouse.rot = Vec3Make(0.f, 0.f, 0.f);
    rscHouse.pos = Vec3Make(3.f, 0.f, -10.f);
    cube.model = &cubeModel;
    cube.scale = Vec3Make(1.f, 1.f, 1.f);
    cube.rot = Vec3Make(0.f, 0.f, 3.f);
    cube.pos = Vec3Make(0.f, 0.f, -5.f);
    carp.id = 12;
    ground .id = 11;
    ShadowMapperUpdate(&shadowMapper);
    UpdateCamera(&cam);
    /*
    puts("Paused - press any key to continue");
    getchar();
    */
    debugX, debugY = 0;
    frameCount = 0;
}

void Render(){
    /*
    static int once = 0;
    if (once) {
        printf("paused\n");
        getchar();
    }
    */
    /* clear to grey */
    ClearScreen(255);
    DepthBufferClear(&depthbuf, 1.f);
    ClearPickbuf();
    ShadowMapperClear(&shadowMapper, 1.f);

    /* some spinning */
    carp.rot.x += 0.1f * timer.dt;
    carp.rot.y += -0.2f * timer.dt;
    carp.rot.z += 0.1f * timer.dt;
    /*
    cube.rot.x += 0.1f * timer.dt;
    cube.rot.y += -0.1f * timer.dt;
    cube.rot.z += 0.1f * timer.dt;
    */


    ShadowMapperRender(&shadowMapper, &cube);
    /*
    DrawObj3DLambert(&cam, &carp, &renderer.framebuffer, &light[0], 2, 
            &depthbuf);
            */
    /*
    DrawObj3DLambertShadowFloatClip(&cam, &carp, &renderer.framebuffer,
            &light[0], 2, &depthbuf, &shadowMapper);
    DrawObj3DLambertShadowFloatClip(&cam, &ground, &renderer.framebuffer,
            &light[0], 2, &depthbuf, &shadowMapper);
    DrawObj3DLambertShadowFloatClip(&cam, &rscHouse, &renderer.framebuffer,
            &light[0], 2, &depthbuf, &shadowMapper);
            */
    Obj3DDrawWireframe(&cam, &cube, &renderer.framebuffer, &depthbuf);
    /*
    VisualizeBuffer(shadowMapper.buf, shadowMapper.w, shadowMapper.h,
            "float");
    */
    /*
    PutPixel(debugX, debugY, RGBA_INT(255, 0, 255, 255));
    once = 1;
    */
}

void debugCorner(char* str, Vec3 v){
    printf("%s {%.3f, %.3f, %.3f}\n", str, v.x, v.y, v.z);
}

void Update(){
    InputUpdate(&inputSystem, timer.dt);
    UpdateTimer();
    frameCount++;
    /*
    printf("frame %d\n", frameCount);
    */
    /*
    UpdateFrustum(&cam);
    */
    static int once = 0;
    if (!once){
        debugCorner("near top left", cam.frustum[0]);
        debugCorner("near top right", cam.frustum[1]);
        debugCorner("near bottom left", cam.frustum[2]);
        debugCorner("near bottom right", cam.frustum[3]);
        debugCorner("far top left", cam.frustum[4]);
        debugCorner("far top right", cam.frustum[5]);
        debugCorner("far bottom left", cam.frustum[6]);
        debugCorner("far bottom right", cam.frustum[7]);
        once = 1;
    }
    UpdateObj3DModelMatrix(&carp);
    UpdateObj3DModelMatrix(&ground);
    UpdateObj3DModelMatrix(&rscHouse);
    UpdateObj3DModelMatrix(&cube);

    Event evt;
    while (EventQueueNotEmpty(eventQueue)){
        if (EventDequeue(eventQueue, &evt) == 0){
            switch (evt.type){
            case EVT_LBUTTONDOWN:{
                                     /*
                Mat4Printf(&cam.proj, "cam->proj\n");
                Mat4Printf(&cam.view, "cam->view\n");
                debugCorner("near top left", cam.frustum[0]);
                debugCorner("near top right", cam.frustum[1]);
                debugCorner("near bottom left", cam.frustum[2]);
                debugCorner("near bottom right", cam.frustum[3]);
                debugCorner("far top left", cam.frustum[4]);
                debugCorner("far top right", cam.frustum[5]);
                debugCorner("far bottom left", cam.frustum[6]);
                debugCorner("far bottom right", cam.frustum[7]);
                printf("&cam.view from demo: %d\n", (int)&cam.view);
                */
                debugX = evt.buf[0];
                debugY = evt.buf[1];
                printf("clicked on pixel (%d, %d)\n", debugX, debugY);
            } break;
            case EVT_KEYDOWN: {
                InputHandleKeyEvent(&inputSystem, evt.buf[0], 1);
            } break;
            case EVT_KEYUP: {
                InputHandleKeyEvent(&inputSystem, evt.buf[0], 0);
            } break;
            case EVT_MOUSEMOVE: {
                InputHandleMouseEvent(&inputSystem, (float)evt.buf[0],
                        (float)evt.buf[1]);
            } break;
            }
        }
    }
    if (InputIsActionPressed(&inputSystem, ACTION_CAM_TRANS_L_X_MINUS)){
        CameraTransLocalXMinus(&cam);
    }
    if (InputIsActionPressed(&inputSystem, ACTION_CAM_TRANS_L_X_PLUS)){
        CameraTransLocalXPlus(&cam);
    }
    if (InputIsActionPressed(&inputSystem, ACTION_CAM_TRANS_L_Y_MINUS)){
        CameraTransLocalYMinus(&cam);
        /*
        puts("Paused - press any key to continue");
        getchar();
        */
    }
    if (InputIsActionPressed(&inputSystem, ACTION_CAM_TRANS_L_Y_PLUS)){
        CameraTransLocalYPlus(&cam);
    }
    if (InputIsActionPressed(&inputSystem, ACTION_CAM_TRANS_L_Z_MINUS)){
        CameraTransLocalZMinus(&cam);
    }
    if (InputIsActionPressed(&inputSystem, ACTION_CAM_TRANS_L_Z_PLUS)){
        CameraTransLocalZPlus(&cam);
    }
    if (InputIsActionPressed(&inputSystem, ACTION_CAM_ROT_L_Y_MINUS)){
        CameraRotLocalYMinus(&cam);
    }
    if (InputIsActionPressed(&inputSystem, ACTION_CAM_ROT_L_Y_PLUS)){
        CameraRotLocalYPlus(&cam);
    }
    if (InputIsActionPressed(&inputSystem, ACTION_CAM_ROT_L_X_MINUS)){
        CameraRotLocalXMinus(&cam);
    }
    if (InputIsActionPressed(&inputSystem, ACTION_CAM_ROT_L_X_PLUS)){
        /*
        CameraPrint(&cam);
        */
        CameraRotLocalXPlus(&cam);
    }
    /*
    if (InputIsActionJustPressed(&inputSystem, ACTION_CAM_ROT_SNAP_L_Y_MINUS)){
        CameraRotSnapLocalYMinus(&cam, 30.f);
    }
    */
    float dx = InputIsActionMouseMoved(&inputSystem, ACTION_CAM_ROT_L_X);
    if (dx != 0.f){
        /*
        printf("mouse move value: %f\n", dx );
        */
        CameraRotLocalXFloat(&cam, dx);
    }
    float dy = InputIsActionMouseMoved(&inputSystem, ACTION_CAM_ROT_L_Y);
    if (dy != 0.f){
        CameraRotLocalYFloat(&cam, dy);
    }
    float oldX = cam.pos.x;
    float oldY = cam.pos.y;
    float oldZ = cam.pos.z;
    UpdateCamera(&cam);
    /*
    if (fabsf(oldX - cam.pos.x) > 1e-4) printf("moved in x axis\n");
    if (fabsf(oldY - cam.pos.y) > 1e-4) printf("moved in y axis\n");
    if (fabsf(oldZ - cam.pos.z) > 1e-4) printf("moved in z axis\n");
    */
    /*
    printf("cam.right: %f, %f, %f\n", cam.right.x, cam.right.y, cam.right.z);
    */
    ShadowMapperUpdate(&shadowMapper);
}
