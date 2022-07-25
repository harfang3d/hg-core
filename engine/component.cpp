// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "foundation/file.h"
#include "foundation/log.h"
#include "foundation/math.h"

#include "engine/assets.h"
#include "engine/scene.h"

#include <fmt/format.h>

namespace hg {

//
void Scene::ReserveTransforms(const size_t count) { transforms.reserve(transforms.size() + count); }
void Scene::ReserveCameras(const size_t count) { cameras.reserve(cameras.size() + count); }
void Scene::ReserveObjects(const size_t count) { objects.reserve(objects.size() + count); }
void Scene::ReserveLights(const size_t count) { lights.reserve(lights.size() + count); }
void Scene::ReserveScripts(const size_t count) { scripts.reserve(scripts.size() + count); }

//
static bool IsChildOf(Scene &scene, NodeRef node, ComponentRef transform) {
	if (scene.GetNode(node).GetTransform().ref == transform)
		return true;

	while (node != InvalidNodeRef) {
		const NodeRef parent = scene.GetNode(node).GetTransform().GetParent();
		if (scene.GetNode(parent).GetTransform().ref == transform)
			return true;
		node = parent;
	}
	return false;
}

//
Transform Scene::CreateTransform() {
	Transform_ transform_;
	transform_.parent = InvalidNodeRef;

	const ComponentRef ref = transforms.add_ref(transform_);
	if (ref.idx >= transform_worlds.size())
		transform_worlds.resize(ref.idx + 64, Mat4::Identity); // so that GetWorld works straight away

	Transform transform;
	transform.scene_ref = scene_ref;
	transform.ref = ref;
	return transform;
}

void Scene::DestroyTransform(ComponentRef ref) { transforms.remove_ref(ref); }

Vec3 Scene::GetTransformPos(ComponentRef ref) const {
	if (const Transform_ *c = GetComponent_(transforms, ref))
		return c->TRS.pos;

	warn("Invalid transform component");
	return Vec3::Zero;
}

void Scene::SetTransformPos(ComponentRef ref, const Vec3 &v) {
	if (Transform_ *c = GetComponent_(transforms, ref))
		c->TRS.pos = v;
	else
		warn("Invalid transform component");
}

bool Transform::IsValid() const { return scene_ref && scene_ref->scene ? scene_ref->scene->IsValidTransformRef(ref) : false; }

Vec3 Transform::GetPos() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetTransformPos(ref);

	warn("Orphaned transform component");
	return Vec3::Zero;
}

void Transform::SetPos(const Vec3 &v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetTransformPos(ref, v);
	else
		warn("Orphaned transform component");
}

void Transform::GetPosRot(Vec3 &p, Vec3 &r) {
	p = GetPos();
	r = GetRot();
}

void Transform::SetPosRot(const Vec3 &p, const Vec3 &r) {
	SetPos(p);
	SetRot(r);
}

Vec3 Scene::GetTransformRot(ComponentRef ref) const {
	if (const Transform_ *c = GetComponent_(transforms, ref))
		return c->TRS.rot;

	warn("Invalid transform component");
	return Vec3::Zero;
}

void Scene::SetTransformRot(ComponentRef ref, const Vec3 &v) {
	if (Transform_ *c = GetComponent_(transforms, ref))
		c->TRS.rot = v;
	else
		warn("Invalid transform component");
}

Vec3 Transform::GetRot() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetTransformRot(ref);

	warn("Orphaned transform component");
	return Vec3::Zero;
}

void Transform::SetRot(const Vec3 &v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetTransformRot(ref, v);
	else
		warn("Orphaned transform component");
}

Vec3 Scene::GetTransformScale(ComponentRef ref) const {
	if (const Transform_ *c = GetComponent_(transforms, ref))
		return c->TRS.scl;

	warn("Invalid transform component");
	return Vec3::One;
}

void Scene::SetTransformScale(ComponentRef ref, const Vec3 &v) {
	if (Transform_ *c = GetComponent_(transforms, ref))
		c->TRS.scl = v;
	else
		warn("Invalid transform component");
}

Vec3 Transform::GetScale() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetTransformScale(ref);

	warn("Orphaned transform component");
	return Vec3::One;
}

void Transform::SetScale(const Vec3 &v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetTransformScale(ref, v);
	else
		warn("Orphaned transform component");
}

TransformTRS Scene::GetTransformTRS(ComponentRef ref) const {
	if (const Transform_ *c = GetComponent_(transforms, ref))
		return c->TRS;

	warn("Invalid transform component");
	return TransformTRS();
}

void Scene::SetTransformTRS(ComponentRef ref, const TransformTRS &v) {
	if (Transform_ *c = GetComponent_(transforms, ref))
		c->TRS = v;
	else
		warn("Invalid transform component");
}

TransformTRS Transform::GetTRS() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetTransformTRS(ref);

	warn("Orphaned transform component");
	return TransformTRS();
}

void Transform::SetTRS(const TransformTRS &v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetTransformTRS(ref, v);
	else
		warn("Orphaned transform component");
}

