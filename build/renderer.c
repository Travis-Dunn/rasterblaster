#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "renderer.h"
#include "mouse.h"
#include "logger.h"

Framebuffer framebuffer = {0};
Renderer renderer = {0};
float* depthbuffer = 0;

void RendererInit(int w, int h, void* buf){
    /* framebuffer */
    renderer.framebuffer.w = w;
    renderer.framebuffer.h = h;
    renderer.framebuffer.buf = buf;
    /* depth testing */
    if (renderer.enableDepthTest = DEFAULT_ENABLE_DEPTH_TEST) {
        DepthBufferInit();
        renderer.depthTestInit = 1;
    } else renderer.depthTestInit = 0;

    GammaLUTInit(&renderer.gammaLUT[0]);
    renderer.running = 1;
}

static inline void PutPixel_(int x, int y, int c){
    int* pixel = (int*)renderer.framebuffer.buf +
        (y * renderer.framebuffer.w + x);
    *pixel = c;
}

void PutPixel_external(int x, int y, int c){
    PutPixel_(x, y, c);
}

void PutPixel_external_safe(int x, int y, int c){
    if (x >= renderer.framebuffer.w || x < 0 ||
        y >= renderer.framebuffer.h || y < 0) {
        printf("Out of bounds PutPixel: (%d, %d)\n", x, y);
        return;
    }
    PutPixel_(x, y, c);
}

void BlendPixel_(int x, int y, int c){
    if (x >= renderer.framebuffer.w || x < 0 ||
        y >= renderer.framebuffer.h || y < 0){
        printf("x: %d, y: %d\n", x, y);
        getchar();
        return;
    }    
 
    int* addr = (int*)renderer.framebuffer.buf +
        (y * renderer.framebuffer.w + x); 
    unsigned char a = GETA(c);
    if (a == 0) return;
    if (a == 255){
        *addr = c;
        return;
    }
    int fbval = *addr;
    unsigned char fba = GETA(fbval);
    unsigned char fbr = GETR(fbval);
    unsigned char fbg = GETG(fbval);
    unsigned char fbb = GETB(fbval);
    unsigned char r =   GETR(c);
    unsigned char g =   GETG(c);
    unsigned char b =   GETB(c);
    unsigned char r_ = ((255 - a) * fbr + a * r) / 255;
    unsigned char g_ = ((255 - a) * fbg + a * g) / 255;
    unsigned char b_ = ((255 - a) * fbb + a * b) / 255;
    *addr = RGBA_INT(r_, g_, b_, fba);
}

static inline void DrawLineOct0_(int x0, int y0, int dx, int dy, int xdir, 
        int c){
    int dYx2, dYx2MinusdXx2, err;
    dYx2 = dy * 2;
    dYx2MinusdXx2 = dYx2 - (dx * 2);
    err = dYx2 - dx;
    PutPixel_(x0, y0, c);
    while (dx--){
        if (err >= 0){
            y0++;
            err += dYx2MinusdXx2;
        } else err += dYx2;
        x0 += xdir;
        PutPixel_(x0, y0, c);
    }
}

static inline void DrawLineOct1_(int x0, int y0, int dx, int dy, int xdir,
        int c){
    int dXx2, dXx2MinusdYx2, err;
    dXx2 = dx * 2;
    dXx2MinusdYx2 = dXx2 - (dy * 2);
    err = dXx2 - dy;
    PutPixel_(x0, y0, c);
    while (dy--){
        if (err >= 0){
            x0 += xdir;
            err += dXx2MinusdYx2;
        } else err += dXx2;
        y0++;
        PutPixel_(x0, y0, c);
    }
}

static inline void DrawHorizontalLine_(int x0, int y0, int x1, int c){
    if (x1 < x0){
        int temp = x0;
        x0 = x1;
        x1 = temp;
    }
    while(x0 <= x1){
        PutPixel_(x0, y0, c);
        x0++;
    }
}

static inline void DrawVerticalLine_(int x0, int y0, int y1, int c){
    while(y0 <= y1){
        PutPixel_(x0, y0, c);
        y0++;
    }
}

static inline void DrawSlopeOneLine_(int x0, int y0, int y1, int c){
    while (y0 <= y1){
        PutPixel_(x0, y0, c);
        x0++;
        y0++;
    }
}

static inline void DrawSlopeMinusOneLine_(int x0, int y0, int y1, int c){
    while (y0 <= y1){
        PutPixel_(x0, y0, c);
        y0++;
        x0--;
    }
}

void DrawLine_(int x0, int y0, int x1, int y1, int c){
    int dx, dy, temp;
    if (y0 > y1){
        temp = y0;
        y0 = y1;
        y1 = temp;
        temp = x0;
        x0 = x1;
        x1 = temp;
    }
    dx = x1 - x0;
    if (dx == 0){
        DrawVerticalLine_(x0, y0, y1, c);
        return;
    }
    dy = y1 - y0;
    if (dy == 0){
        DrawHorizontalLine_(x0, y0, x1, c); 
        return;
    }
    if (dx == dy){
        DrawSlopeOneLine_(x0, y0, y1, c);
        return;
    }
    if (-dx == dy){
        DrawSlopeMinusOneLine_(x0, y0, y1, c);
        return;
    }
    if (dx > 0){
        if (dx > dy){
            DrawLineOct0_(x0, y0, dx, dy, 1, c);
            return;
        } else {
            DrawLineOct1_(x0, y0, dx, dy, 1, c);
            return;
        }
    } else {
        dx = -dx;
        if (dx > dy){
            DrawLineOct0_(x0, y0, dx, dy, -1, c);
            return;
        } else {
            DrawLineOct1_(x0, y0, dx, dy, -1, c);
            return;
        }
    }
}

void DrawTri_(int x0, int y0, int x1, int y1, int x2, int y2, int c){
    DrawLine_(x0, y0, x1, y1, c);
    DrawLine_(x1, y1, x2, y2, c);
    DrawLine_(x2, y2, x0, y0, c);
}

void ClearScreen_(unsigned char grey){
    (void)memset(renderer.framebuffer.buf, grey, sizeof(unsigned char) *
            renderer.framebuffer.w * renderer.framebuffer.h * 4);
}

void FilledTri(int x0, int y0, int x1, int y1, int x2, int y2, int color) {
    // Compute triangle bounding box
    int minX = (x0 < x1) ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2);
    int minY = (y0 < y1) ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2);
    int maxX = (x0 > x1) ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2);
    int maxY = (y0 > y1) ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2);

    // Precompute edge functions
    int area = (x1 - x0)*(y2 - y0) - (y1 - y0)*(x2 - x0);
    if (area == 0) return; // Degenerate triangle, skip

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            int w0 = (x1 - x0)*(y - y0) - (y1 - y0)*(x - x0);
            int w1 = (x2 - x1)*(y - y1) - (y2 - y1)*(x - x1);
            int w2 = (x0 - x2)*(y - y2) - (y0 - y2)*(x - x2);

            // If all weights have the same sign as the area, inside triangle
            if ((w0 >= 0 && w1 >= 0 && w2 >= 0) || (w0 <= 0 && w1 <= 0 && w2 <= 0)) {
                PutPixel(x, y, color);
            }
        }
    }
}

void TexturedTri(Texture* t, int x0, int y0, float z0, float u0, float v0,
               int x1, int y1, float z1, float u1, float v1,
               int x2, int y2, float z2, float u2, float v2, DepthBuffer* db)
{
    /* 1. Bounding‑box, clamped to framebuffer */
    int minX = (x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2));
    int minY = (y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2));
    int maxX = (x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2));
    int maxY = (y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2));

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX >= renderer.framebuffer.w) maxX = renderer.framebuffer.w - 1;
    if (maxY >= renderer.framebuffer.h) maxY = renderer.framebuffer.h - 1;

    /* 2. Pre‑compute denominator and edge deltas */
    float denom = (float)((y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2));
    if (denom == 0.0f) return;          /* Degenerate triangle */

    float invDen = 1.0f / denom;

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            /* 3. Barycentric weights (affine) */
            float l0 = ((y1 - y2) * (x - x2) + (x2 - x1) * (y - y2)) * invDen;
            float l1 = ((y2 - y0) * (x - x2) + (x0 - x2) * (y - y2)) * invDen;
            float l2 = 1.0f - l0 - l1;

            /* Inside test (all weights in [0,1]) */
            if (l0 < 0.0f || l1 < 0.0f || l2 < 0.0f) continue;

            float depth = l0 * z0 + l1 * z1 + l2 * z2;
            
            if (!DepthBufferTestWrite(db, x, y, depth)) continue;
            

            /* 4. Interpolate UV */
            float u = l0 * u0 + l1 * u1 + l2 * u2;
            float v = l0 * v0 + l1 * v1 + l2 * v2;

            /* 5. Sample & draw */
            int texel = SampleTex(t, u, v);
            PutPixel(x, y, texel);
        }
    }
}

