// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#define TEST_NO_MAIN
#include "acutest.h"

#include <string>

#include "engine/node.h"

#include "foundation/log.h"

#include "engine/scene.h"

using namespace hg;

static void on_log(const std::string&, int mask, const std::string&, void *user) {
	int *ptr = reinterpret_cast<int*>(user);
	*ptr = mask;
}

static void test_transform() {
	int mask = 0;

	set_log_level(LL_All);
	set_log_hook(on_log, &mask); // mute logs but catch the mask of the log being issued.

	{
		TransformTRS trs;
		TEST_CHECK(trs.pos == Vec3::Zero);
		TEST_CHECK(trs.rot == Vec3::Zero);
		TEST_CHECK(trs.scl == Vec3::One);
	}

	{
		Transform transform;
		Vec3 pos, rot;
		
		TEST_CHECK(transform.IsValid() == false);
		TEST_CHECK(transform.GetPos() == Vec3::Zero);
		TEST_CHECK(transform.GetRot() == Vec3::Zero);
		TEST_CHECK(transform.GetScale() == Vec3::One);
		TEST_CHECK(transform.GetParent() == InvalidNodeRef);
		TEST_CHECK(transform.GetParentNode().IsValid() == false);
		TEST_CHECK(transform.GetWorld() == Mat4::Identity);

		transform.GetPosRot(pos, rot);
		TEST_CHECK(pos == Vec3::Zero);
		TEST_CHECK(rot == Vec3::Zero);

		TransformTRS trs = transform.GetTRS();
		TEST_CHECK(trs.pos == Vec3::Zero);
		TEST_CHECK(trs.rot == Vec3::Zero);
		TEST_CHECK(trs.scl == Vec3::One);

		// check that a warning is triggered as the transform is invalid.
		mask = 0;
		transform.SetPos(Vec3::One);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		transform.SetRot(Vec3::One);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		mask = 0;
		transform.SetScale(Vec3(0.5f, 0.5f, 0.5f));
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		mask = 0;
		transform.SetTRS(trs);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		mask = 0;
		transform.SetParent(InvalidNodeRef);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		mask = 0;
		transform.SetParentNode(Node());
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		mask = 0;
		transform.SetPosRot(pos, rot);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		mask = 0;
		transform.SetWorld(Mat4::Identity);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		mask = 0;
		transform.SetLocal(Mat4::Identity);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
	}

	{ 
		Scene scene;
		Transform transform = scene.CreateTransform();

		mask = 0;

		{
			Vec3 t(-1.0f, 5.0f, 0.4f);
			Vec3 r(0.5f, 1.0f, -0.5f);
			Vec3 s(3.0f, 7.0f, 2.0f);

			transform.SetPos(t);
			TEST_CHECK(mask == 0);

			transform.SetRot(r);
			TEST_CHECK(mask == 0);

			transform.SetScale(s);
			TEST_CHECK(mask == 0);

			TEST_CHECK(AlmostEqual(transform.GetPos(), t, 0.000001f) == true);
			TEST_CHECK(AlmostEqual(transform.GetRot(), r, 0.000001f) == true);
			TEST_CHECK(AlmostEqual(transform.GetScale(), s, 0.000001f) == true);
		}

		{
			Vec3 t(3.0f, -7.0f, 1.0f);
			Vec3 r(-0.5f, 0.5f, -1.0f);
			Vec3 s(2.0f, 5.0f, 3.0f);

			{
				TransformTRS trs;

				trs.pos = t;
				trs.rot = r;
				trs.scl = s;

				transform.SetTRS(trs);
				TEST_CHECK(mask == 0);
			}

			{
				TransformTRS trs = transform.GetTRS();
				TEST_CHECK(AlmostEqual(trs.pos, t, 0.000001f) == true);
				TEST_CHECK(AlmostEqual(trs.rot, r, 0.000001f) == true);
				TEST_CHECK(AlmostEqual(trs.scl, s, 0.000001f) == true);

				TEST_CHECK(AlmostEqual(transform.GetPos(), t, 0.000001f) == true);
				TEST_CHECK(AlmostEqual(transform.GetRot(), r, 0.000001f) == true);
				TEST_CHECK(AlmostEqual(transform.GetScale(), s, 0.000001f) == true);
			}
		}

		{
			Vec3 t(-1.0f,-1.0f,-1.0f);
			Vec3 r(Deg3(30.f, 45.f, 60.f));

			Mat4 m0 = TransformationMat4(t, r);

			transform.SetPosRot(t, r);
			transform.SetScale(hg::Vec3::One);
			TEST_CHECK(mask == 0);

			TEST_CHECK(AlmostEqual(transform.GetPos(), t, 0.000001f) == true);
			TEST_CHECK(AlmostEqual(transform.GetRot(), r, 0.000001f) == true);

			scene.Update(time_from_ms(20));

			Mat4 m1 = transform.GetWorld();
			TEST_CHECK(AlmostEqual(GetColumn(m0, 0), GetColumn(m1, 0), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m0, 1), GetColumn(m1, 1), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m0, 2), GetColumn(m1, 2), 0.00001f));
			TEST_CHECK(AlmostEqual(GetColumn(m0, 3), GetColumn(m1, 3), 0.00001f));
		}

		Transform t0 = transform;
		Transform t1 = scene.CreateTransform();

		TEST_CHECK(transform == t0);
		TEST_CHECK((transform == t1) == false);
		TEST_CHECK((t0 == t1) == false);
	}
	// parents/setlocal and all will be tested in scene
	set_log_hook(nullptr, nullptr);
}

