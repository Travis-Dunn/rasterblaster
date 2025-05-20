#ifndef ARITHMETIC_H
#define ARITHMETIC_H

typedef struct {
    float x, y, z, w;
} Vec4;

typedef struct {
    float m[4][4]; /* Column major */
} Matrix;

Vec4 MatVertMul(Matrix* mat, Vec4 vert);
Matrix MatIdentity();
Matrix MatMatMul(Matrix* a, Matrix* b);
Matrix MatTranslate(float tx, float ty, float tz);
Matrix MatScale(float sx, float sy, float sz);
Matrix MatPitch(float pitch); /* Euler, radians */
Matrix MatYaw(float yaw);
Matrix MatRoll(float roll);
Matrix MatView(float pitch, float yaw, float roll, float xPos, float yPos,
        float zPos);
Matrix MatPerspective(float fov, float aspectRatio, float zNear, float zFar);

#endif /* ARITHMETIC_H */