NodeRef Scene::GetTransformParent(ComponentRef ref) const {
	if (const Transform_ *c = GetComponent_(transforms, ref))
		return c->parent;

	warn("Invalid transform component");
	return InvalidNodeRef;
}

void Scene::SetTransformParent(ComponentRef ref, const NodeRef &v) {
	if (Transform_ *c = GetComponent_(transforms, ref)) {
		if (!hg::IsChildOf(*scene_ref->scene, v, ref))
			c->parent = v;
		else
			warn("Cyclical reference detected");
	} else {
		warn("Invalid transform component");
	}
}

NodeRef Transform::GetParent() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetTransformParent(ref);

	warn("Orphaned transform component");
	return InvalidNodeRef;
}

void Transform::SetParent(NodeRef v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetTransformParent(ref, v);
	else
		warn("Orphaned transform component");
}

Node Transform::GetParentNode() const {
	if (scene_ref && scene_ref->scene) {
		Node node;
		node.scene_ref = scene_ref;
		node.ref = scene_ref->scene->GetTransformParent(ref);
		return node;
	}

	warn("Orphaned transform component");
	return Node();
}

void Transform::SetParentNode(const Node &n) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetTransformParent(ref, n.ref);
	else
		warn("Orphaned transform component");
}

Transform Scene::CreateTransform(const Vec3 &pos, const Vec3 &rot, const Vec3 &scl, NodeRef parent) {
	Transform_ transform_;
	transform_.TRS.pos = pos;
	transform_.TRS.rot = rot;
	transform_.TRS.scl = scl;
	transform_.parent = parent;

	Transform transform;
	transform.scene_ref = scene_ref;
	transform.ref = transforms.add_ref(transform_);

	return transform;
}

Transform Scene::CreateTransform(const Mat4 &mtx, NodeRef parent) {
	Vec3 pos, rot, scl;
	Decompose(mtx, &pos, &rot, &scl);
	return CreateTransform(pos, rot, scl);
}

//
void Scene::SetTransformLocalMatrix(ComponentRef ref, const Mat4 &local) {
	if (Transform_ *trs = GetComponent_(transforms, ref)) {
		Decompose(local, &trs->TRS.pos, &trs->TRS.rot, &trs->TRS.scl);

		const ComponentRef parent_trs_ref = GetNodeComponentRef_<NCI_Transform>(trs->parent);
		const Mat4 world = IsValidTransformRef(parent_trs_ref) ? transform_worlds[parent_trs_ref.idx] * local : local;

		transform_worlds[ref.idx] = world;
	} else {
		warn("Invalid transform component");
	}
}

Mat4 Scene::GetTransformWorldMatrix(const uint32_t transform_idx) const {
	if (transform_idx < transform_worlds.size())
		return transform_worlds[transform_idx];

	warn("Invalid transform index");
	return Mat4::Identity;
}

Mat4 Scene::GetPreviousTransformWorldMatrix(const uint32_t transform_idx) const {
	if (transform_idx < previous_transform_worlds.size())
		return previous_transform_worlds[transform_idx];

	warn("Invalid transform index");
	return transform_worlds[transform_idx];
}

void Scene::SetTransformWorldMatrix(ComponentRef ref, const Mat4 &world) {
	if (Transform_ *trs = GetComponent_(transforms, ref)) {
		if (ref.idx < transform_worlds.size()) {
			transform_worlds[ref.idx] = world;

			const ComponentRef parent_trs_ref = GetNodeComponentRef_<NCI_Transform>(trs->parent);
			const Mat4 local = IsValidTransformRef(parent_trs_ref) ? InverseFast(transform_worlds[parent_trs_ref.idx]) * world : world;

			Decompose(local, &trs->TRS.pos, &trs->TRS.rot, &trs->TRS.scl);
		} else {
			warn("Invalid transform index");
		}
	} else {
		warn("Invalid transform component");
	}
}

//
Mat4 Transform::GetWorld() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetTransformWorldMatrix(ref.idx);

	warn("Orphaned transform component");
	return Mat4::Identity;
}

void Transform::SetWorld(const Mat4 &world) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetTransformWorldMatrix(ref, world);
	else
		warn("Orphaned transform component");
}

void Transform::SetLocal(const Mat4 &local) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetTransformLocalMatrix(ref, local);
	else
		warn("Orphaned transform component");
}

//
Camera Scene::CreateCamera() {
	Camera camera;
	camera.scene_ref = scene_ref;
	Camera_ camera_;
	camera.ref = cameras.add_ref(camera_);
	return camera;
}

void Scene::DestroyCamera(ComponentRef ref) { cameras.remove_ref(ref); }

float Scene::GetCameraZNear(ComponentRef ref) const {
	if (const Camera_ *c = GetComponent_(cameras, ref))
		return c->zrange.znear;

	warn("Invalid camera component");
	return 0;
}

void Scene::SetCameraZNear(ComponentRef ref, float v) {
	if (Camera_ *c = GetComponent_(cameras, ref))
		c->zrange.znear = Clamp(v, 0.0001f, c->zrange.zfar - 0.0001f);
	else
		warn("Invalid camera component");
}

bool Camera::IsValid() const { return scene_ref && scene_ref->scene ? scene_ref->scene->IsValidCameraRef(ref) : false; }

