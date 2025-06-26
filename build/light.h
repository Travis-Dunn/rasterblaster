#ifndef LIGHT_H
#define LIGHT_H

#include "arithmetic.h"

typedef enum {
    LIGHT_AMBIENT = 0,
    LIGHT_DIRECTIONAL = 1,
    LIGHT_POINT = 2,
    LIGHT_SPOT = 3
} LightType;

typedef struct {
    LightType type;
    int rgb;
    Vec3 pos;
    Vec3 dir;
    Vec3 inverseDir;
} Light;

Light   MakeAmbient     (unsigned char r, unsigned char g, unsigned char b);
Light   MakeDirectional (unsigned char r, unsigned char g, unsigned char b,
                         Vec3 dir);

#endif /* LIGHT_H */
