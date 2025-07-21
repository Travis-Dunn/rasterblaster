#ifndef GROUND_H
#define GROUND_H

#include "plyfile.h"

typedef struct {
    float* m;
} HeightMap;

int HeightMapBuild(PLY_Mesh* mesh, HeightMap* map);

#endif /* GROUND_H */
