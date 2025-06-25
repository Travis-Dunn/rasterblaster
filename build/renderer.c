#include "math.h"
#include "stdlib.h"
#include "string.h"
#include "renderer.h"
#include "stdio.h"
#include "mouse.h"

Framebuffer framebuffer = {0};
Renderer renderer = {0};
float* depthbuffer = 0;

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
        y >= renderer.framebuffer.h || y < 0) return;
 
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
        /*
        printf("degen, coords: (%d, %d), (%d, %d), (%d, %d)\n", x0, y0, x1, y1, x2, y2);
        */
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
            if (tx < 0 || ty < 0 || tx >= sm->w || ty >= sm->h){
                printf("Tried to sample outside of shadow map bounds\n");
            }
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
    /*
    int minX = (ix0 < ix1 ? (ix0 < ix2 ? ix0 : ix2) : (ix1 < ix2 ? ix1 : ix2));
    int minY = (iy0 < iy1 ? (iy0 < iy2 ? iy0 : iy2) : (iy1 < iy2 ? iy1 : iy2));
    int maxX = (ix0 > ix1 ? (ix0 > ix2 ? ix0 : ix2) : (ix1 > ix2 ? ix1 : ix2));
    int maxY = (iy0 > iy1 ? (iy0 > iy2 ? iy0 : iy2) : (iy1 > iy2 ? iy1 : iy2));
    */
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
        /*
        printf("degen, screen coords: (%f.1, %f.1), (%f.1, %f.1), (%f.1, %f.1)\n", fx0, fy0, fx1, fy1, fx2, fy2);
        */
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
            /*
            if (tx < 0 || ty < 0 || tx >= sm->w || ty >= sm->h){
                printf("Tried to sample outside of shadow map bounds\n");
                printf("sh0: (%.2f, %.2f, %.2f)\n", sh0.x, sh0.y, sh0.z);

                printf("sh1: (%.2f, %.2f, %.2f)\n", sh1.x, sh1.y, sh1.z);

                printf("sh2: (%.2f, %.2f, %.2f)\n", sh2.x, sh2.y, sh2.z);
                puts("paused\n");
                getchar();
            }
            */
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

        /* NDC -> screen */
        /*
        sx0 = (int)((v0.x * 0.5f + 0.5f) * fb->w);
        sy0 = (int)((0.5f - v0.y * 0.5f) * fb->h);
        sx1 = (int)((v1.x * 0.5f + 0.5f) * fb->w);
        sy1 = (int)((0.5f - v1.y * 0.5f) * fb->h);
        sx2 = (int)((v2.x * 0.5f + 0.5f) * fb->w);
        sy2 = (int)((0.5f - v2.y * 0.5f) * fb->h);
        */

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

