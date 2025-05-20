#include "config.h"
#include "wavefront.h"

#define CHARS_PER_LINE 512
static int countOBJElements(char* filename, int* vCount, int* vtCount,
        int*vnCount, int* fCount){
    FILE* file = 0;
    char line[CHARS_PER_LINE];
    *vCount = 0;
    *vtCount = 0;
    *vnCount = 0;
    *fCount = 0;
    file = fopen(filename, "r");
    if (!file) return 1;
    while (fgets(line, sizeof(line), file)){
        if (line[0] == 'v'){
            if (line[1] == ' '){
                (*vCount)++;
            } else if (line[1] == 't'){
                (*vtCount)++; 
            } else if (line[1] == 'n'){
                (*vnCount)++;
            }
        } else if (line[0] == 'f' && line[1] == ' '){
            (*fCount)++;
        }
    }
    fclose(file);
#ifdef PRINTFLOGGING
    printf("OBJ counts: %d verts, %d texCoords, %d normals, %d faces\n", 
            *vCount, *vtCount, *vnCount, *fCount);
#endif
    return 0;
}

static void freeOBJData(OBJData* data) {
    if (data->positions) free(data->positions);
    if (data->texcoords) free(data->texcoords);
    if (data->normals) free(data->normals);
}

static int addPosition(OBJData* data, float x, float y, float z) {
    data->positions[data->positionCount++] = x;
    data->positions[data->positionCount++] = y;
    data->positions[data->positionCount++] = z;
    return 1;
}

static int addTexcoord(OBJData* data, float u, float v) {
    data->texcoords[data->texcoordCount++] = u;
    data->texcoords[data->texcoordCount++] = v;
    return 1;
}

static int addNormal(OBJData* data, float x, float y, float z) {
    data->normals[data->normalCount++] = x;
    data->normals[data->normalCount++] = y;
    data->normals[data->normalCount++] = z;
    return 1;
}

static int parseFaceToken(const char* token, int* vIdx, int* vtIdx, int* vnIdx) {
    return sscanf(token, "%d/%d/%d", vIdx, vtIdx, vnIdx) == 3;
}

Mesh* loadOBJ(const char* filename) {
    int vCount, vtCount, vnCount, fCount;
    FILE* file = NULL;
    char line[CHARS_PER_LINE];
    OBJData objData;
    Mesh* mesh = NULL;
    int* tempIndices = NULL;
    int indexCursor = 0;

    countOBJElements(filename, &vCount, &vtCount, &vnCount, &fCount);

    objData.positions = (float*)malloc(sizeof(float) * vCount * 3);
    objData.texcoords = (float*)malloc(sizeof(float) * vtCount * 2);
    objData.normals = (float*)malloc(sizeof(float) * vnCount * 3);
    objData.positionCount = 0;
    objData.texcoordCount = 0;
    objData.normalCount = 0;

    tempIndices = (int*)malloc(sizeof(int) * fCount * 9);

    if (!objData.positions || !objData.texcoords || !objData.normals || !tempIndices) {
        goto cleanup;
    }

    file = fopen(filename, "r");
    if (!file) goto cleanup;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            float x, y, z;
            sscanf(line + 2, "%f %f %f", &x, &y, &z);
            addPosition(&objData, x, y, z);
        } else if (line[0] == 'v' && line[1] == 't') {
            float u, v;
            sscanf(line + 3, "%f %f", &u, &v);
            addTexcoord(&objData, u, v);
        } else if (line[0] == 'v' && line[1] == 'n') {
            float nx, ny, nz;
            sscanf(line + 3, "%f %f %f", &nx, &ny, &nz);
            addNormal(&objData, nx, ny, nz);
        } else if (line[0] == 'f' && line[1] == ' ') {
            char* token;
            int vIdx, vtIdx, vnIdx;
            token = strtok(line + 2, " \t\r\n");
            for (int i = 0; i < 3 && token; ++i) {
                if (parseFaceToken(token, &vIdx, &vtIdx, &vnIdx)) {
                    tempIndices[indexCursor++] = vIdx - 1;  // 0-based
                    tempIndices[indexCursor++] = vtIdx - 1;
                    tempIndices[indexCursor++] = vnIdx - 1;
                }
                token = strtok(NULL, " \t\r\n");
            }
        }
    }

    fclose(file);

    mesh = (Mesh*)malloc(sizeof(Mesh));
    if (!mesh) goto cleanup;

    mesh->positions = objData.positions;
    mesh->positionCount = objData.positionCount;

    mesh->texcoords = objData.texcoords;
    mesh->texcoordCount = objData.texcoordCount;

    mesh->normals = objData.normals;
    mesh->normalsCount = objData.normalCount;

    mesh->indices = tempIndices;
    mesh->indexCount = indexCursor;

    return mesh;

cleanup:
    if (file) fclose(file);
    if (objData.positions) free(objData.positions);
    if (objData.texcoords) free(objData.texcoords);
    if (objData.normals) free(objData.normals);
    if (tempIndices) free(tempIndices);
    if (mesh) free(mesh);
    return NULL;
}
/*
void freeMesh(Mesh* mesh) {
    if (mesh) {
        if (mesh->vertices) free(mesh->vertices);
        if (mesh->indices) free(mesh->indices);
        free(mesh);
    }
}
*/

void GetTriIndices(Mesh* m, int triIdx, int* p0, int* p1, int* p2,
                                        int* t0, int* t1, int* t2,
                                        int* n0, int* n1, int* n2){
    int base = triIdx * 9;
    *p0 = m->indices[base + 0];
    *t0 = m->indices[base + 1];
    *n0 = m->indices[base + 2];
    *p1 = m->indices[base + 3];
    *t1 = m->indices[base + 4];
    *n1 = m->indices[base + 5];
    *p2 = m->indices[base + 6];
    *t2 = m->indices[base + 7];
    *n2 = m->indices[base + 8];
}

void GetVertex(Mesh* m, int pIdx, int tIdx, int nIdx, Vec4* p, float* u,
                float* v, Vec4* n){
    p->x = m->positions[pIdx * 3 + 0];
    p->y = m->positions[pIdx * 3 + 1];
    p->z = m->positions[pIdx * 3 + 2];
    p->w = 1.f;
    *u = m->texcoords[tIdx * 2 + 0];
    *v = m->texcoords[tIdx * 2 + 1];
    n->x = m->normals[nIdx * 3 + 0];
    n->y = m->normals[nIdx * 3 + 1];
    n->z = m->normals[nIdx * 3 + 2];
    n->w = 0.f;
}
