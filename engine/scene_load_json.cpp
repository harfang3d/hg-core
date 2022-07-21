// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "engine/file_format.h"
#include "engine/json.h"
#include "engine/render_pipeline.h"
#include "engine/scene.h"

#include "foundation/log.h"
#include "foundation/matrix4.h"
#include "foundation/profiler.h"
#include "foundation/string.h"

#include <fmt/format.h>
#include <rapidjson/document.h>
#include <set>

namespace hg {

// [EJ] note: SaveComponent/LoadComponent are friend of class Scene and cannot be made static (breaks clang/gcc builds)

void SaveComponent(const Scene::Transform_ *data_, rapidjson::Document &jd, rapidjson::Value &js) {
	js.SetObject();
	set_json_key(jd, js, "pos", data_->TRS.pos);
	set_json_key(jd, js, "rot", data_->TRS.rot);
	set_json_key(jd, js, "scl", data_->TRS.scl);
	set_json_key(jd, js, "parent", data_->parent);
}

void SaveComponent(const Scene::Camera_ *data_, rapidjson::Document &jd, rapidjson::Value &js) {
	js.SetObject();
	set_json_key(jd, js, "zrange", data_->zrange);
	set_json_key(jd, js, "fov", data_->fov);
	set_json_key(jd, js, "ortho", data_->ortho);
	set_json_key(jd, js, "size", data_->size);
}

void SaveComponent(const Scene::Object_ *data_, rapidjson::Document &jd, rapidjson::Value &js, const PipelineResources &resources) {
	js.SetObject();
	set_json_key(jd, js, "name", resources.models.GetName(data_->model));

	const uint16_t mat_count = numeric_cast<uint16_t>(data_->materials.size());

	{
		rapidjson::Value materials_js(rapidjson::kArrayType);
		for (size_t j = 0; j < mat_count; ++j) {
			rapidjson::Value material_js;
			SaveMaterial(jd, material_js, data_->materials[j], resources);
			materials_js.PushBack(material_js, jd.GetAllocator());
		}
		set_json_key(jd, js, "materials", materials_js);
	}

	{
		rapidjson::Value materials_info_js(rapidjson::kArrayType);
		for (size_t j = 0; j < mat_count; ++j) {
			rapidjson::Value info_js(rapidjson::kObjectType);

			if (j < data_->material_infos.size())
				set_json_key(jd, info_js, "name", data_->material_infos[j].name);
			else
				set_json_key(jd, info_js, "name", "");

			materials_info_js.PushBack(info_js, jd.GetAllocator());
		}
		set_json_key(jd, js, "material_infos", materials_info_js);
	}

	{
		rapidjson::Value bones_js(rapidjson::kArrayType);
		for (std::vector<NodeRef>::const_iterator i = data_->bones.begin(); i != data_->bones.end(); ++i) {
			rapidjson::Value v;
			to_json(jd, v, *i);
			bones_js.PushBack(v, jd.GetAllocator());
		}
		set_json_key(jd, js, "bones", bones_js);
	}
}

void SaveComponent(const Scene::Light_ *data_, rapidjson::Document &jd, rapidjson::Value &js) {
	js.SetObject();
	set_json_key(jd, js, "type", data_->type);
	set_json_key(jd, js, "shadow_type", data_->shadow_type);
	set_json_key(jd, js, "diffuse", data_->diffuse);
	set_json_key(jd, js, "diffuse_intensity", data_->diffuse_intensity);
	set_json_key(jd, js, "specular", data_->specular);
	set_json_key(jd, js, "specular_intensity", data_->specular_intensity);
	set_json_key(jd, js, "radius", data_->radius);
	set_json_key(jd, js, "inner_angle", data_->inner_angle);
	set_json_key(jd, js, "outer_angle", data_->outer_angle);
	set_json_key(jd, js, "pssm_split", data_->pssm_split);
	set_json_key(jd, js, "priority", data_->priority);
	set_json_key(jd, js, "shadow_bias", data_->shadow_bias);
}

void SaveComponent(const Scene::RigidBody_ *data_, rapidjson::Document &jd, rapidjson::Value &js) {
	js.SetObject();
	set_json_key(jd, js, "type", data_->type);
	set_json_key(jd, js, "linear_damping", unpack_float(data_->linear_damping));
	set_json_key(jd, js, "angular_damping", unpack_float(data_->angular_damping));
	set_json_key(jd, js, "restitution", unpack_float(data_->restitution));
	set_json_key(jd, js, "friction", unpack_float(data_->friction));
	set_json_key(jd, js, "rolling_friction", unpack_float(data_->rolling_friction));
}

void SaveComponent(const Scene::Collision_ *data_, rapidjson::Document &jd, rapidjson::Value &js) {
	js.SetObject();
	set_json_key(jd, js, "type", data_->type);
	set_json_key(jd, js, "mass", data_->mass);
	set_json_key(jd, js, "path", data_->resource_path);
	set_json_key(jd, js, "pos", data_->trs.pos);
	set_json_key(jd, js, "rot", data_->trs.rot);
	set_json_key(jd, js, "scl", data_->trs.scl);
}

void SaveComponent(const Scene::Script_ *data_, rapidjson::Document &jd, rapidjson::Value &js) {
	js.SetObject();
	set_json_key(jd, js, "path", data_->path);

	if (!data_->params.empty()) {
		rapidjson::Value params_js(rapidjson::kObjectType);

		for (std::map<std::string, ScriptParam>::const_iterator i = data_->params.begin(); i != data_->params.end(); ++i) {
			rapidjson::Value param_js(rapidjson::kObjectType);

			if (i->second.type == SPT_Bool) {
				set_json_key(jd, param_js, "type", "bool");
				set_json_key(jd, param_js, "value", i->second.bv);
			} else if (i->second.type == SPT_Int) {
				set_json_key(jd, param_js, "type", "int");
				set_json_key(jd, param_js, "value", i->second.iv);
			} else if (i->second.type == SPT_Float) {
				set_json_key(jd, param_js, "type", "float");
				set_json_key(jd, param_js, "value", i->second.fv);
			} else if (i->second.type == SPT_String) {
				set_json_key(jd, param_js, "type", "string");
				set_json_key(jd, param_js, "value", i->second.sv);
			}

			set_json_key(jd, params_js, i->first, param_js);
		}

		set_json_key(jd, js, "parameters", params_js);
	}
}

void SaveComponent(const Scene::Instance_ *data_, rapidjson::Document &jd, rapidjson::Value &js) {
	js.SetObject();
	set_json_key(jd, js, "name", data_->name);

	if (!data_->anim.empty()) {
		set_json_key(jd, js, "anim", data_->anim);
		set_json_key(jd, js, "loop_mode", data_->loop_mode);
	}
}

static void SaveProbe(const Probe &probe, rapidjson::Document &jd, rapidjson::Value &js, const PipelineResources &resources) {
	js.SetObject();
	set_json_key(jd, js, "irradiance_map", resources.textures.GetName(probe.irradiance_map));
	set_json_key(jd, js, "radiance_map", resources.textures.GetName(probe.radiance_map));

	set_json_key(jd, js, "type", probe.type);
	set_json_key(jd, js, "parallax", probe.parallax);

	set_json_key(jd, js, "pos", probe.trs.pos);
	set_json_key(jd, js, "rot", probe.trs.rot);
	set_json_key(jd, js, "scl", probe.trs.scl);
}

//
void LoadComponent(Scene::Transform_ *data_, const rapidjson::Value &js) {
	get_json_key(js, "pos", data_->TRS.pos);

	Vec3 tmp;
	get_json_key(js, "rot", tmp);
	data_->TRS.rot = DegreeToRadian(tmp);

	get_json_key(js, "scl", data_->TRS.scl);
	get_json_key(js, "parent", data_->parent);
}

void LoadComponent(Scene::Camera_ *data_, const rapidjson::Value &js) {
	get_json_key(js, "zrange", data_->zrange);
	get_json_key(js, "fov", data_->fov);
	get_json_key(js, "ortho", data_->ortho);
	get_json_key(js, "size", data_->size);
}

void LoadComponent(Scene::Object_ *data_, const rapidjson::Value &js, const Reader &deps_ir, const ReadProvider &deps_ip, PipelineResources &resources,
	const PipelineInfo &pipeline, bool queue_model_loads, bool queue_texture_loads, bool do_not_load_resources) {
	const std::string name = get_json_key<std::string>(js, "name");

	if (!name.empty())
		if (name[0] != '#')
			data_->model = SkipLoadOrQueueModelLoad(deps_ir, deps_ip, name, resources, queue_model_loads, do_not_load_resources);

	rapidjson::Value::ConstMemberIterator i_mats = js.FindMember("materials");
	if (i_mats != js.MemberEnd()) {
		const rapidjson::Value &mats = i_mats->value;
		const size_t mat_count = mats.Size();
		data_->materials.resize(mat_count);
		for (size_t i = 0; i < mat_count; ++i)
			data_->materials[i] = LoadMaterial(mats[i], deps_ir, deps_ip, resources, pipeline, queue_texture_loads, do_not_load_resources);
	}

	rapidjson::Value::ConstMemberIterator i_mat_infos = js.FindMember("material_infos");
	if (i_mat_infos != js.MemberEnd()) {
		const rapidjson::Value &mat_infos = i_mat_infos->value;
		const size_t mat_count = data_->materials.size();
		data_->material_infos.resize(mat_count);
		for (size_t i = 0; i < mat_count; ++i) {
			if (i == mat_infos.Size())
				break;
			get_json_key(mat_infos[i], "name", data_->material_infos[i].name);
		}
	}

	rapidjson::Value::ConstMemberIterator i_bones = js.FindMember("bones");
	if (i_bones != js.MemberEnd()) {
		const rapidjson::Value &bones = i_bones->value;
		const size_t bone_count = bones.Size();
		data_->bones.resize(bone_count);
		for (size_t i = 0; i < bone_count; ++i)
			from_json(bones[i], data_->bones[i]);
	}
}

void LoadComponent(Scene::Light_ *data_, const rapidjson::Value &js) {
	data_->type = LightType_from_string(get_json_key<std::string>(js, "type"));
	data_->shadow_type = LightShadowType_from_string(get_json_key<std::string>(js, "shadow_type"));
	get_json_key(js, "diffuse", data_->diffuse);
	get_json_key(js, "diffuse_intensity", data_->diffuse_intensity);
	get_json_key(js, "specular", data_->specular);
	get_json_key(js, "specular_intensity", data_->specular_intensity);
	get_json_key(js, "radius", data_->radius);
	get_json_key(js, "inner_angle", data_->inner_angle);
	get_json_key(js, "outer_angle", data_->outer_angle);
	get_json_key(js, "pssm_split", data_->pssm_split);
	get_json_key(js, "priority", data_->priority);
	get_json_key(js, "shadow_bias", data_->shadow_bias);
}

void LoadComponent(Scene::RigidBody_ *data_, const rapidjson::Value &js) {
	RigidBodyType_from_json(js["type"], data_->type);
	data_->linear_damping = pack_float<uint8_t>(get_json_key<float>(js, "linear_damping"));
	data_->angular_damping = pack_float<uint8_t>(get_json_key<float>(js, "angular_damping"));
	data_->restitution = pack_float<uint8_t>(get_json_key<float>(js, "restitution"));
	data_->friction = pack_float<uint8_t>(get_json_key<float>(js, "friction"));
	data_->rolling_friction = pack_float<uint8_t>(get_json_key<float>(js, "rolling_friction"));
}

void LoadComponent(Scene::Collision_ *data_, const rapidjson::Value &js) {
	CollisionType_from_json(js["type"], data_->type);
	get_json_key(js, "mass", data_->mass);
	get_json_key(js, "path", data_->resource_path);
	get_json_key(js, "pos", data_->trs.pos);
	get_json_key(js, "rot", data_->trs.rot);
	get_json_key(js, "scl", data_->trs.scl);
}

void LoadComponent(Scene::Script_ *data_, const rapidjson::Value &js) {
	get_json_key(js, "path", data_->path);

	rapidjson::Value::ConstMemberIterator j = js.FindMember("parameters");
	if (j != js.MemberEnd())
		for (rapidjson::Value::ConstMemberIterator i = j->value.MemberBegin(); i != j->value.MemberEnd(); ++i) {
			ScriptParam &parm = data_->params[i->name.GetString()];

			const std::string type = get_json_key<std::string>(i->value, "type");

			if (type == "bool") {
				parm.type = SPT_Bool;
				get_json_key(i->value, "value", parm.bv);
			} else if (type == "int") {
				parm.type = SPT_Int;
				get_json_key(i->value, "value", parm.iv);
			} else if (type == "float") {
				parm.type = SPT_Float;
				get_json_key(i->value, "value", parm.fv);
			} else if (type == "string") {
				parm.type = SPT_String;
				get_json_key(i->value, "value", parm.sv);
			}
		}
}

void LoadComponent(Scene::Instance_ *data_, const rapidjson::Value &js) {
	for_json_object_const(i, js) {
		const std::string &name = i->name.GetString();

		if (name == "name")
			from_json(i->value, data_->name);
		else if (name == "anim")
			from_json(i->value, data_->anim);
		else if (name == "loop_mode")
			data_->loop_mode = AnimLoopMode_from_string(i->value.GetString());
	}
}

static void LoadProbe(Probe &probe, const rapidjson::Value &js, const Reader &deps_ir, const ReadProvider &deps_ip, PipelineResources &resources,
	bool queue_texture_loads, bool do_not_load_resources, bool silent) {
	const std::string irradiance_map = get_json_key<std::string>(js, "irradiance_map");
	const std::string radiance_map = get_json_key<std::string>(js, "radiance_map");

	probe.irradiance_map = SkipLoadOrQueueTextureLoad(deps_ir, deps_ip, irradiance_map, resources, queue_texture_loads, do_not_load_resources, silent);
	probe.radiance_map = SkipLoadOrQueueTextureLoad(deps_ir, deps_ip, radiance_map, resources, queue_texture_loads, do_not_load_resources, silent);

	ProbeType_from_json(js["type"], probe.type);
	probe.parallax = pack_float<uint8_t>(get_json_key<float>(js, "parallax"));

	get_json_key(js, "pos", probe.trs.pos);
	get_json_key(js, "rot", probe.trs.rot);
	get_json_key(js, "scl", probe.trs.scl);
}

//
void Scene::Save_json(
	rapidjson::Document &jd, rapidjson::Value &js, const PipelineResources &resources, uint32_t save_flags, const std::vector<NodeRef> *nodes_to_save) const {
	js.SetObject();

	// prepare list of nodes to save
	std::vector<NodeRef> node_refs;

	if (save_flags & LSSF_Nodes) {
		if (nodes_to_save) {
			node_refs = *nodes_to_save;
		} else {
			node_refs.reserve(nodes.size());
			for (NodeRef ref = nodes.first_ref(); ref != InvalidNodeRef; ref = nodes.next_ref(ref))
				node_refs.push_back(ref);
		}
	}

	// [EJ-10082019] do not save instantiated nodes
	for (std::vector<NodeRef>::iterator i = node_refs.begin(); i != node_refs.end();) {
		bool is_instantiated = false;
		if (const Node_ *node_ = GetNode_(*i))
			if (node_->flags & NF_Instantiated)
				is_instantiated = true;

		if (is_instantiated)
			i = node_refs.erase(i);
		else
			++i;
	}

	//
	std::set<ComponentRef> used_component_refs[NCI_Count];
	std::set<ComponentRef> used_script_refs, used_instance_refs, used_collision_refs;

	if (save_flags & LSSF_Nodes)
		for (std::vector<NodeRef>::iterator j = node_refs.begin(); j != node_refs.end(); ++j) {
			if (const Node_ *node_ = GetNode_(*j)) {
				if (transforms.is_valid(node_->components[NCI_Transform]))
					used_component_refs[NCI_Transform].insert(node_->components[NCI_Transform]);
				if (cameras.is_valid(node_->components[NCI_Camera]))
					used_component_refs[NCI_Camera].insert(node_->components[NCI_Camera]);
				if (objects.is_valid(node_->components[NCI_Object]))
					used_component_refs[NCI_Object].insert(node_->components[NCI_Object]);
				if (lights.is_valid(node_->components[NCI_Light]))
					used_component_refs[NCI_Light].insert(node_->components[NCI_Light]);
				if (rigid_bodies.is_valid(node_->components[NCI_RigidBody]))
					used_component_refs[NCI_RigidBody].insert(node_->components[NCI_RigidBody]);

				{
					const std::map<NodeRef, std::vector<ComponentRef> >::const_iterator &i = node_collisions.find(*j);
					if (i != node_collisions.end())
						for (std::vector<ComponentRef>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
							used_collision_refs.insert(*j);
				}

				{
					const std::map<NodeRef, std::vector<ComponentRef> >::const_iterator &i = node_scripts.find(*j);
					if (i != node_scripts.end())
						for (std::vector<ComponentRef>::const_iterator j = i->second.begin(); j != i->second.end(); ++j)
							used_script_refs.insert(*j);
				}

				{
					const std::map<NodeRef, ComponentRef>::const_iterator &i = node_instance.find(*j);
					if (i != node_instance.end())
						used_instance_refs.insert(i->second);
				}
			}
		}

	if (save_flags & LSSF_Scene)
		for (std::vector<ComponentRef>::const_iterator j = scene_scripts.begin(); j != scene_scripts.end(); ++j)
			used_script_refs.insert(*j); // flag scene scripts as in-use

	//
	if (!used_component_refs[NCI_Transform].empty()) {
		rapidjson::Value js_trsf(rapidjson::kArrayType);
		for (std::set<ComponentRef>::const_iterator j = used_component_refs[NCI_Transform].begin(); j != used_component_refs[NCI_Transform].end(); ++j)
			if (transforms.is_valid(*j)) {
				rapidjson::Value js;
				SaveComponent(&transforms[j->idx], jd, js);
				js_trsf.PushBack(js, jd.GetAllocator());
			}
		set_json_key(jd, js, "transforms", js_trsf);
	}

	if (!used_component_refs[NCI_Camera].empty()) {
		rapidjson::Value js_cams(rapidjson::kArrayType);
		for (std::set<ComponentRef>::const_iterator j = used_component_refs[NCI_Camera].begin(); j != used_component_refs[NCI_Camera].end(); ++j)
			if (cameras.is_valid(*j)) {
				rapidjson::Value js;
				SaveComponent(&cameras[j->idx], jd, js);
				js_cams.PushBack(js, jd.GetAllocator());
			}
		set_json_key(jd, js, "cameras", js_cams);
	}

	if (!used_component_refs[NCI_Object].empty()) {
		rapidjson::Value js_objs(rapidjson::kArrayType);
		for (std::set<ComponentRef>::const_iterator j = used_component_refs[NCI_Object].begin(); j != used_component_refs[NCI_Object].end(); ++j)
			if (objects.is_valid(*j)) {
				rapidjson::Value js;
				SaveComponent(&objects[j->idx], jd, js, resources);
				js_objs.PushBack(js, jd.GetAllocator());
			}
		set_json_key(jd, js, "objects", js_objs);
	}

	if (!used_component_refs[NCI_Light].empty()) {
		rapidjson::Value js_lgts(rapidjson::kArrayType);
		for (std::set<ComponentRef>::const_iterator j = used_component_refs[NCI_Light].begin(); j != used_component_refs[NCI_Light].end(); ++j)
			if (lights.is_valid(*j)) {
				rapidjson::Value js;
				SaveComponent(&lights[j->idx], jd, js);
				js_lgts.PushBack(js, jd.GetAllocator());
			}
		set_json_key(jd, js, "lights", js_lgts);
	}

	if (!used_component_refs[NCI_RigidBody].empty()) {
		rapidjson::Value js_bodies(rapidjson::kArrayType);
		for (std::set<ComponentRef>::const_iterator j = used_component_refs[NCI_RigidBody].begin(); j != used_component_refs[NCI_RigidBody].end(); ++j)
			if (rigid_bodies.is_valid(*j)) {
				rapidjson::Value js;
				SaveComponent(&rigid_bodies[j->idx], jd, js);
				js_bodies.PushBack(js, jd.GetAllocator());
			}
		set_json_key(jd, js, "rigid_bodies", js_bodies);
	}

	if (!used_collision_refs.empty()) {
		rapidjson::Value js_cols(rapidjson::kArrayType);
		for (std::set<ComponentRef>::const_iterator j = used_collision_refs.begin(); j != used_collision_refs.end(); ++j)
			if (collisions.is_valid(*j)) {
				rapidjson::Value js;
				SaveComponent(&collisions[j->idx], jd, js);
				js_cols.PushBack(js, jd.GetAllocator());
			}
		set_json_key(jd, js, "collisions", js_cols);
	}

	if (!used_script_refs.empty()) {
		rapidjson::Value js_scripts(rapidjson::kArrayType);
		for (std::set<ComponentRef>::const_iterator j = used_script_refs.begin(); j != used_script_refs.end(); ++j)
			if (scripts.is_valid(*j)) {
				rapidjson::Value js;
				SaveComponent(&scripts[j->idx], jd, js);
				js_scripts.PushBack(js, jd.GetAllocator());
			}
		set_json_key(jd, js, "scripts", js_scripts);
	}

	if (!used_instance_refs.empty()) {
		rapidjson::Value js_inss(rapidjson::kArrayType);
		for (std::set<ComponentRef>::const_iterator j = used_instance_refs.begin(); j != used_instance_refs.end(); ++j)
			if (instances.is_valid(*j)) {
				rapidjson::Value js;
				SaveComponent(&instances[j->idx], jd, js);
				js_inss.PushBack(js, jd.GetAllocator());
			}
		set_json_key(jd, js, "instances", js_inss);
	}

	//
	if (save_flags & LSSF_Nodes) {
		rapidjson::Value js_nodes(rapidjson::kArrayType);

		for (std::vector<NodeRef>::iterator i = node_refs.begin(); i != node_refs.end(); ++i) {
			const NodeRef ref = *i;

			if (const Scene::Node_ *node_ = GetNode_(ref)) {
				rapidjson::Value js_node(rapidjson::kObjectType);

				set_json_key(jd, js_node, "idx", ref);
				set_json_key(jd, js_node, "name", node_->name);
				const bool is_disabled = node_->flags & NF_Disabled ? true : false;
				set_json_key(jd, js_node, "disabled", is_disabled);

				uint32_t idxs[NCI_Count];
				std::fill(idxs, idxs + NCI_Count, 0xffffffff);

				if (transforms.is_valid(node_->components[NCI_Transform])) {
					const std::set<ComponentRef>::iterator i = used_component_refs[NCI_Transform].find(node_->components[NCI_Transform]);
					idxs[NCI_Transform] = numeric_cast<uint32_t>(std::distance(used_component_refs[NCI_Transform].begin(), i));
				}

				if (cameras.is_valid(node_->components[NCI_Camera])) {
					const std::set<ComponentRef>::iterator i = used_component_refs[NCI_Camera].find(node_->components[NCI_Camera]);
					idxs[NCI_Camera] = numeric_cast<uint32_t>(std::distance(used_component_refs[NCI_Camera].begin(), i));
				}

				if (objects.is_valid(node_->components[NCI_Object])) {
					const std::set<ComponentRef>::iterator i = used_component_refs[NCI_Object].find(node_->components[NCI_Object]);
					idxs[NCI_Object] = numeric_cast<uint32_t>(std::distance(used_component_refs[NCI_Object].begin(), i));
				}

				if (lights.is_valid(node_->components[NCI_Light])) {
					const std::set<ComponentRef>::iterator i = used_component_refs[NCI_Light].find(node_->components[NCI_Light]);
					idxs[NCI_Light] = numeric_cast<uint32_t>(std::distance(used_component_refs[NCI_Light].begin(), i));
				}

				if (rigid_bodies.is_valid(node_->components[NCI_RigidBody])) {
					const std::set<ComponentRef>::iterator i = used_component_refs[NCI_RigidBody].find(node_->components[NCI_RigidBody]);
					idxs[NCI_RigidBody] = numeric_cast<uint32_t>(std::distance(used_component_refs[NCI_RigidBody].begin(), i));
				}

				{
					rapidjson::Value js_idxs(rapidjson::kArrayType);
					for (size_t i = 0; i < NCI_Count; ++i)
						js_idxs.PushBack(idxs[i], jd.GetAllocator());
					set_json_key(jd, js_node, "components", js_idxs);
				}

				{
					const std::map<NodeRef, std::vector<ComponentRef> >::const_iterator c = node_collisions.find(ref);

					if (c != node_collisions.end()) {
						rapidjson::Value js_node_cols(rapidjson::kArrayType);

						for (std::vector<ComponentRef>::const_iterator j = c->second.begin(); j != c->second.end(); ++j) {
							const std::set<ComponentRef>::iterator i = used_collision_refs.find(*j);

							rapidjson::Value col(rapidjson::kObjectType);
							const uint32_t idx = numeric_cast<uint32_t>(std::distance(used_collision_refs.begin(), i));
							set_json_key(jd, col, "idx", idx);

							js_node_cols.PushBack(col, jd.GetAllocator());
						}

						set_json_key(jd, js_node, "collisions", js_node_cols);
					}
				}

				{
					const std::map<NodeRef, std::vector<ComponentRef> >::const_iterator c = node_scripts.find(ref);

					if (c != node_scripts.end()) {
						rapidjson::Value js_node_scripts(rapidjson::kArrayType);

						for (std::vector<ComponentRef>::const_iterator j = c->second.begin(); j != c->second.end(); ++j) {
							const std::set<NodeRef>::iterator i = used_script_refs.find(*j);

							rapidjson::Value script(rapidjson::kObjectType);
							const uint32_t idx = numeric_cast<uint32_t>(std::distance(used_script_refs.begin(), i));
							set_json_key(jd, script, "idx", idx);

							js_node_scripts.PushBack(script, jd.GetAllocator());
						}

						set_json_key(jd, js_node, "scripts", js_node_scripts);
					}
				}

				{
					const std::map<NodeRef, ComponentRef>::const_iterator &c = node_instance.find(ref);
					if (c != node_instance.end()) {
						const std::set<ComponentRef>::iterator &i = used_instance_refs.find(c->second);
						const uint32_t idx = numeric_cast<uint32_t>(std::distance(used_instance_refs.begin(), i));
						set_json_key(jd, js_node, "instance", idx);
					}
				}

				js_nodes.PushBack(js_node, jd.GetAllocator());
			}
		}
	}

	if (save_flags & LSSF_Scene) {
		{
			rapidjson::Value env(rapidjson::kObjectType);

			if (save_flags & LSSF_Nodes)
				set_json_key(jd, env, "current_camera", current_camera);

			set_json_key(jd, env, "ambient", environment.ambient);
			set_json_key(jd, env, "fog_near", environment.fog_near);
			set_json_key(jd, env, "fog_far", environment.fog_far);
			set_json_key(jd, env, "fog_color", environment.fog_color);

			rapidjson::Value js_probe;
			SaveProbe(environment.probe, jd, js_probe, resources);
			set_json_key(jd, env, "probe", js_probe);

			if (environment.brdf_map != InvalidTextureRef)
				set_json_key(jd, env, "brdf_map", resources.textures.GetName(environment.brdf_map));

			set_json_key(jd, js, "environment", env);
		}

		{
			rapidjson::Value cvs(rapidjson::kObjectType);

			set_json_key(jd, cvs, "clear_z", canvas.clear_z);
			set_json_key(jd, cvs, "clear_color", canvas.clear_color);
			set_json_key(jd, cvs, "color", canvas.color);

			set_json_key(jd, js, "canvas", cvs);
		}
	}

	if (save_flags & LSSF_Anims) {
		{
			rapidjson::Value anims_js(rapidjson::kArrayType);

			for (AnimRef ref = anims.first_ref(); ref != InvalidAnimRef; ref = anims.next_ref(ref)) {
				const Anim &anim = anims[ref.idx];

				if (anim.flags & AF_Instantiated)
					continue;

				rapidjson::Value anim_js(rapidjson::kObjectType);
				set_json_key(jd, anim_js, "idx", ref.idx);
				rapidjson::Value js_anim;
				SaveAnimToJson(jd, js_anim, anim);
				set_json_key(jd, anim_js, "anim", js_anim);
				anims_js.PushBack(anim_js, jd.GetAllocator());
			}

			if (anims_js.Size() > 0)
				set_json_key(jd, js, "anims", anims_js);
		}

		{
			rapidjson::Value scene_anims_js(rapidjson::kArrayType);

			for (SceneAnimRef ref = scene_anims.first_ref(); ref != InvalidSceneAnimRef; ref = scene_anims.next_ref(ref)) {
				const SceneAnim &scene_anim = scene_anims[ref.idx];

				if (scene_anim.flags & SAF_Instantiated)
					continue;

				rapidjson::Value scene_anim_js(rapidjson::kObjectType);
				set_json_key(jd, scene_anim_js, "name", scene_anim.name);
				set_json_key(jd, scene_anim_js, "t_start", scene_anim.t_start);
				set_json_key(jd, scene_anim_js, "t_end", scene_anim.t_end);
				set_json_key(jd, scene_anim_js, "anim", scene_anim.scene_anim);
				set_json_key(jd, scene_anim_js, "frame_duration", scene_anim.frame_duration);

				{
					rapidjson::Value node_anims_js(rapidjson::kArrayType);

					for (std::vector<NodeAnim>::const_iterator i = scene_anim.node_anims.begin(); i != scene_anim.node_anims.end(); ++i) {
						rapidjson::Value node_anim_js(rapidjson::kObjectType);
						set_json_key(jd, node_anim_js, "node", i->node);
						set_json_key(jd, node_anim_js, "anim", i->anim);
						node_anims_js.PushBack(node_anim_js, jd.GetAllocator());
					}

					set_json_key(jd, scene_anim_js, "node_anims", node_anims_js);
				}

				scene_anims_js.PushBack(scene_anim_js, jd.GetAllocator());
			}

			if (scene_anims_js.Size() > 0)
				set_json_key(jd, js, "scene_anims", scene_anims_js);
		}
	}

	if (save_flags & LSSF_KeyValues) {
		rapidjson::Value kv_js(rapidjson::kObjectType);

		for (std::map<std::string, std::string>::const_iterator i = key_values.begin(); i != key_values.end(); ++i)
			set_json_key(jd, kv_js, i->first, i->second);

		set_json_key(jd, js, "key_values", kv_js);
	}
}

bool Scene::Load_json(const rapidjson::Value &js, const std::string &name, const Reader &deps_ir, const ReadProvider &deps_ip, PipelineResources &resources,
	const PipelineInfo &pipeline, LoadSceneContext &ctx, uint32_t load_flags) {
	ProfilerPerfSection section("Scene::Load_json");

	if (!js.IsObject()) {
		warn(fmt::format("Cannot load scene '{}', invalid JSON", name));
		return false;
	}

	std::vector<ComponentRef> transform_refs, camera_refs, object_refs, light_refs, rigid_body_refs, col_refs, script_refs, instance_refs;

	const time_ns t_start = time_now();

	const ProfilerSectionIndex load_component_section_index = BeginProfilerSection("Scene::Load_json: Load Components");

	for_json_object_const(i, js) {
		const std::string name = i->name.GetString();

		if (name == "transforms") {
			assert(i->value.IsArray());
			const size_t transform_count = i->value.Size();
			transform_refs.resize(transform_count);

			for (size_t n = 0; n < transform_count; ++n) {
				const ComponentRef ref = transform_refs[n] = CreateTransform().ref;
				LoadComponent(&transforms[ref.idx], i->value[n]);
			}
		} else if (name == "cameras") {
			assert(i->value.IsArray());
			const size_t camera_count = i->value.Size();
			camera_refs.resize(camera_count);

			for (size_t n = 0; n < camera_count; ++n) {
				const ComponentRef ref = camera_refs[n] = CreateCamera().ref;
				LoadComponent(&cameras[ref.idx], i->value[n]);
			}
		} else if (name == "objects") {
			assert(i->value.IsArray());
			const size_t object_count = i->value.Size();
			object_refs.resize(object_count);

			for (size_t n = 0; n < object_count; ++n) {
				const ComponentRef ref = object_refs[n] = CreateObject().ref;
				LoadComponent(&objects[ref.idx], i->value[n], deps_ir, deps_ip, resources, pipeline, load_flags & LSSF_QueueModelLoads,
					load_flags & LSSF_QueueTextureLoads, load_flags & LSSF_DoNotLoadResources);
			}
		} else if (name == "lights") {
			assert(i->value.IsArray());
			const size_t light_count = i->value.Size();
			light_refs.resize(light_count);

			for (size_t n = 0; n < light_count; ++n) {
				const ComponentRef ref = light_refs[n] = CreateLight().ref;
				LoadComponent(&lights[ref.idx], i->value[n]);
			}
		} else if (name == "rigid_bodies") {
			assert(i->value.IsArray());
			const size_t rigid_body_count = i->value.Size();
			rigid_body_refs.resize(rigid_body_count);

			for (size_t n = 0; n < rigid_body_count; ++n) {
				const ComponentRef ref = rigid_body_refs[n] = CreateRigidBody().ref;
				LoadComponent(&rigid_bodies[ref.idx], i->value[n]);
			}
		} else if (name == "collisions") {
			assert(i->value.IsArray());
			const size_t col_count = i->value.Size();
			col_refs.resize(col_count);

			for (size_t n = 0; n < col_count; ++n) {
				const ComponentRef ref = col_refs[n] = CreateCollision().ref;
				LoadComponent(&collisions[ref.idx], i->value[n]);
			}
		} else if (name == "scripts") {
			assert(i->value.IsArray());
			const size_t scripts_count = i->value.Size();
			script_refs.resize(scripts_count);

			for (size_t n = 0; n < scripts_count; ++n) {
				const ComponentRef ref = script_refs[n] = CreateScript().ref;
				LoadComponent(&scripts[ref.idx], i->value[n]);
			}
		} else if (name == "instances") {
			assert(i->value.IsArray());
			const size_t instance_count = i->value.Size();
			instance_refs.resize(instance_count);

			for (size_t n = 0; n < instance_count; ++n) {
				const ComponentRef ref = instance_refs[n] = CreateInstance().ref;
				LoadComponent(&instances[ref.idx], i->value[n]);
			}
		}
	}

	EndProfilerSection(load_component_section_index);

	//
	const ProfilerSectionIndex load_nodes_section_index = BeginProfilerSection("Scene::Load_json: Load Nodes");

	std::vector<NodeRef> nodes_to_disable;
	std::map<uint32_t, AnimRef> anim_refs;

	for_json_object_const(i, js) {
		const std::string name = i->name.GetString();

		if (name == "nodes") {
			std::vector<NodeRef> node_with_instance_to_setup;
			node_with_instance_to_setup.reserve(64);

			assert(i->value.IsArray());
			const size_t node_count = i->value.Size();
			for (size_t j = 0; j < node_count; ++j) {
				const rapidjson::Value &js_node = i->value[j];

				Node node = CreateNode();
				Node_ &node_ = nodes[node.ref.idx];

				const uint32_t idx = get_json_key<uint32_t>(js_node, "idx");

				ctx.node_refs[idx] = node.ref;
				ctx.view.nodes.push_back(node.ref);

				get_json_key<std::string>(js_node, "name", node_.name);

				if (get_json_key<bool>(js_node, "disabled"))
					nodes_to_disable.push_back(node.ref);

				{
					const rapidjson::Value &js_node_components = js_node["components"];

					const uint32_t transform_idx = js_node_components[NCI_Transform].GetUint();
					if (transform_idx != 0xffffffff)
						node_.components[NCI_Transform] = transform_refs[transform_idx];

					const uint32_t camera_idx = js_node_components[NCI_Camera].GetUint();
					if (camera_idx != 0xffffffff)
						node_.components[NCI_Camera] = camera_refs[camera_idx];

					const uint32_t object_idx = js_node_components[NCI_Object].GetUint();
					if (object_idx != 0xffffffff)
						node_.components[NCI_Object] = object_refs[object_idx];

					const uint32_t light_idx = js_node_components[NCI_Light].GetUint();
					if (light_idx != 0xffffffff)
						node_.components[NCI_Light] = light_refs[light_idx];

					const uint32_t rigid_body_idx = js_node_components[NCI_RigidBody].GetUint();
					if (rigid_body_idx != 0xffffffff)
						node_.components[NCI_RigidBody] = rigid_body_refs[rigid_body_idx];
				}

				{
					rapidjson::Value::ConstMemberIterator js_cols = js_node.FindMember("collisions");

					if (js_cols != js_node.MemberEnd()) {
						const size_t node_col_count = js_cols->value.Size();
						node_collisions[node.ref].reserve(node_col_count);

						for (size_t i = 0; i < node_col_count; ++i) {
							const uint32_t col_idx = get_json_key<uint32_t>(js_cols->value[i], "idx");

							if (col_idx != 0xffffffff) {
								const ComponentRef col_ref = col_refs[col_idx];
								node_collisions[node.ref].push_back(col_ref);
							} else {
								node_collisions[node.ref].push_back(InvalidComponentRef);
							}
						}
					}
				}

				{
					rapidjson::Value::ConstMemberIterator js_scripts = js_node.FindMember("scripts");

					if (js_scripts != js_node.MemberEnd()) {
						const size_t node_script_count = js_scripts->value.Size();
						node_scripts[node.ref].reserve(node_script_count);

						for (size_t i = 0; i < node_script_count; ++i) {
							const uint32_t script_idx = get_json_key<uint32_t>(js_scripts->value[i], "idx");

							if (script_idx != 0xffffffff) {
								const ComponentRef script_ref = col_refs[script_idx];
								node_scripts[node.ref].push_back(script_ref);
							} else {
								node_scripts[node.ref].push_back(InvalidComponentRef);
							}
						}
					}
				}

				{
					rapidjson::Value::ConstMemberIterator js_node_instance = js_node.FindMember("instance");

					if (js_node_instance != js_node.MemberEnd()) {
						const uint32_t instance_idx = js_node_instance->value.GetUint();
						if (instance_idx != 0xffffffff) {
							node_instance[node.ref] = instance_refs[instance_idx];
							node_with_instance_to_setup.push_back(node.ref);
						}
					}
				}
			}

			// setup instances
			if (!(load_flags & LSSF_DoNotLoadResources))
				for (std::vector<NodeRef>::const_iterator i = node_with_instance_to_setup.begin(); i != node_with_instance_to_setup.end(); ++i) {
					NodeSetupInstance(
						*i, deps_ir, deps_ip, resources, pipeline, LSSF_AllNodeFeatures | (load_flags & LSSF_OptionsMask), ctx.recursion_level + 1);
					NodeStartOnInstantiateAnim(*i);
				}

			// fix parent references
			for (std::vector<ComponentRef>::const_iterator i = transform_refs.begin(); i != transform_refs.end(); ++i) {
				Transform_ &c = transforms[i->idx];
				if (c.parent != InvalidNodeRef) {
					const std::map<uint32_t, AnimRef>::const_iterator i = ctx.node_refs.find(c.parent.idx);
					c.parent = i != ctx.node_refs.end() ? i->second : InvalidNodeRef;
				}
			}

			// fix bone references
			for (std::vector<ComponentRef>::const_iterator i = object_refs.begin(); i != object_refs.end(); ++i) {
				Object_ &c = objects[i->idx];

				for (std::vector<NodeRef>::iterator j = c.bones.begin(); j != c.bones.end(); ++j)
					if (*j != InvalidNodeRef) {
						const std::map<uint32_t, NodeRef>::const_iterator &i = ctx.node_refs.find(j->idx);
						*j = i != ctx.node_refs.end() ? i->second : InvalidNodeRef;
					}
			}
		} else if (name == "canvas") {
			if (load_flags & LSSF_Scene) {
				assert(i->value.IsObject());
				for_json_object_const(j, i->value) {
					const std::string name = j->name.GetString();

					if (name == "clear_z")
						from_json(j->value, canvas.clear_z);
					else if (name == "clear_color")
						from_json(j->value, canvas.clear_color);
					else if (name == "color")
						from_json(j->value, canvas.color);
				}
			}
		} else if (name == "anims") {
			if (load_flags & LSSF_Anims) {
				assert(i->value.IsArray());
				const size_t anim_count = i->value.Size();

				for (size_t n = 0; n < anim_count; ++n) {
					const rapidjson::Value &js_anim = i->value[n];

					Anim anim;
					LoadAnimFromJson(js_anim["anim"], anim);

					const AnimRef anim_ref = AddAnim(anim);
					ctx.view.anims.push_back(anim_ref);

					const uint32_t idx = get_json_key<uint32_t>(js_anim, "idx");
					anim_refs[idx] = anim_ref;
				}
			}
		} else if (name == "key_values") {
			if (i->value.IsObject())
				if (load_flags & LSSF_KeyValues)
					for_json_object_const(j, i->value) { key_values[j->name.GetString()] = j->value.GetString(); }
		}
	}

	EndProfilerSection(load_nodes_section_index);

	//
	const ProfilerSectionIndex load_anim_section_index = BeginProfilerSection("Scene::Load_json: Load Animations");

	for_json_object_const(i, js) {
		const std::string name = i->name.GetString();

		if (name == "environment") {
			if (load_flags & LSSF_Scene) {
				assert(i->value.IsObject());
				for_json_object_const(j, i->value) {
					const std::string name = j->name.GetString();

					if (name == "current_camera") {
						const bool can_change_current_camera = current_camera == InvalidNodeRef || !(load_flags & LSSF_DoNotChangeCurrentCameraIfValid);

						if (can_change_current_camera) {
							const uint32_t current_camera_idx = j->value.GetUint();
							current_camera = ctx.node_refs[current_camera_idx];
						}
					} else if (name == "ambient") {
						from_json(j->value, environment.ambient);
					} else if (name == "fog_near") {
						from_json(j->value, environment.fog_near);
					} else if (name == "fog_far") {
						from_json(j->value, environment.fog_far);
					} else if (name == "fog_color") {
						from_json(j->value, environment.fog_color);
					} else if (name == "probe") {
						LoadProbe(environment.probe, j->value, deps_ir, deps_ip, resources, load_flags & LSSF_QueueTextureLoads,
							load_flags & LSSF_DoNotLoadResources, load_flags & LSSF_Silent);
					} else if (name == "brdf_map") {
						environment.brdf_map = SkipLoadOrQueueTextureLoad(deps_ir, deps_ip, j->value.GetString(), resources,
							load_flags & LSSF_QueueTextureLoads, load_flags & LSSF_DoNotLoadResources, load_flags & LSSF_Silent);
					}
				}
			}
		} else if (name == "scene_anims") {
			if (load_flags & LSSF_Anims) {
				assert(i->value.IsArray());
				const size_t scene_anim_count = i->value.Size();

				for (size_t n = 0; n < scene_anim_count; ++n) {
					SceneAnim scene_anim;

					uint32_t scene_anim_idx = invalid_gen_ref.idx;

					for_json_object_const(j, i->value[n]) {
						const std::string name = j->name.GetString();

						if (name == "name") {
							from_json(j->value, scene_anim.name);
						} else if (name == "t_start") {
							from_json(j->value, scene_anim.t_start);
						} else if (name == "t_end") {
							from_json(j->value, scene_anim.t_end);
						} else if (name == "frame_duration") {
							from_json(j->value, scene_anim.frame_duration);
						} else if (name == "anim") {
							if (j->value.IsUint())
								scene_anim_idx = j->value.GetUint();
						} else if (name == "node_anims") {
							assert(j->value.IsArray());
							const size_t node_anim_count = j->value.Size();

							for (size_t n = 0; n < node_anim_count; ++n) {
								const rapidjson::Value &js_node_anim = j->value[n];

								const uint32_t node_idx = get_json_key<uint32_t>(js_node_anim, "node");
								const uint32_t anim_idx = get_json_key<uint32_t>(js_node_anim, "anim");

								const std::map<uint32_t, NodeRef>::iterator i_node_ref = ctx.node_refs.find(node_idx); // remap node
								const std::map<uint32_t, AnimRef>::iterator i_anim_ref = anim_refs.find(anim_idx); // remap anim

								if (i_node_ref != ctx.node_refs.end() && i_anim_ref != anim_refs.end()) {
									NodeAnim node_anim;
									node_anim.node = i_node_ref->second;
									node_anim.anim = i_anim_ref->second;
									scene_anim.node_anims.push_back(node_anim);
								}
							}
						}
					}

					const std::map<uint32_t, AnimRef>::iterator i_scene_anim = anim_refs.find(scene_anim_idx);
					if (i_scene_anim != anim_refs.end())
						scene_anim.scene_anim = i_scene_anim->second;

					const SceneAnimRef scene_anim_ref = AddSceneAnim(scene_anim);
					ctx.view.scene_anims.push_back(scene_anim_ref);
				}
			}
		}
	}

	EndProfilerSection(load_anim_section_index);

	//
	{
		ProfilerPerfSection section("Scene::Load_json: Disable Nodes to Disable");
		for (std::vector<NodeRef>::const_iterator i = nodes_to_disable.begin(); i != nodes_to_disable.end(); ++i)
			DisableNode(*i);
	}

	{
		ProfilerPerfSection section("Scene::Load_json: Ready Matrices");
		ReadyWorldMatrices();
		ComputeWorldMatrices();
	}

	debug(fmt::format("Load scene '{}' took {} ms", name, time_to_ms(time_now() - t_start)));
	return true;
}

} // namespace hg