void DrawObj3DLambert(Camera* cam, Obj3D* obj, Framebuffer* fb, Light* l,
        int nLights, DepthBuffer* db){
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

        /* model -> world */
        v0 = MatVertMul(&obj->matModel, v0);
        v1 = MatVertMul(&obj->matModel, v1);
        v2 = MatVertMul(&obj->matModel, v2);

        /* world -> view */
        v0 = MatVertMul(&cam->view, v0);
        v1 = MatVertMul(&cam->view, v1);
        v2 = MatVertMul(&cam->view, v2);

        /* calculate tri normal */
        Vec3 v0_ = Vec3Make(v0.x, v0.y, v0.z);
        Vec3 v1_ = Vec3Make(v1.x, v1.y, v1.z);
        Vec3 v2_ = Vec3Make(v2.x, v2.y, v2.z);
        Vec3 side0 = Vec3Sub(v1_, v0_);
        Vec3 side1 = Vec3Sub(v2_, v0_);
        Vec3 normal = Vec3Cross(side0, side1);
        
        /* reject tris facing away from camera */
        if (!(normal.z > 0.f)) continue;

        /* accumulate light */
        int i;
        int rAcc = 0;
        int gAcc = 0;
        int bAcc = 0;
        for (i = 0; i < nLights; i++){
            if (l[i].type == LIGHT_AMBIENT){
                rAcc += GETR(l[i].rgb);
                gAcc += GETG(l[i].rgb);
                bAcc += GETB(l[i].rgb);
            }
            if (l[i].type == LIGHT_DIRECTIONAL){
                float s = Vec3Dot(Vec3Norm(normal), Vec3Norm(l[i].inverseDir));
                s = s > 0.f ? s : 0.f;
                s = s <= 1.f ? s : 1.f;
                rAcc += (int)(GETR(l[i].rgb) * s);
                gAcc += (int)(GETG(l[i].rgb) * s);
                bAcc += (int)(GETB(l[i].rgb) * s);
            }
        }
        rAcc = rAcc > 255 ? 255 : rAcc;
        gAcc = gAcc > 255 ? 255 : gAcc;
        bAcc = bAcc > 255 ? 255 : bAcc;
        float lR = rAcc / 255.f;
        float lG = gAcc / 255.f;
        float lB = bAcc / 255.f;
        Vec3 color = Vec3Make(lR, lG, lB); 
 
        /* view -> clip */
        v0 = MatVertMul(&cam->proj, v0);
        v1 = MatVertMul(&cam->proj, v1);
        v2 = MatVertMul(&cam->proj, v2);

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
        sx0 = (int)((v0.x * 0.5f + 0.5f) * fb->w);
        sy0 = (int)((0.5f - v0.y * 0.5f) * fb->h);
        sx1 = (int)((v1.x * 0.5f + 0.5f) * fb->w);
        sy1 = (int)((0.5f - v1.y * 0.5f) * fb->h);
        sx2 = (int)((v2.x * 0.5f + 0.5f) * fb->w);
        sy2 = (int)((0.5f - v2.y * 0.5f) * fb->h);

        TexturedLambertTri_(obj->model->tex, color, obj->id, db, sx0, sy0, v0.z,
                tu0, tv0, sx1, sy1, v1.z, tu1, tv1, sx2, sy2, v2.z, tu2, tv2);
    }
}

static inline void TexturedLambertTri_(Texture* t, Vec3 color, int id, 
        DepthBuffer* db,
        int x0, int y0, float z0, float u0, float v0, int x1, int y1, float z1,
        float u1, float v1, int x2, int y2, float z2, float u2, float v2){
    /* 1. Bounding‑box, clamped to framebuffer */
    int minX = (x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2));
    int minY = (y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2));
    int maxX = (x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2));
    int maxY = (y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2));

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX >= renderer.framebuffer.w) maxX = renderer.framebuffer.w - 1;
    if (maxY >= renderer.framebuffer.h) maxY = renderer.framebuffer.h - 1;

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

            /* Inside test (all weights in [0,1]) */
            if (l0 < 0.0f || l1 < 0.0f || l2 < 0.0f) continue;

            float depth = l0 * z0 + l1 * z1 + l2 * z2;
            
            if (!DepthBufferTestWrite(db, x, y, depth)) continue;
            

            /* 4. Interpolate UV */
            float u = l0 * u0 + l1 * u1 + l2 * u2;
            float v = l0 * v0 + l1 * v1 + l2 * v2;

            /* 5. Sample & draw */
            int texel = SampleTex(t, u, v);

            float tR = GETR(texel) / 255.f;
            float tG = GETG(texel) / 255.f;
            float tB = GETB(texel) / 255.f;
            unsigned char fR = (unsigned char)((tR * color.x) * 255);
            unsigned char fG = (unsigned char)((tG * color.y) * 255);
            unsigned char fB = (unsigned char)((tB * color.z) * 255);

            PutPixel(x, y, RGBA_INT(fR, fG, fB, 255));
            UpdatePickbuf(x, y, id);
        }
    }
}

void VisualizeBuffer(void* buf, int w, int h, char* type){
    if (!strcmp(type, "float")){
        float* fbuf = (float*)buf;
        int val;
        for (int y = 0; y < h; y++){
            for (int x = 0; x < w; x++){
                val = (int)(fbuf[y * w + x] * 255 + 0.5f);
                val = val > 255 ? 255 : val;
                val = val < 0 ? 0 : val;
                PutPixel(x, y, RGBA_INT(val, val, val, 255));
            }
        }
    }
}

void DrawObj3DLambertShadow(Camera* cam, Obj3D* obj, Framebuffer* fb, Light* l,
        int nLights, DepthBuffer* db, ShadowMapper* sm){
    int numTris = obj->model->mesh->indexCount / 9;
    Mat4 matShadow = MatMatMul(&sm->matTransform, &obj->matModel);

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

        /* model -> shadow clip */
        Vec4 vs0 = MatVertMul(&matShadow, v0);
        Vec4 vs1 = MatVertMul(&matShadow, v1);
        Vec4 vs2 = MatVertMul(&matShadow, v2);

        /* model -> world */
        v0 = MatVertMul(&obj->matModel, v0);
        v1 = MatVertMul(&obj->matModel, v1);
        v2 = MatVertMul(&obj->matModel, v2);

        /* world -> view */
        v0 = MatVertMul(&cam->view, v0);
        v1 = MatVertMul(&cam->view, v1);
        v2 = MatVertMul(&cam->view, v2);

        /* calculate tri normal */
        Vec3 v0_ = Vec3Make(v0.x, v0.y, v0.z);
        Vec3 v1_ = Vec3Make(v1.x, v1.y, v1.z);
        Vec3 v2_ = Vec3Make(v2.x, v2.y, v2.z);
        Vec3 side0 = Vec3Sub(v1_, v0_);
        Vec3 side1 = Vec3Sub(v2_, v0_);
        Vec3 normal = Vec3Cross(side0, side1);

        Vec3 los = Vec3Sub(v0_, Vec3Make(0.f, 0.f, 0.f));
        Vec3 invLos = Vec3Make(-los.x, -los.y, -los.z);
        float nDotLos = Vec3Dot(Vec3Norm(invLos), Vec3Norm(normal));
        
        /* reject tris facing away from camera */
        if (!(nDotLos > -0.001f)) continue;

        /* accumulate light */
        int i;
        int rAcc = 0;
        int gAcc = 0;
        int bAcc = 0;
        int ldra = 0;
        int ldga = 0;
        int ldba = 0;
        for (i = 0; i < nLights; i++){
            if (l[i].type == LIGHT_AMBIENT){
                rAcc += GETR(l[i].rgb);
                gAcc += GETG(l[i].rgb);
                bAcc += GETB(l[i].rgb);
            }
            if (l[i].type == LIGHT_DIRECTIONAL){
                float s = Vec3Dot(Vec3Norm(normal), Vec3Norm(l[i].inverseDir));
                s = s > 0.f ? s : 0.f;
                s = s <= 1.f ? s : 1.f;
                ldra += (int)(GETR(l[i].rgb) * s);
                ldga += (int)(GETG(l[i].rgb) * s);
                ldba += (int)(GETB(l[i].rgb) * s);
            }
        }
        rAcc = rAcc > 255 ? 255 : rAcc;
        gAcc = gAcc > 255 ? 255 : gAcc;
        bAcc = bAcc > 255 ? 255 : bAcc;
        ldra = ldra > 255 ? 255 : ldra;
        ldga = ldga > 255 ? 255 : ldga;
        ldba = ldba > 255 ? 255 : ldba;
        float lR = rAcc / 255.f;
        float lG = gAcc / 255.f;
        float lB = bAcc / 255.f;
        float ldraf = ldra / 255.f;
        float ldgaf = ldga / 255.f;
        float ldbaf = ldba / 255.f;
        Vec3 la = Vec3Make(lR, lG, lB); 
        Vec3 ld = Vec3Make(ldraf, ldgaf, ldbaf);
 
        /* view -> clip */
        v0 = MatVertMul(&cam->proj, v0);
        v1 = MatVertMul(&cam->proj, v1);
        v2 = MatVertMul(&cam->proj, v2);

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

        /* also do this for shadow verts */
        vs0.x /= vs0.w;
        vs0.y /= vs0.w;
        vs0.z /= vs0.w;
        vs1.x /= vs1.w;
        vs1.y /= vs1.w;
        vs1.z /= vs1.w;
        vs2.x /= vs2.w;
        vs2.y /= vs2.w;
        vs2.z /= vs2.w;

        /* NDC -> screen */
        sx0 = (int)((v0.x * 0.5f + 0.5f) * fb->w);
        sy0 = (int)((0.5f - v0.y * 0.5f) * fb->h);
        sx1 = (int)((v1.x * 0.5f + 0.5f) * fb->w);
        sy1 = (int)((0.5f - v1.y * 0.5f) * fb->h);
        sx2 = (int)((v2.x * 0.5f + 0.5f) * fb->w);
        sy2 = (int)((0.5f - v2.y * 0.5f) * fb->h);

        /* also to this for the shadow verts */
        vs0.x = (vs0.x * 0.5f + 0.5f);
        vs0.y = (0.5f - vs0.y * 0.5f);
        vs1.x = (vs1.x * 0.5f + 0.5f);
        vs1.y = (0.5f - vs1.y * 0.5f);
        vs2.x = (vs2.x * 0.5f + 0.5f);
        vs2.y = (0.5f - vs2.y * 0.5f);
        vs0.z = vs0.z * 0.5f + 0.5f;
        vs1.z = vs1.z * 0.5f + 0.5f;
        vs2.z = vs2.z * 0.5f + 0.5f;

        TexturedLambertShadowTri_(obj->model->tex, la, obj->id, db, ld, sx0, sy0, v0.z,
                tu0, tv0, sx1, sy1, v1.z, tu1, tv1, sx2, sy2, v2.z, tu2, tv2
                , sm, vs0, vs1, vs2);
    }
}