void DrawObj3DLambertShadowFloatClip(Camera* cam, Obj3D* obj, Framebuffer* fb,
        Light* l, int nLights, DepthBuffer* db, ShadowMapper* sm){
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
        Vec3 normalizedNormal = Vec3Norm(normal);
        float nDotLos = Vec3Dot(Vec3Norm(invLos), normalizedNormal);
        
        /* reject tris facing away from camera */
        if (!(nDotLos > -0.001f)) continue;

        /* clipping */
        Vec4 normalizedNormal4 = Vec4Make(normalizedNormal.x,
                normalizedNormal.y, normalizedNormal.z, 0.f);
        ClipVertex clipVerts[3];
        clipVerts[0].pos = v0;
        clipVerts[0].u = tu0;
        clipVerts[0].v = tv0;
        clipVerts[0].normal = normalizedNormal4;
        clipVerts[0].shadow = vs0;
        clipVerts[1].pos = v1;
        clipVerts[1].u = tu1;
        clipVerts[1].v = tv1;
        clipVerts[1].normal = normalizedNormal4;
        clipVerts[1].shadow = vs1;
        clipVerts[2].pos = v2;
        clipVerts[2].u = tu2;
        clipVerts[2].v = tv2;
        clipVerts[2].normal = normalizedNormal4;
        clipVerts[2].shadow = vs2;
        
        ClipResult clipped = ClipTri_(clipVerts, &cam->viewFrustum);

        for (int j = 0; j < clipped.numTris; j++){
            Vec4 cv0 = clipped.tris[j][0].pos;
            Vec4 cv1 = clipped.tris[j][1].pos;
            Vec4 cv2 = clipped.tris[j][2].pos;
            float ctu0 = clipped.tris[j][0].u;
            float ctv0 = clipped.tris[j][0].v;
            float ctu1 = clipped.tris[j][1].u;
            float ctv1 = clipped.tris[j][1].v;
            float ctu2 = clipped.tris[j][2].u;
            float ctv2 = clipped.tris[j][2].v;
            Vec4 cs0 = clipped.tris[j][0].shadow;
            Vec4 cs1 = clipped.tris[j][1].shadow;
            Vec4 cs2 = clipped.tris[j][2].shadow;
            /* flat shading, all normals are the same */
            Vec4 cn = clipped.tris[j][0].normal;
            Vec3 cn3 = Vec3Make(cn.x, cn.y, cn.z);

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
                    float s = Vec3Dot(cn3, Vec3Norm(l[i].inverseDir));
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
            cv0 = MatVertMul(&cam->proj, cv0);
            cv1 = MatVertMul(&cam->proj, cv1);
            cv2 = MatVertMul(&cam->proj, cv2);
            cs0 = MatVertMul(&sm->matTransformProj, cs0);
            cs1 = MatVertMul(&sm->matTransformProj, cs1);
            cs2 = MatVertMul(&sm->matTransformProj, cs2);
            /* clip -> NDC (clipping not yet implemented) */
            cv0.x /= cv0.w;
            cv0.y /= cv0.w;
            cv0.z /= cv0.w;
            cv1.x /= cv1.w;
            cv1.y /= cv1.w;
            cv1.z /= cv1.w;
            cv2.x /= cv2.w;
            cv2.y /= cv2.w;
            cv2.z /= cv2.w;

            /*
            if (cv0.x > 1.f || cv0.x < -1.f ||
                cv0.y > 1.f || cv0.y < -1.f ||
                cv0.z > 1.f || cv0.z < -1.f ||
                cv1.x > 1.f || cv1.x < -1.f ||
                cv1.y > 1.f || cv1.y < -1.f ||
                cv1.z > 1.f || cv1.z < -1.f ||
                cv2.x > 1.f || cv2.x < -1.f ||
                cv2.y > 1.f || cv2.y < -1.f ||
                cv2.z > 1.f || cv2.z < -1.f) {
                return;
                printf("some verts were out of -1 to +1\n");
                printf("%.2f, %.2f, %.2f\n"
                        "%.2f, %.2f, %.2f\n"
                        "%.2f, %.2f, %.2f\n", cv0.x, cv0.y, cv0.z, cv1.x, cv1.y,
                        cv1.z, cv2.x, cv2.y, cv2.z);
                puts("paused\n");
                getchar();
            }
            */

            
            cs0.x /= cs0.w;
            cs0.y /= cs0.w;
            cs0.z /= cs0.w;
            cs1.x /= cs1.w;
            cs1.y /= cs1.w;
            cs1.z /= cs1.w;
            cs2.x /= cs2.w;
            cs2.y /= cs2.w;
            cs2.z /= cs2.w;
            /* convert shadow verts to screen space, or perhaps not? */
            cs0.x = (cs0.x * 0.5f + 0.5f);
            cs0.y = (0.5f - cs0.y * 0.5f);
            cs1.x = (cs1.x * 0.5f + 0.5f);
            cs1.y = (0.5f - cs1.y * 0.5f);
            cs2.x = (cs2.x * 0.5f + 0.5f);
            cs2.y = (0.5f - cs2.y * 0.5f);
            cs0.z = cs0.z * 0.5f + 0.5f;
            cs1.z = cs1.z * 0.5f + 0.5f;
            cs2.z = cs2.z * 0.5f + 0.5f;

            TexturedLambertShadowFloatTri_(obj->model->tex, la, obj->id, db, ld, cv0.x, cv0.y, cv0.z,
                ctu0, ctv0, cv1.x, cv1.y, cv1.z, ctu1, ctv1, cv2.x, cv2.y, cv2.z, ctu2, ctv2
                , sm, cs0, cs1, cs2);
        }
    }
}

/* if we have a problem, try calling the non-underscore versions */
static inline float ClipLine_(Vec3 p1, Vec3 p2, Plane plane, Vec3* out){
    float d1 = SIGNED_DIST_POINT_PLANE(p1, plane);
    float d2 = SIGNED_DIST_POINT_PLANE(p2, plane);
    if (d1 * d2 >= 0.f) return -1.f;
    float t = d1 / (d1 - d2);
    *out = Vec3Add(p1, Vec3Scale(Vec3Sub(p2, p1), t));
    return t;
}

