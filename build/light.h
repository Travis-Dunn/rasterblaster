#ifndef LIGHT_H
#define LIGHT_H

#include "arithmetic.h"

typedef enum {
    LIGHT_AMBIENT = 0,
    LIGHT_DIRECTIONAL = 1,
    LIGHT_POINT = 2
} LightType;

typedef struct {
    LightType type;
    int rgb;
    Vec3 pos;
    Vec3 dir;
} Light;

#endif /* LIGHT_H */
