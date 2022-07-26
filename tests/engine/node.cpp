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

		mask = 0;
		obj.SetModelRef(InvalidModelRef);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		obj.ClearModelRef();
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		obj.SetMaterial(0, mat);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		obj.SetMaterialCount(3);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		obj.SetMaterialName(3, "dummy");
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		obj.SetBoneCount(5);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(obj.SetBone(4, InvalidNodeRef) == false);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(obj.SetBoneNode(2, Node()) == false);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(obj.GetModelRef() == InvalidModelRef);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		mask = 0;
		TEST_CHECK(obj.GetMaterialCount() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		mask = 0;
		mat = obj.GetMaterial(1);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		TEST_CHECK(mat.program == InvalidPipelineProgramRef);
		TEST_CHECK(mat.textures.empty() == true);
		TEST_CHECK(mat.values.empty() == true);

		mask = 0;
		TEST_CHECK(obj.GetMaterialName(0).empty() == true);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(obj.GetMaterial("any") == nullptr);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		MinMax box;
		TEST_CHECK(obj.GetMinMax(res, box) == false);

		mask = 0;
		TEST_CHECK(obj.GetBoneCount() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(obj.GetBone(0) == InvalidNodeRef);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		TEST_CHECK(obj.GetBone(0) == InvalidNodeRef);
		
		mask = 0;
		Node bone = obj.GetBoneNode(1);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		TEST_CHECK(bone.IsValid() == false);
	}
	{
		Scene scene;

		Object obj = scene.CreateObject();
		TEST_CHECK(obj.IsValid() == true);
		TEST_CHECK(obj == true);

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

		mask = 0;
		l.SetShadowType(LST_Map);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		l.SetDiffuseColor(Color::Orange);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		l.SetDiffuseIntensity(0.5f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		l.SetSpecularColor(Color::Yellow);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		l.SetSpecularIntensity(0.8f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		l.SetRadius(2.f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		l.SetInnerAngle(Deg(20.f));
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		l.SetOuterAngle(Deg(30.f));
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		l.SetPSSMSplit(Vec4(1.f, 10.f, 20.f, 50.f));
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		l.SetPriority(1.f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		l.SetShadowBias(0.01f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(l.GetType() == LT_Point);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(l.GetShadowType() == LST_None);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(l.GetDiffuseColor() == Color::Red);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(l.GetDiffuseIntensity() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(l.GetSpecularColor() == Color::Red);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(l.GetSpecularIntensity() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(l.GetRadius() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(l.GetInnerAngle() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(l.GetOuterAngle() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(l.GetPSSMSplit() == default_pssm_split);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(l.GetPriority() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
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

static void test_rigid_body() {
	int mask = 0;

	set_log_level(LL_All);
	set_log_hook(on_log, &mask);

	{
		RigidBody body;
		TEST_CHECK(body.IsValid() == false);

		mask = 0;
		body.SetType(RBT_Kinematic);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		body.SetLinearDamping(0.1f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		body.SetAngularDamping(0.4f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		body.SetRestitution(0.2f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		body.SetFriction(0.01f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		body.SetRollingFriction(0.02f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(body.GetType() == RBT_Dynamic);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(body.GetLinearDamping() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(body.GetAngularDamping() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(body.GetRestitution() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(body.GetFriction() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(body.GetRollingFriction() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
	}
	{ 
		Scene scene;

		RigidBody body = scene.CreateRigidBody();
		TEST_CHECK(body.IsValid() == true);

		mask = 0;

		body.SetType(RBT_Kinematic);
		TEST_CHECK(mask == 0);

		body.SetLinearDamping(0.1f);
		TEST_CHECK(mask == 0);

		body.SetAngularDamping(0.4f);
		TEST_CHECK(mask == 0);

		body.SetRestitution(0.2f);
		TEST_CHECK(mask == 0);

		body.SetFriction(0.7f);
		TEST_CHECK(mask == 0);

		body.SetRollingFriction(0.3f);
		TEST_CHECK(mask == 0);

		mask = 0;

		TEST_CHECK(body.GetType() == RBT_Kinematic);
		TEST_CHECK(mask == 0);

		float v = body.GetLinearDamping();
		TEST_CHECK(TestEqual(body.GetLinearDamping(), 0.1f, 1.f/255.f) == true);
		TEST_CHECK(mask == 0);

		TEST_CHECK(TestEqual(body.GetAngularDamping(), 0.4f, 1.f / 255.f) == true);
		TEST_CHECK(mask == 0);

		TEST_CHECK(TestEqual(body.GetRestitution(), 0.2f, 1.f / 255.f) == true);
		TEST_CHECK(mask == 0);

		TEST_CHECK(TestEqual(body.GetFriction(), 0.7f, 1.f / 255.f) == true);
		TEST_CHECK(mask == 0);

		TEST_CHECK(TestEqual(body.GetRollingFriction(), 0.3f, 1.f / 255.f) == true);
		TEST_CHECK(mask == 0);

		RigidBody r0 = body;
		RigidBody r1 = scene.CreateRigidBody();
		TEST_CHECK(body == r0);
		TEST_CHECK((r1 == r0) == false);
		TEST_CHECK((body == r1) == false);
	}

	set_log_hook(nullptr, nullptr);
}

static void test_collision() {
	int mask = 0;

	set_log_level(LL_All);
	set_log_hook(on_log, &mask);

	{
		Collision c0;
		TEST_CHECK(c0.IsValid() == false);

		mask = 0;

		Vec3 pos(5.f, 7.f, -1.f);
		Vec3 rot(Deg3(30.f, 60.f, -45.f));

		mask = 0;
		c0.SetType(CT_Capsule);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		c0.SetLocalTransform(TransformationMat4(pos, rot));
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		c0.SetPosition(pos);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		c0.SetRotation(rot);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		mask = 0;
		c0.SetMass(10.f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		c0.SetRadius(3.f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		c0.SetHeight(5.f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		c0.SetSize(Vec3(2.f, 9.f, 7.f));
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		c0.SetCollisionResource("dummy collision");
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

#if 0
		mask = 0;
		c0.SetRestitution(0.5f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		c0.SetFriction(0.25f);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
#endif

		mask = 0;
		TEST_CHECK(c0.GetType() == CT_Sphere);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
		mask = 0;
		TEST_CHECK(c0.GetLocalTransform() == Mat4::Identity);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(c0.GetPosition() == Vec3::Zero);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(c0.GetRotation() == Vec3::Zero);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(c0.GetMass() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(c0.GetRadius() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(c0.GetHeight() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(c0.GetSize() == Vec3::Zero);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(c0.GetCollisionResource().empty() == true);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		
#if 0
		mask = 0;
		TEST_CHECK(c0.GetRestitution() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(c0.GetFriction() == 0);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
#endif
	}

	{
		Scene scene;
		Collision c0 = scene.CreateCollision();

		mask = 0;

		Vec3 pos(5.f, 7.f, -1.f);
		Vec3 rot(Deg3(30.f, 60.f, -45.f));

		c0.SetType(CT_Capsule);
		TEST_CHECK(mask == 0);

		TEST_CHECK(c0.GetType() == CT_Capsule);
		TEST_CHECK(mask == 0);

		Mat4 m0 = TransformationMat4(Vec3(-1.f, 0.1f, 3.f), Deg3(-90.f, 45.f, 120.f));
		c0.SetLocalTransform(m0);
		TEST_CHECK(mask == 0);

		Mat4 m1 = c0.GetLocalTransform();
		TEST_CHECK(mask == 0);
		TEST_CHECK(AlmostEqual(GetColumn(m1, 0), GetColumn(m0, 0), 0.00001f) == true);
		TEST_CHECK(AlmostEqual(GetColumn(m1, 1), GetColumn(m0, 1), 0.00001f) == true);
		TEST_CHECK(AlmostEqual(GetColumn(m1, 2), GetColumn(m0, 2), 0.00001f) == true);
		TEST_CHECK(AlmostEqual(GetColumn(m1, 3), GetColumn(m0, 3), 0.00001f) == true);

		c0.SetPosition(pos);
		TEST_CHECK(mask == 0);

		TEST_CHECK(AlmostEqual(c0.GetPosition(), pos, 0.000001f) == true);
		TEST_CHECK(mask == 0);

		c0.SetRotation(rot);
		TEST_CHECK(mask == 0);

		TEST_CHECK(AlmostEqual(c0.GetRotation(), rot, 0.000001f) == true);
		TEST_CHECK(mask == 0);

		c0.SetMass(10.f);
		TEST_CHECK(mask == 0);

		TEST_CHECK(c0.GetMass() == 10.f);
		TEST_CHECK(mask == 0);

		c0.SetRadius(3.f);
		TEST_CHECK(mask == 0);

		TEST_CHECK(c0.GetRadius() == 3.f);
		TEST_CHECK(mask == 0);

		c0.SetHeight(5.f);
		TEST_CHECK(mask == 0);

		TEST_CHECK(c0.GetHeight() == 5.f);
		TEST_CHECK(mask == 0);

		Vec3 size(2.f, 9.f, 7.f);
		c0.SetSize(size);
		TEST_CHECK(mask == 0);

		TEST_CHECK(AlmostEqual(c0.GetSize(), size, 0.000001f) == true);
		TEST_CHECK(mask == 0);

		c0.SetCollisionResource("dummy collision");
		TEST_CHECK(mask == 0);
				
		TEST_CHECK(c0.GetCollisionResource() == "dummy collision");
		TEST_CHECK(mask == 0);
#if 0
		c0.SetRestitution(0.5f);
		TEST_CHECK(mask == 0);

		TEST_CHECK(c0.GetRestitution() == 0.5f);
		TEST_CHECK(mask == 0);

		c0.SetFriction(0.25f);
		TEST_CHECK(mask == 0);

		TEST_CHECK(c0.GetFriction() == 0.25f);
		TEST_CHECK(mask == 0);
#endif

		Collision c1 = c0;
		Collision c2 = scene.CreateCollision();
		TEST_CHECK(c1 == c0);
		TEST_CHECK((c1 == c2) == false);
		TEST_CHECK((c2 == c0) == false);
	}
	set_log_hook(nullptr, nullptr);
}

static void test_instance() {
	int mask = 0;

	set_log_level(LL_All);
	set_log_hook(on_log, &mask);

	{
		Instance i0;
		TEST_CHECK(i0.IsValid() == false);

		mask = 0;
		i0.SetPath("instance path");
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		i0.SetOnInstantiateAnim("on instantiate");
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		i0.SetOnInstantiateAnimLoopMode(ALM_Loop);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		i0.ClearOnInstantiateAnim();
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(i0.GetPath().empty() == true);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(i0.GetOnInstantiateAnim().empty() == true);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(i0.GetOnInstantiateAnimLoopMode() == ALM_Once);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(i0.GetOnInstantiatePlayAnimRef() == InvalidScenePlayAnimRef);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
	}

	{
		Scene scene;

		Instance i0 = scene.CreateInstance();
		TEST_CHECK(i0.IsValid() == true);
		TEST_CHECK(i0 == true);

		mask = 0;

		i0.SetPath("instance path");
		TEST_CHECK(mask == 0);

		i0.SetOnInstantiateAnim("on instantiate");
		TEST_CHECK(mask == 0);

		i0.SetOnInstantiateAnimLoopMode(ALM_Loop);
		TEST_CHECK(mask == 0);

		TEST_CHECK(i0.GetPath() == "instance path");
		TEST_CHECK(mask == 0);

		TEST_CHECK(i0.GetOnInstantiateAnim() == "on instantiate");
		TEST_CHECK(mask == 0);

		TEST_CHECK(i0.GetOnInstantiateAnimLoopMode() == ALM_Loop);
		TEST_CHECK(mask == 0);
		
		TEST_CHECK(i0.GetOnInstantiatePlayAnimRef() == InvalidScenePlayAnimRef);
		TEST_CHECK(mask == 0);
		
		i0.ClearOnInstantiateAnim();
		TEST_CHECK(mask == 0);

		TEST_CHECK(i0.GetOnInstantiateAnim().empty() == true);
		TEST_CHECK(mask == 0);

		Instance i1 = i0;
		Instance i2 = scene.CreateInstance("another instance");
		TEST_CHECK(i1 == i0);
		TEST_CHECK((i2 == i1) == false);
		TEST_CHECK((i0 == i2) == false);
	}
	set_log_hook(nullptr, nullptr);
}

static void test_script() {
	int mask = 0;

	set_log_level(LL_All);
	set_log_hook(on_log, &mask);

	{ 
		Script s0;
		TEST_CHECK(s0.IsValid() == false);
	
		mask = 0;
		s0.SetPath("script_00.lua");
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		ScriptParam p0, p1;
		p0.type = SPT_Int;
		p0.iv = 0xcafe;

		mask = 0;
		TEST_CHECK(s0.SetParam("param_00", p0) == false);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(s0.GetPath().empty() == true);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		TEST_CHECK(s0.HasParam("param_01") == false);
		TEST_CHECK((mask & LL_Warning) == LL_Warning);

		mask = 0;
		p1 = s0.GetParam("param_00");
		TEST_CHECK((mask & LL_Warning) == LL_Warning);
		TEST_CHECK(p1.type == SPT_Null);
		TEST_CHECK(p1.iv == 0);
	}
	{
		Scene scene;
		Script s0 = scene.CreateScript();
		TEST_CHECK(s0.IsValid() == true);

		mask = 0;

		s0.SetPath("script_00.lua");
		TEST_CHECK(mask == 0);

		ScriptParam p0, p1;
		p0.type = SPT_Int;
		p0.iv = 0xcafe;

		TEST_CHECK(s0.SetParam("param_00", p0) == true);
		TEST_CHECK(mask == 0);

		TEST_CHECK(s0.GetPath() == "script_00.lua");
		TEST_CHECK(mask == 0);

		TEST_CHECK(s0.HasParam("param_01") == false);
		TEST_CHECK(mask == 0);

		TEST_CHECK(s0.HasParam("param_00") == true);
		TEST_CHECK(mask == 0);

		p1 = s0.GetParam("param_00");
		TEST_CHECK(mask == 0);
		TEST_CHECK(p1.type == p0.type);
		TEST_CHECK(p1.iv == p0.iv);

		Script s1 = scene.CreateScript("script_01.lua");
		Script s2 = s0;
		TEST_CHECK(s2 == s0);
		TEST_CHECK((s2 == s1) == false);
		TEST_CHECK((s1 == s0) == false);
	}
	set_log_hook(nullptr, nullptr);
}

static void test_node_impl() {
	int mask = 0;

	set_log_level(LL_All);
	set_log_hook(on_log, &mask);

	{ 
		PipelineResources res;
		Scene scene;

		Node n0;
		TEST_CHECK(n0.IsValid() == false);
		TEST_CHECK(n0 == false);
		TEST_CHECK(n0.GetUid() == InvalidNodeRef.idx);

		// Node::(Set|Get)* methods don't issue any warnings if the node is invalid.
		n0.SetName("node #00");
		TEST_CHECK(n0.GetName().empty() == true);

		n0.SetFlags(0xc0c0a);
		TEST_CHECK(n0.GetFlags() == 0);

		Transform trs = scene.CreateTransform();
		n0.SetTransform(trs);
		TEST_CHECK(n0.GetTransform().IsValid() == false);
		TEST_CHECK((n0.GetTransform() == trs) == false);

		Camera cam = scene.CreateCamera();
		n0.SetCamera(cam);
		TEST_CHECK(n0.GetCamera().IsValid() == false);
		TEST_CHECK((n0.GetCamera() == cam) == false);

		Object obj = scene.CreateObject();
		n0.SetObject(obj);
		TEST_CHECK(n0.GetObject().IsValid() == false);
		TEST_CHECK((n0.GetObject() == obj) == false);

		Light lt = scene.CreateLight();
		n0.SetLight(lt);
		TEST_CHECK(n0.GetLight().IsValid() == false);
		TEST_CHECK((n0.GetLight() == lt) == false);

		RigidBody body = scene.CreateRigidBody();
		n0.SetRigidBody(body);
		TEST_CHECK(n0.GetRigidBody().IsValid() == false);
		TEST_CHECK((n0.GetRigidBody() == body) == false);

		Collision collision = scene.CreateCollision();
		n0.SetCollision(0, collision);
		TEST_CHECK(n0.GetCollision(0).IsValid() == false);
		TEST_CHECK((n0.GetCollision(0) == collision) == false);

		Instance instance = scene.CreateInstance();
		n0.SetInstance(instance);
		TEST_CHECK(n0.GetInstance().IsValid() == false);
		TEST_CHECK((n0.GetInstance() == instance) == false);

		Script script = scene.CreateScript();
		n0.SetScript(0, script);
		TEST_CHECK(n0.GetScript(0).IsValid() == false);
		TEST_CHECK((n0.GetScript(0) == script) == false);

		n0.SetWorld(Mat4::Identity);

		TEST_CHECK(n0.HasTransform() == false);
		TEST_CHECK(n0.HasCamera() == false);
		TEST_CHECK(n0.HasObject() == false);
		TEST_CHECK(n0.HasLight() == false);
		TEST_CHECK(n0.HasRigidBody() == false);
		TEST_CHECK(n0.HasInstance() == false);
		TEST_CHECK(n0.GetScriptCount() == 0);
		TEST_CHECK(n0.GetCollisionCount() == 0);

		n0.Enable();
		TEST_CHECK(n0.IsEnabled() == false);
		TEST_CHECK(n0.IsItselfEnabled() == false);

		n0.Disable();
		TEST_CHECK(n0.IsEnabled() == false);
		TEST_CHECK(n0.IsItselfEnabled() == false);

		n0.RemoveTransform();
		TEST_CHECK(n0.HasTransform() == false);

		n0.RemoveCamera();
		TEST_CHECK(n0.HasCamera() == false);

		n0.RemoveObject();
		TEST_CHECK(n0.HasObject() == false);

		n0.RemoveLight();
		TEST_CHECK(n0.HasLight() == false);

		n0.RemoveRigidBody();
		TEST_CHECK(n0.HasRigidBody() == false);
	
		n0.DestroyInstance();
		TEST_CHECK(n0.HasInstance() == false);

		n0.RemoveScript(script.ref);
		TEST_CHECK(n0.GetScriptCount() == 0);
		
		n0.RemoveScript(0);
		TEST_CHECK(n0.GetScriptCount() == 0);
		
		n0.RemoveScript(script);
		TEST_CHECK(n0.GetScriptCount() == 0);
		
		n0.RemoveCollision(0);
		TEST_CHECK(n0.GetCollisionCount() == 0);

		n0.RemoveCollision(collision);
		TEST_CHECK(n0.GetCollisionCount() == 0);

		n0.RemoveCollision(collision.ref);
		TEST_CHECK(n0.GetCollisionCount() == 0);

		ViewState view = n0.ComputeCameraViewState(Vec2(4.f/3.f, 1.f));
		TEST_CHECK(view.proj == Mat44::Identity);
		TEST_CHECK(view.view == Mat4::Identity);

		MinMax minmax;
		TEST_CHECK(n0.GetMinMax(res, minmax) == false);
		TEST_CHECK(n0.IsInstantiatedBy().IsValid() == false);

		const SceneView& scn_view = n0.GetInstanceSceneView();
		TEST_CHECK(scn_view.nodes.empty() == true);
		TEST_CHECK(scn_view.anims.empty() == true);
		TEST_CHECK(scn_view.scene_anims.empty() == true);

		TEST_CHECK(n0.GetWorld() == Mat4::Identity);
		TEST_CHECK(n0.ComputeWorld() == Mat4::Identity);
	}

	{
		Scene scene;
		Node n0 = scene.CreateNode("node #00");
		TEST_CHECK(n0.IsValid() == true);

		TEST_CHECK(n0.GetUid() != InvalidNodeRef.idx);

		n0.SetName("node #00");
		TEST_CHECK(n0.GetName() == "node #00");

		n0.SetFlags(0xc0c0a);
		TEST_CHECK(n0.GetFlags() == 0xc0c0a);

		TEST_CHECK(n0.GetWorld() == Mat4::Identity);
		TEST_CHECK(n0.ComputeWorld() == Mat4::Identity);
		
		Vec3 pos(-1.f, 3.f, 5.f);
		Vec3 rot(Deg3(30.f, 45.f, 60.f));
		Mat4 world = TransformationMat4(pos, rot);
		Transform trs = scene.CreateTransform(pos, rot);
		n0.SetTransform(trs);
		TEST_CHECK(n0.GetTransform().IsValid() == true);
		TEST_CHECK(n0.GetTransform() == trs);

		TEST_CHECK(n0.GetWorld() == Mat4::Identity);
		TEST_CHECK(n0.ComputeWorld() == world);
		
		n0.RemoveTransform();
		TEST_CHECK(n0.GetTransform().IsValid() == false);

		n0.Enable();
		TEST_CHECK(n0.IsEnabled() == true);
		TEST_CHECK(n0.IsItselfEnabled() == true);

		n0.Disable();
		TEST_CHECK(n0.IsEnabled() == false);
		TEST_CHECK(n0.IsItselfEnabled() == false);

		n0.Enable();
		TEST_CHECK(n0.IsEnabled() == true);
		TEST_CHECK(n0.IsItselfEnabled() == true);

		// [todo]

		Node n1 = scene.CreateNode("node #01");
		Node n2 = n0;
		TEST_CHECK(n2 == n0);
		TEST_CHECK((n1 == n0) == false);
		TEST_CHECK((n2 == n1) == false);
	}

	set_log_hook(nullptr, nullptr);
}

/* [todo]
	bool SetupInstance(
		const Reader &ir, const ReadProvider &ip, PipelineResources &resources, const PipelineInfo &pipeline, uint32_t flags = LSSF_AllNodeFeatures);
	bool SetupInstanceFromFile(PipelineResources &resources, const PipelineInfo &pipeline, uint32_t flags = LSSF_AllNodeFeatures);
	bool SetupInstanceFromAssets(PipelineResources &resources, const PipelineInfo &pipeline, uint32_t flags = LSSF_AllNodeFeatures);
	void DestroyInstance();
	SceneAnimRef GetInstanceSceneAnim(const std::string &path) const;
	void StartOnInstantiateAnim();
	void StopOnInstantiateAnim();
*/

void test_node() {
	test_transform();
	test_camera();
	test_object();
	test_light();
	test_rigid_body();
	test_collision();
	test_instance();
	test_script();
	test_node_impl();
}