static inline ClipVertex InterpolateVertex_(ClipVertex v1, ClipVertex v2,
        float t){
    ClipVertex result;
    result.pos = Vec4Add(v1.pos, Vec4Scale(Vec4Sub(v2.pos, v1.pos), t));
    result.u = v1.u + (v2.u - v1.u) * t;
    result.v = v1.v + (v2.v - v1.v) * t;
    result.normal = Vec4Add(v1.normal, Vec4Scale(Vec4Sub(v2.normal, v1.normal),
                t));
    result.shadow = Vec4Add(v1.shadow, Vec4Scale(Vec4Sub(v2.shadow, v1.shadow),
                t));
    return result;
}

static inline int ClipTriPlane_(ClipVertex in[3], Plane plane,
        ClipVertex out[4]){
    int outputCount = 0;
    ClipVertex current, previous;
    previous = in[2];
    for (int i = 0; i < 3; i++){
        current = in[i];
        Vec3 currentPos = Vec3Make(current.pos.x, current.pos.y, current.pos.z);
        Vec3 previousPos = Vec3Make(previous.pos.x, previous.pos.y,
                previous.pos.z);
        float currentDist = SIGNED_DIST_POINT_PLANE(currentPos, plane);
        float previousDist = SIGNED_DIST_POINT_PLANE(previousPos, plane);
        if (currentDist <= 0.f){
            if (previousDist > 0.f){
                Vec3 intersection;
                float t = ClipLine_(previousPos, currentPos, plane,
                        &intersection);
                if (t >= 0.f) out[outputCount++] =
                    InterpolateVertex_(previous, current, t);
            }
            out[outputCount++] = current;
        } else if (previousDist <= 0.f){
            Vec3 intersection;
            float t = ClipLine_(previousPos, currentPos, plane, &intersection);
            if (t >= 0.f) out[outputCount++] =
                InterpolateVertex_(previous, current, t);
        }
        previous = current;
    }
    return outputCount;
}


static inline ClipResult ClipTri_(ClipVertex tri[3], Frustum* frustum){
    ClipResult result;
    ClipVertex buffer1[8], buffer2[8];
    ClipVertex* in = tri;
    ClipVertex* out = buffer1;
    int vertCount = 3;
    Plane planes[6] = {frustum->nearPlane, frustum->farPlane,
        frustum->leftPlane, frustum->rightPlane, frustum->topPlane,
        frustum->bottomPlane};
    for (int p = 0; p < 6; p++){
        if (!vertCount) break;
        int newCount = ClipTriPlane_(in, planes[p], out);
        vertCount = newCount;
        ClipVertex* temp = in;
        in = out;
        out = (out == buffer1)? buffer2 : buffer1;
    }
    if (vertCount >= 3){
        result.numTris = vertCount - 2;
        if (result.numTris > 2) result.numTris = 2;
        for (int i = 0; i < result.numTris; i++){
            result.tris[i][0] = in[0];
            result.tris[i][1] = in[i + 1];
            result.tris[i][2] = in[i + 2];
        }
    }
    return result;
}