float Camera::GetZNear() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetCameraZNear(ref);

	warn("Orphaned camera component");
	return 0;
}

void Camera::SetZNear(float v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetCameraZNear(ref, v);
	else
		warn("Orphaned camera component");
}

float Scene::GetCameraZFar(ComponentRef ref) const {
	if (const Camera_ *c = GetComponent_(cameras, ref))
		return c->zrange.zfar;

	warn("Invalid camera component");
	return 0;
}

void Scene::SetCameraZFar(ComponentRef ref, float v) {
	if (Camera_ *c = GetComponent_(cameras, ref))
		c->zrange.zfar = Max(v, c->zrange.znear + 0.0001f);
	else
		warn("Invalid camera component");
}

float Camera::GetZFar() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetCameraZFar(ref);

	warn("Orphaned camera component");
	return 0;
}

void Camera::SetZFar(float v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetCameraZFar(ref, v);
	else
		warn("Orphaned camera component");
}

CameraZRange Scene::GetCameraZRange(ComponentRef ref) const {
	if (const Camera_ *c = GetComponent_(cameras, ref))
		return c->zrange;

	warn("Invalid camera component");
	return CameraZRange();
}

void Scene::SetCameraZRange(ComponentRef ref, CameraZRange v) {
	if (Camera_ *c = GetComponent_(cameras, ref)) {
		const float znear = Clamp(v.znear, 0.0001f, v.zfar - 0.0001f), zfar = Max(v.zfar, znear);
		c->zrange.znear = znear;
		c->zrange.zfar = zfar;
	} else {
		warn("Invalid camera component");
	}
}

CameraZRange Camera::GetZRange() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetCameraZRange(ref);

	warn("Orphaned camera component");
	return CameraZRange();
}

void Camera::SetZRange(CameraZRange v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetCameraZRange(ref, v);
	else
		warn("Orphaned camera component");
}

float Scene::GetCameraFov(ComponentRef ref) const {
	if (const Camera_ *c = GetComponent_(cameras, ref))
		return c->fov;

	warn("Invalid camera component");
	return 0;
}

void Scene::SetCameraFov(ComponentRef ref, float v) {
	if (Camera_ *c = GetComponent_(cameras, ref))
		c->fov = Clamp(v, Deg(0.0001f), Deg(180.f - 0.0001f));
	else
		warn("Invalid camera component");
}

float Camera::GetFov() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetCameraFov(ref);

	warn("Orphaned camera component");
	return 0;
}

void Camera::SetFov(float v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetCameraFov(ref, v);
	else
		warn("Orphaned camera component");
}

bool Scene::GetCameraIsOrthographic(ComponentRef ref) const {
	if (const Camera_ *c = GetComponent_(cameras, ref))
		return c->ortho;

	warn("Invalid camera component");
	return false;
}

void Scene::SetCameraIsOrthographic(ComponentRef ref, const bool &v) {
	if (Camera_ *c = GetComponent_(cameras, ref))
		c->ortho = v;
}

bool Camera::GetIsOrthographic() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetCameraIsOrthographic(ref);

	warn("Orphaned camera component");
	return false;
}

void Camera::SetIsOrthographic(bool v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetCameraIsOrthographic(ref, v);
	else
		warn("Orphaned camera component");
}

float Scene::GetCameraSize(ComponentRef ref) const {
	if (const Camera_ *c = GetComponent_(cameras, ref))
		return c->size;

	warn("Invalid camera component");
	return 0;
}

void Scene::SetCameraSize(ComponentRef ref, float v) {
	if (Camera_ *c = GetComponent_(cameras, ref))
		c->size = v != 0.f ? v : 0.0001f;
	else
		warn("Invalid camera component");
}

float Camera::GetSize() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetCameraSize(ref);

	warn("Orphaned camera component");
	return 0;
}

void Camera::SetSize(float v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetCameraSize(ref, v);
	else
		warn("Orphaned camera component");
}

Camera Scene::CreateCamera(const float znear, const float zfar, const float fov) {
	Camera_ camera_;
	camera_.zrange.znear = znear;
	camera_.zrange.zfar = zfar;
	camera_.fov = fov;
	camera_.size = 0.f;
	camera_.ortho = false;

	Camera camera;
	camera.scene_ref = scene_ref;
	camera.ref = cameras.add_ref(camera_);

	return camera;
}

Camera Scene::CreateOrthographicCamera(const float znear, const float zfar, const float size) {
	Camera_ camera_;
	camera_.zrange.znear = znear;
	camera_.zrange.zfar = zfar;
	camera_.fov = Deg(45.f);
	camera_.size = size;
	camera_.ortho = true;

	Camera camera;
	camera.scene_ref = scene_ref;
	camera.ref = cameras.add_ref(camera_);

	return camera;
}

//
Object Scene::CreateObject() {
	Object object;
	object.scene_ref = scene_ref;
	Object_ object_;
	object.ref = objects.add_ref(object_);
	return object;
}

void Scene::DestroyObject(ComponentRef ref) { objects.remove_ref(ref); }

