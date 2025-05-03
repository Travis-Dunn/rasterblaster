#ifndef TIMER_H
#define TIMER_H

#include "stdio.h"
#include "windows.h"

typedef struct {
    float dt;               /* time in seconds since last frame */
    float et;               /* time since program start */
    float fpsInstant;       /* 1.f / dt */
    float fpsAvg;           /* fps rolling average over last {nFpsSamples} */
    unsigned nFpsSamples;   /* current number of frame durs being averaged */
    float lastFrameTime;
    float frequency;
    unsigned fpsSampleCount;/* number of frame durs to average */
    int init;
} Timer_t;

extern Timer_t timer;

void InitTimer(unsigned fpsSampleCount);
void UpdateTimer();

#endif /* TIMER_H */
