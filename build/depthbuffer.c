#include "stdlib.h"
#include "stdio.h"
#include "depthbuffer.h"

int DepthBufferInit(DepthBuffer* db, int w, int h){
    if (!db){
        /* TODO */
        printf("can't pass null to DepthBufferInit\n");
        return 1;
    }
    if (db->init){
        /* TODO: some sort of warning or something, print statement is temp */
        printf("What a buffoon! Init things once, not twice. \n");
        return 1;
    }
    if (!(w > 0 && w < MAX_W && h > 0 && h < MAX_H)){
        /* TODO: probably just exit with error */
        printf("DepthBuffer size invalid. W: %d, H: %d\n", db->w, db->h);
        return 1;
    }
    db->w = w; db->h = h; db->sampleCount = h * w;
    db->buf = malloc(sizeof(float) * db->w * db->h);
    if (!db->buf){
        /* TODO: probably just exit with error */
        printf("Failed to allocate depthbuffer\n");
        return 1;
    }
    db->init = 1;
    return 0;
}

int DepthBufferFree(DepthBuffer* db){
    if (!db){
        /* TODO */
        printf("can't pass null to DepthBufferFree\n");
        return 1;
    }
    if (!db->buf){
        /* TODO: A warning or exit or something */
        printf("Tried to free depthbuffer.buf, but it was already null.\n");
        printf("You probably freed it manually and then called \
                DepthBufferFree().\n");
        return 1;
    }
    if (!db->init){
        /* A warning or exit or seomthing */
        printf("Tried to free depthbuffer, but it was never properly \
                initialized.\n");
        printf("You need to initialize it with DepthBufferInit().\n");
        return 1;
    }
    free(db->buf);
    db->buf = 0;
    db->init = 0;
    db->w = db->h = db->sampleCount = 0;
    return 0;
}

void DepthBufferClear(DepthBuffer* db, float val){
    if (!db || !db->init || !db->buf){
        /*TODO: error handling or something */
        printf("DepthBuffer not ready to use\n");
        return;
    }
    for (int i = 0; i < db->sampleCount; i++){
        db->buf[i] = val;
    }
}

int DepthBufferTestWrite(DepthBuffer* db, int x, int y, float z){
    float* address = db->buf + y * db->w + x;
    if (z < *address){
        *address = z;
        return 1;
    } else return 0;
}
