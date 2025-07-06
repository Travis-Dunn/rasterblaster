#ifndef PLYFILE_H
#define PLYFILE_H

#include "arithmetic.h"

typedef struct {
    Vec3 pos;
    Vec3 normal;
    Vec2 texCoord;
    int color;
} PLY_Vertex;

typedef struct {
    PLY_Vertex v0, v1, v2;
} PLY_Triangle;

typedef struct {
    PLY_Triangle* triangles;
    int triCount;
    int allocated;
} PLY_Mesh;

int PLYLoadFile(char* filename, PLY_Mesh* mesh);
PLY_Triangle PLYGetTriangle(PLY_Mesh* mesh, int idx);
void PLYMeshFree(PLY_Mesh* mesh);
    
#endif /* PLYFILE_H */