ModelRef Scene::GetObjectModel(ComponentRef ref) const {
	if (const Object_ *c = GetComponent_(objects, ref))
		return c->model;

	warn("Invalid object component");
	return InvalidModelRef;
}

void Scene::SetObjectModel(ComponentRef ref, const ModelRef &v) {
	if (Object_ *c = GetComponent_(objects, ref))
		c->model = v;
	else
		warn("Invalid object component");
}

bool Object::IsValid() const { return scene_ref && scene_ref->scene ? scene_ref->scene->IsValidObjectRef(ref) : false; }

ModelRef Object::GetModelRef() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetObjectModel(ref);

	warn("Orphaned object component");
	return InvalidModelRef;
}

void Object::SetModelRef(ModelRef v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetObjectModel(ref, v);
	else
		warn("Orphaned object component");
};

void Object::ClearModelRef() {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetObjectModel(ref, InvalidModelRef);
	else
		warn("Orphaned object component");
};

//
static Material null_bound_material;

Material &Scene::GetObjectMaterial(ComponentRef ref, size_t slot_idx) {
	if (Object_ *c = GetComponent_(objects, ref))
		return slot_idx < c->materials.size() ? c->materials[slot_idx] : null_bound_material;

	warn("Invalid object component");
	return null_bound_material;
}

void Scene::SetObjectMaterial(ComponentRef ref, size_t slot_idx, Material material) {
	if (Object_ *c = GetComponent_(objects, ref)) {
		if (c->materials.size() <= slot_idx)
			c->materials.resize(slot_idx + 1);
		c->materials[slot_idx] = material;
	} else {
		warn("Invalid object component");
	}
}

Material *Scene::GetObjectMaterial(ComponentRef ref, const std::string &name) {
	if (Object_ *c = GetComponent_(objects, ref)) {
		size_t i = 0;
		for (; i < c->material_infos.size(); ++i)
			if (c->material_infos[i].name == name)
				break;

		if (i == c->material_infos.size() || i >= c->materials.size()) {
			warn(fmt::format("Object has no material named '{}'", name));
			return nullptr;
		}

		return &c->materials[i];
	}

	warn("Invalid object component");
	return nullptr;
}

Material &Object::GetMaterial(const size_t slot_idx) const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetObjectMaterial(ref, slot_idx);

	warn("Orphaned object component");
	return null_bound_material;
}

void Object::SetMaterial(const size_t slot_idx, Material material) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetObjectMaterial(ref, slot_idx, material);
	else
		warn("Orphaned object component");
}

std::string Scene::GetObjectMaterialName(ComponentRef ref, size_t slot_idx) const {
	if (const Object_ *c = GetComponent_(objects, ref)) {
		if (slot_idx < c->material_infos.size())
			return c->material_infos[slot_idx].name;

		warn("Invalid object material slot index");
	}

	warn("Invalid object component");
	return std::string();
}

void Scene::SetObjectMaterialName(ComponentRef ref, size_t slot_idx, const std::string &name) {
	if (Object_ *c = GetComponent_(objects, ref)) {
		c->material_infos.resize(slot_idx + 1);
		c->material_infos[slot_idx].name = name;
	} else {
		warn("Invalid object component");
	}
}

//
size_t Scene::GetObjectBoneCount(ComponentRef ref) const {
	if (const Object_ *c = GetComponent_(objects, ref))
		return c->bones.size();

	warn("Invalid object component");
	return 0;
}

size_t Object::GetBoneCount() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetObjectBoneCount(ref);

	warn("Orphaned object component");
	return 0;
}

void Scene::SetObjectBoneCount(ComponentRef ref, size_t count) {
	if (Object_ *c = GetComponent_(objects, ref))
		c->bones.resize(count);
	else
		warn("Invalid object component");
}

void Object::SetBoneCount(size_t count) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetObjectBoneCount(ref, count);
	else
		warn("Orphaned object component");
}

bool Scene::SetObjectBone(ComponentRef ref, size_t idx, NodeRef bone_node) {
	if (Object_ *c = GetComponent_(objects, ref)) {
		if (idx < c->bones.size()) {
			c->bones[idx] = bone_node;
			return true;
		} else {
			warn("Invalid bone index");
		}
	} else {
		warn("Invalid object component");
	}
	return false;
}

bool Object::SetBone(size_t idx, NodeRef bone) {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->SetObjectBone(ref, idx, bone);

	warn("Orphaned object component");
	return false;
}

NodeRef Scene::GetObjectBone(ComponentRef ref, size_t idx) const {
	if (const Object_ *c = GetComponent_(objects, ref)) {
		if (idx < c->bones.size())
			return c->bones[idx];
		else
			warn("Invalid bone index");
	}
	return InvalidNodeRef;
}

NodeRef Object::GetBone(size_t idx) const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetObjectBone(ref, idx);

	warn("Orphaned object component");
	return InvalidNodeRef;
}

Node Object::GetBoneNode(size_t idx) const {
	Node node;
	node.scene_ref = scene_ref;
	node.ref = GetBone(idx);
	return node;
}

bool Object::SetBoneNode(size_t idx, const Node &node) { return SetBone(idx, node.ref); }