static void test_camera() {
	int mask = 0;

	set_log_level(LL_All);
	set_log_hook(on_log, &mask);

	{
		CameraZRange range;
		TEST_CHECK(range.znear == 0.01f);
		TEST_CHECK(range.zfar == 1000.f);
	}
	{ 
		CameraZRange range;

		Camera cam;
		TEST_CHECK(cam.IsValid() == false);
		
		mask = 0;
		TEST_CHECK(cam.GetZNear() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		mask = 0;
		TEST_CHECK(cam.GetZFar() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		mask = 0;
		TEST_CHECK(cam.GetFov() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		range = cam.GetZRange();
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		TEST_CHECK(range.znear == 0.01f);
		TEST_CHECK(range.zfar == 1000.f);

		mask = 0;
		TEST_CHECK(cam.GetIsOrthographic() == false);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(cam.GetSize() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		cam.SetZNear(1.f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		cam.SetZFar(100.f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		cam.SetFov(Deg(60.f));
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		cam.SetZRange(range);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		cam.SetIsOrthographic(true);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		cam.SetSize(2.f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
	}
	{ 
		Scene scn;
		Camera cam = scn.CreateCamera(0.1f, 1000.f);
		CameraZRange range;

		range.znear = 0.5f;
		range.zfar = 500.f;

		mask = 0;

		TEST_CHECK(cam.IsValid() == true);

		cam.SetZNear(1.f);
		TEST_CHECK(mask == 0);

		cam.SetZFar(100.f);
		TEST_CHECK(mask == 0);

		cam.SetFov(Deg(60.f));
		TEST_CHECK(mask == 0);

		cam.SetZRange(range);
		TEST_CHECK(mask == 0);

		cam.SetIsOrthographic(true);
		TEST_CHECK(mask == 0);

		cam.SetSize(2.f);
		TEST_CHECK(mask == 0);

		TEST_CHECK(cam.GetZNear() == range.znear);
		TEST_CHECK(cam.GetZFar() == range.zfar);
		TEST_CHECK(cam.GetFov() == Deg(60.f));

		CameraZRange r0 = cam.GetZRange();
		TEST_CHECK(r0.znear == range.znear);
		TEST_CHECK(r0.zfar == range.zfar);

		TEST_CHECK(cam.GetIsOrthographic() == true);
		TEST_CHECK(cam.GetSize() == 2.f);

		Camera c0 = cam;
		Camera c1 = scn.CreateCamera(1.f, 10.f);

		TEST_CHECK(cam == c0);
		TEST_CHECK((c1 == c0) == false);
		TEST_CHECK((c1 == cam) == false);

	}

	set_log_hook(nullptr, nullptr);
}

void test_node() {
	test_transform();
	test_camera();
	// [todo]
}