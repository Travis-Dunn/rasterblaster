#ifndef DEPTHBUFFER_H
#define DEPTHBUFFER_H

typedef struct {
    int init;
    int w, h;
    int sampleCount;
    float* buf;
} DepthBuffer;

int     DepthBufferInit             (DepthBuffer* db, int w, int h);
int     DepthBufferFree             (DepthBuffer* db);
void    DepthBufferClear            (DepthBuffer* db, float val);
int     DepthBufferTestWrite        (DepthBuffer* db, int x, int y, float z);
int     DepthBufferTestWriteDebug   (DepthBuffer* db, int x, int y, float z,
                                     float* out);

#define MAX_W 3840
#define MAX_H 2160

#endif /* DEPTHBUFFER_H */
