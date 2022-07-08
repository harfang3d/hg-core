// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.
#include <math.h>

#define TEST_NO_MAIN 
#include "acutest.h"

#include "foundation/matrix3.h"

#include "foundation/math.h"
#include "foundation/vector2.h"
#include "foundation/vector3.h"
#include "foundation/vector4.h"
#include "foundation/matrix4.h"

using namespace hg;

void test_mat3() {
	{
		Mat3 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
		TEST_CHECK(TestEqual(m.m[0][0], 1.f));
		TEST_CHECK(TestEqual(m.m[1][0], 2.f));
		TEST_CHECK(TestEqual(m.m[2][0], 3.f));

		TEST_CHECK(TestEqual(m.m[0][1], 4.f));
		TEST_CHECK(TestEqual(m.m[1][1], 5.f));
		TEST_CHECK(TestEqual(m.m[2][1], 6.f));

		TEST_CHECK(TestEqual(m.m[0][2], 7.f));
		TEST_CHECK(TestEqual(m.m[1][2], 8.f));
		TEST_CHECK(TestEqual(m.m[2][2], 9.f));
	}
	{
		const float raw[] = {1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f};
		Mat3 m(raw);
		TEST_CHECK(TestEqual(m.m[0][0], 1.f));
		TEST_CHECK(TestEqual(m.m[1][0], 2.f));
		TEST_CHECK(TestEqual(m.m[2][0], 3.f));

		TEST_CHECK(TestEqual(m.m[0][1], 4.f));
		TEST_CHECK(TestEqual(m.m[1][1], 5.f));
		TEST_CHECK(TestEqual(m.m[2][1], 6.f));

		TEST_CHECK(TestEqual(m.m[0][2], 7.f));
		TEST_CHECK(TestEqual(m.m[1][2], 8.f));
		TEST_CHECK(TestEqual(m.m[2][2], 9.f));
	}
	{
		const Vec3 u(1.f, 2.f, 3.f);
		const Vec3 v(4.f, 5.f, 6.f);
		const Vec3 w(7.f, 8.f, 9.f);
		Mat3 m(u, v, w);
		TEST_CHECK(TestEqual(m.m[0][0], u.x));
		TEST_CHECK(TestEqual(m.m[1][0], u.y));
		TEST_CHECK(TestEqual(m.m[2][0], u.z));

		TEST_CHECK(TestEqual(m.m[0][1], v.x));
		TEST_CHECK(TestEqual(m.m[1][1], v.y));
		TEST_CHECK(TestEqual(m.m[2][1], v.z));

		TEST_CHECK(TestEqual(m.m[0][2], w.x));
		TEST_CHECK(TestEqual(m.m[1][2], w.y));
		TEST_CHECK(TestEqual(m.m[2][2], w.z));
	}
	{
		Mat3 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
		m += Mat3(0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f);

		TEST_CHECK(TestEqual(m.m[0][0], 1.1f));
		TEST_CHECK(TestEqual(m.m[1][0], 2.2f));
		TEST_CHECK(TestEqual(m.m[2][0], 3.3f));

		TEST_CHECK(TestEqual(m.m[0][1], 4.4f));
		TEST_CHECK(TestEqual(m.m[1][1], 5.5f));
		TEST_CHECK(TestEqual(m.m[2][1], 6.6f));

		TEST_CHECK(TestEqual(m.m[0][2], 7.7f));
		TEST_CHECK(TestEqual(m.m[1][2], 8.8f));
		TEST_CHECK(TestEqual(m.m[2][2], 9.9f));
	}
	{
		Mat3 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
		m -= Mat3(0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f);

		TEST_CHECK(TestEqual(m.m[0][0], 0.9f));
		TEST_CHECK(TestEqual(m.m[1][0], 1.8f));
		TEST_CHECK(TestEqual(m.m[2][0], 2.7f));

		TEST_CHECK(TestEqual(m.m[0][1], 3.6f));
		TEST_CHECK(TestEqual(m.m[1][1], 4.5f));
		TEST_CHECK(TestEqual(m.m[2][1], 5.4f));

		TEST_CHECK(TestEqual(m.m[0][2], 6.3f));
		TEST_CHECK(TestEqual(m.m[1][2], 7.2f));
		TEST_CHECK(TestEqual(m.m[2][2], 8.1f));
	}
	{
		Mat3 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
		m *= 0.1f;

		TEST_CHECK(TestEqual(m.m[0][0], 0.1f));
		TEST_CHECK(TestEqual(m.m[1][0], 0.2f));
		TEST_CHECK(TestEqual(m.m[2][0], 0.3f));

		TEST_CHECK(TestEqual(m.m[0][1], 0.4f));
		TEST_CHECK(TestEqual(m.m[1][1], 0.5f));
		TEST_CHECK(TestEqual(m.m[2][1], 0.6f));

		TEST_CHECK(TestEqual(m.m[0][2], 0.7f));
		TEST_CHECK(TestEqual(m.m[1][2], 0.8f));
		TEST_CHECK(TestEqual(m.m[2][2], 0.9f));
	}
	{
		Mat3 m(-0.1f, 0.4f, -0.7f, 0.2f, -0.5f, 0.8f, -0.3f, 0.6f, -0.9f);
		m *= Mat3(-1.0f, 2.0f, 3.0f, 4.0f, -5.0f, 6.0f, 7.0f, 8.0f, -9.0f);
		
		TEST_CHECK(TestEqual(m.m[0][0],-0.4f));
		TEST_CHECK(TestEqual(m.m[1][0], 0.4f));
		TEST_CHECK(TestEqual(m.m[2][0],-0.4f));

		TEST_CHECK(TestEqual(m.m[0][1],-3.2f));
		TEST_CHECK(TestEqual(m.m[1][1], 7.7f));
		TEST_CHECK(TestEqual(m.m[2][1],-12.2f));

		TEST_CHECK(TestEqual(m.m[0][2], 3.6f));
		TEST_CHECK(TestEqual(m.m[1][2],-6.6f));
		TEST_CHECK(TestEqual(m.m[2][2], 9.6f));
	}
	{
		const Mat3 m0 = Mat3(0.1f, 0.4f, 0.7f, 0.2f, 0.5f, 0.8f, 0.3f, 0.6f, 0.9f);
		const Mat3 m1 = m0 * Mat3::Identity;
		const Mat3 m2 = Mat3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
	
		TEST_CHECK(m0 == m1);
		TEST_CHECK((m0 == m2) == false);
		TEST_CHECK((m1 == m2) == false);
	}
	{
		const Mat3 m0 = Mat3(0.1f, 0.4f, 0.7f, 0.2f, 0.5f, 0.8f, 0.3f, 0.6f, 0.9f);
		const Mat3 m1 = Mat3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
		const Mat3 m2 = m0 * Mat3::Identity;
		
		TEST_CHECK(m0 != m1);
		TEST_CHECK((m0 != m2) == false);
	}
	{
		Mat3 m = Mat3(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f) + Mat3(0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f);

		TEST_CHECK(TestEqual(m.m[0][0], 1.1f));
		TEST_CHECK(TestEqual(m.m[1][0], 2.2f));
		TEST_CHECK(TestEqual(m.m[2][0], 3.3f));

		TEST_CHECK(TestEqual(m.m[0][1], 4.4f));
		TEST_CHECK(TestEqual(m.m[1][1], 5.5f));
		TEST_CHECK(TestEqual(m.m[2][1], 6.6f));

		TEST_CHECK(TestEqual(m.m[0][2], 7.7f));
		TEST_CHECK(TestEqual(m.m[1][2], 8.8f));
		TEST_CHECK(TestEqual(m.m[2][2], 9.9f));
	}
	{
		Mat3 m = Mat3(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f) - Mat3(0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f);

		TEST_CHECK(TestEqual(m.m[0][0], 0.9f));
		TEST_CHECK(TestEqual(m.m[1][0], 1.8f));
		TEST_CHECK(TestEqual(m.m[2][0], 2.7f));

		TEST_CHECK(TestEqual(m.m[0][1], 3.6f));
		TEST_CHECK(TestEqual(m.m[1][1], 4.5f));
		TEST_CHECK(TestEqual(m.m[2][1], 5.4f));

		TEST_CHECK(TestEqual(m.m[0][2], 6.3f));
		TEST_CHECK(TestEqual(m.m[1][2], 7.2f));
		TEST_CHECK(TestEqual(m.m[2][2], 8.1f));
	}
	{
		Mat3 m = Mat3(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f) * 0.1f;

		TEST_CHECK(TestEqual(m.m[0][0], 0.1f));
		TEST_CHECK(TestEqual(m.m[1][0], 0.2f));
		TEST_CHECK(TestEqual(m.m[2][0], 0.3f));

		TEST_CHECK(TestEqual(m.m[0][1], 0.4f));
		TEST_CHECK(TestEqual(m.m[1][1], 0.5f));
		TEST_CHECK(TestEqual(m.m[2][1], 0.6f));

		TEST_CHECK(TestEqual(m.m[0][2], 0.7f));
		TEST_CHECK(TestEqual(m.m[1][2], 0.8f));
		TEST_CHECK(TestEqual(m.m[2][2], 0.9f));
	}
	{
		const Mat3 m0 = Mat3(0.1f, 0.4f, 0.7f, 0.2f, 0.5f, 0.8f, 0.3f, 0.6f, 0.9f);
		const Mat3 m1 = Mat3(1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f);
		Mat3 m = m0 * m1;

		TEST_CHECK(TestEqual(m.m[0][0], 1.4f));
		TEST_CHECK(TestEqual(m.m[1][0], 3.2f));
		TEST_CHECK(TestEqual(m.m[2][0], 5.0f));

		TEST_CHECK(TestEqual(m.m[0][1], 3.2f));
		TEST_CHECK(TestEqual(m.m[1][1], 7.7f));
		TEST_CHECK(TestEqual(m.m[2][1], 12.2f));

		TEST_CHECK(TestEqual(m.m[0][2], 5.0f));
		TEST_CHECK(TestEqual(m.m[1][2], 12.2f));
		TEST_CHECK(TestEqual(m.m[2][2], 19.4f));
	}
	{
		Vec3 v = Mat3(0.1f, -0.4f, 0.7f, -0.2f, 0.5f, -0.8f, 0.3f, -0.6f, 0.9f) * Vec3(2.0f, 3.0f, 5.0f);
		
		TEST_CHECK(TestEqual(v.x, 1.1f));
		TEST_CHECK(TestEqual(v.y, -2.3f));
		TEST_CHECK(TestEqual(v.z, 3.5f));
	}
	{
		Vec4 v = Mat3(-0.1f, 0.4f, -0.7f, 0.2f, -0.5f, 0.8f, -0.3f, 0.6f, -0.9f) * Vec4(1.0f, -1.0f, 2.0f, 6.f);

		TEST_CHECK(TestEqual(v.x, -0.9f));
		TEST_CHECK(TestEqual(v.y, 2.1f));
		TEST_CHECK(TestEqual(v.z, -3.3f));
		TEST_CHECK(TestEqual(v.w, 6.0f));
	}

// void TransformVec2(const Mat3 &__restrict m, tVec2<float> *__restrict out, const tVec2<float> *__restrict in, int count = 1);
// void TransformVec3(const Mat3 &__restrict m, Vec3 *__restrict out, const Vec3 *__restrict in, int count = 1);
// void TransformVec4(const Mat3 &__restrict m, Vec4 *__restrict out, const Vec4 *__restrict in, int count = 1);
	{
		const Mat3 m0(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
		const Mat3 m1(7.f, 5.f, 6.f, 4.f, 2.f, 9.f, 1.f, 8.f, 3.f);
		TEST_CHECK(Det(m0) == 0.f);
		TEST_CHECK(Det(m1) == -297.f);
	}
	{
		const Mat3 m0(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
		const Mat3 m1(7.f, 5.f, 6.f, 4.f, 2.f, 9.f, 1.f, 8.f, 3.f);
		const Mat3 n = Mat3(22.f, -11.f, -11.f, 1.f, -5.f, 13.f, -10.f, 17.f, 2.f) / 99.f;
		Mat3 w;
		TEST_CHECK(Inverse(m0, w) == false);
		TEST_CHECK(Inverse(m1, w) == true);
		TEST_CHECK(w == n);
	}
	TEST_CHECK(Transpose(Mat3(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f)) == Mat3(1.f, 4.f, 7.f, 2.f, 5.f, 8.f, 3.f, 6.f, 9.f));
	{
		const float cs = Cos(Pi / 3.f);
		const float sn = Sin(Pi / 3.f);
		const Mat3 rx(1.f, 0.f, 0.f, 0.f, cs, sn, 0.f, -sn, cs);
		const Mat3 ry(cs, 0.f, -sn, 0.f, 1.f, 0.f, sn, 0.f, cs);
		const Mat3 rz(cs, sn, 0.f, -sn, cs, 0.f, 0.f, 0.f, 1.f);
		TEST_CHECK(RotationMatX(Pi / 3.f) == rx);
		TEST_CHECK(RotationMatY(Pi / 3.f) == ry);
		TEST_CHECK(RotationMatZ(Pi / 3.f) == rz);
    }
// Mat3 RotationMatXZY(float x, float y, float z);
// Mat3 RotationMatZYX(float x, float y, float z);
// Mat3 RotationMatXYZ(float x, float y, float z);
// Mat3 RotationMatZXY(float x, float y, float z);
// Mat3 RotationMatYZX(float x, float y, float z);
// Mat3 RotationMatYXZ(float x, float y, float z);
// Mat3 RotationMatXY(float x, float y);
	TEST_CHECK(VectorMat3(Vec3(1.f, -2.f, -3.f)) == Mat3(1.f, 0.f, 0.f, -2.f, 0.f, 0.f, -3.f, 0.f, 0.f));
	TEST_CHECK(TranslationMat3(Vec2(2.f, -3.f)) == Mat3(1.f, 0.f, 0.f, 0.f, 1.f, 0.f, 2.f, -3.f, 1.f));
	TEST_CHECK(TranslationMat3(Vec3(-1.f, 2.f, -5.f)) == Mat3(1.f, 0.f, 0.f, 0.f, 1.f, 0.f, -1.f, 2.f, 1.f));
	TEST_CHECK(ScaleMat3(Vec2(-2.f, 3.f)) == Mat3(-2.f, 0.f, 0.f, 0.f, 3.f, 0.f, 0.f, 0.f, 1.f));
	TEST_CHECK(ScaleMat3(Vec3(0.5f, -1.f, -0.33f)) == Mat3(0.5f, 0.f, 0.f, 0.f, -1.f, 0.f, 0.f, 0.f, -0.33f));
	{
		{
			const Vec3 u(2.02f, -1.5151f, 0.997f);
			const Vec3 v(2.4042f, -0.67f, 0.789f);
			Vec3 w0 = Cross(u, v);
			Vec3 w1 = CrossProductMat3(u) * v;

			TEST_CHECK(TestEqual(w0.x, w1.x));
			TEST_CHECK(TestEqual(w0.y, w1.y));
			TEST_CHECK(TestEqual(w0.z, w1.z));
		}
		{
			const Vec3 u(-1.f, 1.f, 1.f);
			const Vec3 v(1.f, -1.f, -1.f);
			Vec3 w0 = Cross(u, v);
			Vec3 w1 = CrossProductMat3(u) * v;

			TEST_CHECK(TestEqual(w0.x, w1.x));
			TEST_CHECK(TestEqual(w0.y, w1.y));
			TEST_CHECK(TestEqual(w0.z, w1.z));
		}	
	}
// Mat3 RotationMat2D(float angle, const tVec2<float> &pivot);
// Mat3 RotationMat3(float x = 0, float y = 0, float z = 0, RotationOrder order = RO_Default);
// Mat3 RotationMat3(const Vec3 &euler, RotationOrder order = RO_Default);
// Mat3 Mat3LookAt(const Vec3 &front);
// Mat3 Mat3LookAt(const Vec3 &front, const Vec3 &up);
	{
		Mat3 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
		Vec3 u = GetRow(m, 0);
		Vec3 v = GetRow(m, 1);
		Vec3 w = GetRow(m, 2);

		TEST_CHECK(TestEqual(u.x, 1.f));
		TEST_CHECK(TestEqual(v.x, 2.f));
		TEST_CHECK(TestEqual(w.x, 3.f));

		TEST_CHECK(TestEqual(u.y, 4.f));
		TEST_CHECK(TestEqual(v.y, 5.f));
		TEST_CHECK(TestEqual(w.y, 6.f));

		TEST_CHECK(TestEqual(u.z, 7.f));
		TEST_CHECK(TestEqual(v.z, 8.f));
		TEST_CHECK(TestEqual(w.z, 9.f));
	}
	{
		Mat3 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
		Vec3 u = GetColumn(m, 0);
		Vec3 v = GetColumn(m, 1);
		Vec3 w = GetColumn(m, 2);

		TEST_CHECK(TestEqual(u.x, 1.f));
		TEST_CHECK(TestEqual(u.y, 2.f));
		TEST_CHECK(TestEqual(u.z, 3.f));

		TEST_CHECK(TestEqual(v.x, 4.f));
		TEST_CHECK(TestEqual(v.y, 5.f));
		TEST_CHECK(TestEqual(v.z, 6.f));

		TEST_CHECK(TestEqual(w.x, 7.f));
		TEST_CHECK(TestEqual(w.y, 8.f));
		TEST_CHECK(TestEqual(w.z, 9.f));
	}
	{
		Mat3 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
		Vec3 u = GetX(m);
		Vec3 v = GetY(m);
		Vec3 w = GetZ(m);

		TEST_CHECK(TestEqual(u.x, 1.f));
		TEST_CHECK(TestEqual(u.y, 2.f));
		TEST_CHECK(TestEqual(u.z, 3.f));

		TEST_CHECK(TestEqual(v.x, 4.f));
		TEST_CHECK(TestEqual(v.y, 5.f));
		TEST_CHECK(TestEqual(v.z, 6.f));

		TEST_CHECK(TestEqual(w.x, 7.f));
		TEST_CHECK(TestEqual(w.y, 8.f));
		TEST_CHECK(TestEqual(w.z, 9.f));
	}	
	{
		{
			Mat3 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
			SetRow(m, 0, Vec3(30.1f, 40.2f, 50.3f));
			Vec3 u = GetRow(m, 0);
			Vec3 v = GetRow(m, 1);
			Vec3 w = GetRow(m, 2);

			TEST_CHECK(TestEqual(u.x, 30.1f));
			TEST_CHECK(TestEqual(v.x, 2.f));
			TEST_CHECK(TestEqual(w.x, 3.f));

			TEST_CHECK(TestEqual(u.y, 40.2f));
			TEST_CHECK(TestEqual(v.y, 5.f));
			TEST_CHECK(TestEqual(w.y, 6.f));

			TEST_CHECK(TestEqual(u.z, 50.3f));
			TEST_CHECK(TestEqual(v.z, 8.f));
			TEST_CHECK(TestEqual(w.z, 9.f));
		}
		{
			Mat3 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
			SetRow(m, 1, Vec3(60.4f, 70.5f, 80.6f));
			Vec3 u = GetRow(m, 0);
			Vec3 v = GetRow(m, 1);
			Vec3 w = GetRow(m, 2);

			TEST_CHECK(TestEqual(u.x, 1.f));
			TEST_CHECK(TestEqual(v.x, 60.4f));
			TEST_CHECK(TestEqual(w.x, 3.f));

			TEST_CHECK(TestEqual(u.y, 4.f));
			TEST_CHECK(TestEqual(v.y, 70.5f));
			TEST_CHECK(TestEqual(w.y, 6.f));

			TEST_CHECK(TestEqual(u.z, 7.f));
			TEST_CHECK(TestEqual(v.z, 80.6f));
			TEST_CHECK(TestEqual(w.z, 9.f));
		}
		{
			Mat3 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
			SetRow(m, 2, Vec3(90.7f, 10.8f, 20.9f));
			Vec3 u = GetRow(m, 0);
			Vec3 v = GetRow(m, 1);
			Vec3 w = GetRow(m, 2);

			TEST_CHECK(TestEqual(u.x, 1.f));
			TEST_CHECK(TestEqual(v.x, 2.f));
			TEST_CHECK(TestEqual(w.x, 90.7f));

			TEST_CHECK(TestEqual(u.y, 4.f));
			TEST_CHECK(TestEqual(v.y, 5.f));
			TEST_CHECK(TestEqual(w.y, 10.8f));

			TEST_CHECK(TestEqual(u.z, 7.f));
			TEST_CHECK(TestEqual(v.z, 8.f));
			TEST_CHECK(TestEqual(w.z, 20.9f));
		}
	}
	{
		{
			Mat3 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
			SetColumn(m, 0, Vec3(30.1f, 40.2f, 50.3f));
			Vec3 u = GetColumn(m, 0);
			Vec3 v = GetColumn(m, 1);
			Vec3 w = GetColumn(m, 2);

			TEST_CHECK(TestEqual(u.x, 30.1f));
			TEST_CHECK(TestEqual(u.y, 40.2f));
			TEST_CHECK(TestEqual(u.z, 50.3f));

			TEST_CHECK(TestEqual(v.x, 4.f));
			TEST_CHECK(TestEqual(v.y, 5.f));
			TEST_CHECK(TestEqual(v.z, 6.f));

			TEST_CHECK(TestEqual(w.x, 7.f));
			TEST_CHECK(TestEqual(w.y, 8.f));
			TEST_CHECK(TestEqual(w.z, 9.f));
		}
		{
			Mat3 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
			SetColumn(m, 1, Vec3(60.4f, 70.5f, 80.6f));
			Vec3 u = GetColumn(m, 0);
			Vec3 v = GetColumn(m, 1);
			Vec3 w = GetColumn(m, 2);

			TEST_CHECK(TestEqual(u.x, 1.f));
			TEST_CHECK(TestEqual(u.y, 2.f));
			TEST_CHECK(TestEqual(u.z, 3.f));

			TEST_CHECK(TestEqual(v.x, 60.4f));
			TEST_CHECK(TestEqual(v.y, 70.5f));
			TEST_CHECK(TestEqual(v.z, 80.6f));

			TEST_CHECK(TestEqual(w.x, 7.f));
			TEST_CHECK(TestEqual(w.y, 8.f));
			TEST_CHECK(TestEqual(w.z, 9.f));
		}
		{
			Mat3 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
			SetColumn(m, 2, Vec3(90.7f, 10.8f, 20.9f));
			Vec3 u = GetColumn(m, 0);
			Vec3 v = GetColumn(m, 1);
			Vec3 w = GetColumn(m, 2);

			TEST_CHECK(TestEqual(u.x, 1.f));
			TEST_CHECK(TestEqual(u.y, 2.f));
			TEST_CHECK(TestEqual(u.z, 3.f));

			TEST_CHECK(TestEqual(v.x, 4.f));
			TEST_CHECK(TestEqual(v.y, 5.f));
			TEST_CHECK(TestEqual(v.z, 6.f));

			TEST_CHECK(TestEqual(w.x, 90.7f));
			TEST_CHECK(TestEqual(w.y, 10.8f));
			TEST_CHECK(TestEqual(w.z, 20.9f));
		}
	}
	{
		Mat3 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
		SetX(m, Vec3(30.1f, 40.2f, 50.3f));
		Vec3 u = GetColumn(m, 0);
		Vec3 v = GetColumn(m, 1);
		Vec3 w = GetColumn(m, 2);

		TEST_CHECK(TestEqual(u.x, 30.1f));
		TEST_CHECK(TestEqual(u.y, 40.2f));
		TEST_CHECK(TestEqual(u.z, 50.3f));

		TEST_CHECK(TestEqual(v.x, 4.f));
		TEST_CHECK(TestEqual(v.y, 5.f));
		TEST_CHECK(TestEqual(v.z, 6.f));

		TEST_CHECK(TestEqual(w.x, 7.f));
		TEST_CHECK(TestEqual(w.y, 8.f));
		TEST_CHECK(TestEqual(w.z, 9.f));
	}
	{
		Mat3 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
		SetY(m, Vec3(60.4f, 70.5f, 80.6f));
		Vec3 u = GetColumn(m, 0);
		Vec3 v = GetColumn(m, 1);
		Vec3 w = GetColumn(m, 2);

		TEST_CHECK(TestEqual(u.x, 1.f));
		TEST_CHECK(TestEqual(u.y, 2.f));
		TEST_CHECK(TestEqual(u.z, 3.f));

		TEST_CHECK(TestEqual(v.x, 60.4f));
		TEST_CHECK(TestEqual(v.y, 70.5f));
		TEST_CHECK(TestEqual(v.z, 80.6f));

		TEST_CHECK(TestEqual(w.x, 7.f));
		TEST_CHECK(TestEqual(w.y, 8.f));
		TEST_CHECK(TestEqual(w.z, 9.f));
	}
	{
		Mat3 m(1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f);
		SetZ(m, Vec3(90.7f, 10.8f, 20.9f));
		Vec3 u = GetColumn(m, 0);
		Vec3 v = GetColumn(m, 1);
		Vec3 w = GetColumn(m, 2);

		TEST_CHECK(TestEqual(u.x, 1.f));
		TEST_CHECK(TestEqual(u.y, 2.f));
		TEST_CHECK(TestEqual(u.z, 3.f));

		TEST_CHECK(TestEqual(v.x, 4.f));
		TEST_CHECK(TestEqual(v.y, 5.f));
		TEST_CHECK(TestEqual(v.z, 6.f));

		TEST_CHECK(TestEqual(w.x, 90.7f));
		TEST_CHECK(TestEqual(w.y, 10.8f));
		TEST_CHECK(TestEqual(w.z, 20.9f));
	}
// Vec3 GetTranslation(const Mat3 &m);
// Vec3 GetScale(const Mat3 &m);
// void SetTranslation(Mat3 &m, const tVec2<float> &T);
// void SetTranslation(Mat3 &m, const Vec3 &T);
// void SetScale(Mat3 &m, const Vec3 &S);
// void SetAxises(Mat3 &m, const Vec3 &x, const Vec3 &y, const Vec3 &z);
// Mat3 Orthonormalize(const Mat3 &m);
// Mat3 Normalize(const Mat3 &m);
// Vec3 ToEuler(const Mat3 &m, RotationOrder order = RO_Default);
}