//
std::string Object::GetMaterialName(size_t slot_idx) const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetObjectMaterialName(ref, slot_idx);

	warn("Orphaned object component");
	return std::string();
}

void Object::SetMaterialName(size_t slot_idx, const std::string &name) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetObjectMaterialName(ref, slot_idx, name);
	else
		warn("Orphaned object component");
}

size_t Scene::GetObjectMaterialCount(ComponentRef ref) const {
	if (const Object_ *c = GetComponent_(objects, ref))
		return c->materials.size();

	warn("Invalid object component");
	return 0;
}

void Scene::SetObjectMaterialCount(ComponentRef ref, size_t v) {
	if (Object_ *c = GetComponent_(objects, ref)) {
		c->material_infos.resize(v);
		c->materials.resize(v);
	} else {
		warn("Invalid object component");
	}
}

size_t Object::GetMaterialCount() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetObjectMaterialCount(ref);

	warn("Orphaned object component");
	return 0;
}

void Object::SetMaterialCount(size_t v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetObjectMaterialCount(ref, v);
	else
		warn("Orphaned object component");
}

Material *Object::GetMaterial(const std::string &name) const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetObjectMaterial(ref, name);

	warn("Orphaned object component");
	return nullptr;
}

bool Scene::GetObjectMinMax(ComponentRef ref, const PipelineResources &resources, MinMax &minmax) const {
	const Object_ *c = GetComponent_(objects, ref);
	if (!c) {
		warn("Invalid object component");
		return false;
	}

	if (!resources.models.IsValidRef(c->model))
		return false;

	const Model &mdl = resources.models.Get_unsafe_(c->model.ref.idx);
	if (mdl.bounds.empty())
		return false;

	minmax = mdl.bounds[0];
	for (size_t i = 1; i < mdl.bounds.size(); ++i)
		minmax = Union(minmax, mdl.bounds[i]);

	return true;
}

bool Object::GetMinMax(const PipelineResources &resources, MinMax &minmax) const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetObjectMinMax(ref, resources, minmax);

	warn("Orphaned object component");
	return false;
}

Object Scene::CreateObject(const ModelRef &model, const std::vector<Material> &materials) {
	Object_ object_;
	object_.model = model;
	object_.materials = materials;

	Object object;
	object.scene_ref = scene_ref;
	object.ref = objects.add_ref(object_);

	return object;
}

//
Light Scene::CreateLight() {
	Light light;
	light.scene_ref = scene_ref;
	Light_ light_;
	light.ref = lights.add_ref(light_);
	return light;
}

void Scene::DestroyLight(ComponentRef ref) { lights.remove_ref(ref); }

LightType Scene::GetLightType(ComponentRef ref) const {
	if (const Light_ *c = GetComponent_(lights, ref))
		return c->type;

	warn("Invalid light component");
	return LT_Point;
}

void Scene::SetLightType(ComponentRef ref, LightType v) {
	if (Light_ *c = GetComponent_(lights, ref))
		c->type = v;
	else
		warn("Invalid light component");
}

LightShadowType Scene::GetLightShadowType(ComponentRef ref) const {
	if (const Light_ *c = GetComponent_(lights, ref))
		return c->shadow_type;

	warn("Invalid light component");
	return LST_None;
}

void Scene::SetLightShadowType(ComponentRef ref, LightShadowType v) {
	if (Light_ *c = GetComponent_(lights, ref))
		c->shadow_type = v;
	else
		warn("Invalid light component");
}

bool Light::IsValid() const { return scene_ref && scene_ref->scene ? scene_ref->scene->IsValidLightRef(ref) : false; }

LightType Light::GetType() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetLightType(ref);

	warn("Orphaned light component");
	return LT_Point;
}

void Light::SetType(LightType v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetLightType(ref, v);
	else
		warn("Orphaned light component");
}

LightShadowType Light::GetShadowType() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetLightShadowType(ref);

	warn("Orphaned light component");
	return LST_None;
}

void Light::SetShadowType(LightShadowType v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetLightShadowType(ref, v);
	else
		warn("Orphaned light component");
}

Color Scene::GetLightDiffuseColor(ComponentRef ref) const {
	if (const Light_ *c = GetComponent_(lights, ref))
		return c->diffuse;

	warn("Invalid light component");
	return Color::Red;
}

void Scene::SetLightDiffuseColor(ComponentRef ref, const Color &v) {
	if (Light_ *c = GetComponent_(lights, ref))
		c->diffuse = v;
	else
		warn("Invalid light component");
}

float Scene::GetLightDiffuseIntensity(ComponentRef ref) const {
	if (const Light_ *c = GetComponent_(lights, ref))
		return c->diffuse_intensity;

	warn("Invalid light component");
	return 0;
}

void Scene::SetLightDiffuseIntensity(ComponentRef ref, float v) {
	if (Light_ *c = GetComponent_(lights, ref))
		c->diffuse_intensity = v;
	else
		warn("Invalid light component");
}

Color Light::GetDiffuseColor() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetLightDiffuseColor(ref);

	warn("Orphaned light component");
	return Color::Red;
}

void Light::SetDiffuseColor(const Color &v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetLightDiffuseColor(ref, v);
	else
		warn("Orphaned light component");
}

