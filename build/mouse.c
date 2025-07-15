#include "mouse.h"

int* pickbuf = 0;
static int width, height, displayScale;

int InitPickbuf(int w, int h, int scale){
    pickbuf = malloc(sizeof(int) * w * h);
    if (!pickbuf) return 1;
    width = w, height = h, displayScale = scale;
    return 0;
}

void ClearPickbuf(){
    if (!pickbuf) return;
    memset(pickbuf, 0, sizeof(int) * width * height);
}

void FreePickbuf(){
    if (!pickbuf) return;
    free(pickbuf);
    pickbuf = 0;
    width = height = 0;
}

void UpdatePickbuf(int x, int y, int id){
    /* no safety, in hot path */
    /* caller will guarantee coords are valid framebuffer dimensions anyway */
    pickbuf[y * width + x] = id;
}

/* This isn't called that much and so can assume some error handling
 * responsibility, but I don't feel like implementing it right now. */
int GetClicked(int mouseX, int mouseY){
    if (!pickbuf) return -1;
    return pickbuf[(mouseY / displayScale) * width + (mouseX / displayScale)];
}
