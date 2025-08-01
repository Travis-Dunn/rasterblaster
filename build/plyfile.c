#include "stdlib.h"
#include "string.h"
#include "assert.h"
#include "plyfile.h"
#include "renderer.h"

/* return codes - for all loading functions */
#define FILE_NOT_FOUND                  -1
#define MALFORMED_HEADER                -2
#define FORMAT_NOT_ASCII                -3
#define ZERO_FACES                      -4
#define ZERO_VERTICES                   -5
#define ALLOCATION_FAILURE              -6
#define UNEXPECTED_EOF                  -7
#define INCOMPATIBLE_VERTEX_LINE_FORMAT -8
#define INCOMPATIBLE_FACE_LINE_FORMAT   -9
#define NOT_TRIANGULATED                -10
#define OUT_OF_BOUNDS_VERTEX_INDEX      -11
#define SUCCESS                          0

/* this is what we're working to replace */
int PLYLoadFile(char* filename, PLY_Mesh* mesh){
    assert(filename); /* assert(mesh); */

    FILE* file = fopen(filename, "r");
    if (!file) return 1;

    printf("getting here\n");
    mesh->triangles = 0;

    printf("triangles\n");
    mesh->triCount = 0;
    printf("triCount\n");
    mesh->allocated = 0;
    printf("allocated\n");

    char line[256];

    if (!fgets(line, sizeof(line), file) || strncmp(line, "ply", 3) != 0) {
        fclose(file);
        return 2; /* .ply files have "ply" as the first line */
    }

    int vertexCount = 0;
    int faceCount = 0;
    int isAscii = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "format ascii", 12) == 0) {
            isAscii = 1;
        } else if (strncmp(line, "element vertex", 14) == 0) {
            sscanf(line, "element vertex %d", &vertexCount);
        } else if (strncmp(line, "element face", 12) == 0) {
            sscanf(line, "element face %d", &faceCount);
        } else if (strncmp(line, "end_header", 10) == 0) {
            break;
        }
    }

    if (!isAscii || vertexCount == 0 ||faceCount == 0) {
        fclose(file);
        return 3; /* empty or binary. binary support is for the future */
    }

    PLY_Vertex* vertices =
        (PLY_Vertex*)(malloc(vertexCount * sizeof(PLY_Vertex)));
    if (!vertices) {
        fclose(file);
        return 4; /* allocation failure */
    }

    for (int i = 0; i < vertexCount; i++) {
        if (!fgets(line, sizeof(line), file)) {
            free(vertices);
            fclose(file);
            return 5; /* unexpected OEF */
        }

        float x, y, z, nx, ny, nz, s, t;
        unsigned int r, g, b, a;
        int parsed = sscanf(line, "%f %f %f %f %f %f %u %u %u %u %f %f",
                &x, &y, &z, &nx, &ny, &nz, &r, &g, &b, &a, &s, &t);
        if (parsed < 3) {
            free(vertices);
            fclose(file);
            return 6; /* unexpected vertex line format */
        }

        vertices[i].pos = Vec3Make(x, y, z);
        vertices[i].normal =
            (parsed >= 6) ? Vec3Make(nx, ny, nz) : Vec3Make(0.f, 0.f, 1.f);
        vertices[i].texCoord
            = (parsed >= 8) ? Vec2Make(s, t) : Vec2Make(0.f, 0.f);
        vertices[i].color = (parsed >= 11) ? RGBA_INT(r, g, b, a) : 0;
    }

    mesh->triCount = faceCount; /* Only supporting triangulated meshes! */
    mesh->allocated = mesh->triCount;
    mesh->triangles =
        (PLY_Triangle*)(malloc(mesh->triCount * sizeof(PLY_Triangle)));

    if (!mesh->triangles) {
        free(vertices);
        fclose(file);
        return 4; /* allocation failure */
    }

    for (int i = 0; i < faceCount; i++) {
        if (!fgets(line, sizeof(line), file)) {
            free(vertices);
            PLYMeshFree(mesh);
            fclose(file);
            return 5; /* unexpected EOF */
        }

        int vertCount, idx0, idx1, idx2;
        int parsed =
            sscanf(line, "%d %d %d %d", &vertCount, &idx0, &idx1, &idx2);
        if (parsed < 4 || vertCount != 3) {
            free(vertices);
            PLYMeshFree(mesh);
            fclose(file);
            return 7; /* face doesn't match expectation */
        }

        mesh->triangles[i].v0 = vertices[idx0];
        mesh->triangles[i].v1 = vertices[idx1];
        mesh->triangles[i].v2 = vertices[idx2];
    }

    free(vertices);
    fclose(file);
    return 0; /* success */
}

PLY_Triangle PLYGetTriangle(PLY_Mesh* mesh, int idx) {
    assert(mesh); assert(mesh->triangles); assert(idx >= 0);
    assert(idx < mesh->triCount);
    return (mesh->triangles[idx]);
}

