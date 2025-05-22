#include "demo.h"
#include "renderer.h"
#include "arithmetic.h"
#include "timer.h"
#include "wavefront.h"
#include "texture.h"
#include "model.h"

static Mat4 modelMatrix;
static Mat4 viewMatrix;
static Mat4 perspectiveProjMatrix;
static Model model;

void Init(){
    InitTimer(1024);

    printf("width: %d\n", renderer.framebuffer.w);
    printf("height: %d\n", renderer.framebuffer.h);
    if (!InitDepthBuffer()){
        printf("couldn't allocate for depth buffer\n");
    }

    viewMatrix = MatIdentity();
    perspectiveProjMatrix = MatPerspective(0.96f /* 55 degrees in rads */
            , (float)renderer.framebuffer.w / renderer.framebuffer.h
            , 0.1f, 100.f);
    model.scale.x = model.scale.y = model.scale.z = 15.f;
    model.rot.x = model.rot.y = model.rot.z = 0.f;
    model.pos.x = model.pos.y = 0.f; model.pos.z = -5.f;
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

    /* each vertex is a position, texcoords, and normal index, x3 per tri */
    int numTris = model.mesh->indexCount / 9;
    
    int i;
    for (i = 0; i < numTris; i++) {
        /* get indices in format pos/pos/pos/tex/tex/tex/normal/normal/normal */
        int i0, i1, i2, i3, i4, i5, i6, i7, i8;
        GetTriIndices(model.mesh, i, &i0, &i1, &i2, &i3, &i4, &i5, &i6, &i7,
                &i8);

        /* use indices to get data */ 
        Vec4 v0, v1, v2, n0, n1, n2;
        float tu0, tv0, tu1, tv1, tu2, tv2;

        GetVertex(model.mesh, i0, i3, i6, &v0, &tu0, &tv0, &n0);
        GetVertex(model.mesh, i1, i4, i7, &v1, &tu1, &tv1, &n1);
        GetVertex(model.mesh, i2, i5, i8, &v2, &tu2, &tv2, &n2);
        
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
        TexturedTri(model.tex, sx0, sy0, v0.z, tu0, tv0,
                            sx1, sy1, v1.z, tu1, tv1,
                            sx2, sy2, v2.z, tu2, tv2);
    }
}

void Update(){
    UpdateTimer();
}