float Light::GetDiffuseIntensity() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetLightDiffuseIntensity(ref);

	warn("Orphaned light component");
	return 0;
}

void Light::SetDiffuseIntensity(float v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetLightDiffuseIntensity(ref, v);
	else
		warn("Ophaned Light");
}

Color Scene::GetLightSpecularColor(ComponentRef ref) const {
	if (const Light_ *c = GetComponent_(lights, ref))
		return c->specular;

	warn("Invalid light component");
	return Color::Red;
}

void Scene::SetLightSpecularColor(ComponentRef ref, const Color &v) {
	if (Light_ *c = GetComponent_(lights, ref))
		c->specular = v;
	else
		warn("Invalid light component");
}

float Scene::GetLightSpecularIntensity(ComponentRef ref) const {
	if (const Light_ *c = GetComponent_(lights, ref))
		return c->specular_intensity;

	warn("Invalid light component");
	return 0;
}

void Scene::SetLightSpecularIntensity(ComponentRef ref, float v) {
	if (Light_ *c = GetComponent_(lights, ref))
		c->specular_intensity = v;
	else
		warn("Invalid light component");
}

Color Light::GetSpecularColor() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetLightSpecularColor(ref);

	warn("Orphaned light component");
	return Color::Red;
}

void Light::SetSpecularColor(const Color &v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetLightSpecularColor(ref, v);
	else
		warn("Orphaned light component");
}

float Light::GetSpecularIntensity() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetLightSpecularIntensity(ref);

	warn("Orphaned light component");
	return 0;
}

void Light::SetSpecularIntensity(float v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetLightSpecularIntensity(ref, v);
	else
		warn("Orphaned light component");
}

float Scene::GetLightRadius(ComponentRef ref) const {
	if (const Light_ *c = GetComponent_(lights, ref))
		return c->radius;

	warn("Invalid light component");
	return 0;
}

void Scene::SetLightRadius(ComponentRef ref, float v) {
	if (Light_ *c = GetComponent_(lights, ref))
		c->radius = Max(v, 0.f);
	else
		warn("Invalid light component");
}

float Light::GetRadius() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetLightRadius(ref);

	warn("Orphaned light component");
	return 0;
}

void Light::SetRadius(float v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetLightRadius(ref, v);
	else
		warn("Orphaned light component");
}

float Scene::GetLightInnerAngle(ComponentRef ref) const {
	if (const Light_ *c = GetComponent_(lights, ref))
		return c->inner_angle;

	warn("Invalid light component");
	return 0;
}

void Scene::SetLightInnerAngle(ComponentRef ref, float v) {
	if (Light_ *c = GetComponent_(lights, ref))
		c->inner_angle = Clamp(v, 0.0001f, c->outer_angle - 0.0001f);
	else
		warn("Invalid light component");
}

float Light::GetInnerAngle() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetLightInnerAngle(ref);

	warn("Orphaned light component");
	return 0;
}

void Light::SetInnerAngle(float v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetLightInnerAngle(ref, v);
	else
		warn("Orphaned light component");
}

float Scene::GetLightOuterAngle(ComponentRef ref) const {
	if (const Light_ *c = GetComponent_(lights, ref))
		return c->outer_angle;

	warn("Invalid light component");
	return 0;
}

void Scene::SetLightOuterAngle(ComponentRef ref, float v) {
	if (Light_ *c = GetComponent_(lights, ref))
		c->outer_angle = Max(v, c->inner_angle + 0.0001f);
	else
		warn("Invalid light component");
}

float Light::GetOuterAngle() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetLightOuterAngle(ref);

	warn("Orphaned light component");
	return 0;
}

void Light::SetOuterAngle(float v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetLightOuterAngle(ref, v);
	else
		warn("Orphaned light component");
}

Vec4 Scene::GetLightPSSMSplit(ComponentRef ref) const {
	if (const Light_ *c = GetComponent_(lights, ref))
		return c->pssm_split;

	warn("Invalid light component");
	return default_pssm_split;
}

void Scene::SetLightPSSMSplit(ComponentRef ref, const Vec4 &v) {
	if (Light_ *c = GetComponent_(lights, ref))
		c->pssm_split = v;
	else
		warn("Invalid light component");
}

Vec4 Light::GetPSSMSplit() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetLightPSSMSplit(ref);

	warn("Orphaned light component");
	return default_pssm_split;
}

void Light::SetPSSMSplit(const Vec4 &v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetLightPSSMSplit(ref, v);
	else
		warn("Orphaned light component");
}

float Scene::GetLightPriority(ComponentRef ref) const {
	if (const Light_ *c = GetComponent_(lights, ref))
		return c->priority;

	warn("Invalid light component");
	return 0;
}

void Scene::SetLightPriority(ComponentRef ref, float v) {
	if (Light_ *c = GetComponent_(lights, ref))
		c->priority = v;
	else
		warn("Invalid light component");
}

float Light::GetPriority() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetLightPriority(ref);

	warn("Orphaned light component");
	return 0;
}

void Light::SetPriority(float v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetLightPriority(ref, v);
	else
		warn("Orphaned light component");
}

