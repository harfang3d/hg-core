// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "engine/anim.h"
#include "engine/json.h"

namespace hg {

template <typename T> void SaveAnimKey(rapidjson::Document &jd, rapidjson::Value &js, const AnimKeyT<T> &key) {
	js.SetObject();
	set_json_key(jd, js, "t", key.t);
	set_json_key(jd, js, "v", key.v);
}

template <typename T> void SaveAnimKey(rapidjson::Document &jd, rapidjson::Value &js, const AnimKeyHermiteT<T> &key) {
	js.SetObject();
	set_json_key(jd, js, "t", key.t);
	set_json_key(jd, js, "v", key.v);
	set_json_key(jd, js, "tension", key.tension);
	set_json_key(jd, js, "bias", key.bias);
}

template <typename Track> void SaveAnimTrack(rapidjson::Document &jd, rapidjson::Value &js, const Track &track) {
	js.SetObject();
	set_json_key(jd, js, "target", track.target);

	if (!track.keys.empty()) {
		rapidjson::Value keys_js(rapidjson::kArrayType);

		for (typename std::deque<typename Track::Key>::const_iterator i = track.keys.begin(); i != track.keys.end(); ++i) {
			rapidjson::Value key_js;
			SaveAnimKey(jd, key_js, *i);
			keys_js.PushBack(key_js, jd.GetAllocator());
		}

		set_json_key(jd, js, "keys", keys_js);
	}
}

template <typename Track> void SaveAnimTracks(rapidjson::Document &jd, rapidjson::Value &js, const std::string &name, const std::vector<Track> &tracks) {
	if (!tracks.empty()) {
		rapidjson::Value tracks_js(rapidjson::kArrayType);

		for (typename std::vector<Track>::const_iterator i = tracks.begin(); i != tracks.end(); ++i) {
			rapidjson::Value track_js;
			SaveAnimTrack(jd, track_js, *i);
			tracks_js.PushBack(track_js, jd.GetAllocator());
		}

		set_json_key(jd, js, name.c_str(), tracks_js);
	}
}

static void SaveInstanceAnimTrack(rapidjson::Document &jd, rapidjson::Value &js, const AnimTrackT<InstanceAnimKey> &track) {
	if (!track.keys.empty()) {
		rapidjson::Value instance_anim_js(rapidjson::kObjectType);
		rapidjson::Value keys_js(rapidjson::kArrayType);

		for (typename std::deque<typename AnimTrackT<InstanceAnimKey>::Key>::const_iterator i = track.keys.begin(); i != track.keys.end(); ++i) {
			rapidjson::Value key_js(rapidjson::kObjectType);
			set_json_key(jd, key_js, "t", i->t);
			set_json_key(jd, key_js, "v", i->v.anim_name);
			set_json_key(jd, key_js, "loop", i->v.loop_mode); // FIXMEJSONTYPE
			set_json_key(jd, key_js, "scale", i->v.t_scale);
			keys_js.PushBack(key_js, jd.GetAllocator());
		}

		set_json_key(jd, instance_anim_js, "keys", keys_js);
		set_json_key(jd, js, "instance_anim", instance_anim_js);
	}
}

void SaveAnimToJson(rapidjson::Document &jd, rapidjson::Value &js, const Anim &anim) {
	js.SetObject();

	set_json_key(jd, js, "t_start", anim.t_start);
	set_json_key(jd, js, "t_end", anim.t_end);

	rapidjson::Value flags_js(rapidjson::kArrayType);
	if (anim.flags & AF_UseQuaternionForRotation)
		flags_js.PushBack("UseQuaternionForRotation", jd.GetAllocator());

	set_json_key(jd, js, "flags", flags_js);

	SaveAnimTracks(jd, js, "bool", anim.bool_tracks);
	SaveAnimTracks(jd, js, "int", anim.int_tracks);
	SaveAnimTracks(jd, js, "float", anim.float_tracks);
	SaveAnimTracks(jd, js, "vec2", anim.vec2_tracks);
	SaveAnimTracks(jd, js, "vec3", anim.vec3_tracks);
	SaveAnimTracks(jd, js, "vec4", anim.vec4_tracks);
	SaveAnimTracks(jd, js, "quat", anim.quat_tracks);
	SaveAnimTracks(jd, js, "color", anim.color_tracks);
	SaveAnimTracks(jd, js, "string", anim.string_tracks);

	SaveInstanceAnimTrack(jd, js, anim.instance_anim_track);
}

//
template <typename T> void LoadAnimKey(const rapidjson::Value &js, AnimKeyT<T> &key) {
	get_json_key(js, "t", key.t);
	get_json_key(js, "v", key.v);
}

template <typename T> void LoadAnimKey(const rapidjson::Value &js, AnimKeyHermiteT<T> &key) {
	get_json_key(js, "t", key.t);
	get_json_key(js, "v", key.v);
	get_json_key(js, "tension", key.tension);
	get_json_key(js, "bias", key.bias);
}

template <typename Track> void LoadAnimTrack(const rapidjson::Value &js, Track &track) {
	get_json_key(js, "target", track.target);

	rapidjson::Value::ConstMemberIterator i = js.FindMember("keys");
	if (i != js.MemberEnd()) {
		const size_t key_count = i->value.Size();
		track.keys.resize(key_count);
		for (size_t n = 0; n < key_count; ++n)
			LoadAnimKey(i->value[n], track.keys[n]);
	}

	SortAnimTrackKeys(track);
}

void LoadInstanceAnimTrack(const rapidjson::Value &js, AnimTrackT<InstanceAnimKey> &track) {
	rapidjson::Value::ConstMemberIterator i_inst_anm = js.FindMember("instance_anim");
	if (i_inst_anm == js.MemberEnd())
		return; // no such track

	rapidjson::Value::ConstMemberIterator i_keys = i_inst_anm->value.FindMember("keys");
	if (i_keys != i_inst_anm->value.MemberEnd()) {
		track.keys.resize(i_keys->value.Size());
		for (size_t i = 0; i < i_keys->value.Size(); ++i) {
			const rapidjson::Value &key_js = i_keys->value[i];
			AnimTrackT<InstanceAnimKey>::Key &key = track.keys[i];
			get_json_key(key_js, "t", key.t);
			get_json_key(key_js, "name", key.v.anim_name);
			key.v.loop_mode = AnimLoopMode_from_string(get_json_key<std::string>(key_js, "loop"));
			get_json_key(key_js, "scale", key.v.t_scale);
		}
	}

	SortAnimTrackKeys(track);
}

template <typename Track> void LoadAnimTracks(const rapidjson::Value &js, const std::string &name, std::vector<Track> &tracks) {
	rapidjson::Value::ConstMemberIterator i_track = js.FindMember(name);
	if (i_track == js.MemberEnd())
		return; // no such group

	const size_t track_count = i_track->value.Size();
	tracks.resize(track_count);
	for (size_t i = 0; i < track_count; ++i)
		LoadAnimTrack(i_track->value[i], tracks[i]);
}

void LoadAnimFromJson(const rapidjson::Value &js, Anim &anim) {
	get_json_key(js, "t_start", anim.t_start);
	get_json_key(js, "t_end", anim.t_end);

	LoadAnimTracks(js, "bool", anim.bool_tracks);
	LoadAnimTracks(js, "int", anim.int_tracks);
	LoadAnimTracks(js, "float", anim.float_tracks);
	LoadAnimTracks(js, "vec2", anim.vec2_tracks);
	LoadAnimTracks(js, "vec3", anim.vec3_tracks);
	LoadAnimTracks(js, "vec4", anim.vec4_tracks);
	LoadAnimTracks(js, "quat", anim.quat_tracks);
	LoadAnimTracks(js, "color", anim.color_tracks);
	LoadAnimTracks(js, "string", anim.string_tracks);

	LoadInstanceAnimTrack(js, anim.instance_anim_track);

	rapidjson::Value::ConstMemberIterator i_flags = js.FindMember("flags");
	if (i_flags != js.MemberEnd()) {
		for (rapidjson::Value::ConstValueIterator i = i_flags->value.Begin(); i != i_flags->value.End(); ++i) {
			const std::string flag = i->GetString();

			if (flag == "UseQuaternionForRotation")
				anim.flags |= AF_UseQuaternionForRotation;
		}
	} else {
		// legacy fallback
		if (!anim.quat_tracks.empty()) // any quaternion track takes precedence
			anim.flags |= AF_UseQuaternionForRotation;
	}

	MigrateLegacyAnimTracks(anim);
}

} // namespace hg