static inline void TexturedLambertShadowTri_(Texture* t, Vec3 la, int id,
        DepthBuffer* db, Vec3 ld,
        int x0, int y0, float z0, float u0, float v0, int x1, int y1, float z1,
        float u1, float v1, int x2, int y2, float z2, float u2, float v2
        , ShadowMapper* sm, Vec4 sh0, Vec4 sh1, Vec4 sh2){
    /* 1. Bounding‑box, clamped to framebuffer */
    int minX = (x0 < x1 ? (x0 < x2 ? x0 : x2) : (x1 < x2 ? x1 : x2));
    int minY = (y0 < y1 ? (y0 < y2 ? y0 : y2) : (y1 < y2 ? y1 : y2));
    int maxX = (x0 > x1 ? (x0 > x2 ? x0 : x2) : (x1 > x2 ? x1 : x2));
    int maxY = (y0 > y1 ? (y0 > y2 ? y0 : y2) : (y1 > y2 ? y1 : y2));

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX >= renderer.framebuffer.w) maxX = renderer.framebuffer.w - 1;
    if (maxY >= renderer.framebuffer.h) maxY = renderer.framebuffer.h - 1;

    /* 2. Pre‑compute denominator and edge deltas */
    float denom = (float)((y1 - y2) * (x0 - x2) + (x2 - x1) * (y0 - y2));
    if (denom == 0.0f){
        return;
    }

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

            /* Inside test (all weights in [0,1]) */
            if (l0 < -0.001f || l1 < -0.001f || l2 < -0.001f){
                continue;
            }

            float depth = l0 * z0 + l1 * z1 + l2 * z2;
            
            if (!DepthBufferTestWrite(db, x, y, depth)) continue;
            

            /* 4. Interpolate UV */
            float u = l0 * u0 + l1 * u1 + l2 * u2;
            float v = l0 * v0 + l1 * v1 + l2 * v2;
            float sx = l0 * sh0.x + l1 * sh1.x + l2 * sh2.x;
            float sy = l0 * sh0.y + l1 * sh1.y + l2 * sh2.y;
            float sz = l0 * sh0.z + l1 * sh1.z + l2 * sh2.z;

            int tx = (int)(sx * (sm->w - 1) + 0.5f);
            int ty = (int)(sy * (sm->h - 1) + 0.5f);
            
            int lit = 0;
            float sample = sm->buf[ty * sm->w + tx];
            lit = sz - 0.004f <= sample;

            /* 5. Sample & draw */
            int texel = SampleTex(t, u, v);

            float tR = GETR(texel) / 255.f;
            float tG = GETG(texel) / 255.f;
            float tB = GETB(texel) / 255.f;
            float lR = la.x;
            float lG = la.y;
            float lB = la.z;
            if (lit){
                lR += ld.x;
                lG += ld.y;
                lB += ld.z;
            }
            unsigned char iR = (unsigned char)(tR * lR * 255.f);
            unsigned char iG = (unsigned char)(tG * lG * 255.f);
            unsigned char iB = (unsigned char)(tB * lB * 255.f);

            PutPixel(x, y, RGBA_INT(iR, iG, iB, 255));
            UpdatePickbuf(x, y, id);
        }
    }
}

static inline void TexturedLambertShadowFloatTri_(Texture* t, Vec3 la, int id,
        DepthBuffer* db, Vec3 ld,
        float x0, float y0, float z0, float u0, float v0, float x1, float y1, float z1,
        float u1, float v1, float x2, float y2, float z2, float u2, float v2
        , ShadowMapper* sm, Vec4 sh0, Vec4 sh1, Vec4 sh2){
    int w = renderer.framebuffer.w;
    int h = renderer.framebuffer.h;
    float fx0 = ((x0 * 0.5f + 0.5f) * w);
    float fy0 = ((0.5f - y0 * 0.5f) * h);
    float fx1 = ((x1 * 0.5f + 0.5f) * w);
    float fy1 = ((0.5f - y1 * 0.5f) * h);
    float fx2 = ((x2 * 0.5f + 0.5f) * w);
    float fy2 = ((0.5f - y2 * 0.5f) * h);



    /* 1. Bounding‑box, clamped to framebuffer */
    int minX = (int)fminf(fminf(fx0, fx1), fx2);
    int minY = (int)fminf(fminf(fy0, fy1), fy2);
    int maxX = (int)fmaxf(fmaxf(fx0, fx1), fx2);
    int maxY = (int)fmaxf(fmaxf(fy0, fy1), fy2);

    if (minX < 0) minX = 0;
    if (minY < 0) minY = 0;
    if (maxX >= w) maxX = w - 1;
    if (maxY >= h) maxY = h - 1;

    /* 2. Pre‑compute denominator and edge deltas */
    float denom = ((fy1 - fy2) * (fx0 - fx2) + (fx2 - fx1) * (fy0 - fy2));
    if (fabsf(denom) < 1e-6f){
        return;
    }

    /* accumulate contribution from ambient lights */
    float invDen = 1.0f / denom;

    for (int y = minY; y <= maxY; ++y)
    {
        for (int x = minX; x <= maxX; ++x)
        {
            float px = x + 0.5f;
            float py = y + 0.5f;
            /* 3. Barycentric weights (affine) */
            float l0 = ((fy1 - fy2) * (px - fx2) + (fx2 - fx1) * (py - fy2)) * invDen;
            float l1 = ((fy2 - fy0) * (px - fx2) + (fx0 - fx2) * (py - fy2)) * invDen;
            float l2 = 1.0f - l0 - l1;

            /* Inside test (all weights in [0,1]) */
            if (l0 < -0.001f || l1 < -0.001f || l2 < -0.001f){
                continue;
            }

            float depth = l0 * z0 + l1 * z1 + l2 * z2;
            
            if (!DepthBufferTestWrite(db, x, y, depth)) continue;
            
            /* 4. Interpolate UV */
            float u = l0 * u0 + l1 * u1 + l2 * u2;
            float v = l0 * v0 + l1 * v1 + l2 * v2;
            float sx = l0 * sh0.x + l1 * sh1.x + l2 * sh2.x;
            float sy = l0 * sh0.y + l1 * sh1.y + l2 * sh2.y;
            float sz = l0 * sh0.z + l1 * sh1.z + l2 * sh2.z;

            int tx = (int)(sx * (sm->w - 1) + 0.5f);
            int ty = (int)(sy * (sm->h - 1) + 0.5f);
            
            int lit = 0;
            /* clamp for debug purposes only - better solution later */
            tx = tx < 0 ? 0 : tx;
            tx = tx >= sm->w ? sm->w - 1 : tx;
            ty = ty < 0 ? 0 : ty;
            ty = ty >= sm->h ? sm->h - 1 : ty;
            
            float sample = sm->buf[ty * sm->w + tx];
            lit = sz - 0.004f <= sample;

            /* 5. Sample & draw */
            int texel = SampleTex(t, u, v);

            float tR = GETR(texel) / 255.f;
            float tG = GETG(texel) / 255.f;
            float tB = GETB(texel) / 255.f;
            float lR = la.x;
            float lG = la.y;
            float lB = la.z;
            if (lit){
                lR += ld.x;
                lG += ld.y;
                lB += ld.z;
            }
            unsigned char iR = (unsigned char)(tR * lR * 255.f);
            unsigned char iG = (unsigned char)(tG * lG * 255.f);
            unsigned char iB = (unsigned char)(tB * lB * 255.f);

            PutPixel(x, y, RGBA_INT(iR, iG, iB, 255));
            UpdatePickbuf(x, y, id);
        }
    }
}