float Scene::GetLightShadowBias(ComponentRef ref) const {
	if (const Light_ *c = GetComponent_(lights, ref))
		return c->shadow_bias;

	warn("Invalid light component");
	return default_shadow_bias;
}

void Scene::SetLightShadowBias(ComponentRef ref, float v) {
	if (Light_ *c = GetComponent_(lights, ref))
		c->shadow_bias = v;
	else
		warn("Invalid light component");
}

float Light::GetShadowBias() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetLightShadowBias(ref);

	warn("Orphaned light component");
	return default_shadow_bias;
}

void Light::SetShadowBias(float v) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetLightShadowBias(ref, v);
	else
		warn("Orphaned light component");
}

//
Light Scene::CreateLinearLight(const Color &diffuse, float diffuse_intensity, const Color &specular, float specular_intensity, float priority,
	LightShadowType shadow_type, float shadow_bias, const Vec4 &pssm_split) {
	Light_ light_;
	light_.type = LT_Linear;
	light_.shadow_type = shadow_type;
	light_.diffuse = diffuse;
	light_.diffuse_intensity = diffuse_intensity;
	light_.specular = specular;
	light_.specular_intensity = specular_intensity;
	light_.radius = 0.f;
	light_.inner_angle = 0.f;
	light_.outer_angle = 0.f;
	light_.pssm_split = pssm_split;
	light_.priority = priority;
	light_.shadow_bias = shadow_bias;

	Light light;
	light.scene_ref = scene_ref;
	light.ref = lights.add_ref(light_);

	return light;
}

Light Scene::CreatePointLight(const float radius, const Color &diffuse, float diffuse_intensity, const Color &specular, float specular_intensity,
	float priority, LightShadowType shadow_type, float shadow_bias) {
	Light_ light_;
	light_.type = LT_Point;
	light_.shadow_type = shadow_type;
	light_.diffuse = diffuse;
	light_.diffuse_intensity = diffuse_intensity;
	light_.specular = specular;
	light_.specular_intensity = specular_intensity;
	light_.radius = radius;
	light_.inner_angle = 0.f;
	light_.outer_angle = 0.f;
	light_.pssm_split = Vec4::Zero;
	light_.priority = priority;
	light_.shadow_bias = shadow_bias;

	Light light;
	light.scene_ref = scene_ref;
	light.ref = lights.add_ref(light_);

	return light;
}

Light Scene::CreateSpotLight(const float radius, const float inner_angle, const float outer_angle, const Color &diffuse, float diffuse_intensity,
	const Color &specular, float specular_intensity, float priority, LightShadowType shadow_type, float shadow_bias) {
	Light_ light_;
	light_.type = LT_Spot;
	light_.shadow_type = shadow_type;
	light_.diffuse = diffuse;
	light_.diffuse_intensity = diffuse_intensity;
	light_.specular = specular;
	light_.specular_intensity = specular_intensity;
	light_.radius = radius;
	light_.inner_angle = inner_angle;
	light_.outer_angle = outer_angle;
	light_.pssm_split = Vec4::Zero;
	light_.priority = priority;
	light_.shadow_bias = shadow_bias;

	Light light;
	light.scene_ref = scene_ref;
	light.ref = lights.add_ref(light_);

	return light;
}

//
bool RigidBody::IsValid() const { return scene_ref && scene_ref->scene ? scene_ref->scene->IsValidRigidBodyRef(ref) : false; }

RigidBodyType RigidBody::GetType() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetRigidBodyType(ref);

	warn("Orphaned rigidBody component");
	return RBT_Dynamic;
}

void RigidBody::SetType(RigidBodyType type) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetRigidBodyType(ref, type);
	else
		warn("Orphaned rigidBody component");
}

float RigidBody::GetLinearDamping() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetRigidBodyLinearDamping(ref);

	warn("Orphaned rigidBody component");
	return 0;
}

void RigidBody::SetLinearDamping(float damping) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetRigidBodyLinearDamping(ref, damping);
	else
		warn("Orphaned rigidBody component");
}

float RigidBody::GetAngularDamping() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetRigidBodyAngularDamping(ref);

	warn("Orphaned rigidBody component");
	return 0;
}

void RigidBody::SetAngularDamping(float damping) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetRigidBodyAngularDamping(ref, damping);
	else
		warn("Orphaned rigidBody component");
}

float RigidBody::GetRestitution() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetRigidBodyRestitution(ref);

	warn("Orphaned rigidBody component");
	return 0;
}

void RigidBody::SetRestitution(float restitution) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetRigidBodyRestitution(ref, restitution);
	else
		warn("Orphaned rigidBody component");
}

float RigidBody::GetFriction() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetRigidBodyFriction(ref);

	warn("Orphaned rigidBody component");
	return 0;
}

void RigidBody::SetFriction(float friction) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetRigidBodyFriction(ref, friction);
	else
		warn("Orphaned rigidBody component");
}

float RigidBody::GetRollingFriction() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetRigidBodyRollingFriction(ref);

	warn("Orphaned rigidBody component");
	return 0;
}

void RigidBody::SetRollingFriction(float rolling_friction) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetRigidBodyRollingFriction(ref, rolling_friction);
	else
		warn("Orphaned rigidBody component");
}

