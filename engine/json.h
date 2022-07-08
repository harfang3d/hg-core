// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "engine/scene.h"

#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

#include <map>

namespace hg {

inline std::string LightType_to_string(LightType v) {
	if (v == LT_Spot)
		return "spot";
	if (v == LT_Linear)
		return "linear";
	return "point";
}

inline LightType LightType_from_string(const std::string &v) {
	if (v == "spot")
		return LT_Spot;
	if (v == "linear")
		return LT_Linear;
	return LT_Point;
}

inline std::string LightShadowType_to_string(LightShadowType v) {
	if (v == LST_Map)
		return "map";
	return "none";
}

inline LightShadowType LightShadowType_from_string(const std::string &v) {
	if (v == "map")
		return LST_Map;
	return LST_None;
}

inline std::string AnimLoopMode_to_string(AnimLoopMode v) {
	if (v == ALM_Infinite)
		return "infinite";
	if (v == ALM_Loop)
		return "loop";
	return "none";
}

inline AnimLoopMode AnimLoopMode_from_string(const std::string &v) {
	if (v == "infinite")
		return ALM_Infinite;
	if (v == "loop")
		return ALM_Loop;
	return ALM_Once;
}

inline void ProbeType_to_json(rapidjson::Document &jd, rapidjson::Value &o, ProbeType v) {
	if (v == PT_Sphere)
		o.SetString("sphere", jd.GetAllocator());
	else if (v == PT_Cube)
		o.SetString("cube", jd.GetAllocator());
	else
		o.SetNull();
}

inline void ProbeType_from_json(const rapidjson::Value &j, ProbeType &v) {
	const std::string j_ = j.GetString();

	if (j_ == "sphere")
		v = PT_Sphere;
	else if (j_ == "cube")
		v = PT_Cube;
}

inline void RigidBodyType_to_json(rapidjson::Document &jd, rapidjson::Value &o, RigidBodyType v) {
	if (v == RBT_Dynamic)
		o.SetString("dynamic", jd.GetAllocator());
	else if (v == RBT_Kinematic)
		o.SetString("kinematic", jd.GetAllocator());
	else
		o.SetNull();
}

inline void RigidBodyType_from_json(const rapidjson::Value &j, RigidBodyType &v) {
	const std::string j_ = j.GetString();

	if (j_ == "dynamic")
		v = RBT_Dynamic;
	else if (j_ == "kinematic")
		v = RBT_Kinematic;
}

inline void CollisionType_to_json(rapidjson::Document &jd, rapidjson::Value &o, CollisionType v) {
	if (v == CT_Sphere)
		o.SetString("sphere", jd.GetAllocator());
	if (v == CT_Cube)
		o.SetString("cube", jd.GetAllocator());
	if (v == CT_Cone)
		o.SetString("cone", jd.GetAllocator());
	if (v == CT_Capsule)
		o.SetString("capsule", jd.GetAllocator());
	if (v == CT_Cylinder)
		o.SetString("cylinder", jd.GetAllocator());
	if (v == CT_Mesh)
		o.SetString("mesh", jd.GetAllocator());
	else
		o.SetNull();
}

inline void CollisionType_from_json(const rapidjson::Value &j, CollisionType &v) {
	const std::string j_ = j.GetString();

	if (j_ == "sphere")
		v = CT_Sphere;
	else if (j_ == "cube")
		v = CT_Cube;
	else if (j_ == "cone")
		v = CT_Cone;
	else if (j_ == "capsule")
		v = CT_Capsule;
	else if (j_ == "cylinder")
		v = CT_Cylinder;
	else if (j_ == "mesh")
		v = CT_Mesh;
}

//
inline void to_json(rapidjson::Document &jd, rapidjson::Value &o, bool v) { o.SetBool(v); }
inline void from_json(const rapidjson::Value &j, bool &v) { v = j.GetBool(); }
inline void to_json(rapidjson::Document &jd, rapidjson::Value &o, float v) { o.SetFloat(v); }
inline void from_json(const rapidjson::Value &j, float &v) { v = j.GetFloat(); }
inline void to_json(rapidjson::Document &jd, rapidjson::Value &o, int32_t v) { o.SetInt(v); }
inline void from_json(const rapidjson::Value &j, int32_t &v) { v = j.GetInt(); }
inline void to_json(rapidjson::Document &jd, rapidjson::Value &o, uint32_t v) { o.SetUint(v); }
inline void from_json(const rapidjson::Value &j, uint32_t &v) { v = j.GetUint(); }
inline void to_json(rapidjson::Document &jd, rapidjson::Value &o, int64_t v) { o.SetInt64(v); }
inline void from_json(const rapidjson::Value &j, int64_t &v) { v = j.GetInt64(); }
inline void to_json(rapidjson::Document &jd, rapidjson::Value &o, uint64_t v) { o.SetUint64(v); }
inline void from_json(const rapidjson::Value &j, uint64_t &v) { v = j.GetUint64(); }
inline void to_json(rapidjson::Document &jd, rapidjson::Value &o, const std::string &v) { o.SetString(v, jd.GetAllocator()); }
inline void from_json(const rapidjson::Value &j, std::string &v) { v = j.GetString(); }

inline void to_json(rapidjson::Document &jd, rapidjson::Value &o, const Vec2 &v) {
	o.SetArray();
	o.PushBack(v.x, jd.GetAllocator());
	o.PushBack(v.y, jd.GetAllocator());
}

inline void from_json(const rapidjson::Value &j, Vec2 &v) {
	v.x = j[0].GetFloat();
	v.y = j[1].GetFloat();
}

inline void to_json(rapidjson::Document &jd, rapidjson::Value &o, const Vec3 &v) {
	o.SetArray();
	o.PushBack(v.x, jd.GetAllocator());
	o.PushBack(v.y, jd.GetAllocator());
	o.PushBack(v.z, jd.GetAllocator());
}

inline void from_json(const rapidjson::Value &j, Vec3 &v) {
	v.x = j[0].GetFloat();
	v.y = j[1].GetFloat();
	v.z = j[2].GetFloat();
}

inline void to_json(rapidjson::Document &jd, rapidjson::Value &o, const Vec4 &v) {
	o.SetArray();
	o.PushBack(v.x, jd.GetAllocator());
	o.PushBack(v.y, jd.GetAllocator());
	o.PushBack(v.z, jd.GetAllocator());
	o.PushBack(v.w, jd.GetAllocator());
}

inline void from_json(const rapidjson::Value &j, Vec4 &v) {
	v.x = j[0].GetFloat();
	v.y = j[1].GetFloat();
	v.z = j[2].GetFloat();
	v.w = j[3].GetFloat();
}

inline void to_json(rapidjson::Document &jd, rapidjson::Value &o, const Quaternion &v) {
	o.SetArray();
	o.PushBack(v.x, jd.GetAllocator());
	o.PushBack(v.y, jd.GetAllocator());
	o.PushBack(v.z, jd.GetAllocator());
	o.PushBack(v.w, jd.GetAllocator());
}

inline void from_json(const rapidjson::Value &j, Quaternion &v) {
	v.x = j[0].GetFloat();
	v.y = j[1].GetFloat();
	v.z = j[2].GetFloat();
	v.w = j[3].GetFloat();
}

inline void to_json(rapidjson::Document &jd, rapidjson::Value &o, const Color &v) {
	o.SetArray();
	o.PushBack(v.r * 255.f, jd.GetAllocator());
	o.PushBack(v.g * 255.f, jd.GetAllocator());
	o.PushBack(v.b * 255.f, jd.GetAllocator());
	o.PushBack(v.a * 255.f, jd.GetAllocator());
}

inline void from_json(const rapidjson::Value &j, Color &v) {
	v.r = j[0].GetFloat() / 255.f;
	v.g = j[1].GetFloat() / 255.f;
	v.b = j[2].GetFloat() / 255.f;
	v.a = j[3].GetFloat() / 255.f;
}

inline void to_json(rapidjson::Document &jd, rapidjson::Value &o, const CameraZRange &v) {
	o.SetObject();
	o.AddMember("znear", v.znear, jd.GetAllocator());
	o.AddMember("zfar", v.zfar, jd.GetAllocator());
}

inline void from_json(const rapidjson::Value &j, CameraZRange &v) {
	v.znear = j["znear"].GetFloat();
	v.zfar = j["zfar"].GetFloat();
}

inline void to_json(rapidjson::Document &jd, rapidjson::Value &o, const gen_ref &v) {
	if (v != invalid_gen_ref)
		o.SetUint(v.idx);
	else
		o.SetNull();
}

inline void from_json(const rapidjson::Value &j, gen_ref &v) {
	if (j.IsNull())
		v = invalid_gen_ref;
	else
		v.idx = j.GetUint();
}

inline void to_json(rapidjson::Document &jd, rapidjson::Value &o, const Mat4 &v) {
	o.SetArray();
	for (size_t j = 0; j < 3; ++j)
		for (size_t i = 0; i < 4; ++i)
			o.PushBack(v.m[j][i], jd.GetAllocator());
}

inline void from_json(const rapidjson::Value &j_, Mat4 &v) {
	size_t k = 0;
	for (size_t j = 0; j < 3; ++j)
		for (size_t i = 0; i < 4; ++i)
			v.m[j][i] = j_[k++].GetFloat();
}

inline void to_json(rapidjson::Document &jd, rapidjson::Value &o, const Mat44 &v) {
	o.SetArray();
	for (size_t j = 0; j < 4; ++j)
		for (size_t i = 0; i < 4; ++i)
			o.PushBack(v.m[j][i], jd.GetAllocator());
}

inline void from_json(const rapidjson::Value &j_, Mat44 &v) {
	size_t k = 0;
	for (size_t j = 0; j < 4; ++j)
		for (size_t i = 0; i < 4; ++i)
			v.m[j][i] = j_[k++].GetFloat();
}

//
template <typename T> void set_json_key(rapidjson::Document &jd, rapidjson::Value &js, const std::string &key, const T &v) {
	rapidjson::Value k;
	k.SetString(key, jd.GetAllocator());
	rapidjson::Value o;
	to_json(jd, o, v);
	js.AddMember(k, o, jd.GetAllocator());
}

template <typename T> void set_json_key(rapidjson::Document &jd, rapidjson::Value &js, const char *key, const T &v) {
	rapidjson::Value k;
	k.SetString(key, jd.GetAllocator());
	rapidjson::Value o;
	to_json(jd, o, v);
	js.AddMember(k, o, jd.GetAllocator());
}

inline void set_json_key(rapidjson::Document &jd, rapidjson::Value &js, const std::string &key, rapidjson::Value &v) {
	rapidjson::Value k;
	k.SetString(key, jd.GetAllocator());
	js.AddMember(k, v, jd.GetAllocator());
}

inline void set_json_key(rapidjson::Document &jd, rapidjson::Value &js, const char *key, rapidjson::Value &v) {
	rapidjson::Value k;
	k.SetString(key, jd.GetAllocator());
	js.AddMember(k, v, jd.GetAllocator());
}

// helpers requiring a binary search: you will get MUCH better performance iterating over an object and acting on the key value
template <typename T> void get_json_key(const rapidjson::Value &js, const std::string &key, T &v) { from_json(js[key], v); }
template <typename T> void get_json_key(const rapidjson::Value &js, const char *key, T &v) { from_json(js[key], v); }

template <typename T> T get_json_key(const rapidjson::Value &js, const std::string &key) {
	T v;
	get_json_key(js, key, v);
	return v;
}
template <typename T> T get_json_key(const rapidjson::Value &js, const char *key) {
	T v;
	get_json_key(js, key, v);
	return v;
}

//
bool LoadJson(const Reader &ir, const Handle &h, rapidjson::Document &doc);
bool LoadJsonFromFile(const std::string &path, rapidjson::Document &doc);
bool LoadJsonFromAssets(const std::string &name, rapidjson::Document &doc);

bool SaveJsonToFile(const rapidjson::Value &js, const std::string &path);

#define for_json_object(I, V) for (rapidjson::Value::MemberIterator I = (V).MemberBegin(); I != (V).MemberEnd(); ++I)
#define for_json_object_const(I, V) for (rapidjson::Value::ConstMemberIterator I = (V).MemberBegin(); I != (V).MemberEnd(); ++I)

} // namespace hg
