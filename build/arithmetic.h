#ifndef ARITHMETIC_H
#define ARITHMETIC_H

/*********************************    Vec3    *********************************/

typedef struct { float x, y, z; } Vec3;
Vec3 MakeVec3                   (float x, float y, float z);
static inline Vec3 MakeVec3_    (float x, float y, float z);
static inline Vec3 AddVec3_     (Vec3 a, Vec3 b);
static inline Vec3 SubVec3_     (Vec3 a, Vec3 b);
static inline Vec3 ScaleVec3_   (Vec3 v, float s);
static inline float DotVec3_    (Vec3 a, Vec3 b);
static inline Vec3 CrossVec3_   (Vec3 a, Vec3 b);
static inline float LenSqVec3_  (Vec3 v);
static inline float LenVec3_    (Vec3 v);
static inline Vec3 NormVec3_    (Vec3 v);

typedef struct { float x, y, z, w;  } Vec4;

/* Column major */
typedef struct { float m[3][3]; } Mat3;
typedef struct { float m[4][4]; } Mat4;


Vec4 MatVertMul(Mat4* mat, Vec4 vert);
Mat4 MatIdentity();
Mat4 MatMatMul(Mat4* a, Mat4* b);
Mat4 MatTranslate(float tx, float ty, float tz);
Mat4 MatScale(float sx, float sy, float sz);
Mat4 MatPitch(float pitch); /* Euler, radians */
Mat4 MatYaw(float yaw);
Mat4 MatRoll(float roll);
Mat4 MatView(float pitch, float yaw, float roll, float xPos, float yPos,
        float zPos);
Mat4 MatPerspective(float fov, float aspectRatio, float zNear, float zFar);

#endif /* ARITHMETIC_H */
