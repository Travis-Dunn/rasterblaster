#ifndef PLYFILE_H
#define PLYFILE_H

#include "stdio.h"
#include "arithmetic.h"
#include "model_types.h"

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

/* new code */
int      ModelColLoadPLY        (ModelCol* model, char* filename);
int      ModelCol16LoadPLY      (ModelCol16* model, char* filename);
int      ModelTex_stubLoadPLY   (ModelTex_stub* model, char* filename);

/* internal */
static inline int ParseHeader_(FILE* file, int* vCount, int* fCount);
static inline int ParseFaceLine_(char* line, int* vCount, int* idx0, int* idx1,
        int* idx2);
    
#endif /* PLYFILE_H */