void Obj3DDrawWireframe(Camera* cam, Obj3D* obj, Framebuffer* fb,
        DepthBuffer* db){
    assert(cam);
    assert(obj);
    assert(fb);
    assert(db);

    int triCount = obj->model->mesh->indexCount / 9;

    Mat4 mModelView = MatMatMul(&cam->view, &obj->matModel);
    
    for (int i = 0; i < triCount; i++){
        /* get indices in format pos/pos/pos/tex/tex/tex/normal/normal/normal */
        /* we are pulling out stuff that we don't need, but that's just because
         * I don't feel like making a version of GetTriIndices and GetVertex 
         * that only get the positions, because it would only be used for 
         * wireframe/debugging */
        int i0, i1, i2, i3, i4, i5, i6, i7, i8;
        GetTriIndices(obj->model->mesh, i, &i0, &i1, &i2, &i3, &i4, &i5, &i6,
                &i7, &i8);

        /* use indices to get data */ 
        Vec4 v0, v1, v2, n0, n1, n2;
        float tu0, tv0, tu1, tv1, tu2, tv2;

        GetVertex(obj->model->mesh, i0, i3, i6, &v0, &tu0, &tv0, &n0);
        GetVertex(obj->model->mesh, i1, i4, i7, &v1, &tu1, &tv1, &n1);
        GetVertex(obj->model->mesh, i2, i5, i8, &v2, &tu2, &tv2, &n2);
        
        /* model -> world -> view */
        /*
        v0 = MatVertMul(&mModelView, v0);
        v1 = MatVertMul(&mModelView, v1);
        v2 = MatVertMul(&mModelView, v2);
        */
        v0 = MatVertMul(&obj->matModel, v0);
        v1 = MatVertMul(&obj->matModel, v1);
        v2 = MatVertMul(&obj->matModel, v2);
        v0 = MatVertMul(&cam->view, v0);
        v1 = MatVertMul(&cam->view, v1);
        v2 = MatVertMul(&cam->view, v2);

        /* calculate tri normal, faster than transforming the existing normals
         * stored in the model, and you only need one normal for backface
         * culling and flat shading. For gouraud/phong, transform the
         * per-vertex normals */
        Vec3 v0_ = Vec3Make(v0.x, v0.y, v0.z);
        Vec3 v1_ = Vec3Make(v1.x, v1.y, v1.z);
        Vec3 v2_ = Vec3Make(v2.x, v2.y, v2.z);
        Vec3 side0 = Vec3Sub(v1_, v0_);
        Vec3 side1 = Vec3Sub(v2_, v0_);
        Vec3 normal = Vec3Cross(side0, side1);

        /* pretty sure this line is a no-op */
        Vec3 los = Vec3Sub(v0_, Vec3Make(0.f, 0.f, 0.f));
        Vec3 invLos = Vec3Make(-los.x, -los.y, -los.z);
        Vec3 normalizedNormal = Vec3Norm(normal);
        float nDotLos = Vec3Dot(Vec3Norm(invLos), normalizedNormal);
        
        /* reject tris facing away from camera */
        /* TODO: move this epsilon into a macro somewhere */
        if (!(nDotLos > -0.001f)) continue;
        v0 = MatVertMul(&cam->proj, v0);
        v1 = MatVertMul(&cam->proj, v1);
        v2 = MatVertMul(&cam->proj, v2);
 

        /*
        ClipVertex clipVerts[3];
        Vec4 empty = Vec4Make(0.f, 0.f, 0.f, 0.f);
        clipVerts[0].pos = v0;
        clipVerts[0].normal = empty;
        clipVerts[0].shadow = empty;
        clipVerts[0].u = 0.f;
        clipVerts[0].v = 0.f;
        clipVerts[1].pos = v1;
        clipVerts[1].normal = empty;
        clipVerts[1].shadow = empty;
        clipVerts[1].u = 0.f;
        clipVerts[1].v = 0.f;
        clipVerts[2].pos = v2;
        clipVerts[2].normal = empty;
        clipVerts[2].shadow = empty;
        clipVerts[2].u = 0.f;
        clipVerts[2].v = 0.f;

        ClipResult clipResult = ClipTri_(clipVerts, &cam->viewFrustum);
        */

        NGon result = TriClip_(v0, v1, v2);
        TriCluster cluster = Triangulate_(result);

        for (int j = 0; j < cluster.count; j++){
            Vec4 v0 = cluster.tris[j].v0;
            Vec4 v1 = cluster.tris[j].v1;
            Vec4 v2 = cluster.tris[j].v2;
            /* view -> clip */
            /*
            v0 = MatVertMul(&cam->proj, v0);
            v1 = MatVertMul(&cam->proj, v1);
            v2 = MatVertMul(&cam->proj, v2);
            */
    
            v0.x /= v0.w;
            v0.y /= v0.w;
            v0.z /= v0.w;
            v1.x /= v1.w;
            v1.y /= v1.w;
            v1.z /= v1.w;
            v2.x /= v2.w;
            v2.y /= v2.w;
            v2.z /= v2.w;

            /*
            if (v0.x < -1.f || v0.x > 1.f ||
                v0.y < -1.f || v0.y > 1.f ||
                v0.z < -1.f || v0.z > 1.f ||
                v1.x < -1.f || v1.x > 1.f ||
                v1.y < -1.f || v1.y > 1.f ||
                v1.z < -1.f || v1.z > 1.f ||
                v2.x < -1.f || v2.x > 1.f ||
                v2.y < -1.f || v2.y > 1.f ||
                v2.z < -1.f || v2.z > 1.f) continue;
                */




            /*
            assert(v0.x >= -1.f); assert(v0.x <= 1.f);
            assert(v0.y >= -1.f); assert(v0.y <= 1.f);
            assert(v0.z >= -1.f); assert(v0.z <= 1.f);
            assert(v1.x >= -1.f); assert(v1.x <= 1.f);
            assert(v1.y >= -1.f); assert(v1.y <= 1.f);
            assert(v1.z >= -1.f); assert(v1.z <= 1.f);
            assert(v2.x >= -1.f); assert(v2.x <= 1.f);
            assert(v2.y >= -1.f); assert(v2.y <= 1.f);
            assert(v2.z >= -1.f); assert(v2.z <= 1.f);
            */

            Vec3 ndc0 = Vec3Make(v0.x, v0.y, v0.z);
            Vec3 ndc1 = Vec3Make(v1.x, v1.y, v1.z);
            Vec3 ndc2 = Vec3Make(v2.x, v2.y, v2.z);
        
            DrawWireframeTri_(ndc0, ndc1, ndc2, db, fb);
        }
    }
}