void DrawObj3DLambertShadowFloat(Camera* cam, Obj3D* obj, Framebuffer* fb, Light* l,
        int nLights, DepthBuffer* db, ShadowMapper* sm){
    int numTris = obj->model->mesh->indexCount / 9;
    Mat4 matShadow = MatMatMul(&sm->matTransform, &obj->matModel);

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

        /* model -> shadow clip */
        Vec4 vs0 = MatVertMul(&matShadow, v0);
        Vec4 vs1 = MatVertMul(&matShadow, v1);
        Vec4 vs2 = MatVertMul(&matShadow, v2);

        /* model -> world */
        v0 = MatVertMul(&obj->matModel, v0);
        v1 = MatVertMul(&obj->matModel, v1);
        v2 = MatVertMul(&obj->matModel, v2);

        /* world -> view */
        v0 = MatVertMul(&cam->view, v0);
        v1 = MatVertMul(&cam->view, v1);
        v2 = MatVertMul(&cam->view, v2);

        /* calculate tri normal */
        Vec3 v0_ = Vec3Make(v0.x, v0.y, v0.z);
        Vec3 v1_ = Vec3Make(v1.x, v1.y, v1.z);
        Vec3 v2_ = Vec3Make(v2.x, v2.y, v2.z);
        Vec3 side0 = Vec3Sub(v1_, v0_);
        Vec3 side1 = Vec3Sub(v2_, v0_);
        Vec3 normal = Vec3Cross(side0, side1);

        Vec3 los = Vec3Sub(v0_, Vec3Make(0.f, 0.f, 0.f));
        Vec3 invLos = Vec3Make(-los.x, -los.y, -los.z);
        float nDotLos = Vec3Dot(Vec3Norm(invLos), Vec3Norm(normal));
        
        /* reject tris facing away from camera */
        if (!(nDotLos > -0.001f)) continue;

        /* accumulate light */
        int i;
        int rAcc = 0;
        int gAcc = 0;
        int bAcc = 0;
        int ldra = 0;
        int ldga = 0;
        int ldba = 0;
        for (i = 0; i < nLights; i++){
            if (l[i].type == LIGHT_AMBIENT){
                rAcc += GETR(l[i].rgb);
                gAcc += GETG(l[i].rgb);
                bAcc += GETB(l[i].rgb);
            }
            if (l[i].type == LIGHT_DIRECTIONAL){
                float s = Vec3Dot(Vec3Norm(normal), Vec3Norm(l[i].inverseDir));
                s = s > 0.f ? s : 0.f;
                s = s <= 1.f ? s : 1.f;
                ldra += (int)(GETR(l[i].rgb) * s);
                ldga += (int)(GETG(l[i].rgb) * s);
                ldba += (int)(GETB(l[i].rgb) * s);
            }
        }
        rAcc = rAcc > 255 ? 255 : rAcc;
        gAcc = gAcc > 255 ? 255 : gAcc;
        bAcc = bAcc > 255 ? 255 : bAcc;
        ldra = ldra > 255 ? 255 : ldra;
        ldga = ldga > 255 ? 255 : ldga;
        ldba = ldba > 255 ? 255 : ldba;
        float lR = rAcc / 255.f;
        float lG = gAcc / 255.f;
        float lB = bAcc / 255.f;
        float ldraf = ldra / 255.f;
        float ldgaf = ldga / 255.f;
        float ldbaf = ldba / 255.f;
        Vec3 la = Vec3Make(lR, lG, lB); 
        Vec3 ld = Vec3Make(ldraf, ldgaf, ldbaf);
 
        /* view -> clip */
        v0 = MatVertMul(&cam->proj, v0);
        v1 = MatVertMul(&cam->proj, v1);
        v2 = MatVertMul(&cam->proj, v2);

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

        /* also do this for shadow verts */
        vs0.x /= vs0.w;
        vs0.y /= vs0.w;
        vs0.z /= vs0.w;
        vs1.x /= vs1.w;
        vs1.y /= vs1.w;
        vs1.z /= vs1.w;
        vs2.x /= vs2.w;
        vs2.y /= vs2.w;
        vs2.z /= vs2.w;

        /* also to this for the shadow verts */
        vs0.x = (vs0.x * 0.5f + 0.5f);
        vs0.y = (0.5f - vs0.y * 0.5f);
        vs1.x = (vs1.x * 0.5f + 0.5f);
        vs1.y = (0.5f - vs1.y * 0.5f);
        vs2.x = (vs2.x * 0.5f + 0.5f);
        vs2.y = (0.5f - vs2.y * 0.5f);
        vs0.z = vs0.z * 0.5f + 0.5f;
        vs1.z = vs1.z * 0.5f + 0.5f;
        vs2.z = vs2.z * 0.5f + 0.5f;

        TexturedLambertShadowFloatTri_(obj->model->tex, la, obj->id, db, ld, v0.x, v0.y, v0.z,
                tu0, tv0, v1.x, v1.y, v1.z, tu1, tv1, v2.x, v2.y, v2.z, tu2, tv2
                , sm, vs0, vs1, vs2);
    }
}

static inline void DrawLineDDA_(Vec3 v0, Vec3 v1, DepthBuffer* db){
    int c = RGBA_INT(192, 64, 64, 255);

    float depth0 = v0.z;
    float depth1 = v1.z;

    float dx = v1.x - v0.x;
    float dy = v1.y - v0.y;
    float dz = depth1 - depth0;

    float abs_dx = (dx < 0) ? -dx : dx;
    float abs_dy = (dy < 0) ? -dy : dy;

    int steps = (abs_dx > abs_dy) ? (int)(abs_dx + 0.5f) : (int)(abs_dy + 0.5f);
    if (steps == 0){
        int x = (int)(v0.x + 0.5f);
        int y = (int)(v0.y + 0.5f);
        if (!DepthBufferTestWrite(db, x, y, depth0)) return;
        PutPixel(x, y, c);
        return;
    }

    float x_inc = dx / steps;
    float y_inc = dy / steps;
    float z_inc = dz / steps;

    for (int i = 0; i <= steps; i++){
        float x_ = v0.x + ((i / (float)steps) * dx);
        float y_ = v0.y + ((i / (float)steps) * dy);

        int x = (int)(x_ + 0.5f);
        int y = (int)(y_ + 0.5f);
        float valInDB = 420.f;

        float d = depth0 + ((i / (float)steps) * dz);
        int depthTest = DepthBufferTestWriteDebug(db, x, y, d, &valInDB);
        if (!depthTest){
            continue;
        } else {
        }
        PutPixel(x, y, c);
    }
}

