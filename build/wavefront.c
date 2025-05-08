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
    float* newPositions = (float*)realloc(data->positions, sizeof(float) * (data->positionCount + 3));
    if (!newPositions) return 0;
    data->positions = newPositions;
    data->positions[data->positionCount++] = x;
    data->positions[data->positionCount++] = y;
    data->positions[data->positionCount++] = z;
    return 1;
}

static int addTexcoord(OBJData* data, float u, float v) {
    float* newTexcoords = (float*)realloc(data->texcoords, sizeof(float) * (data->texcoordCount + 2));
    if (!newTexcoords) return 0;
    data->texcoords = newTexcoords;
    data->texcoords[data->texcoordCount++] = u;
    data->texcoords[data->texcoordCount++] = v;
    return 1;
}

static int addNormal(OBJData* data, float x, float y, float z) {
    float* newNormals = (float*)realloc(data->normals, sizeof(float) * (data->normalCount + 3));
    if (!newNormals) return 0;
    data->normals = newNormals;
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
    countOBJElements(filename, &vCount, &vtCount, &vnCount, &fCount);
    FILE* file = NULL;
    char line[512];
    OBJData objData;
    Mesh* mesh = NULL;
    Vertex* tempVertices = NULL;
    unsigned* tempIndices = NULL;
    int vertexCapacity = 0;
    int indexCapacity = 0;
    int i;

    objData.positions = NULL;
    objData.positionCount = 0;
    objData.texcoords = NULL;
    objData.texcoordCount = 0;
    objData.normals = NULL;
    objData.normalCount = 0;

    file = fopen(filename, "r");
    if (!file) {
        return NULL;
    }

    // Temporary face arrays
    tempVertices = (Vertex*)malloc(sizeof(Vertex) * 128);
    tempIndices = (unsigned*)malloc(sizeof(unsigned) * 128);
    if (!tempVertices || !tempIndices) {
        goto cleanup;
    }
    vertexCapacity = 128;
    indexCapacity = 128;

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == 'v' && line[1] == ' ') {
            float x, y, z;
            if (sscanf(line + 2, "%f %f %f", &x, &y, &z) == 3) {
                if (!addPosition(&objData, x, y, z)) {
                    goto cleanup;
                }
            }
        } else if (line[0] == 'v' && line[1] == 't') {
            float u, v;
            if (sscanf(line + 3, "%f %f", &u, &v) == 2) {
                if (!addTexcoord(&objData, u, v)) {
                    goto cleanup;
                }
            }
        } else if (line[0] == 'v' && line[1] == 'n') {
            float nx, ny, nz;
            if (sscanf(line + 3, "%f %f %f", &nx, &ny, &nz) == 3) {
                if (!addNormal(&objData, nx, ny, nz)) {
                    goto cleanup;
                }
            }
        } else if (line[0] == 'f' && line[1] == ' ') {
            char* token;
            char* context;
            int indices[3][3];
            int tokenCount = 0;

            token = strtok(line + 2, " \t\r\n");
            while (token && tokenCount < 3) {
                if (!parseFaceToken(token, &indices[tokenCount][0], &indices[tokenCount][1], &indices[tokenCount][2])) {
                    goto cleanup;
                }
                tokenCount++;
                token = strtok(NULL, " \t\r\n");
            }

            assert(tokenCount == 3); // We assume triangles only

            for (i = 0; i < 3; ++i) {
                int vi = indices[i][0] - 1;
                int ti = indices[i][1] - 1;
                int ni = indices[i][2] - 1;
                Vertex v;

                assert(vi >= 0 && vi * 3 + 2 < objData.positionCount);
                assert(ti >= 0 && ti * 2 + 1 < objData.texcoordCount);
                assert(ni >= 0 && ni * 3 + 2 < objData.normalCount);

                v.x = objData.positions[vi * 3 + 0];
                v.y = objData.positions[vi * 3 + 1];
                v.z = objData.positions[vi * 3 + 2];
                v.w = 1.0f;

                v.u = objData.texcoords[ti * 2 + 0];
                v.v = objData.texcoords[ti * 2 + 1];

                v.nx = objData.normals[ni * 3 + 0];
                v.ny = objData.normals[ni * 3 + 1];
                v.nz = objData.normals[ni * 3 + 2];

                if (mesh == NULL || mesh->vertexCount >= vertexCapacity) {
                    Vertex* newVerts = (Vertex*)realloc(tempVertices, sizeof(Vertex) * vertexCapacity * 2);
                    if (!newVerts) {
                        goto cleanup;
                    }
                    tempVertices = newVerts;
                    vertexCapacity *= 2;
                }
                if (mesh == NULL || mesh->indexCount >= indexCapacity) {
                    unsigned* newIndices = (unsigned*)realloc(tempIndices, sizeof(unsigned) * indexCapacity * 2);
                    if (!newIndices) {
                        goto cleanup;
                    }
                    tempIndices = newIndices;
                    indexCapacity *= 2;
                }

                tempVertices[mesh ? mesh->vertexCount : 0] = v;
                tempIndices[mesh ? mesh->indexCount : 0] = (mesh ? mesh->vertexCount : 0);

                if (mesh) {
                    mesh->vertexCount++;
                    mesh->indexCount++;
                } else {
                    mesh = (Mesh*)malloc(sizeof(Mesh));
                    if (!mesh) {
                        goto cleanup;
                    }
                    mesh->vertices = tempVertices;
                    mesh->vertexCount = 1;
                    mesh->indices = tempIndices;
                    mesh->indexCount = 1;
                }
            }
        }
    }

    fclose(file);
    freeOBJData(&objData);
    return mesh;

cleanup:
    if (file) fclose(file);
    if (tempVertices) free(tempVertices);
    if (tempIndices) free(tempIndices);
    freeOBJData(&objData);
    freeMesh(mesh);
    return NULL;
}

void freeMesh(Mesh* mesh) {
    if (mesh) {
        if (mesh->vertices) free(mesh->vertices);
        if (mesh->indices) free(mesh->indices);
        free(mesh);
    }
}
