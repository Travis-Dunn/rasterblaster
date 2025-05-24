#include "stdlib.h"
#include "string.h"
#include "renderer.h"
#include "stdio.h"

Framebuffer framebuffer = {0};
Renderer renderer = {0};
float* depthbuffer = 0;

int InitDepthBuffer(){
    depthbuffer = (float*)malloc(sizeof(float) * renderer.framebuffer.w *
                                                 renderer.framebuffer.h);
    if (depthbuffer) return 1;
    return 0;
}

int UpdateDepthBuffer(int screenX, int screenY, float depth){
    float* address = depthbuffer + screenY * renderer.framebuffer.w + screenX;
    if (depth < *address){
        *address = depth;
        return 1;
    } else return 0;
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
        y >= renderer.framebuffer.h || y < 0) return;
    PutPixel_(x, y, c);
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

void ClearDepthBuffer(){
    int numPixels = renderer.framebuffer.w * renderer.framebuffer.h;
    int i;
    for (i = 0; i < numPixels; i++){
        depthbuffer[i] = 1.f;
    }
}

void TexturedTri(Texture* t, int x0, int y0, float z0, float u0, float v0,
               int x1, int y1, float z1, float u1, float v1,
               int x2, int y2, float z2, float u2, float v2)
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
            
            if (!UpdateDepthBuffer(x, y, depth)) continue;
            

            /* 4. Interpolate UV */
            float u = l0 * u0 + l1 * u1 + l2 * u2;
            float v = l0 * v0 + l1 * v1 + l2 * v2;

            /* 5. Sample & draw */
            int texel = SampleTex(t, u, v);
            PutPixel(x, y, texel);
        }
    }
}

void DrawModelLambert(Camera* cam, Model* model, Framebuffer* fb, Light* l,
        int nLights, Mat4 modelMatrix){
    int numTris = model->mesh->indexCount / 9;

    int i;
    for (i = 0; i < numTris; i++) {
        /* get indices in format pos/pos/pos/tex/tex/tex/normal/normal/normal */
        int i0, i1, i2, i3, i4, i5, i6, i7, i8;
        GetTriIndices(model->mesh, i, &i0, &i1, &i2, &i3, &i4, &i5, &i6, &i7,
                &i8);

        /* use indices to get data */ 
        Vec4 v0, v1, v2, n0, n1, n2;
        float tu0, tv0, tu1, tv1, tu2, tv2;

        GetVertex(model->mesh, i0, i3, i6, &v0, &tu0, &tv0, &n0);
        GetVertex(model->mesh, i1, i4, i7, &v1, &tu1, &tv1, &n1);
        GetVertex(model->mesh, i2, i5, i8, &v2, &tu2, &tv2, &n2);
        
        /* set up ints for the screen space triangle coordinates */
        int sx0, sy0, sx1, sy1, sx2, sy2;

        /* model -> world */
        v0 = MatVertMul(&modelMatrix, v0);
        v1 = MatVertMul(&modelMatrix, v1);
        v2 = MatVertMul(&modelMatrix, v2);

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
        /*
        float similarity = Vec3Dot(Vec3Norm(normal), cam->inverseDir);
        */

        /* use normal for calculating net lighting (lambert) */
        /* to be implemented... */

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
        sy0 = (int)((v0.y * 0.5f + 0.5f) * fb->h);
        sx1 = (int)((v1.x * 0.5f + 0.5f) * fb->w);
        sy1 = (int)((v1.y * 0.5f + 0.5f) * fb->h);
        sx2 = (int)((v2.x * 0.5f + 0.5f) * fb->w);
        sy2 = (int)((v2.y * 0.5f + 0.5f) * fb->h);

        TexturedLambertTri_(model->tex, l, nLights, sx0, sy0, v0.z, tu0, tv0,
                            sx1, sy1, v1.z, tu1, tv1,
                            sx2, sy2, v2.z, tu2, tv2);
    }
}

static inline void TexturedLambertTri_(Texture* t, Light* l, int nLights,
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
    }
    rAcc = rAcc > 255 ? 255 : rAcc;
    gAcc = gAcc > 255 ? 255 : gAcc;
    bAcc = bAcc > 255 ? 255 : bAcc;

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
            
            if (!UpdateDepthBuffer(x, y, depth)) continue;
            

            /* 4. Interpolate UV */
            float u = l0 * u0 + l1 * u1 + l2 * u2;
            float v = l0 * v0 + l1 * v1 + l2 * v2;

            /* 5. Sample & draw */
            int texel = SampleTex(t, u, v);

            float tR = GETR(texel) / 255.f;
            float tG = GETG(texel) / 255.f;
            float tB = GETB(texel) / 255.f;
            float lR = rAcc / 255.f;
            float lG = gAcc / 255.f;
            float lB = bAcc / 255.f;
            unsigned char fR = (unsigned char)((tR * lR) * 255);
            unsigned char fG = (unsigned char)((tG * lG) * 255);
            unsigned char fB = (unsigned char)((tB * lB) * 255);

            PutPixel(x, y, RGBA_INT(fR, fG, fB, 255));
        }
    }
}


