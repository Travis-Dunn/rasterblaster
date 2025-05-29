#ifndef DEPTHBUFFER_H
#define DEPTHBUFFER_H

typedef struct {
    int init;
    int w, h;
    int sampleCount;
    float* buf;
} Depthbuffer;

int               DepthbufferInit     (Depthbuffer* db, int w, int h);
int               DepthbufferFree     (Depthbuffer* db);
void              DepthbufferClear    (Depthbuffer* db, float val);
int               DepthbufferTestWrite(Depthbuffer* db, int x, int y, float z);

#define MAX_W 3840
#define MAX_H 2160

#endif /* DEPTHBUFFER_H */