void PLYMeshFree(PLY_Mesh* mesh) {
    if (mesh && mesh->triangles) {
        free(mesh->triangles);
        mesh->triangles = 0;
        mesh->triCount = 0;
        mesh->allocated = 0;
    }
}

/* start of new code */
int ModelColLoadPLY(ModelCol* model, char* filename) {
    assert(filename); assert(model);

    int ret;

    FILE* file = fopen(filename, "r");
    if (!file) {
        ret = FILE_NOT_FOUND;
        goto cleanup;
    }

    model->tris = 0;
    model->count = 0;
    model->init = 0;

    int vertexCount, faceCount;
    ret = ParseHeader_(file, &vertexCount, &faceCount);
    if (ret != SUCCESS) goto cleanup;

    VertexCol* vertices = (VertexCol*)(malloc(vertexCount * sizeof(VertexCol)));
    if (!vertices) {
        ret = ALLOCATION_FAILURE;
        goto cleanup;
    }

    char line[256];
    for (int i = 0; i < vertexCount; i++) {
        if (!fgets(line, sizeof(line), file)) {
            ret = UNEXPECTED_EOF;
            goto cleanup;
        }

        float x, y, z;
        unsigned int r, g, b, a;
        int parsed = sscanf(line, "%f %f %f %u %u %u %u",
                &x, &y, &z, &r, &g, &b, &a);
        if (parsed != 7) {
            ret = INCOMPATIBLE_VERTEX_LINE_FORMAT;
            goto cleanup;
        }

        vertices[i].pos = Vec3Make(x, y, z);
        vertices[i].color = RGBA_INT(r, g, b, a);
    }
    model->count = faceCount;
    model->tris = (TriCol*)(malloc(model->count * sizeof(TriCol)));

    if (!model->tris) {
        ret = ALLOCATION_FAILURE;
        goto cleanup;
    }

    for (int i = 0; i < faceCount; i++) {
        if (!fgets(line, sizeof(line), file)) {
            ret = UNEXPECTED_EOF;
            goto cleanup;
        }

        int idx0, idx1, idx2;
        ret = ParseFaceLine_(line, &vertexCount, &idx0, &idx1, &idx2);
        if (ret != SUCCESS) goto cleanup;

        model->tris[i].v0 = vertices[idx0];
        model->tris[i].v1 = vertices[idx1];
        model->tris[i].v2 = vertices[idx2];
    } 
    model->init = 1;

cleanup:
    /* TODO: these braces are not necessary, they're only here because my vim
     * config shits the bed otherwise. Not for all one line if statements, and I
     * don't know why. Fix the vim config. */
    if (vertices) { free(vertices); }
    if (file) { fclose(file); }
    if (ret != SUCCESS) {
        if (model->tris) {
            free(model->tris);
            model->tris = 0;
        }
        model->count = 0;
        model->init = 0;
    }
    
    return ret;
}

int ModelCol16LoadPLY(ModelCol16* model, char* filename) {
    assert(filename); assert(model);

    int ret;

    FILE* file = fopen(filename, "r");
    if (!file) {
        ret = FILE_NOT_FOUND;
        goto cleanup;
    }

    model->tris = 0;
    model->count = 0;
    model->init = 0;

    int vertexCount, faceCount;
    ret = ParseHeader_(file, &vertexCount, &faceCount);
    if (ret != SUCCESS) goto cleanup;

    VertCol16* vertices = (VertCol16*)(malloc(vertexCount * sizeof(VertCol16)));
    if (!vertices) {
        ret = ALLOCATION_FAILURE;
        goto cleanup;
    }

    char line[256];
    for (int i = 0; i < vertexCount; i++) {
        if (!fgets(line, sizeof(line), file)) {
            ret = UNEXPECTED_EOF;
            goto cleanup;
        }

        float x, y, z;
        unsigned int r, g, b, a;
        int parsed = sscanf(line, "%f %f %f %u %u %u %u",
                &x, &y, &z, &r, &g, &b, &a);
        if (parsed != 7) {
            ret = INCOMPATIBLE_VERTEX_LINE_FORMAT;
            goto cleanup;
        }

        vertices[i].pos[0] = FLOAT_TO_SF610(x);
        vertices[i].pos[1] = FLOAT_TO_SF610(y);
        vertices[i].pos[2] = FLOAT_TO_SF610(z);
        vertices[i].color = RGB565_MAKE(r, g, b);
    }
    model->count = faceCount;
    model->tris = (TriCol16*)(malloc(model->count * sizeof(TriCol16)));

    if (!model->tris) {
        ret = ALLOCATION_FAILURE;
        goto cleanup;
    }

    for (int i = 0; i < faceCount; i++) {
        if (!fgets(line, sizeof(line), file)) {
            ret = UNEXPECTED_EOF;
            goto cleanup;
        }

        int idx0, idx1, idx2;
        ret = ParseFaceLine_(line, &vertexCount, &idx0, &idx1, &idx2);
        if (ret != SUCCESS) goto cleanup;

        model->tris[i].v0 = vertices[idx0];
        model->tris[i].v1 = vertices[idx1];
        model->tris[i].v2 = vertices[idx2];
    } 
    model->init = 1;

cleanup:
    /* TODO: these braces are not necessary, they're only here because my vim
     * config shits the bed otherwise. Not for all one line if statements, and I
     * don't know why. Fix the vim config. */
    if (vertices) { free(vertices); }
    if (file) { fclose(file); }
    if (ret != SUCCESS) {
        if (model->tris) {
            free(model->tris);
            model->tris = 0;
        }
        model->count = 0;
        model->init = 0;
    }
    
    return ret;
}

