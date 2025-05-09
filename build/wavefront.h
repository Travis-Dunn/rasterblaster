#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "assert.h"

typedef struct {
    float x, y, z, w;
    float nx, ny, nz;
    float u, v;
} Vertex;

typedef struct {
    float* positions;
    int positionCount;
    float* texcoords;
    int texcoordCount;
    float* normals;
    int normalsCount;
    int* indices;
    int indexCount;
} Mesh;

typedef struct {
    float* positions;
    int positionCount;
    float* texcoords;
    int texcoordCount;
    float* normals;
    int normalCount;
} OBJData;

static int countOBJElements(char* filename, int* vCount, int* vtCount,
        int*vnCount, int* fCount);
static void freeOBJData(OBJData* data);
static int addPosition(OBJData* data, float x, float y, float z);
static int addTexcoord(OBJData* data, float u, float v);
static int addNormal(OBJData* data, float x, float y, float z);
static int parseFaceToken(const char* token, int* vIdx, int* vtIdx, int* vnIdx);
void freeMesh(Mesh* mesh);
Mesh* loadOBJ(const char* filename);
