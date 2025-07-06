#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "assert.h"
#include "plyfile.h"
#include "renderer.h"

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
