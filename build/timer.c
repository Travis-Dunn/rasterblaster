#include "timer.h"

#define N_FPS_SAMPLES_DEFAULT 0xFF
#define MIN_FRAME_TIME 1e-7

Timer_t timer = {0};

void InitTimer(unsigned fpsSampleCount){
    LARGE_INTEGER frequency_LI, lastFrameTime_LI;
    QueryPerformanceFrequency(&frequency_LI);
    QueryPerformanceCounter(&lastFrameTime_LI);
    timer.frequency = (float)frequency_LI.QuadPart;
    /* My intent here is to guard against potential division by zero during
     * UpdateTimer, but I'm not sure if this is the best way to do it.
     * Depends on what sort of values QueryPerformanceFrequency can return. */
    if (timer.frequency < 1e-10){
        /* TODO: Better error handling */
        printf("Error initializing timer, frequency too small");
        printf("framerate values are invalid");
    }
    /* This also should be checked for !0, but I don't know the best approach */
    timer.lastFrameTime = (float)lastFrameTime_LI.QuadPart;
    timer.fpsSampleCount = fpsSampleCount;
    if (!(fpsSampleCount > 0 && fpsSampleCount < 0xFFFF)){
        timer.fpsSampleCount = N_FPS_SAMPLES_DEFAULT;
        printf("Error initializing timer, fps sample count too large");
        printf("Fps sample count set to default (%d)\n", timer.fpsSampleCount);
    }
    timer.nFpsSamples = 0;
    timer.dt = 0.f;
    timer.et = 0.f;
    timer.fpsInstant = 0.f;
    timer.fpsAvg = 0.f;
    timer.init = 1;
}

void UpdateTimer(){
    LARGE_INTEGER currentTime_LI;
    if (!timer.init){
        /* TODO: Better error handling */
        printf("Timer not initialized!");
        return;
    }
    QueryPerformanceCounter(&currentTime_LI);
    float currentTime = (float)currentTime_LI.QuadPart;
    timer.dt = (currentTime - timer.lastFrameTime) / timer.frequency;
    if (timer.dt < MIN_FRAME_TIME) timer.dt = MIN_FRAME_TIME;
    timer.et += timer.dt;
    timer.lastFrameTime = currentTime;
    timer.fpsInstant = 1 / timer.dt;
    timer.fpsAvg = ((timer.fpsAvg * timer.nFpsSamples) + timer.fpsInstant) /
        (timer.nFpsSamples + 1);
    if (timer.nFpsSamples < timer.fpsSampleCount) timer.nFpsSamples++;
}
