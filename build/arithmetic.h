#ifndef ARITHMETIC_H
#define ARITHMETIC_H

#define PI          3.14159265358979323846264
#define TWO_PI      6.28318530717958647692528
#define HALF_PI     1.57079632679489661923132
#define FRADS(D)    ((D) * PI / 180.f)
#define FDEGS(R)    ((R) * 180.f / PI)

/*********************************    Vec3    *********************************/

typedef struct { float x, y, z; } Vec3;

static inline Vec3 Vec3Make_    (float x, float y, float z);
Vec3 Vec3Make                   (float x, float y, float z);
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

/*********************************    Vec4    *********************************/

typedef struct { float x, y, z, w;  } Vec4;

static inline Vec4 Vec4Make_    (float x, float y, float z, float w);
Vec4 Vec4Make                   (float x, float y, float z, float w);
static inline Vec4 Vec4Add_     (Vec4 a, Vec4 b);
Vec4 Vec4Add                    (Vec4 a, Vec4 b);
static inline Vec4 Vec4Sub_     (Vec4 a, Vec4 b);
Vec4 Vec4Sub                    (Vec4 a, Vec4 b);
static inline Vec4 Vec4Scale_   (Vec4 v, float s);
Vec4 Vec4Scale                  (Vec4 v, float s);
Vec4 Vec4Lerp                   (Vec4 a, Vec4 b, float t);

/********************************    Matrix    ********************************/
typedef struct { float m[3][3]; } Mat3;
typedef struct { float m[4][4]; } Mat4;

Vec4 MatVertMul                 (Mat4* mat, Vec4 vert);
Mat4 MatIdentity                ();
Mat4 MatMatMul                  (Mat4* a, Mat4* b);
Mat4 MatTranslate               (float tx, float ty, float tz);
Mat4 MatScale                   (float sx, float sy, float sz);
Mat4 MatPitch                   (float pitch); /* Euler, radians */
Mat4 MatYaw                     (float yaw);
Mat4 MatRoll                    (float roll);
Mat4 MatView                    (float pitch, float yaw, float roll, float xPos,
                                 float yPos, float zPos);
Mat4 Mat4Perspective            (float fov, float aspectRatio, float zNear,
                                 float zFar);
void Mat4Printf                 (Mat4* m, char* label);

/*********************************    Quat    *********************************/
typedef struct { float w, x, y, z; } Quat;

static inline Quat QuatMake_            (float w, float x, float y, float z);
Quat QuatMake                           (float w, float x, float y, float z);
/* axis must be normalized */
static inline Quat QuatFromAxisAngle_   (Vec3 axis, float rads);
Quat QuatFromAxisAngle                  (Vec3 axis, float rads);
/* hamilton product */
static inline Quat QuatMul_             (Quat a, Quat b);
Quat QuatMul                            (Quat a, Quat b);
static inline Quat QuatConj_            (Quat q);
static inline Quat QuatNorm_            (Quat q);
static inline Vec3 QuatRotateVec3_      (Quat q, Vec3 v);
Vec3 QuatRotateVec3                     (Quat q, Vec3 v);
static inline Mat4 Mat4FromQuat_        (Quat q);
Mat4 Mat4FromQuat                       (Quat q);
/* right handed, opengl style */
static inline Mat4 Mat4LookAt_          (Vec3 eye, Vec3 centre, Vec3 up);
Mat4 Mat4LookAt                         (Vec3 eye, Vec3 centre, Vec3 up);


/********************************    Planes   *********************************/
/* Not currently in use, potentially deprecated as we currently don't support
 * clipping against arbitrary planes, only the unit frustum in clip space */
typedef struct {
    Vec3 normal;
    float distance;
} Plane;

typedef struct {
    Plane nearPlane, farPlane, leftPlane, rightPlane, topPlane, bottomPlane;
} Frustum;

/* returns float */
#define SIGNED_DIST_POINT_PLANE(point, plane)                                  \
    (Vec3Dot((point), (plane).normal) - (plane).distance)
/* returns logical int */
#define POINT_IN_FRUSTUM(point, frustum)                                       \
    (SIGNED_DIST_POINT_PLANE((point), (frustum)->nearPlane)     <= 0.f &&      \
     SIGNED_DIST_POINT_PLANE((point), (frustum)->farPlane)      <= 0.f &&      \
     SIGNED_DIST_POINT_PLANE((point), (frustum)->leftPlane)     <= 0.f &&      \
     SIGNED_DIST_POINT_PLANE((point), (frustum)->rightPlane)    <= 0.f &&      \
     SIGNED_DIST_POINT_PLANE((point), (frustum)->topPlane)      <= 0.f &&      \
     SIGNED_DIST_POINT_PLANE((point), (frustum)->bottomPlane)   <= 0.f)


#endif /* ARITHMETIC_H */