int ModelTex_stubLoadPLY(ModelTex_stub* model, char* filename) {
    assert(filename); assert(model);

    int ret;

    FILE* file = fopen(filename, "r");
    if (!file) {
        ret = FILE_NOT_FOUND;
        goto cleanup;
    }

    model->tris = 0;
    model->count = 0;
    model->init = 0;

    int vertexCount, faceCount;
    ret = ParseHeader_(file, &vertexCount, &faceCount);
    if (ret != SUCCESS) goto cleanup;

    VertexTex* vertices = (VertexTex*)(malloc(vertexCount * sizeof(VertexTex)));
    if (!vertices) {
        ret = ALLOCATION_FAILURE;
        goto cleanup;
    }

    char line[256];
    for (int i = 0; i < vertexCount; i++) {
        if (!fgets(line, sizeof(line), file)) {
            ret = UNEXPECTED_EOF;
            goto cleanup;
        }

        float x, y, z, s, t;
        int parsed = sscanf(line, "%f %f %f %f %f",
                &x, &y, &z, &s, &t);
        if (parsed != 5) {
            ret = INCOMPATIBLE_VERTEX_LINE_FORMAT;
            goto cleanup;
        }

        vertices[i].pos = Vec3Make(x, y, z);
        vertices[i].uv = Vec2Make(s, t);
    }
    model->count = faceCount;
    model->tris = (TriTex*)(malloc(model->count * sizeof(TriTex)));

    if (!model->tris) {
        ret = ALLOCATION_FAILURE;
        goto cleanup;
    }

    for (int i = 0; i < faceCount; i++) {
        if (!fgets(line, sizeof(line), file)) {
            ret = UNEXPECTED_EOF;
            goto cleanup;
        }

        int idx0, idx1, idx2;
        ret = ParseFaceLine_(line, &vertexCount, &idx0, &idx1, &idx2);
        if (ret != SUCCESS) goto cleanup;

        model->tris[i].v0 = vertices[idx0];
        model->tris[i].v1 = vertices[idx1];
        model->tris[i].v2 = vertices[idx2];
    } 
    model->init = 1;

cleanup:
    if (vertices) { free(vertices); }
    if (file)  { fclose(file); }
    if (ret != SUCCESS) {
        if (model->tris) {
            free(model->tris);
            model->tris = 0;
        }
        model->count = 0;
        model->init = 0;
    }
    
    return ret;
}

static inline int ParseHeader_(FILE* file, int* vCount, int* fCount) {
    char line[256];
    int isAscii = 0, isPlyFile = 0;
    *vCount = *fCount = 0;

    while (fgets(line, sizeof(line), file)) {
        if (strncmp(line, "format ascii", 12) == 0) {
            isAscii = 1;
        } else if (strncmp(line, "element vertex", 14) == 0) {
            sscanf(line, "element vertex %d", vCount);
        } else if (strncmp(line, "element face", 12) == 0) {
            sscanf(line, "element face %d", fCount);
        } else if (strncmp(line, "ply", 3) == 0) {
            isPlyFile = 1;
        } else if (strncmp(line, "end_header", 10) == 0) {
            break;
        }
    }
    if (!isPlyFile) return MALFORMED_HEADER;
    if (!isAscii) return FORMAT_NOT_ASCII;
    if (*vCount == 0) return ZERO_VERTICES;
    if (*fCount == 0) return ZERO_FACES;
    return SUCCESS;
}

static inline int ParseFaceLine_(char* line, int* vCount, int* idx0,
        int* idx1, int* idx2) {
    int vertCount;
    int parsed = sscanf(line, "%d %d %d %d", &vertCount, idx0, idx1, idx2);

    if (parsed != 4) return INCOMPATIBLE_FACE_LINE_FORMAT;
    if (vertCount != 3) return NOT_TRIANGULATED;
    if (*idx0 >= *vCount || *idx1 >= *vCount || *idx2 >= *vCount ||
            *idx0 < 0 || *idx1 < 0 || *idx2 < 0) {
        return OUT_OF_BOUNDS_VERTEX_INDEX;
    }
    return SUCCESS;
}
 
