// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.
#include <math.h>

#define TEST_NO_MAIN 
#include "acutest.h"

#include "foundation/matrix4.h"

#include "foundation/math.h"
#include "foundation/matrix3.h"
#include "foundation/vector3.h"
#include "foundation/vector4.h"

using namespace hg;

void test_mat4() {
	{
		Mat4 m;
		TEST_CHECK(m.m[0][0] == 1.f);
		TEST_CHECK(m.m[1][0] == 0.f);
		TEST_CHECK(m.m[2][0] == 0.f);

		TEST_CHECK(m.m[0][1] == 0.f);
		TEST_CHECK(m.m[1][1] == 1.f);
		TEST_CHECK(m.m[2][1] == 0.f);

		TEST_CHECK(m.m[0][2] == 0.f);
		TEST_CHECK(m.m[1][2] == 0.f);
		TEST_CHECK(m.m[2][2] == 1.f);

		TEST_CHECK(m.m[0][3] == 0.f);
		TEST_CHECK(m.m[1][3] == 0.f);
		TEST_CHECK(m.m[2][3] == 0.f);
	}
	{
		Mat4 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		TEST_CHECK(m.m[0][0] == 1.f);
		TEST_CHECK(m.m[1][0] == 2.f);
		TEST_CHECK(m.m[2][0] == 3.f);

		TEST_CHECK(m.m[0][1] == 4.f);
		TEST_CHECK(m.m[1][1] == 5.f);
		TEST_CHECK(m.m[2][1] == 6.f);

		TEST_CHECK(m.m[0][2] == 7.f);
		TEST_CHECK(m.m[1][2] == 8.f);
		TEST_CHECK(m.m[2][2] == 9.f);

		TEST_CHECK(m.m[0][3] == 10.f);
		TEST_CHECK(m.m[1][3] == 11.f);
		TEST_CHECK(m.m[2][3] == 12.f);
	}
	{
		Mat4 n(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		Mat4 m(n);
		TEST_CHECK(m.m[0][0] == n.m[0][0]);
		TEST_CHECK(m.m[1][0] == n.m[1][0]);
		TEST_CHECK(m.m[2][0] == n.m[2][0]);

		TEST_CHECK(m.m[0][1] == n.m[0][1]);
		TEST_CHECK(m.m[1][1] == n.m[1][1]);
		TEST_CHECK(m.m[2][1] == n.m[2][1]);

		TEST_CHECK(m.m[0][2] == n.m[0][2]);
		TEST_CHECK(m.m[1][2] == n.m[1][2]);
		TEST_CHECK(m.m[2][2] == n.m[2][2]);

		TEST_CHECK(m.m[0][3] == n.m[0][3]);
		TEST_CHECK(m.m[1][3] == n.m[1][3]);
		TEST_CHECK(m.m[2][3] == n.m[2][3]);
	}
	{
		const float n[] = {1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f};
		Mat4 m(n);
		TEST_CHECK(m.m[0][0] == 1.f);
		TEST_CHECK(m.m[1][0] == 2.f);
		TEST_CHECK(m.m[2][0] == 3.f);

		TEST_CHECK(m.m[0][1] == 4.f);
		TEST_CHECK(m.m[1][1] == 5.f);
		TEST_CHECK(m.m[2][1] == 6.f);

		TEST_CHECK(m.m[0][2] == 7.f);
		TEST_CHECK(m.m[1][2] == 8.f);
		TEST_CHECK(m.m[2][2] == 9.f);

		TEST_CHECK(m.m[0][3] == 10.f);
		TEST_CHECK(m.m[1][3] == 11.f);
		TEST_CHECK(m.m[2][3] == 12.f);
	}
	{
		const Mat3 n(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
		Mat4 m(n);
		TEST_CHECK(m.m[0][0] == 1.f);
		TEST_CHECK(m.m[1][0] == 2.f);
		TEST_CHECK(m.m[2][0] == 3.f);

		TEST_CHECK(m.m[0][1] == 4.f);
		TEST_CHECK(m.m[1][1] == 5.f);
		TEST_CHECK(m.m[2][1] == 6.f);

		TEST_CHECK(m.m[0][2] == 7.f);
		TEST_CHECK(m.m[1][2] == 8.f);
		TEST_CHECK(m.m[2][2] == 9.f);

		TEST_CHECK(m.m[0][3] == 0.f);
		TEST_CHECK(m.m[1][3] == 0.f);
		TEST_CHECK(m.m[2][3] == 0.f);
	}
	{ 
		Mat4 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f); 
		Mat4 n(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f); 
		Mat4 p(0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.1f, 1.2f);
		TEST_CHECK(m == n);
		TEST_CHECK((m == p) == false);
	}
	{
		Mat4 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		Mat4 n(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		Mat4 p(0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.1f, 1.2f);
		TEST_CHECK(m != p);
		TEST_CHECK((m != n) == false);
	}
	{
		const Mat4 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		const Mat4 n(0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.1f, 1.2f);
		Mat4 p = m * n;
		TEST_CHECK(AlmostEqual(GetColumn(p, 0), Vec3(3.f, 3.6f, 4.2f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(p, 1), Vec3(6.6f, 8.1f, 9.6f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(p, 2), Vec3(10.2f, 12.6f, 15.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(p, 3), Vec3(23.8f, 28.1f, 32.4f), 0.00001f));
	}
	{
		Mat4 m = Mat4(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f) * 2.f;
		TEST_CHECK(AlmostEqual(GetColumn(m, 0), Vec3(2.f, 4.f, 6.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(m, 1), Vec3(8.f, 10.f, 12.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(m, 2), Vec3(14.f, 16.f, 18.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(m, 3), Vec3(20.f, 22.f, 24.f), 0.00001f));
	}
	{
		Mat4 m = Mat4(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f) / -10.f;
		TEST_CHECK(AlmostEqual(GetColumn(m, 0), Vec3(-0.1f,-0.2f,-0.3f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(m, 1), Vec3(-0.4f,-0.5f,-0.6f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(m, 2), Vec3(-0.7f,-0.8f,-0.9f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(m, 3), Vec3(-1.0f,-1.1f,-1.2f), 0.00001f));
	}
	{
		const Mat4 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		const Mat4 n(0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.1f, 1.2f);
		Mat4 p = m + n;
		TEST_CHECK(AlmostEqual(GetColumn(p, 0), Vec3(1.1f, 2.2f, 3.3f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(p, 1), Vec3(4.4f, 5.5f, 6.6f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(p, 2), Vec3(7.7f, 8.8f, 9.9f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(p, 3), Vec3(11.0f, 12.1f, 13.2f), 0.00001f));
	}
	{
		const Mat4 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		const Mat4 n(0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.0f, 1.1f, 1.2f);
		Mat4 p = m - n;
		TEST_CHECK(AlmostEqual(GetColumn(p, 0), Vec3(0.9f, 1.8f, 2.7f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(p, 1), Vec3(3.6f, 4.5f, 5.4f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(p, 2), Vec3(6.3f, 7.2f, 8.1f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(p, 3), Vec3(9.0f, 9.9f, 10.8f), 0.00001f));
	}
	{
		Vec3 v = Mat4(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f) * Vec3(0.1f, 0.2f, 0.3f);
		TEST_CHECK(TestEqual(v.x, 13.f));
		TEST_CHECK(TestEqual(v.y, 14.6f));
		TEST_CHECK(TestEqual(v.z, 16.2f));
	}
	{
		Vec4 v = Mat4(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f) * Vec4(0.1f,-0.2f, 0.3f,-0.4f);
		TEST_CHECK(TestEqual(v.x, -2.6f));
		TEST_CHECK(TestEqual(v.y, -2.8f));
		TEST_CHECK(TestEqual(v.z, -3.f));
		TEST_CHECK(TestEqual(v.w, -0.4f));
	}
	{
		const Mat4 m(-1.f, 2.f, -3.f, 4.f, -5.f, 6.f, -7.f, 8.f, 9.f, 10.f, -11.f, -12.f);
		Mat4 n;
		bool res = Inverse(m, n);
		TEST_CHECK(AlmostEqual(GetColumn(n, 0), Vec3(31.f, 14.f, 1.f) / 18.f, 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(n, 1), Vec3(26.f, 10.f, 2.f) / 18.f, 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(n, 2), Vec3(1.f, 2.f, 1.f) / 18.f, 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(n, 3), Vec3(-12.f, -6.f, 24.f) / 18.f, 0.00001f));
	
		Mat4 p = m * n;
		TEST_CHECK(AlmostEqual(GetColumn(p, 0), Vec3(1.f, 0.f, 0.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(p, 1), Vec3(0.f, 1.f, 0.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(p, 2), Vec3(0.f, 0.f, 1.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(p, 3), Vec3(0.f, 0.f, 0.f), 0.00001f));
	}
	{
		const Mat4 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		Mat4 n;
		bool res = Inverse(m, n);
		TEST_CHECK(res == false);
		TEST_CHECK(n == Mat4::Identity);
	}
	{
		const float u = 2.f * sqrt(2.f);
		const Mat4 m(u, 0.f, -u, 2.f, u, 2.f, 2.f, -u, 2.f, -1.f, 0.5f, 2.f);
		Mat4 n = m * InverseFast(m);
		TEST_CHECK(AlmostEqual(GetColumn(n, 0), Vec3(1.f, 0.f, 0.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(n, 1), Vec3(0.f, 1.f, 0.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(n, 2), Vec3(0.f, 0.f, 1.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(n, 3), Vec3(0.f, 0.f, 0.f), 0.00001f));
	}
	{ 
		Mat4 m = Orthonormalize(Mat4(-1.f, 2.f, -3.f, 4.f, -5.f, 6.f, -7.f, 8.f, 9.f, 10.f, -11.f, -12.f));
		Vec3 i = GetX(m), j = GetY(m), k = GetZ(m), t = GetT(m);
		TEST_CHECK(TestEqual(Dot(i, j), 0.f));
		TEST_CHECK(TestEqual(Dot(j, k), 0.f));
		TEST_CHECK(TestEqual(Dot(k, i), 0.f));
		TEST_CHECK(TestEqual(Len2(i), 1.f));
		TEST_CHECK(TestEqual(Len2(j), 1.f));
		TEST_CHECK(TestEqual(Len2(k), 1.f));
		TEST_CHECK(AlmostEqual(t, Vec3(10.f, -11.f, -12.f), 0.000001f));
	}
	{ 
		Mat4 m = Normalize(Mat4(-1.f, 2.f, -3.f, 4.f, -5.f, 6.f, -7.f, 8.f, 9.f, 10.f, -11.f, -12.f));
		Vec3 i = GetX(m), j = GetY(m), k = GetZ(m), t = GetT(m);
		TEST_CHECK(TestEqual(Len2(i), 1.f));
		TEST_CHECK(TestEqual(Len2(j), 1.f));
		TEST_CHECK(TestEqual(Len2(k), 1.f));
		TEST_CHECK(AlmostEqual(t, Vec3(10.f, -11.f, -12.f), 0.000001f));
	}
    {
        const Vec3 u0 = Deg3(30.f, 45.f, 60.f);
        const Vec3 s0(2.f, 3.f, 4.f);
        const Vec3 t0(-4.f, 5.f, -3.f);
        Mat3 r0 = RotationMat3(u0, RO_ZYX);
        Mat4 m = TranslationMat4(t0) * Mat4(r0 * ScaleMat3(s0));
        Vec3 s, t;
        Mat3 r;
        Decompose(m, &t, &r, &s);
		TEST_CHECK(AlmostEqual(t, t0, 0.000001f));
		TEST_CHECK(AlmostEqual(s, s0, 0.000001f));
		TEST_CHECK(AlmostEqual(GetColumn(r, 0), GetColumn(r0, 0), 0.000001f));
		TEST_CHECK(AlmostEqual(GetColumn(r, 1), GetColumn(r0, 1), 0.000001f));
		TEST_CHECK(AlmostEqual(GetColumn(r, 2), GetColumn(r0, 2), 0.000001f));
    }
// void TransformVec3(const Mat4 &__restrict m, Vec3 *__restrict out, const Vec3 *__restrict in, unsigned int count = 1);
// void TransformVec3(const Mat4 &__restrict m, Vec4 *__restrict out, const Vec3 *__restrict in, unsigned int count = 1);
// void RotateVec3(const Mat4 &__restrict m, Vec3 *__restrict out, const Vec3 *__restrict in, unsigned int count = 1);
	{
		Mat4 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		TEST_CHECK(AlmostEqual(GetX(m), Vec3(1.f, 2.f, 3.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetY(m), Vec3(4.f, 5.f, 6.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetZ(m), Vec3(7.f, 8.f, 9.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetT(m), Vec3(10.f, 11.f, 12.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetTranslation(m), Vec3(10.f, 11.f, 12.f), 0.00001f));
	}
	{
		const Vec3 u = Deg3(30.f, 45.f, 60.f);
        TEST_CHECK(AlmostEqual(GetR(RotationMat4(u, RO_ZYX), RO_ZYX), u, 0.000001f));
        TEST_CHECK(AlmostEqual(GetR(RotationMat4(u, RO_YZX), RO_YZX), u, 0.000001f));
        TEST_CHECK(AlmostEqual(GetR(RotationMat4(u, RO_ZXY), RO_ZXY), u, 0.000001f));
        TEST_CHECK(AlmostEqual(GetR(RotationMat4(u, RO_XZY), RO_XZY), u, 0.000001f));
        TEST_CHECK(AlmostEqual(GetR(RotationMat4(u, RO_YXZ), RO_YXZ), u, 0.000001f));
        TEST_CHECK(AlmostEqual(GetR(RotationMat4(u, RO_XYZ), RO_XYZ), u, 0.000001f));
        TEST_CHECK(AlmostEqual(GetR(RotationMat4(u, RO_XY), RO_XY), u, 0.000001f));

        TEST_CHECK(AlmostEqual(GetRotation(RotationMat4(u, RO_ZYX), RO_ZYX), u, 0.000001f));
        TEST_CHECK(AlmostEqual(GetRotation(RotationMat4(u, RO_YZX), RO_YZX), u, 0.000001f));
        TEST_CHECK(AlmostEqual(GetRotation(RotationMat4(u, RO_ZXY), RO_ZXY), u, 0.000001f));
        TEST_CHECK(AlmostEqual(GetRotation(RotationMat4(u, RO_XZY), RO_XZY), u, 0.000001f));
        TEST_CHECK(AlmostEqual(GetRotation(RotationMat4(u, RO_YXZ), RO_YXZ), u, 0.000001f));
        TEST_CHECK(AlmostEqual(GetRotation(RotationMat4(u, RO_XYZ), RO_XYZ), u, 0.000001f));
        TEST_CHECK(AlmostEqual(GetRotation(RotationMat4(u, RO_XY), RO_XY), u, 0.000001f));
	}
// Mat3 GetRMatrix(const Mat4 &m);
// Mat3 GetRotationMatrix(const Mat4 &m);
// Vec3 GetS(const Mat4 &m);
// Vec3 GetScale(const Mat4 &m);
	{
		Mat4 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		SetX(m, Vec3(-0.1f,-0.2f,-0.3f));
		TEST_CHECK(AlmostEqual(GetX(m), Vec3(-0.1f,-0.2f,-0.3f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetY(m), Vec3(4.f, 5.f, 6.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetZ(m), Vec3(7.f, 8.f, 9.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetT(m), Vec3(10.f, 11.f, 12.f), 0.00001f));
	}
	{
		Mat4 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		SetY(m, Vec3(-0.1f,-0.2f,-0.3f));
		TEST_CHECK(AlmostEqual(GetX(m), Vec3(1.f, 2.f, 3.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetY(m), Vec3(-0.1f,-0.2f,-0.3f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetZ(m), Vec3(7.f, 8.f, 9.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetT(m), Vec3(10.f, 11.f, 12.f), 0.00001f));
	}
	{
		Mat4 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		SetZ(m, Vec3(-0.1f,-0.2f,-0.3f));
		TEST_CHECK(AlmostEqual(GetX(m), Vec3(1.f, 2.f, 3.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetY(m), Vec3(4.f, 5.f, 6.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetZ(m), Vec3(-0.1f,-0.2f,-0.3f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetT(m), Vec3(10.f, 11.f, 12.f), 0.00001f));
	}
	{
		Mat4 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		SetT(m, Vec3(-0.1f,-0.2f,-0.3f));
		TEST_CHECK(AlmostEqual(GetX(m), Vec3(1.f, 2.f, 3.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetY(m), Vec3(4.f, 5.f, 6.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetZ(m), Vec3(7.f, 8.f, 9.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetT(m), Vec3(-0.1f,-0.2f,-0.3f), 0.00001f));
	}
	{
		Mat4 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		SetTranslation(m, Vec3(-0.1f,-0.2f,-0.3f));
		TEST_CHECK(AlmostEqual(GetX(m), Vec3(1.f, 2.f, 3.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetY(m), Vec3(4.f, 5.f, 6.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetZ(m), Vec3(7.f, 8.f, 9.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetT(m), Vec3(-0.1f,-0.2f,-0.3f), 0.00001f));
	}
	{
		Mat4 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		SetS(m, Vec3(0.1f, 10.0f, 2.0f));
		TEST_CHECK(TestEqual(Len(GetX(m)), 0.1f));
		TEST_CHECK(TestEqual(Len(GetY(m)), 10.0f));
		TEST_CHECK(TestEqual(Len(GetZ(m)), 2.0f));
		TEST_CHECK(AlmostEqual(GetT(m), Vec3(10.f, 11.f, 12.f), 0.00001f));
	}
	{
		Mat4 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		SetScale(m, Vec3(3.0f, 0.5f, 11.0f));
		TEST_CHECK(TestEqual(Len(GetX(m)), 3.0f));
		TEST_CHECK(TestEqual(Len(GetY(m)), 0.5f));
		TEST_CHECK(TestEqual(Len(GetZ(m)), 11.0f));
		TEST_CHECK(AlmostEqual(GetT(m), Vec3(10.f, 11.f, 12.f), 0.00001f));
	}
	{
		const float n[] = {1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f};
		Mat4 m(0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.f, 1.1f, 1.2f);
		Set(m, n);
		TEST_CHECK(m.m[0][0] == 1.f);
		TEST_CHECK(m.m[1][0] == 2.f);
		TEST_CHECK(m.m[2][0] == 3.f);

		TEST_CHECK(m.m[0][1] == 4.f);
		TEST_CHECK(m.m[1][1] == 5.f);
		TEST_CHECK(m.m[2][1] == 6.f);

		TEST_CHECK(m.m[0][2] == 7.f);
		TEST_CHECK(m.m[1][2] == 8.f);
		TEST_CHECK(m.m[2][2] == 9.f);

		TEST_CHECK(m.m[0][3] == 10.f);
		TEST_CHECK(m.m[1][3] == 11.f);
		TEST_CHECK(m.m[2][3] == 12.f);
	}
	{
		Mat4 m(0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.f, 1.1f, 1.2f);
		Set(m, 1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		TEST_CHECK(m.m[0][0] == 1.f);
		TEST_CHECK(m.m[1][0] == 2.f);
		TEST_CHECK(m.m[2][0] == 3.f);

		TEST_CHECK(m.m[0][1] == 4.f);
		TEST_CHECK(m.m[1][1] == 5.f);
		TEST_CHECK(m.m[2][1] == 6.f);

		TEST_CHECK(m.m[0][2] == 7.f);
		TEST_CHECK(m.m[1][2] == 8.f);
		TEST_CHECK(m.m[2][2] == 9.f);

		TEST_CHECK(m.m[0][3] == 10.f);
		TEST_CHECK(m.m[1][3] == 11.f);
		TEST_CHECK(m.m[2][3] == 12.f);
	}
	{
		Mat4 m = Mat4(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		TEST_CHECK(AlmostEqual(GetRow(m, 0), Vec4(1.f, 4.f, 7.f, 10.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetRow(m, 1), Vec4(2.f, 5.f, 8.f, 11.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetRow(m, 2), Vec4(3.f, 6.f, 9.f, 12.f), 0.00001f));
	}
	{
		Mat4 m = Mat4(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
		TEST_CHECK(AlmostEqual(GetColumn(m, 0), Vec3(1.f, 2.f, 3.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(m, 1), Vec3(4.f, 5.f, 6.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(m, 2), Vec3(7.f, 8.f, 9.f), 0.00001f));
		TEST_CHECK(AlmostEqual(GetColumn(m, 3), Vec3(10.f, 11.f, 12.f), 0.00001f));
	}
	{
		{
			Mat4 m = Mat4(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
			SetRow(m, 0, Vec4(0.1f, 0.2f, 0.3f, 0.4f));
			TEST_CHECK(AlmostEqual(GetRow(m, 0), Vec4(0.1f, 0.2f, 0.3f, 0.4f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetRow(m, 1), Vec4(2.f, 5.f, 8.f, 11.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetRow(m, 2), Vec4(3.f, 6.f, 9.f, 12.f), 0.00001f));
		}
		{
			Mat4 m = Mat4(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
			SetRow(m, 1, Vec4(0.1f, 0.2f, 0.3f, 0.4f));
			TEST_CHECK(AlmostEqual(GetRow(m, 0), Vec4(1.f, 4.f, 7.f, 10.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetRow(m, 1), Vec4(0.1f, 0.2f, 0.3f, 0.4f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetRow(m, 2), Vec4(3.f, 6.f, 9.f, 12.f), 0.00001f));
		}
		{
			Mat4 m = Mat4(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
			SetRow(m, 2, Vec4(0.1f, 0.2f, 0.3f, 0.4f));
			TEST_CHECK(AlmostEqual(GetRow(m, 0), Vec4(1.f, 4.f, 7.f, 10.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetRow(m, 1), Vec4(2.f, 5.f, 8.f, 11.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetRow(m, 2), Vec4(0.1f, 0.2f, 0.3f, 0.4f), 0.00001f));
		}
	}
	{
		{
			Mat4 m = Mat4(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
			SetColumn(m, 0, Vec3(-0.1f, -0.2f, -0.3f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 0), Vec3(-0.1f, -0.2f, -0.3f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 1), Vec3(4.f, 5.f, 6.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 2), Vec3(7.f, 8.f, 9.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 3), Vec3(10.f, 11.f, 12.f), 0.00001f));
		}
		{
			Mat4 m = Mat4(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
			SetColumn(m, 1, Vec3(-0.1f, -0.2f, -0.3f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 0), Vec3(1.f, 2.f, 3.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 1), Vec3(-0.1f, -0.2f, -0.3f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 2), Vec3(7.f, 8.f, 9.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 3), Vec3(10.f, 11.f, 12.f), 0.00001f));
		}
		{
			Mat4 m = Mat4(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
			SetColumn(m, 2, Vec3(-0.1f, -0.2f, -0.3f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 0), Vec3(1.f, 2.f, 3.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 1), Vec3(4.f, 5.f, 6.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 2), Vec3(-0.1f, -0.2f, -0.3f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 3), Vec3(10.f, 11.f, 12.f), 0.00001f));
		}
		{
			Mat4 m = Mat4(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f);
			SetColumn(m, 3, Vec3(-0.1f, -0.2f, -0.3f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 0), Vec3(1.f, 2.f, 3.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 1), Vec3(4.f, 5.f, 6.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 2), Vec3(7.f, 8.f, 9.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 3), Vec3(-0.1f, -0.2f, -0.3f), 0.00001f));
		}
	}
// Mat4 LerpAsOrthonormalBase(const Mat4 &a, const Mat4 &b, float k, bool fast = false);
// Mat4 Mat4LookAt(const Vec3 &p, const Vec3 &at, const Vec3 &s = Vec3::One);
// Mat4 Mat4LookAtUp(const Vec3 &p, const Vec3 &at, const Vec3 &up, const Vec3 &s = Vec3::One);
// Mat4 Mat4LookToward(const Vec3 &p, const Vec3 &d, const Vec3 &s = Vec3::One);
// Mat4 Mat4LookTowardUp(const Vec3 &p, const Vec3 &d, const Vec3 &up, const Vec3 &s = Vec3::One);
		{
			const Vec3 v(-0.1f, -0.2f, -0.3f);
			Mat4 m = TranslationMat4(v);
			TEST_CHECK(AlmostEqual(GetColumn(m, 0), Vec3(1.f, 0.f, 0.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 1), Vec3(0.f, 1.f, 0.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 2), Vec3(0.f, 0.f, 1.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 3), v, 0.00001f));
		}
		{
			const Vec3 u = Deg3(30.f, 45.f, 60.f);
			{
				const Mat3 r = RotationMat3(u, RO_ZYX);
				Mat4 m = RotationMat4(u, RO_ZYX);
				TEST_CHECK(AlmostEqual(GetColumn(m, 0), GetColumn(r, 0), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 1), GetColumn(r, 1), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 2), GetColumn(r, 2), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 3), Vec3(0.f, 0.f, 0.f), 0.00001f));
			}
			{
				const Mat3 r = RotationMat3(u, RO_YZX);
				Mat4 m = RotationMat4(u, RO_YZX);
				TEST_CHECK(AlmostEqual(GetColumn(m, 0), GetColumn(r, 0), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 1), GetColumn(r, 1), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 2), GetColumn(r, 2), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 3), Vec3(0.f, 0.f, 0.f), 0.00001f));
			}
			{
				const Mat3 r = RotationMat3(u, RO_ZXY);
				Mat4 m = RotationMat4(u, RO_ZXY);
				TEST_CHECK(AlmostEqual(GetColumn(m, 0), GetColumn(r, 0), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 1), GetColumn(r, 1), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 2), GetColumn(r, 2), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 3), Vec3(0.f, 0.f, 0.f), 0.00001f));
			}
			{
				const Mat3 r = RotationMat3(u, RO_XZY);
				Mat4 m = RotationMat4(u, RO_XZY);
				TEST_CHECK(AlmostEqual(GetColumn(m, 0), GetColumn(r, 0), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 1), GetColumn(r, 1), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 2), GetColumn(r, 2), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 3), Vec3(0.f, 0.f, 0.f), 0.00001f));
			}
			{
				const Mat3 r = RotationMat3(u, RO_YXZ);
				Mat4 m = RotationMat4(u, RO_YXZ);
				TEST_CHECK(AlmostEqual(GetColumn(m, 0), GetColumn(r, 0), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 1), GetColumn(r, 1), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 2), GetColumn(r, 2), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 3), Vec3(0.f, 0.f, 0.f), 0.00001f));
			}
			{
				const Mat3 r = RotationMat3(u, RO_XYZ);
				Mat4 m = RotationMat4(u, RO_XYZ);
				TEST_CHECK(AlmostEqual(GetColumn(m, 0), GetColumn(r, 0), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 1), GetColumn(r, 1), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 2), GetColumn(r, 2), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 3), Vec3(0.f, 0.f, 0.f), 0.00001f));
			}
			{
				const Mat3 r = RotationMat3(u, RO_XY);
				Mat4 m = RotationMat4(u, RO_XY);
				TEST_CHECK(AlmostEqual(GetColumn(m, 0), GetColumn(r, 0), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 1), GetColumn(r, 1), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 2), GetColumn(r, 2), 0.00001f));
				TEST_CHECK(AlmostEqual(GetColumn(m, 3), Vec3(0.f, 0.f, 0.f), 0.00001f));
			}
		}
		{
			const Vec3 v(-0.1f, -0.2f, -0.3f);
			Mat4 m = ScaleMat4(v);
			TEST_CHECK(AlmostEqual(GetColumn(m, 0), Vec3(-0.1f, 0.f, 0.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 1), Vec3(0.f,-0.2f, 0.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 2), Vec3(0.f, 0.f,-0.3f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 3), Vec3::Zero, 0.00001f));
		}
		{
			Mat4 m = ScaleMat4(3.1);
			TEST_CHECK(AlmostEqual(GetColumn(m, 0), Vec3(3.1f, 0.f, 0.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 1), Vec3(0.f,3.1f, 0.f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 2), Vec3(0.f, 0.f,3.1f), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m, 3), Vec3::Zero, 0.00001f));
		}
// Mat4 TransformationMat4(const Vec3 &p, const Vec3 &r);
// Mat4 TransformationMat4(const Vec3 &p, const Vec3 &r, const Vec3 &s);
// Mat4 TransformationMat4(const Vec3 &p, const Mat3 &r);
// Mat4 TransformationMat4(const Vec3 &p, const Mat3 &r, const Vec3 &s);
// Mat4 Mat4FromFloat16Transposed(const float m[16]);
// void Mat4ToFloat16Transposed(const Mat4 &m, float t[16]);
// Mat4 ComputeBillboardMat4(const Vec3 &pos, const Mat3 &camera, const Vec3 &scale = Vec3::One);
}
