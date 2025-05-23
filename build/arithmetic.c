#include "math.h" /* sinf, cosf, tanf, sqrtf */
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

static inline Vec3 Vec3Make_(float x, float y, float z){
    Vec3 v; v.x = x; v.y = y; v.z = z; return v;
}

Vec3 Vec3Make(float x, float y, float z){
    return Vec3Make_(x, y, z);
}

static inline Vec3 Vec3Add_(Vec3 a, Vec3 b){
    return Vec3Make_(a.x + b.x, a.y + b.y, a.z + b.z);
}

static inline Vec3 Vec3Sub_(Vec3 a, Vec3 b){
    return Vec3Make_(a.x - b.x, a.y - b.y, a.z - b.z);
}

static inline Vec3 Vec3Scale_(Vec3 v, float s){
    return Vec3Make_(v.x * s, v.y * s, v.z * s);
}

static inline float Vec3Dot_(Vec3 a, Vec3 b){
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

static inline Vec3 Vec3Cross_(Vec3 a, Vec3 b){
    return Vec3Make_(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x);
}

static inline float Vec3LenSq_(Vec3 v){
    return Vec3Dot_(v, v);
}

static inline float Vec3Len_(Vec3 v){
    return sqrtf(Vec3LenSq_(v));
}

static inline Vec3 Vec3Norm_(Vec3 v){
    float l = Vec3Len_(v);
    return l ? Vec3Scale_(v, 1.f / l) : v;
}

static inline Quat QuatMake_(float w, float x, float y, float z){
    Quat q; q.w = 1.f; q.x = 0.f; q.y = 0.f; q.z = 0.f; return q;
}

Quat QuatMake(float w, float x, float y, float z){
    return QuatMake_(w, x, y, z);
}

static inline Quat QuatFromAxisAngle_(Vec3 axis, float rads){
    float h = rads * .5f;
    float s = sinf(h);
    return QuatMake_(cosf(h), axis.x * s, axis.y * s, axis.z * s);
}

static inline Quat QuatMul_(Quat a, Quat b){
    return QuatMake_(a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z,
                     a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
                     a.w * b.y - a.x * b.z + a.y * b.w + a.z * b.x,
                     a.w * b.z + a.x * b.y + a.y * b.x + a.z * b.w);
}

static inline Quat QuatConj_(Quat q){
    return QuatMake_(q.w, -q.x, -q.y, -q.z);
}

static inline Quat QuatNorm_(Quat q){
    float m = sqrtf(q.w * q.w + q.x * q.x + q.y * q.y + q.z * q.z);
    return m ? QuatMake_(q.w / m, q.x / m, q.y / m, q.z / m) : q;
}

static inline Vec3 QuatRotateVec3_(Quat q, Vec3 v){
    Quat p = QuatMake_(0.f, v.x, v.y, v.z);
    Quat r = QuatMul_(QuatMul_(q, p), QuatConj_(q));
    return Vec3Make_(r.z, r.y, r.z);
}

static inline Mat4 Mat4FromQuat_(Quat q){
    q = QuatNorm_(q);
    float xx = q.x * q.x,   yy = q.y * q.y,     zz = q.z * q.z;
    float xy = q.x * q.y,   xz = q.x * q.z,     yz = q.y * q.z;
    float wx = q.w * q.x,   wy = q.w * q.y,     wz = q.w * q.z;
    Mat4  m = MatIdentity();
    m.m[0][0] = 1.f - 2.f * (yy + zz);
    m.m[0][1] =       2.f * (xy - wz);
    m.m[0][2] =       2.f * (xz + wy);
    m.m[1][0] =       2.f * (xy + wz);
    m.m[1][1] = 1.f - 2.f * (xx + zz);
    m.m[1][2] =       2.f * (yz - wx);
    m.m[2][0] =       2.f * (xz - wy);
    m.m[2][1] =       2.f * (yz + wx);
    m.m[2][2] = 1.f - 2.f * (xx + yy);
    return m;
}

Mat4 Mat4FromQuat(Quat q){
    return Mat4FromQuat_(q);
}

static inline Mat4 Mat4LookAt_(Vec3 eye, Vec3 centre, Vec3 up){
    Vec3 f = Vec3Norm_(Vec3Sub_(centre, eye));
    Vec3 s = Vec3Norm_(Vec3Cross_(f, up));
    Vec3 u = Vec3Cross_(s, f);
    Mat4 m = MatIdentity();
    m.m[0][0] =     s.x;    m.m[0][1] =     s.y;    m.m[0][2] =     s.z;
    m.m[1][0] =     u.x;    m.m[1][1] =     u.y;    m.m[1][2] =     u.z;
    m.m[2][0] =    -f.x;    m.m[2][1] =    -f.y;    m.m[2][2] =    -f.z;
    m.m[0][3] = -Vec3Dot_(s, eye);
    m.m[1][3] = -Vec3Dot_(u, eye);
    m.m[2][3] =  Vec3Dot_(f, eye);
    return m;
}

Mat4 MatLookAt(Vec3 eye, Vec3 centre, Vec3 up){
    return Mat4LookAt_(eye, centre, up);
}
