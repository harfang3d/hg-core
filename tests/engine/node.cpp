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
	int mask = 0; // here mask will serve as an errno substitute.

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

		transform.SetRot(Vec3::One);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		transform.SetScale(Vec3(0.5f, 0.5f, 0.5f));
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		transform.SetTRS(trs);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		transform.SetParent(InvalidNodeRef);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		transform.SetParentNode(Node());
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		transform.SetPosRot(pos, rot);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		transform.SetWorld(Mat4::Identity);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
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
		
		TEST_CHECK(cam.GetZFar() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		TEST_CHECK(cam.GetFov() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		range = cam.GetZRange();
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		TEST_CHECK(range.znear == 0.01f);
		TEST_CHECK(range.zfar == 1000.f);

		TEST_CHECK(cam.GetIsOrthographic() == false);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(cam.GetSize() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		cam.SetZNear(1.f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		cam.SetZFar(100.f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		cam.SetFov(Deg(60.f));
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		cam.SetZRange(range);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		cam.SetIsOrthographic(true);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

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

static void test_object() {
	int mask = 0;

	set_log_level(LL_All);
	set_log_hook(on_log, &mask);

	PipelineResources res;

	{ 
		Object obj;
		TEST_CHECK(obj.IsValid() == false);

		mask = 0;
		
		Material mat;

		obj.SetModelRef(InvalidModelRef);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		obj.ClearModelRef();
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		obj.SetMaterial(0, mat);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		obj.SetMaterialCount(3);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		obj.SetMaterialName(3, "dummy");
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		obj.SetBoneCount(5);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(obj.SetBone(4, InvalidNodeRef) == false);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(obj.SetBoneNode(2, Node()) == false);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;

		TEST_CHECK(obj.GetModelRef() == InvalidModelRef);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		TEST_CHECK(obj.GetMaterialCount() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		mat = obj.GetMaterial(1);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		TEST_CHECK(mat.program == InvalidPipelineProgramRef);
		TEST_CHECK(mat.textures.empty() == true);
		TEST_CHECK(mat.values.empty() == true);

		TEST_CHECK(obj.GetMaterialName(0).empty() == true);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(obj.GetMaterial("any") == nullptr);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		MinMax box;
		TEST_CHECK(obj.GetMinMax(res, box) == false);

		TEST_CHECK(obj.GetBoneCount() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(obj.GetBone(0) == InvalidNodeRef);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		TEST_CHECK(obj.GetBone(0) == InvalidNodeRef);
		
		Node bone = obj.GetBoneNode(1);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		TEST_CHECK(bone.IsValid() == false);
	}
	{
		Scene scene;

		Object obj = scene.CreateObject();
		TEST_CHECK(obj.IsValid() == true);

		mask = 0;
		obj.SetMaterialCount(2);
		TEST_CHECK(mask == 0);
		TEST_CHECK(obj.GetMaterialCount() == 2);

		Material mat;
		mat.flags = 1;
		obj.SetMaterial(0, mat);
		TEST_CHECK(mask == 0);
		
		mat.flags = 2;
		obj.SetMaterial(1, mat);
		TEST_CHECK(mask == 0);
		
		mat = obj.GetMaterial(0);
		TEST_CHECK(mask == 0);
		TEST_CHECK(mat.flags == 1);

		mat = obj.GetMaterial(1);
		TEST_CHECK(mask == 0);
		TEST_CHECK(mat.flags == 2);

		obj.SetMaterialName(0, "material #0");
		TEST_CHECK(mask == 0);
		
		obj.SetMaterialName(1, "material #1");
		TEST_CHECK(mask == 0);

		TEST_CHECK(obj.GetMaterialName(0) == "material #0");
		TEST_CHECK(mask == 0);

		TEST_CHECK(obj.GetMaterialName(1) == "material #1");
		TEST_CHECK(mask == 0);

		Material *ptr;

		ptr = obj.GetMaterial("material #0");
		TEST_CHECK(ptr != nullptr);
		TEST_CHECK(ptr->flags == 1);

		ptr = obj.GetMaterial("material #1");
		TEST_CHECK(ptr != nullptr);
		TEST_CHECK(ptr->flags == 2);

		obj.SetBoneCount(4);
		TEST_CHECK(mask == 0);

		TEST_CHECK(obj.GetBoneCount() == 4);
		TEST_CHECK(mask == 0);

		Node node = scene.CreateNode("dummy node");
		TEST_CHECK(obj.SetBoneNode(0, node) == true);

		TEST_CHECK(obj.GetBone(1) == InvalidNodeRef);
		TEST_CHECK(mask == 0);

		TEST_CHECK(obj.GetBoneNode(3).IsValid() == false);
		TEST_CHECK(mask == 0);

		Node n0 = obj.GetBoneNode(0);
		TEST_CHECK(n0.IsValid() == true);
		TEST_CHECK(n0.GetName() == "dummy node");
		TEST_CHECK(mask == 0);

		Object o0 = obj;
		Object o1 = scene.CreateObject();
		TEST_CHECK(o0 == obj);
		TEST_CHECK((obj == o1) == false);
		TEST_CHECK((o1 == o0) == false);
	}
	// [todo] GetMinMax

	set_log_hook(nullptr, nullptr);
}

static void test_light() {
	int mask = 0;

	set_log_level(LL_All);
	set_log_hook(on_log, &mask);
	
	{ 
		Light l;
		TEST_CHECK(l.IsValid() == false);

		mask = 0;
		l.SetType(LT_Spot);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		l.SetShadowType(LST_Map);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		l.SetDiffuseColor(Color::Orange);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		l.SetDiffuseIntensity(0.5f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		l.SetSpecularColor(Color::Yellow);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		l.SetSpecularIntensity(0.8f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		l.SetRadius(2.f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		l.SetInnerAngle(Deg(20.f));
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		l.SetOuterAngle(Deg(30.f));
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		l.SetPSSMSplit(Vec4(1.f, 10.f, 20.f, 50.f));
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		l.SetPriority(1.f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		l.SetShadowBias(0.01f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(l.GetType() == LT_Point);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(l.GetShadowType() == LST_None);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(l.GetDiffuseColor() == Color::Red);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(l.GetDiffuseIntensity() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(l.GetSpecularColor() == Color::Red);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(l.GetSpecularIntensity() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(l.GetRadius() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(l.GetInnerAngle() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(l.GetOuterAngle() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(l.GetPSSMSplit() == default_pssm_split);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(l.GetPriority() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		TEST_CHECK(l.GetShadowBias() == default_shadow_bias);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
	}
	{ 
		Scene scene;
		Light l0 = scene.CreateLight();

		mask = 0;
		l0.SetType(LT_Spot);
		TEST_CHECK(mask == 0);

		l0.SetShadowType(LST_Map);
		TEST_CHECK(mask == 0);

		l0.SetDiffuseColor(Color::Orange);
		TEST_CHECK(mask == 0);

		l0.SetDiffuseIntensity(0.5f);
		TEST_CHECK(mask == 0);

		l0.SetSpecularColor(Color::Yellow);
		TEST_CHECK(mask == 0);

		l0.SetSpecularIntensity(0.8f);
		TEST_CHECK(mask == 0);

		l0.SetRadius(2.f);
		TEST_CHECK(mask == 0);
		
		l0.SetInnerAngle(Deg(20.f));
		TEST_CHECK(mask == 0);

		l0.SetOuterAngle(Deg(30.f));
		TEST_CHECK(mask == 0);

		l0.SetPSSMSplit(Vec4(1.f, 10.f, 20.f, 50.f));
		TEST_CHECK(mask == 0);

		l0.SetPriority(1.f);
		TEST_CHECK(mask == 0);

		l0.SetShadowBias(0.01f);
		TEST_CHECK(mask == 0);

		TEST_CHECK(l0.GetType() == LT_Spot);
		TEST_CHECK(mask == 0);

		TEST_CHECK(l0.GetShadowType() == LST_Map);
		TEST_CHECK(mask == 0);

		TEST_CHECK(l0.GetDiffuseColor() == Color::Orange);
		TEST_CHECK(mask == 0);

		TEST_CHECK(l0.GetDiffuseIntensity() == 0.5f);
		TEST_CHECK(mask == 0);

		TEST_CHECK(l0.GetSpecularColor() == Color::Yellow);
		TEST_CHECK(mask == 0);

		TEST_CHECK(l0.GetSpecularIntensity() == 0.8f);
		TEST_CHECK(mask == 0);

		TEST_CHECK(l0.GetRadius() == 2.f);
		TEST_CHECK(mask == 0);

		TEST_CHECK(TestEqual(l0.GetInnerAngle(), Deg(20.f)) == true);
		TEST_CHECK(mask == 0);

		TEST_CHECK(TestEqual(l0.GetOuterAngle(), Deg(30.f)) == true);
		TEST_CHECK(mask == 0);

		TEST_CHECK(AlmostEqual(l0.GetPSSMSplit(), Vec4(1.f, 10.f, 20.f, 50.f), 0.00001f) == true);
		TEST_CHECK(mask == 0);

		TEST_CHECK(l0.GetPriority() == 1.f);
		TEST_CHECK(mask == 0);

		TEST_CHECK(TestEqual(l0.GetShadowBias(), 0.01f) == true);
		TEST_CHECK(mask == 0);

		Light l1 = l0;
		Light l2 = scene.CreateLight();
		TEST_CHECK(l0 == l1);
		TEST_CHECK((l0 == l2) == false);
		TEST_CHECK((l2 == l1) == false);
	}

	set_log_hook(nullptr, nullptr);
}

void test_node() {
	test_transform();
	test_camera();
	test_object();
	test_light();
	// [todo]
}