static inline void DrawLineWu_(Vec3 v0, Vec3 v1, DepthBuffer* db){
    unsigned char r = 192;
    unsigned char g = 128;
    unsigned char b = 128;
    float dx = v1.x - v0.x;
    float dy = v1.y - v0.y;
    float absdx = dx < 0.f ? -dx : dx;
    float absdy = dy < 0.f ? -dy : dy;
    if (absdy < absdx){
        if (v1.x < v0.x){
            float t = v0.x;
            v0.x = v1.x;
            v1.x = t;
            t = v0.y;
            v0.y = v1.y;
            v1.y = t;
            dx *= -1;
            dy *= -1;
        }
        float m = dy / dx;
        float fatx = v0.x + 0.5f;
        int ifatx = (int)fatx;
        float overlap = 1.f - (fatx - ifatx);
        float dist = v0.y - (int)v0.y;
        unsigned char a0 = (unsigned char)(((1.f - dist) * overlap) * 255);
        unsigned char a1 = (unsigned char)((dist * overlap) * 255);
        int iy = (int)v0.y;
        BlendPixel_(ifatx, iy, RGBA_INT(r, g, b, a0)); 
        BlendPixel_(ifatx, iy + 1, RGBA_INT(r, g, b, a1));

        fatx = v1.x + 0.5f;
        ifatx = (int)fatx;
        overlap = fatx - ifatx;
        iy = (int)v1.y;
        dist = v1.y - iy;
        a0 = (unsigned char)(((1.f - dist) * overlap) * 255);
        a1 = (unsigned char)((dist * overlap) * 255);
        BlendPixel_(ifatx, iy, RGBA_INT(r, g, b, a0));
        BlendPixel_(ifatx, iy + 1, RGBA_INT(r, g, b, a1));
        /*
        for (int i = 1; i < (int)roundf(dx + 0.5f); i++){
            float y = v0.y + i * m;
            int ix = (int)(v0.x + i);
            int iy = (int)y;
            float dist = y - iy;
            a0 = (unsigned char)((1.f - dist) * 255);
            a1 = (unsigned char)(dist * 255);
            BlendPixel_(ix, iy, RGBA_INT(r, g, b, a0));
            BlendPixel_(ix, iy + 1, RGBA_INT(r, g, b, a1));
        }
        */
        int x0 = (int)(v0.x + 0.5f);
        int x1 = (int)(v1.x + 0.5f);
        for (int x = x0; x <= x1; x++){
            float y = v0.y + m * (x - v0.x);
            int yFloor = (int)floor(y);
            float fraction = y - yFloor;
            a0 = (unsigned char)((1.f - fraction) * 255);
            a1 = (unsigned char)(fraction * 255);
            BlendPixel_(x, yFloor, RGBA_INT(r, g, b, a0));
            BlendPixel_(x, yFloor + 1, RGBA_INT(r, g, b, a1));
        }
    } else {
        if (v1.y < v0.y){
            float t = v0.x;
            v0.x = v1.x;
            v1.x = t;
            t = v0.y;
            v0.y = v1.y;
            v1.y = t;
            dx *= -1;
            dy *= -1;
        }
        float dx = v1.x - v0.x;
        float m = dx / dy; 
        float faty = v0.y + 0.5f;
        int ifaty = (int)faty;
        int iy = (int)v0.y;
        float overlap = 1.f - (faty - ifaty);
        float dist = v0.y - iy;
        unsigned char a0 = (unsigned char)(((1.f - dist) * overlap) * 255);
        unsigned char a1 = (unsigned char)((dist * overlap) * 255);
        float fatx = v0.x + 0.5f;
        int ifatx = (int)fatx;
        BlendPixel_(ifatx, iy, RGBA_INT(r, g, b, a0));
        BlendPixel_(ifatx, iy, RGBA_INT(r, g, b, a1));

        faty = v1.y + 0.5f;
        ifaty = (int)faty;
        overlap = faty - ifaty;
        iy = (int)v1.y;
        dist = v1.y - iy;
        fatx = v1.x + 0.5f;
        a0 = (unsigned char)(((1.f - dist) * overlap) * 255);
        a1 = (unsigned char)((dist * overlap) * 255);
        BlendPixel_(ifatx, iy, RGBA_INT(r, g, b, a0));
        BlendPixel_(ifatx, iy + 1, RGBA_INT(r, g, b, a1));

        /*
        for (int i = 1; i < (int)roundf(dy + 0.5f); i++){
            float x = v0.x + i * m;
            int ix = (int)x;
            int iy = (int)(v0.y + i);
            float dist = x - ix;
            a0 = (unsigned char)((1.f - dist) * 255);
            a1 = (unsigned char)(dist * 255);
            BlendPixel_(ix, iy, RGBA_INT(r, g, b, a0));
            BlendPixel_(ix + 1, iy, RGBA_INT(r, g, b, a1));
        }
        */
        int y0 = (int)(v0.y + 0.5f);
        int y1 = (int)(v1.y + 0.5f);
        for(int y = y0; y <= y1; y++){
            float x = v0.x + m * (y - v0.y);
            int xFloor = (int)floor(x);
            float fraction = x - xFloor;
            a0 = (unsigned char)((1.f - fraction) * 255);
            a1 = (unsigned char)(fraction * 255);
            BlendPixel_(xFloor, y, RGBA_INT(r, g, b, a0));
            BlendPixel_(xFloor + 1, y, RGBA_INT(r, g, b, a1));
        }
    }
}

static inline void DrawLineWu1_(Vec3 v0, Vec3 v1, DepthBuffer* db) {
    unsigned char r = 192;
    unsigned char g = 128;
    unsigned char b = 128;
    
    float dx = v1.x - v0.x;
    float dy = v1.y - v0.y;
    float absdx = fabsf(dx);
    float absdy = fabsf(dy);
    
    // Determine if line is more horizontal or vertical
    if (absdx > absdy) {
        // Line is more horizontal than vertical
        
        // Ensure we go from left to right
        if (v0.x > v1.x) {
            // Swap endpoints
            Vec3 temp = v0;
            v0 = v1;
            v1 = temp;
        }
        
        float gradient = dy / dx;
        
        // Handle first endpoint
        int x0 = (int)roundf(v0.x);
        int x1 = (int)roundf(v1.x);
        float y = v0.y + gradient * (x0 - v0.x);
        
        // Draw pixels from x0 to x1
        for (int x = x0; x <= x1; x++) {
            int yFloor = (int)floorf(y);
            float fraction = y - yFloor;
            
            // Anti-aliased pixels
            unsigned char a0 = (unsigned char)((1.0f - fraction) * 255);
            unsigned char a1 = (unsigned char)(fraction * 255);
            
            BlendPixel_(x, yFloor, RGBA_INT(r, g, b, a0));
            BlendPixel_(x, yFloor + 1, RGBA_INT(r, g, b, a1));
            
            y += gradient;
        }
    } else {
        // Line is more vertical than horizontal
        
        // Ensure we go from bottom to top
        if (v0.y > v1.y) {
            // Swap endpoints
            Vec3 temp = v0;
            v0 = v1;
            v1 = temp;
        }
        
        float gradient = dx / dy;
        
        // Handle first endpoint
        int y0 = (int)roundf(v0.y);
        int y1 = (int)roundf(v1.y);
        float x = v0.x + gradient * (y0 - v0.y);
        
        // Draw pixels from y0 to y1
        for (int y = y0; y <= y1; y++) {
            int xFloor = (int)floorf(x);
            float fraction = x - xFloor;
            
            // Anti-aliased pixels
            unsigned char a0 = (unsigned char)((1.0f - fraction) * 255);
            unsigned char a1 = (unsigned char)(fraction * 255);
            
            BlendPixel_(xFloor, y, RGBA_INT(r, g, b, a0));
            BlendPixel_(xFloor + 1, y, RGBA_INT(r, g, b, a1));
            
            x += gradient;
        }
    }
}

static inline void DrawLineWu_Gamma(Vec3 v0, Vec3 v1, int c){
    unsigned char r = GETR(c);
    unsigned char g = GETG(c);
    unsigned char b = GETB(c);
    
    float dx = v1.x - v0.x;
    float dy = v1.y - v0.y;
    float absdx = fabsf(dx);
    float absdy = fabsf(dy);
    
    // Determine if line is more horizontal or vertical
    if (absdx > absdy) {
        // Line is more horizontal than vertical
        
        // Ensure we go from left to right
        if (v0.x > v1.x) {
            // Swap endpoints
            Vec3 temp = v0;
            v0 = v1;
            v1 = temp;
        }
        
        float gradient = dy / dx;
        
        // Handle first endpoint
        int x0 = (int)roundf(v0.x);
        int x1 = (int)roundf(v1.x);
        assert(x0 >= 0);
        assert(x0 < 1000);
        assert(x1 >= 0);
        assert(x1 < 1000);
        float y = v0.y + gradient * (x0 - v0.x);

        float dz = v1.z - v0.z;
        float dx = v1.x - v0.x;
        
        // Draw pixels from x0 to x1
        for (int x = x0; x <= x1; x++) {
            int yFloor = (int)floorf(y);
            yFloor = (yFloor == -1) ? 0 : yFloor;
            float fraction = y - yFloor;

            float z = v0.z + ((((float)x - v0.x) / dx) * dz);
            int testy = (yFloor >= 0) ? yFloor : 0;
            testy = (testy < renderer.framebuffer.h) ? testy : renderer.framebuffer.h;
            if (!DepthBufferTestWrite(&renderer.db, x, testy, z)){
                y += gradient;
                continue;
            }

            if (0){
            /* top pixel is dominant */
            if (fraction < 0.5f){
                if (x >= 0 && x < renderer.framebuffer.w &&
                        yFloor >= 0 && yFloor < renderer.framebuffer.h){
                    if (!DepthBufferTestWrite(&renderer.db, x, yFloor, z)){
                        y += gradient;
                        continue;
                    }
                } else if (x >= 0 && x < renderer.framebuffer.w &&
                    yFloor + 1 >= 0 && yFloor + 1 < renderer.framebuffer.h){
                    if (!DepthBufferTestWrite(&renderer.db, x, yFloor + 1, z)){
                        y += gradient;
                        continue;
                    }
                }
            /* bottom pixel is dominant */
            } else {
                if (x >= 0 && x < renderer.framebuffer.w &&
                        yFloor + 1 >= 0 && yFloor + 1 < renderer.framebuffer.h){
                    if (!DepthBufferTestWrite(&renderer.db, x, yFloor + 1, z)){
                        y += gradient;
                        continue;
                    }
                } else if (x >= 0 && x < renderer.framebuffer.w &&
                        yFloor >= 0 && yFloor < renderer.framebuffer.h){
                    if (!DepthBufferTestWrite(&renderer.db, x, yFloor, z)){
                        y += gradient;
                        continue;
                    }
                }
            }
            }

            // Anti-aliased pixels
            unsigned char a0 = (unsigned char)((1.0f - fraction) * 255);
            unsigned char a1 = (unsigned char)(fraction * 255);
            
            assert(a0 >= 0 && a0 < 256 && a1 >= 0 && a1 < 256);
            if (x < 0 || x > 999 || yFloor < 0 || yFloor > 999){
                printf("first block\n");
                printf("x: %d, y: %d\n", x, yFloor);
                printf("y: %8.5f\n", y);
                printf("v0.y: %8.5f, gradient: %8.5f\n", v0.y, gradient);
                getchar();
            }
            BlendPixel_(x, yFloor, RGBA_INT(r, g, b, renderer.gammaLUT[a0]));
            if (yFloor >= 999){
                y += gradient;
                continue;
            }
            BlendPixel_(x, yFloor + 1, RGBA_INT
                    (r, g, b, renderer.gammaLUT[a1]));
 
            y += gradient;
        }
    } else {
        /* slope > 1 */
        if (v0.y > v1.y) {
            // Swap endpoints
            Vec3 temp = v0;
            v0 = v1;
            v1 = temp;
        }
        
        float gradient = dx / dy;
        
        // Handle first endpoint
        int y0 = (int)roundf(v0.y);
        int y1 = (int)roundf(v1.y);
        assert(y0 >= 0);
        assert(y0 < 1000);
        assert(y1 >= 0);
        assert(y1 < 1000);
        float x = v0.x + gradient * (y0 - v0.y);

        float dz = v1.z - v0.z;
        float dy = v1.y - v0.y;
        
        // Draw pixels from y0 to y1
        for (int y = y0; y <= y1; y++) {
            int xFloor = (int)floorf(x);
            xFloor = (xFloor == -1) ? 0 : xFloor;
            float fraction = x - xFloor;

            float z = v0.z + (((y - v0.y) / dy) * dz);
            int testx = (xFloor >= 0) ? xFloor : 0;
            testx = (testx < renderer.framebuffer.w) ? testx : renderer.framebuffer.w;
            if (!DepthBufferTestWrite(&renderer.db, testx, y, z)){
                x += gradient;
                continue;
            }

            
            // Anti-aliased pixels
            unsigned char a0 = (unsigned char)((1.0f - fraction) * 255);
            unsigned char a1 = (unsigned char)(fraction * 255);
            
            assert(a0 >= 0 && a0 < 256 && a1 >= 0 && a1 < 256);
            if (y < 0 || y > 999 || xFloor < 0 || xFloor > 999){
                printf("second block\n");
                printf("y: %d, x: %d\n", y, xFloor);
                getchar();
            }
            BlendPixel_(xFloor, y, RGBA_INT(r, g, b, renderer.gammaLUT[a0]));
            if (xFloor >= 999){
                x += gradient;
                continue;
            }
            BlendPixel_(xFloor + 1, y, RGBA_INT(r, g, b, renderer.gammaLUT[a1]));
            
            x += gradient;
        }
    }
}

