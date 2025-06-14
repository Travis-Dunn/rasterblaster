#ifndef RENDERER_H
#define RENDERER_H

#include "config.h"
#include "texture.h"
#include "camera.h"
#include "model.h"
#include "light.h"
#include "obj3d.h"
#include "depthbuffer.h"
#include "shadowmapper.h"

typedef struct {
    int w, h;
    void* buf;
} Framebuffer;

typedef struct {
    int running;
    Framebuffer framebuffer;
} Renderer;

typedef struct {
    int x, y;
} Point2D;

extern Renderer renderer;

extern void PutPixel_ASM(int x, int y);
static inline void PutPixel_(int x, int y, int c);
void PutPixel_external(int x, int y, int c);
void PutPixel_external_safe(int x, int y, int c);
static inline void DrawLineOct0_(int x0, int y0, int dx, int dy, int xdir, 
        int c);
static inline void DrawLineOct1_(int x0, int y0, int dx, int dy, int xdir,
        int c);
static inline void DrawHorizontalLine_(int x0, int y0, int x1, int c);
static inline void DrawVeritcalLine_(int x0, int y0, int y1, int c);
static inline void DrawSlopeOneLine_(int x0, int y0, int y1, int c);
static inline void DrawSlopeMinusOneLine_(int x0, int y0, int y1, int c);
void DrawLine_(int x0, int y0, int x1, int y1, int c);
void DrawTri_(int x0, int y0, int x1, int y1, int x2, int y2, int c);
void FilledTri(int x0, int y0, int x1, int y1, int x2, int y2, int color);
void TexturedTri(Texture* t, int x0, int y0, float z0, float u0, float v0,
               int x1, int y1, float z1, float u1, float v1,
               int x2, int y2, float z2, float u2, float v2, DepthBuffer* db);
static inline void TexturedLambertTri_(Texture* t, Vec3 color, int id,
        DepthBuffer* db,
        int x0, int y0, float z0, float u0, float v0, int x1, int y1, float z1,
        float u1, float v1, int x2, int y2, float z2, float u2, float v2);
static inline void TexturedLambertShadowTri_(Texture* t, Vec3 la, int id,
        DepthBuffer* db, Vec3 ld,
        int x0, int y0, float z0, float u0, float v0, int x1, int y1, float z1,
        float u1, float v1, int x2, int y2, float z2, float u2, float v2
        , ShadowMapper* sm, Vec4 sh0, Vec4 sh1, Vec4 sh2);
static inline void TexturedLambertShadowFloatTri_(Texture* t, Vec3 la, int id,
        DepthBuffer* db, Vec3 ld,
        float x0, float y0, float z0, float u0, float v0, float x1, float y1, float z1,
        float u1, float v1, float x2, float y2, float z2, float u2, float v2
        , ShadowMapper* sm, Vec4 sh0, Vec4 sh1, Vec4 sh2);



void DrawObj3DLambert(Camera* cam, Obj3D* obj, Framebuffer* fb, Light* l,
        int nLights, DepthBuffer* db);
void DrawObj3DLambertShadow(Camera* cam, Obj3D* obj, Framebuffer* fb, Light* l,
        int nLights, DepthBuffer* db, ShadowMapper* sm);
void DrawObj3DLambertShadowFloat(Camera* cam, Obj3D* obj, Framebuffer* fb, Light* l,
        int nLights, DepthBuffer* db, ShadowMapper* sm);





void VisualizeBuffer(void* buf, int w, int h, char* type);
void ClearScreen_(unsigned char grey);

#ifdef USE_SAFE_PIXEL_FUNCTIONS
#define PutPixel PutPixel_external_safe
#else
#define PutPixel PutPixel_external
#endif

#define DrawLine DrawLine_
#define DrawTri DrawTri_
#define ClearScreen ClearScreen_
#define RGBA_INT(R, G, B, A) ((A << 24) | (R << 16) | (G << 8) | B)
#define GETR(c) ((c >> 16) & 0xFF)
#define GETG(c) ((c >>  8) & 0xFF)
#define GETB(c) ( c        & 0xFF)
#endif /* RENDERER_H */
