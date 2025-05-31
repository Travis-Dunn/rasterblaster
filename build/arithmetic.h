#ifndef ARITHMETIC_H
#define ARITHMETIC_H

/*********************************    Vec3    *********************************/

typedef struct { float x, y, z; } Vec3;
Vec3 Vec3Make                   (float x, float y, float z);
static inline Vec3 Vec3Make_    (float x, float y, float z);
static inline Vec3 Vec3Add_     (Vec3 a, Vec3 b);
Vec3 Vec3Add                    (Vec3 a, Vec3 b);
static inline Vec3 Vec3Sub_     (Vec3 a, Vec3 b);
Vec3 Vec3Sub                    (Vec3 a, Vec3 b);
static inline Vec3 Vec3Scale_   (Vec3 v, float s);
Vec3 Vec3Scale                  (Vec3 v, float s);
static inline float Vec3Dot_    (Vec3 a, Vec3 b);
float Vec3Dot                   (Vec3 a, Vec3 b);
static inline Vec3 Vec3Cross_   (Vec3 a, Vec3 b);
Vec3 Vec3Cross                  (Vec3 a, Vec3 b);
static inline float Vec3LenSq_  (Vec3 v);
static inline float Vec3Len_    (Vec3 v);
static inline Vec3 Vec3Norm_    (Vec3 v);
Vec3 Vec3Norm                   (Vec3 v);

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

/*********************************    Quat    *********************************/
typedef struct { float w, x, y, z; } Quat;
Quat QuatMake                           (float w, float x, float y, float z);
static inline Quat QuatMake_            (float w, float x, float y, float z);
/* axis must be normalized */
static inline Quat QuatFromAxisAngle_   (Vec3 axis, float rads);
/* hamilton product */
static inline Quat QuatMul_(Quat a, Quat b);
static inline Quat QuatConj_(Quat q);
static inline Quat QuatNorm_(Quat q);
static inline Vec3 QuatRotateVec3(Quat q, Vec3 v);
static inline Mat4 Mat4FromQuat_(Quat q);
Mat4 Mat4FromQuat(Quat q);
static inline Mat4 Mat4LookAt_(Vec3 eye, Vec3 centre, Vec3 up);
/* right handed, opengl style */
Mat4 Mat4LookAt(Vec3 eye, Vec3 centre, Vec3 up);

#endif /* ARITHMETIC_H */