static inline int PointInsidePlane_(Vec4 point, int plane){
    assert(plane >= 0 && plane < 6);
    switch (plane) {
    case 0: return point.x > -point.w + 1e-6f; /* left     */
    case 1: return point.x <  point.w - 1e-6f; /* right    */
    case 2: return point.y > -point.w + 1e-6f; /* bottom   */
    case 3: return point.y <  point.w - 1e-6f; /* top      */
    case 4: return point.z > -point.w; /* near     */
    case 5: return point.z <  point.w; /* far      */
    }
}

static inline Vec4 LinePlaneIntersect_(Vec4 a, Vec4 b, int plane){
    assert(plane >= 0 && plane < 6);
    float t, denom;
    switch(plane) {
        case 0: /* left */ {
            if (fabsf(denom = (b.x - a.x) + (b.w - a.w)) < 1e-6f) return a;
            t = (-a.w - a.x) / denom;
        } break;
        case 1: /* right */ {
            if (fabsf(denom = (b.x - a.x) - (b.w - a.w)) < 1e-6f) return a;
            t = (a.w - a.x) / denom;
        } break;
        case 2: /* bottom */ {
            if (fabsf(denom = (b.y - a.y) + (b.w - a.w)) < 1e-6f) return a;
            t = (-a.w - a.y) / denom;
        } break;
        case 3: /* top */ {
            if (fabsf(denom = (b.y - a.y) - (b.w - a.w)) < 1e-6f) return a;
            t = (a.w - a.y) / denom;
        } break;  
        case 4: /* near */ {
            if (fabsf(denom = (b.z - a.z) + (b.w - a.w)) < 1e-6f) return a;
            t = (-a.w - a.z) / denom;
        } break;
        case 5: /* far */ {
            if (fabsf(denom = (b.z - a.z) - (b.w - a.w)) < 1e-6f) return a;
            t = (a.w - a.z) / denom;
        } break;
    }
    return Vec4Lerp(a, b, t);
}

static inline NGon TriPlaneClip_(NGon input, int plane){
    NGon output;
    output.vertCount = 0;

    if (input.vertCount == 0) return output;

    Vec4 prev_vert = input.verts[input.vertCount - 1];
    int prev_inside = PointInsidePlane_(prev_vert, plane);

    for (int i = 0; i < input.vertCount; i++){
        if (output.vertCount >= 7){
            /* TODO: logging */
            break;
        }

        Vec4 curr_vert = input.verts[i];
        int curr_inside = PointInsidePlane_(curr_vert, plane);

        if (curr_inside) {
            if (!prev_inside && output.vertCount < 7){
                output.verts[output.vertCount++] =
                    LinePlaneIntersect_(prev_vert, curr_vert, plane);
            }
            if (output.vertCount < 8)
                output.verts[output.vertCount++] = curr_vert;
        } else if (prev_inside && output.vertCount < 8) {
            output.verts[output.vertCount++] =
                LinePlaneIntersect_(prev_vert, curr_vert, plane);
        }
        prev_vert = curr_vert;
        prev_inside = curr_inside;
    }
    return output;
}

static inline NGon TriClip_(Vec4 v0, Vec4 v1, Vec4 v2){
    NGon ngon;
    ngon.verts[0] = v0;
    ngon.verts[1] = v1;
    ngon.verts[2] = v2;
    ngon.vertCount = 3;

    for (int plane = 0; plane < 6; plane++){
        ngon = TriPlaneClip_(ngon, plane);

        if (ngon.vertCount == 0) break;
    }
    return ngon;
}

static inline Tri4Cluster Triangulate_(NGon ngon){
    Tri4Cluster result;
    result.count = 0;

    if (ngon.vertCount < 3) return result;

    if (ngon.vertCount == 3) {
        result.tris[0].v0 = ngon.verts[0];
        result.tris[0].v1 = ngon.verts[1];
        result.tris[0].v2 = ngon.verts[2];
        result.count = 1;
        return result;
    }

    Vec4 pivot = ngon.verts[0];
    for (int i = 1; i < ngon.vertCount - 1; i++){
        result.tris[result.count].v0 = pivot;
        result.tris[result.count].v1 = ngon.verts[i];
        result.tris[result.count].v2 = ngon.verts[i + 1];
        result.count++;
    }
    return result;
}

static inline VertexColorNGon VertexColorTriClip_(VertexColorVert verts[3]){
    VertexColorNGon ngon;
    ngon.verts[0] = verts[0];
    ngon.verts[1] = verts[1];
    ngon.verts[2] = verts[2];
    ngon.vertCount = 3;

    for (int plane = 0; plane < 6; plane++) {
        ngon = VertexColorTriPlaneClip_(ngon, plane);

        if (ngon.vertCount == 0) break;
    }
    return ngon;
}

static inline VertexColorTriCluster VertexColorTriangulate_
        (VertexColorNGon ngon) {
    VertexColorTriCluster result;
    result.count = 0;

    if (ngon.vertCount < 3) return result;

    if (ngon.vertCount == 3) {
        result.tris[0].v0 = ngon.verts[0];
        result.tris[0].v1 = ngon.verts[1];
        result.tris[0].v2 = ngon.verts[2];
        result.count = 1;
        return result;
    }

    VertexColorVert pivot = ngon.verts[0];
    for (int i = 1; i < ngon.vertCount - 1; i++) {
        result.tris[result.count].v0 = pivot;
        result.tris[result.count].v1 = ngon.verts[i];
        result.tris[result.count].v2 = ngon.verts[i + 1];
        result.count++;
    }
    return result;
}



static inline VertexColorNGon VertexColorTriPlaneClip_(VertexColorNGon input,
        int plane) {
    VertexColorNGon output;
    output.vertCount = 0;

    if (input.vertCount == 0) return output;
    VertexColorVert prev_vert = input.verts[input.vertCount - 1];
    int prev_inside = PointInsidePlane_(prev_vert.pos, plane);

    for (int i = 0; i < input.vertCount; i++){
        if (output.vertCount >= 7){
            /* TODO: logging */
            break;
        }
        VertexColorVert curr_vert = input.verts[i];
        int curr_inside = PointInsidePlane_(curr_vert.pos, plane);

        if (curr_inside) {
            if (!prev_inside && output.vertCount < 7){
                output.verts[output.vertCount++] =
                    VertexColorLinePlaneIntersect_(prev_vert, curr_vert, plane);
            }
            if (output.vertCount < 8)
                output.verts[output.vertCount++] = curr_vert;
        } else if (prev_inside && output.vertCount < 8) {
            output.verts[output.vertCount++] =
                VertexColorLinePlaneIntersect_(prev_vert, curr_vert, plane);
        }
        prev_vert = curr_vert;
        prev_inside = curr_inside;
    }
    return output;
}

