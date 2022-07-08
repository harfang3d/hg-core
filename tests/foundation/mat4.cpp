// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.
#include <math.h>

#define TEST_NO_MAIN 
#include "acutest.h"

#include "foundation/matrix4.h"

#include "foundation/matrix3.h"
#include "foundation/vector3.h"
#include "foundation/vector4.h"

using namespace hg;

void test_mat4() {
	// Mat4();
	// explicit Mat4(const float *v);
	// explicit Mat4(const Mat3 &m);
	// Mat4(float m00, float m10, float m20, float m01, float m11, float m21, float m02, float m12, float m22, float m03, float m13, float m23);
	// 	bool operator==(const Mat4 &a, const Mat4 &b);
	// bool operator!=(const Mat4 &a, const Mat4 &b);
	// 	Mat4 operator*(const Mat4 &a, const Mat4 &b);
	// Mat4 operator*(const Mat4 &m, float v);
	// Mat4 operator/(const Mat4 &m, float v);
	// Mat4 operator+(const Mat4 &a, const Mat4 &b);
	// Mat4 operator-(const Mat4 &a, const Mat4 &b);
	// Vec3 operator*(const Mat4 &m, const Vec3 &v);
	// Vec4 operator*(const Mat4 &m, const Vec4 &v);
	// bool Inverse(const Mat4 &m, Mat4 &i);
	// Mat4 InverseFast(const Mat4 &m);
	// Mat4 Orthonormalize(const Mat4 &m);
	// Mat4 Normalize(const Mat4 &m);
	// void Decompose(const Mat4 &m, Vec3 *position, Mat3 *rotation = nullptr, Vec3 *scale = nullptr);
	// void Decompose(const Mat4 &m, Vec3 *position, Vec3 *rotation, Vec3 *scale, RotationOrder order = RO_Default);
	// void TransformVec3(const Mat4 &__restrict m, Vec3 *__restrict out, const Vec3 *__restrict in, unsigned int count = 1);
	// void TransformVec3(const Mat4 &__restrict m, Vec4 *__restrict out, const Vec3 *__restrict in, unsigned int count = 1);
	// void RotateVec3(const Mat4 &__restrict m, Vec3 *__restrict out, const Vec3 *__restrict in, unsigned int count = 1);
	// Vec3 GetX(const Mat4 &m);
	// 	Vec3 GetY(const Mat4 &m);
	// Vec3 GetZ(const Mat4 &m);
	// Vec3 GetT(const Mat4 &m);
	// Vec3 GetTranslation(const Mat4 &m);
	// Vec3 GetR(const Mat4 &m, RotationOrder order = RO_Default);
	// Vec3 GetRotation(const Mat4 &m, RotationOrder order = RO_Default);
	// Mat3 GetRMatrix(const Mat4 &m);
	// Mat3 GetRotationMatrix(const Mat4 &m);
	// Vec3 GetS(const Mat4 &m);
	// Vec3 GetScale(const Mat4 &m);
	// void SetX(Mat4 & m, const Vec3 &X);
	// void SetY(Mat4 & m, const Vec3 &Y);
	// void SetZ(Mat4 & m, const Vec3 &Z);
	// void SetT(Mat4 & m, const Vec3 &T);
	// void SetTranslation(Mat4 & m, const Vec3 &);
	// void SetS(Mat4 & m, const Vec3 &);
	// void SetScale(Mat4 & m, const Vec3 &);
	// void Set(Mat4 & m, const float *v);
	// void Set(Mat4 & m, float m00, float m10, float m20, float m01, float m11, float m21, float m02, float m12, float m22, float m03, float m13, float m23);
	// 	Vec4 GetRow(const Mat4 &m, unsigned int n);
	// Vec3 GetColumn(const Mat4 &m, unsigned int n);
	// void SetRow(Mat4 & m, unsigned int n, const Vec4 &v);
	// void SetColumn(Mat4 & m, unsigned int n, const Vec3 &v);
	// Mat4 LerpAsOrthonormalBase(const Mat4 &a, const Mat4 &b, float k, bool fast = false);
	// Mat4 Mat4LookAt(const Vec3 &p, const Vec3 &at, const Vec3 &s = Vec3::One);
	// Mat4 Mat4LookAtUp(const Vec3 &p, const Vec3 &at, const Vec3 &up, const Vec3 &s = Vec3::One);
	// Mat4 Mat4LookToward(const Vec3 &p, const Vec3 &d, const Vec3 &s = Vec3::One);
	// Mat4 Mat4LookTowardUp(const Vec3 &p, const Vec3 &d, const Vec3 &up, const Vec3 &s = Vec3::One);
	// Mat4 TranslationMat4(const Vec3 &t);
	// Mat4 RotationMat4(const Vec3 &euler, RotationOrder rorder = RO_Default);
	// Mat4 ScaleMat4(const Vec3 &s);
	// Mat4 ScaleMat4(float s);
	// Mat4 TransformationMat4(const Vec3 &p, const Vec3 &r);
	// Mat4 TransformationMat4(const Vec3 &p, const Vec3 &r, const Vec3 &s);
	// Mat4 TransformationMat4(const Vec3 &p, const Mat3 &r);
	// Mat4 TransformationMat4(const Vec3 &p, const Mat3 &r, const Vec3 &s);
	// 	Mat4 Mat4FromFloat16Transposed(const float m[16]);
	// void Mat4ToFloat16Transposed(const Mat4 &m, float t[16]);
	// 	Mat4 ComputeBillboardMat4(const Vec3 &pos, const Mat3 &camera, const Vec3 &scale = Vec3::One);
}