static inline void DrawWireframeTri_(Vec3 ndc0, Vec3 ndc1, Vec3 ndc2,
        DepthBuffer* db, Framebuffer* fb){
    /* ndc -> floating point screen space for x and y */
    int w = fb->w;
    int h = fb->h;
    ndc0.x = (ndc0.x * 0.5f + 0.5f) * w;
    ndc0.y = (0.5f - ndc0.y * 0.5f) * h;
    ndc1.x = (ndc1.x * 0.5f + 0.5f) * w;
    ndc1.y = (0.5f - ndc1.y * 0.5f) * h;
    ndc2.x = (ndc2.x * 0.5f + 0.5f) * w;
    ndc2.y = (0.5f - ndc2.y * 0.5f) * h;
    
    /*
    assert(ndc0.x >= 0.f); assert(ndc0.x < w);
    assert(ndc0.y >= 0.f); assert(ndc0.y < h);
    assert(ndc1.x >= 0.f); assert(ndc1.x < w);
    assert(ndc1.y >= 0.f); assert(ndc1.y < h);
    assert(ndc2.x >= 0.f); assert(ndc2.x < w);
    assert(ndc2.y >= 0.f); assert(ndc2.y < h);
    */

    /*
    if (ndc0.z == 0.f || ndc1.z == 0.f || ndc2.z == 0.f){
    printf("z0: %.3f, z1: %.3f, z2: %.3f\n", ndc0.z, ndc1.z, ndc2.z);
    }
    */

    /*
    DrawLineDDA_(ndc0, ndc1, db);
    DrawLineDDA_(ndc1, ndc2, db);
    DrawLineDDA_(ndc2, ndc0, db);
    */
    
    DrawLineWu1_(ndc0, ndc1, db);
    DrawLineWu1_(ndc1, ndc2, db);
    DrawLineWu1_(ndc2, ndc0, db);

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
        /*
        if (d >= 1.f){
            printf("pause\n");
            getchar();
        }
        */
        int depthTest = DepthBufferTestWriteDebug(db, x, y, d, &valInDB);
        /*
        if (!DepthBufferTestWriteDebug(db, x, y, depth0, &valInDB)){
            printf("depth0: %.8f, depth in buffer: %.8f, z_inc: %.8f\n", depth0, valInDB, z_inc);
            continue;
        }
        */
        if (!depthTest){
            /*
            printf("failed, x: %d, y: %d, depth: %.8f, buffer: %.8f\n", x, y, depth0, valInDB);
            */
            continue;
        } else {
            /*
            printf("passed, x: %d, y: %d, depth: %.8f, buffer: %.8f\n", x, y, depth0, valInDB);
            */
        }
        /*
        unsigned char a = (unsigned char)(((float)i / steps) * 255);
        int newc = RGBA_INT(192, 128, 128, a);
        BlendPixel_(x, y, newc);
        */
        PutPixel(x, y, c);
 
        /*
        v0.x += x_inc;
        v0.y += y_inc;
        */
        /*
        depth0 += z_inc;
        */
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
            /*
            printf("x: %d, y0: %d, y1: %d, %.3f, a0: %d, a1: %d\n", x, yFloor,
                    yFloor + 1, y, a0, a1);
                    */
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
            /*
            printf("x0: %d, x1: %d, y: %d, a0: %d, a1: %d\n", xFloor, xFloor + 1,
                   y, a0, a1);
                   */
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

static inline int PointInsidePlane_(Vec4 point, int plane){
    assert(plane >= 0 && plane < 6);
    switch (plane) {
    case 0: return point.x > -point.w; /* left     */
    case 1: return point.x <  point.w; /* right    */
    case 2: return point.y > -point.w; /* bottom   */
    case 3: return point.y <  point.w; /* top      */
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

static inline TriCluster Triangulate_(NGon ngon){
    TriCluster result;
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