//
bool Collision::IsValid() const { return scene_ref && scene_ref->scene ? scene_ref->scene->IsValidCollisionRef(ref) : false; }

CollisionType Collision::GetType() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetCollisionType(ref);

	warn("Orphaned collision component");
	return CT_Sphere;
}

void Collision::SetType(CollisionType type) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetCollisionType(ref, type);
	else
		warn("Orphaned collision component");
}

Mat4 Collision::GetLocalTransform() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetCollisionLocalTransform(ref);

	warn("Orphaned collision component");
	return Mat4::Identity;
}

void Collision::SetLocalTransform(const Mat4 &local) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetCollisionLocalTransform(ref, local);
	else
		warn("Orphaned collision component");
}

Vec3 Collision::GetPosition() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetCollisionPosition(ref);

	warn("Orphaned collision component");
	return Vec3::Zero;
}

void Collision::SetPosition(const Vec3 &pos) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetCollisionPosition(ref, pos);
	else
		warn("Orphaned collision component");
}

Vec3 Collision::GetRotation() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetCollisionRotation(ref);

	warn("Orphaned collision component");
	return Vec3::Zero;
}

void Collision::SetRotation(const Vec3 &rot) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetCollisionRotation(ref, rot);
	else
		warn("Orphaned collision component");
}

float Collision::GetMass() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetCollisionMass(ref);

	warn("Orphaned collision component");
	return 0.f;
}

void Collision::SetMass(float mass) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetCollisionMass(ref, mass);
	else
		warn("Orphaned collision component");
}

float Collision::GetRadius() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetCollisionRadius(ref);

	warn("Orphaned collision component");
	return 0.f;
}

void Collision::SetRadius(float radius) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetCollisionRadius(ref, radius);
	else
		warn("Orphaned collision component");
}

float Collision::GetHeight() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetCollisionHeight(ref);

	warn("Orphaned collision component");
	return 0;
}

void Collision::SetHeight(float height) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetCollisionHeight(ref, height);
	else
		warn("Orphaned collision component");
}

Vec3 Collision::GetSize() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetCollisionSize(ref);

	warn("Orphaned collision component");
	return Vec3::Zero;
}

void Collision::SetSize(const Vec3 &size) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetCollisionSize(ref, size);
	else
		warn("Orphaned collision component");
}

std::string Collision::GetCollisionResource() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetCollisionResource(ref);

	warn("Orphaned collision component");
	return std::string();
}

void Collision::SetCollisionResource(const std::string &path) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetCollisionResource(ref, path);
	else
		warn("Orphaned collision component");
}

//
bool Instance::IsValid() const { return scene_ref && scene_ref->scene ? scene_ref->scene->IsValidInstanceRef(ref) : false; }

std::string Instance::GetPath() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetInstancePath(ref);

	warn("Orphaned instance component");
	return std::string();
}

void Instance::SetPath(const std::string &path) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetInstancePath(ref, path);
	else
		warn("Orphaned instance component");
}

void Instance::SetOnInstantiateAnim(const std::string &anim) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetOnInstantiateAnim(ref, anim);
	else
		warn("Orphaned instance component");
}

void Instance::SetOnInstantiateAnimLoopMode(AnimLoopMode loop_mode) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetOnInstantiateAnimLoopMode(ref, loop_mode);
	else
		warn("Orphaned instance component");
}

void Instance::ClearOnInstantiateAnim() {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->ClearOnInstantiateAnim(ref);
	else
		warn("Orphaned instance component");
}

std::string Instance::GetOnInstantiateAnim() {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetOnInstantiateAnim(ref);

	warn("Orphaned instance component");
	return std::string();
}

AnimLoopMode Instance::GetOnInstantiateAnimLoopMode() {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetOnInstantiateAnimLoopMode(ref);

	warn("Orphaned instance component");
	return ALM_Once;
}

ScenePlayAnimRef Instance::GetOnInstantiatePlayAnimRef() {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetOnInstantiatePlayAnimRef(ref);

	warn("Orphaned instance component");
	return invalid_gen_ref;
}

//
bool Script::IsValid() const { return scene_ref && scene_ref->scene ? scene_ref->scene->IsValidScriptRef(ref) : false; }

std::string Script::GetPath() const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetScriptPath(ref);

	warn("Orphaned script component");
	return std::string();
}

void Script::SetPath(const std::string &path) {
	if (scene_ref && scene_ref->scene)
		scene_ref->scene->SetScriptPath(ref, path);
	else
		warn("Orphaned script component");
}

//
bool Script::HasParam(const std::string &name) const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->ScriptHasParam(ref, name);

	warn("Orphaned script component");
	return false;
}

ScriptParam Script::GetParam(const std::string &name) const {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->GetScriptParam(ref, name);

	warn("Orphaned script component");
	return ScriptParam();
}

bool Script::SetParam(const std::string &name, ScriptParam param) {
	if (scene_ref && scene_ref->scene)
		return scene_ref->scene->SetScriptParam(ref, name, param);

	warn("Orphaned script component");
	return false;
}

} // namespace hg
