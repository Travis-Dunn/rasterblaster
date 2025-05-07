#include "demo.h"
#include "renderer.h"
#include "math.h"

static int currentLineIndex = 0;
static float timeAcc = 0.f;
static float lineDisplayTime = 0.1f;
static int totalLines = 256;

void Init(){
    InitTimer(1024);
}

void Render(){
    int c = RGBA_INT(212, 44, 162, 255);
    int centerX = 80;
    int centerY = 50;
    int targetX, targetY, i;
    float angle;

    ClearScreen(22);

    angle = (float)currentLineIndex / (float)totalLines * 6.28318f;
    targetX = (int)(centerX + 48.f * cos(angle));
    targetY = (int)(centerY + 48.f * sin(angle));
    DrawLine(centerX, centerY, targetX, targetY, c);
}

void Update(){
    UpdateTimer();
    timeAcc += timer.dt;
    if (timeAcc >= lineDisplayTime){
        timeAcc -= lineDisplayTime;
        currentLineIndex = (currentLineIndex + 1) % totalLines;
    }
}
