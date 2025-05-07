#include "math.h" /* sinf, cosf, tanf */
#include "arithmetic.h"

Vertex MatVertMul(Matrix* mat, Vertex vert){
    Vertex v;
    v.x = mat->m[0][0] * vert.x + mat->m[0][1] * vert.y + mat->m[0][2] 
        * vert.z + mat->m[0][3] * vert.w;
    v.y = mat->m[1][0] * vert.x + mat->m[1][1] * vert.y + mat->m[1][2] 
        * vert.z + mat->m[1][3] * vert.w;
    v.z = mat->m[2][0] * vert.x + mat->m[2][1] * vert.y + mat->m[2][2] 
        * vert.z + mat->m[2][3] * vert.w;
    v.w = mat->m[3][0] * vert.x + mat->m[3][1] * vert.y + mat->m[3][2] 
        * vert.z + mat->m[3][3] * vert.w;
    return v;
}

Matrix MatIdentity(){
    int i, j;
    Matrix mat;
    for (i = 0; i < 4; i++){
        for (j = 0; j < 4; j++){
            mat.m[i][j] = (i == j) ? 1.f : 0.f; 
        }
    }
    return mat;
}

Matrix MatMatMul(Matrix* a, Matrix* b){
    Matrix mat;
    int row, col, k;
    for (row = 0; row < 4; row++){
        for (col = 0; col < 4; col++){
            mat.m[row][col] = 0.f;
            for (k = 0; k < 4; k++){
                mat.m[row][col] += a->m[row][k] * b->m[k][col];
            }
        }
    }
    return mat;
}

Matrix MatTranslate(float tx, float ty, float tz){
    Matrix mat = MatIdentity();
    mat.m[0][3] = tx;
    mat.m[1][3] = ty;
    mat.m[2][3] = tz;
    return mat;
}

Matrix MatScale(float sx, float sy, float sz){
    Matrix mat = MatIdentity();
    mat.m[0][0] = sx;
    mat.m[1][1] = sy;
    mat.m[2][2] = sz;
    return mat;
}

Matrix MatPitch(float pitch){
    Matrix mat = MatIdentity();
    float c = cosf(pitch);
    float s = sinf(pitch);
    mat.m[1][1] = c;
    mat.m[1][2] = -s;
    mat.m[2][1] = s;
    mat.m[2][2] = c;
    return mat;
}

Matrix MatYaw(float yaw){
    Matrix mat = MatIdentity();
    float c = cosf(yaw);
    float s = sinf(yaw);
    mat.m[0][0] = c;
    mat.m[0][2] = s;
    mat.m[2][0] = -s;
    mat.m[2][2] = c;
    return mat;
}

Matrix MatRoll(float roll){
    Matrix mat = MatIdentity();
    float c = cosf(roll);
    float s = sinf(roll);
    mat.m[0][0] = c;
    mat.m[0][1] = -s;
    mat.m[1][0] = s;
    mat.m[1][1] = c;
    return mat;
}

Matrix MatView(float pitch, float yaw, float roll, float xPos, float yPos,
        float zPos){
    Matrix mPitch = MatPitch(pitch);
    Matrix mYaw = MatYaw(yaw);
    Matrix mRoll = MatRoll(roll);
    Matrix mRot = MatMatMul(&mYaw, &mPitch);
    mRot = MatMatMul(&mRot, &mRoll);
    Matrix mTrans = MatTranslate(-xPos, -yPos, -zPos);
    Matrix view = MatMatMul(&mRot, &mTrans);
    return view;
}

Matrix MatPerspective(float fov, float aspectRatio, float zNear, float zFar){
    Matrix mat = MatIdentity();
    float f = 1 / tanf(fov / 2);
    float temp = zNear - zFar;
    mat.m[0][0] = f / aspectRatio;
    mat.m[1][1] = f;
    mat.m[2][2] = (zFar + zNear) / temp;
    mat.m[2][3] = (2 * zNear * zFar) / temp;
    mat.m[3][2] = -1.f;
    mat.m[3][3] = 0.f;
    return mat;
}