static inline VertexColorVert VertexColorLinePlaneIntersect_
        (VertexColorVert a, VertexColorVert b, int plane) {
    assert(plane >= 0 && plane < 6);
    float t, denom;
    switch(plane) {
        case 0: /* left */ {
            if (fabsf(denom = (b.pos.x - a.pos.x) + (b.pos.w - a.pos.w))
                    < 1e-6f) return a;
            t = (-a.pos.w - a.pos.x) / denom;
        } break;
        case 1: /* right */ {
            if (fabsf(denom = (b.pos.x - a.pos.x) - (b.pos.w - a.pos.w))
                    < 1e-6f) return a;
            t = (a.pos.w - a.pos.x) / denom;
        } break;
        case 2: /* bottom */ {
            if (fabsf(denom = (b.pos.y - a.pos.y) + (b.pos.w - a.pos.w))
                    < 1e-6f) return a;
            t = (-a.pos.w - a.pos.y) / denom;
        } break;
        case 3: /* top */ {
            if (fabsf(denom = (b.pos.y - a.pos.y) - (b.pos.w - a.pos.w))
                    < 1e-6f) return a;
            t = (a.pos.w - a.pos.y) / denom;
        } break;  
        case 4: /* near */ {
            if (fabsf(denom = (b.pos.z - a.pos.z) + (b.pos.w - a.pos.w))
                    < 1e-6f) return a;
            t = (-a.pos.w - a.pos.z) / denom;
        } break;
        case 5: /* far */ {
            if (fabsf(denom = (b.pos.z - a.pos.z) - (b.pos.w - a.pos.w))
                    < 1e-6f) return a;
            t = (a.pos.w - a.pos.z) / denom;
        } break;
    }
    return VertexColorVertLerp_(a, b, t);
}

static inline VertexColorVert VertexColorVertLerp_(VertexColorVert a,
        VertexColorVert b, float t) {
    VertexColorVert result;
    result.pos = Vec4Lerp(a.pos, b.pos, t);
    unsigned char ar = GETR(a.color);
    unsigned char ag = GETG(a.color);
    unsigned char ab = GETB(a.color);
    unsigned char aa = GETA(a.color);
    unsigned char br = GETR(b.color);
    unsigned char bg = GETG(b.color);
    unsigned char bb = GETB(b.color);
    unsigned char ba = GETA(b.color);
    int ti = (int)(t * 256.f);
    unsigned char rr = (unsigned char)(ar + (((br - ar) * ti) >> 8));
    unsigned char rg = (unsigned char)(ag + (((bg - ag) * ti) >> 8));
    unsigned char rb = (unsigned char)(ab + (((bb - ab) * ti) >> 8));
    unsigned char ra = (unsigned char)(aa + (((ba - aa) * ti) >> 8));
    result.color = RGBA_INT(rr, rg, rb, ra);
    return result;
}

/* Assumes that the table is large enough! If it's not, corrupt stack */
void GammaLUTInit(unsigned char* lut){
    for (int i = 0; i < 256; i++){
        float val = powf((i / 255.f), 1.f / 2.2f);
        lut[i] = (unsigned char)(val * 255.f);
    }
}

static inline NdcValidationResult NdcValidate(float f){
    if (f >= -1.f && f <= 1.f) return NDC_VALID;

    float error = fmaxf(f - 1.f, -1.f - f);
    if (error <= 1e-6f){
        return NDC_PRECISION;
    } else if (error <= 1e-4f){
        return NDC_SUSPICIOUS; 
    } else return NDC_PATHOLOGICAL;
}

/* Assumes input is in domain -1.f to +1.f.
 * Output is guaranteed to be in domain 0.f to screen dimension - 1.f.
 * Depth is not changed. */
static inline void TransformTriNDCFloatScreen_(Tri3* tri){
    assert(tri->v0.x >= -1.f); assert(tri->v0.x <= 1.f);    
    assert(tri->v0.y >= -1.f); assert(tri->v0.y <= 1.f);    
    assert(tri->v0.z >= -1.f); assert(tri->v0.z <= 1.f);    
    assert(tri->v1.x >= -1.f); assert(tri->v1.x <= 1.f);    
    assert(tri->v1.y >= -1.f); assert(tri->v1.y <= 1.f);    
    assert(tri->v1.z >= -1.f); assert(tri->v1.z <= 1.f);    
    assert(tri->v2.x >= -1.f); assert(tri->v2.x <= 1.f);    
    assert(tri->v2.y >= -1.f); assert(tri->v2.y <= 1.f);    
    assert(tri->v2.z >= -1.f); assert(tri->v2.z <= 1.f);    

    int w = renderer.framebuffer.w - 1;
    int h = renderer.framebuffer.h - 1;
    tri->v0.x = (tri->v0.x * 0.5f + 0.5f) * w;
    tri->v1.x = (tri->v1.x * 0.5f + 0.5f) * w;
    tri->v2.x = (tri->v2.x * 0.5f + 0.5f) * w;
    tri->v0.y = (0.5f - tri->v0.y * 0.5f) * h;
    tri->v1.y = (0.5f - tri->v1.y * 0.5f) * h;
    tri->v2.y = (0.5f - tri->v2.y * 0.5f) * h;

    tri->v0.x = fmaxf(0.f, fminf(w, tri->v0.x));
    tri->v1.x = fmaxf(0.f, fminf(w, tri->v1.x));
    tri->v2.x = fmaxf(0.f, fminf(w, tri->v2.x));
    tri->v0.y = fmaxf(0.f, fminf(h, tri->v0.y));
    tri->v1.y = fmaxf(0.f, fminf(h, tri->v1.y));
    tri->v2.y = fmaxf(0.f, fminf(h, tri->v2.y));
}

/* Assumes input has w >= near clip distance, i.e. is not clipping through
 * near plane. That's not the only thing that's necessary, but it's the only
 * thing we can catch easily with an assert.
 * Output is guaranteed to be in the domain -1.f to +1.f.
 * W component is consumed and no longer needed, so it's Vec3 from here until
 * rasterization. */
static inline void TransformTri4ClipNDC_(Tri4* clip, Tri3* ndc, Camera* cam){
    /*
    assert(clip[0].w >= cam->nearClip);
    assert(clip[1].w >= cam->nearClip);
    assert(clip[2].w >= cam->nearClip);
    */

    ndc->v0.x = clip->v0.x / clip->v0.w; 
    ndc->v0.y = clip->v0.y / clip->v0.w; 
    ndc->v0.z = clip->v0.z / clip->v0.w; 
    ndc->v1.x = clip->v1.x / clip->v1.w; 
    ndc->v1.y = clip->v1.y / clip->v1.w; 
    ndc->v1.z = clip->v1.z / clip->v1.w; 
    ndc->v2.x = clip->v2.x / clip->v2.w; 
    ndc->v2.y = clip->v2.y / clip->v2.w; 
    ndc->v2.z = clip->v2.z / clip->v2.w; 

    ndc->v0.x = fmaxf(-1.f, fminf(1.f, ndc->v0.x));
    ndc->v0.y = fmaxf(-1.f, fminf(1.f, ndc->v0.y));
    ndc->v0.z = fmaxf(-1.f, fminf(1.f, ndc->v0.z));
    ndc->v1.x = fmaxf(-1.f, fminf(1.f, ndc->v1.x));
    ndc->v1.y = fmaxf(-1.f, fminf(1.f, ndc->v1.y));
    ndc->v1.z = fmaxf(-1.f, fminf(1.f, ndc->v1.z));
    ndc->v2.x = fmaxf(-1.f, fminf(1.f, ndc->v2.x));
    ndc->v2.y = fmaxf(-1.f, fminf(1.f, ndc->v2.y));
    ndc->v2.z = fmaxf(-1.f, fminf(1.f, ndc->v2.z));
}

static inline void DrawWireframeTri_(Tri3* tri, int c){
    TransformTriNDCFloatScreen_(tri);      
    DrawLineWu_Gamma(tri->v0, tri->v1, c);
    DrawLineWu_Gamma(tri->v1, tri->v2, c);
    DrawLineWu_Gamma(tri->v2, tri->v0, c);
}

