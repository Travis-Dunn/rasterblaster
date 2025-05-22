#include "math.h" /* sinf, cosf, tanf */
#include "arithmetic.h"

Vec4 MatVertMul(Mat4* mat, Vec4 vert){
    Vec4 v;
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

Mat4 MatIdentity(){
    int i, j;
    Mat4 mat;
    for (i = 0; i < 4; i++){
        for (j = 0; j < 4; j++){
            mat.m[i][j] = (i == j) ? 1.f : 0.f; 
        }
    }
    return mat;
}

Mat4 MatMatMul(Mat4* a, Mat4* b){
    Mat4 mat;
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

Mat4 MatTranslate(float tx, float ty, float tz){
    Mat4 mat = MatIdentity();
    mat.m[0][3] = tx;
    mat.m[1][3] = ty;
    mat.m[2][3] = tz;
    return mat;
}

Mat4 MatScale(float sx, float sy, float sz){
    Mat4 mat = MatIdentity();
    mat.m[0][0] = sx;
    mat.m[1][1] = sy;
    mat.m[2][2] = sz;
    return mat;
}

Mat4 MatPitch(float pitch){
    Mat4 mat = MatIdentity();
    float c = cosf(pitch);
    float s = sinf(pitch);
    mat.m[1][1] = c;
    mat.m[1][2] = -s;
    mat.m[2][1] = s;
    mat.m[2][2] = c;
    return mat;
}

Mat4 MatYaw(float yaw){
    Mat4 mat = MatIdentity();
    float c = cosf(yaw);
    float s = sinf(yaw);
    mat.m[0][0] = c;
    mat.m[0][2] = s;
    mat.m[2][0] = -s;
    mat.m[2][2] = c;
    return mat;
}

Mat4 MatRoll(float roll){
    Mat4 mat = MatIdentity();
    float c = cosf(roll);
    float s = sinf(roll);
    mat.m[0][0] = c;
    mat.m[0][1] = -s;
    mat.m[1][0] = s;
    mat.m[1][1] = c;
    return mat;
}

Mat4 MatView(float pitch, float yaw, float roll, float xPos, float yPos,
        float zPos){
    Mat4 mPitch = MatPitch(pitch);
    Mat4 mYaw = MatYaw(yaw);
    Mat4 mRoll = MatRoll(roll);
    Mat4 mRot = MatMatMul(&mYaw, &mPitch);
    mRot = MatMatMul(&mRot, &mRoll);
    Mat4 mTrans = MatTranslate(-xPos, -yPos, -zPos);
    Mat4 view = MatMatMul(&mRot, &mTrans);
    return view;
}

Mat4 MatPerspective(float fov, float aspectRatio, float zNear, float zFar){
    Mat4 mat = MatIdentity();
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