void Obj3DDrawWireframe(Camera* cam, Obj3D* obj, int c){
    assert(cam); assert(obj);

    Mat4 matModelView = MatMatMul(&cam->view, &obj->matModel);
    
    for (int i = 0; i < obj->model->plymesh.triCount; i++){
        PLY_Triangle tri = PLYGetTriangle(&obj->model->plymesh, i); 
        /* Convert pos to Vec4 for matrix multiplication.
         * Rest of per-vertex attributes are not needed for wireframe. */
        Vec4 v0 = Vec4Make(tri.v0.pos.x, tri.v0.pos.y, tri.v0.pos.z, 1.f);
        Vec4 v1 = Vec4Make(tri.v1.pos.x, tri.v1.pos.y, tri.v1.pos.z, 1.f);
        Vec4 v2 = Vec4Make(tri.v2.pos.x, tri.v2.pos.y, tri.v2.pos.z, 1.f);
        
        /* transform model -> world -> view */
        v0 = MatVertMul(&matModelView, v0);
        v1 = MatVertMul(&matModelView, v1);
        v2 = MatVertMul(&matModelView, v2);

        /* calculate tri normal, faster than transforming the existing normals
         * stored in the model, and you only need one normal for backface
         * culling and flat shading. For gouraud/phong, transform the
         * per-vertex normals */
        Vec3 side0 = Vec3Make(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
        Vec3 side1 = Vec3Make(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
        Vec3 normal = Vec3Norm(Vec3Cross(side0, side1));
        float nDotCam = Vec3Dot(normal, Vec3Norm(Vec3Make(-v0.x, -v0.y, -v0.z)));
        
        /* reject tris facing away from camera */
        /* TODO: move this epsilon into a macro somewhere */
        if (renderer.enableCulling)
            if (nDotCam <= -0.001f) continue;
        /* transform into clip space */
        v0 = MatVertMul(&cam->proj, v0);
        v1 = MatVertMul(&cam->proj, v1);
        v2 = MatVertMul(&cam->proj, v2);
 
        /* clip the triangle against the view frustum */
        NGon result = TriClip_(v0, v1, v2);
        /* triangulate */
        Tri4Cluster cluster = Triangulate_(result);

        for (int j = 0; j < cluster.count; j++) {
            Tri3 ndc;
             /* transform from clip -> NDC */
            TransformTri4ClipNDC_(&cluster.tris[j], &ndc, cam);

            DrawWireframeTri_(&ndc, c);
        }
    }
}

/* input triangle must be in NDC */
static inline void DrawSolidColorTri_(Tri3* tri, int c){
    TransformTriNDCFloatScreen_(tri);      
    int minX = (int)floorf  (fminf(fminf(tri->v0.x, tri->v1.x), tri->v2.x));
    int maxX = (int)ceilf   (fmaxf(fmaxf(tri->v0.x, tri->v1.x), tri->v2.x));
    int minY = (int)floorf  (fminf(fminf(tri->v0.y, tri->v1.y), tri->v2.y));
    int maxY = (int)ceilf   (fmaxf(fmaxf(tri->v0.y, tri->v1.y), tri->v2.y));

    float denom = ((tri->v1.y - tri->v2.y) * (tri->v0.x - tri->v2.x) +
                   (tri->v2.x - tri->v1.x) * (tri->v0.y - tri->v2.y));
    if (denom == 0.f) return; /* Degenerate triangle */

    float invDen = 1.f / denom;
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            /* Barycentric weights (affine) */
            float l0 = ((tri->v1.y - tri->v2.y) * (x + 0.5f - tri->v2.x) +
                        (tri->v2.x - tri->v1.x) * (y + 0.5f - tri->v2.y)) * invDen;
            float l1 = ((tri->v2.y - tri->v0.y) * (x + 0.5f - tri->v2.x) +
                        (tri->v0.x - tri->v2.x) * (y + 0.5f - tri->v2.y)) * invDen;
            float l2 = 1.f - l0 - l1;

            /* Inside test (all weights in [0,1]) */
            if (l0 < -1e-6f || l1 < -1e-6f || l2 < -1e-6f) continue;

            float depth = l0 * tri->v0.z + l1 * tri->v1.z + l2 * tri->v2.z;

            if (!DepthBufferTestWrite(&renderer.db, x, y, depth)) continue;

            PutPixel(x, y, c);
        }
    }
}

void Obj3DDrawVertexColor(Camera* cam, Obj3D* obj){
    assert(cam); assert(obj);

    Mat4 matModelView = MatMatMul(&cam->view, &obj->matModel);
    
    for (int i = 0; i < obj->model->plymesh.triCount; i++){
        PLY_Triangle tri = PLYGetTriangle(&obj->model->plymesh, i); 
        /* Convert pos to Vec4 for matrix multiplication.
         * Get Vertex colors, which are the only per-vertex attrib we need */
        Vec4 v0 = Vec4Make(tri.v0.pos.x, tri.v0.pos.y, tri.v0.pos.z, 1.f);
        Vec4 v1 = Vec4Make(tri.v1.pos.x, tri.v1.pos.y, tri.v1.pos.z, 1.f);
        Vec4 v2 = Vec4Make(tri.v2.pos.x, tri.v2.pos.y, tri.v2.pos.z, 1.f);
        int c0 = tri.v0.color; int c1 = tri.v1.color; int c2 = tri.v2.color;
        
        /* transform model -> world -> view */
        v0 = MatVertMul(&matModelView, v0);
        v1 = MatVertMul(&matModelView, v1);
        v2 = MatVertMul(&matModelView, v2);

        /* calculate tri normal, faster than transforming the existing normals
         * stored in the model, and you only need one normal for backface
         * culling and flat shading. For gouraud/phong, transform the
         * per-vertex normals */
        Vec3 side0 = Vec3Make(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
        Vec3 side1 = Vec3Make(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);
        Vec3 normal = Vec3Norm(Vec3Cross(side0, side1));
        float nDotCam = Vec3Dot(normal, Vec3Norm(Vec3Make(-v0.x, -v0.y, -v0.z)));
        
        /* reject tris facing away from camera */
        /* TODO: move this epsilon into a macro somewhere */
        if (renderer.enableCulling)
            if (nDotCam <= -0.001f) continue;
        /* transform into clip space */
        v0 = MatVertMul(&cam->proj, v0);
        v1 = MatVertMul(&cam->proj, v1);
        v2 = MatVertMul(&cam->proj, v2);
 
        /* clip the triangle against the view frustum */
        /* uses  a version of clipping code specifically for vertex coloring */
        VertexColorVert vert0 = { v0, c0 };
        VertexColorVert vert1 = { v1, c1 };
        VertexColorVert vert2 = { v2, c2 };
        VertexColorVert verts[3] = { vert0, vert1, vert2 };
        VertexColorNGon result = VertexColorTriClip_(&verts[0]);
        /* triangulate */
        VertexColorTriCluster cluster = VertexColorTriangulate_(result);

        for (int j = 0; j < cluster.count; j++) {
            Tri3 ndc;
             /* transform from clip -> NDC */
            Tri4 pos4;
            int colors[3];
            pos4.v0 = cluster.tris[j].v0.pos;
            colors[0] = cluster.tris[j].v0.color;
            pos4.v1 = cluster.tris[j].v1.pos;
            colors[1] = cluster.tris[j].v1.color;
            pos4.v2 = cluster.tris[j].v2.pos;
            colors[2] = cluster.tris[j].v2.color;
            TransformTri4ClipNDC_(&pos4, &ndc, cam);

            DrawVertexColorTri_(&ndc, &colors[0]);
        }
    }
}

static inline void DrawVertexColorTri_(Tri3* tri, int c[3]){
    TransformTriNDCFloatScreen_(tri);      
    int minX = (int)floorf  (fminf(fminf(tri->v0.x, tri->v1.x), tri->v2.x));
    int maxX = (int)ceilf   (fmaxf(fmaxf(tri->v0.x, tri->v1.x), tri->v2.x));
    int minY = (int)floorf  (fminf(fminf(tri->v0.y, tri->v1.y), tri->v2.y));
    int maxY = (int)ceilf   (fmaxf(fmaxf(tri->v0.y, tri->v1.y), tri->v2.y));

    float denom = ((tri->v1.y - tri->v2.y) * (tri->v0.x - tri->v2.x) +
                   (tri->v2.x - tri->v1.x) * (tri->v0.y - tri->v2.y));
    if (denom == 0.f) return; /* Degenerate triangle */

    float invDen = 1.f / denom;

    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            /* Barycentric weights (affine) */
            float l0 = ((tri->v1.y - tri->v2.y) * (x + 0.5f - tri->v2.x) +
                        (tri->v2.x - tri->v1.x) * (y + 0.5f - tri->v2.y)) * invDen;
            float l1 = ((tri->v2.y - tri->v0.y) * (x + 0.5f - tri->v2.x) +
                        (tri->v0.x - tri->v2.x) * (y + 0.5f - tri->v2.y)) * invDen;
            float l2 = 1.f - l0 - l1;

            /* Inside test (all weights in [0,1]) */
            if (l0 < -1e-6f || l1 < -1e-6f || l2 < -1e-6f) continue;

            float depth = l0 * tri->v0.z + l1 * tri->v1.z + l2 * tri->v2.z;

            if (!DepthBufferTestWrite(&renderer.db, x, y, depth)) continue;

            /* interpolate vertex color */
            float r0 = (float)GETR(c[0]);
            float g0 = (float)GETG(c[0]);
            float b0 = (float)GETB(c[0]);
            float a0 = (float)GETA(c[0]);
            float r1 = (float)GETR(c[1]);
            float g1 = (float)GETG(c[1]);
            float b1 = (float)GETB(c[1]);
            float a1 = (float)GETA(c[1]);
            float r2 = (float)GETR(c[2]);
            float g2 = (float)GETG(c[2]);
            float b2 = (float)GETB(c[2]);
            float a2 = (float)GETA(c[2]);
            unsigned char rr = (unsigned char)(l0 * r0 + l1 * r1 + l2 * r2);
            unsigned char rg = (unsigned char)(l0 * g0 + l1 * g1 + l2 * g2);
            unsigned char rb = (unsigned char)(l0 * b0 + l1 * b1 + l2 * b2);
            unsigned char ra = (unsigned char)(l0 * a0 + l1 * a1 + l2 * a2);
            int color = RGBA_INT(rr, rg, rb, ra);

            PutPixel(x, y, color);
        }
    }
}
