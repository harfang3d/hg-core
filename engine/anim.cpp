// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#include "engine/anim.h"

#include "foundation/log.h"

namespace hg {

template <> bool Evaluate<bool>(const AnimTrackT<bool> &track, time_ns t, bool &v) { return EvaluateStep<AnimTrackT<bool>, bool>(track, t, v); }

template <> bool Evaluate<std::string>(const AnimTrackT<std::string> &track, time_ns t, std::string &v) {
	return EvaluateStep<AnimTrackT<std::string>, std::string>(track, t, v);
}

template <> bool Evaluate(const AnimTrackT<InstanceAnimKey> &track, time_ns t, InstanceAnimKey &v) {
	return EvaluateStep<AnimTrackT<InstanceAnimKey>, InstanceAnimKey>(track, t, v);
}

void ResampleAnim(Anim &anim, time_ns old_start, time_ns old_end, time_ns new_start, time_ns new_end, time_ns frame_duration) {
	const time_ns old_duration = old_end - old_start;
	const time_ns new_duration = new_end - new_start;

	const time_ns scale = (new_duration * 1000) / (old_duration / 1000); // 10^6 precision

	for (std::vector<AnimTrackT<bool> >::iterator i = anim.bool_tracks.begin(); i != anim.bool_tracks.end(); ++i)
		ResampleAnimTrack(*i, old_start, new_start, scale, frame_duration);
	for (std::vector<AnimTrackT<int> >::iterator i = anim.int_tracks.begin(); i != anim.int_tracks.end(); ++i)
		ResampleAnimTrack(*i, old_start, new_start, scale, frame_duration);
	for (std::vector<AnimTrackHermiteT<float> >::iterator i = anim.float_tracks.begin(); i != anim.float_tracks.end(); ++i)
		ResampleAnimTrack(*i, old_start, new_start, scale, frame_duration);
	for (std::vector<AnimTrackHermiteT<Vec2> >::iterator i = anim.vec2_tracks.begin(); i != anim.vec2_tracks.end(); ++i)
		ResampleAnimTrack(*i, old_start, new_start, scale, frame_duration);
	for (std::vector<AnimTrackHermiteT<Vec3> >::iterator i = anim.vec3_tracks.begin(); i != anim.vec3_tracks.end(); ++i)
		ResampleAnimTrack(*i, old_start, new_start, scale, frame_duration);
	for (std::vector<AnimTrackHermiteT<Vec4> >::iterator i = anim.vec4_tracks.begin(); i != anim.vec4_tracks.end(); ++i)
		ResampleAnimTrack(*i, old_start, new_start, scale, frame_duration);
	for (std::vector<AnimTrackT<Quaternion> >::iterator i = anim.quat_tracks.begin(); i != anim.quat_tracks.end(); ++i)
		ResampleAnimTrack(*i, old_start, new_start, scale, frame_duration);
	for (std::vector<AnimTrackHermiteT<Color> >::iterator i = anim.color_tracks.begin(); i != anim.color_tracks.end(); ++i)
		ResampleAnimTrack(*i, old_start, new_start, scale, frame_duration);
	for (std::vector<AnimTrackT<std::string> >::iterator i = anim.string_tracks.begin(); i != anim.string_tracks.end(); ++i)
		ResampleAnimTrack(*i, old_start, new_start, scale, frame_duration);

	ResampleAnimTrack(anim.instance_anim_track, old_start, new_start, scale, frame_duration);

	anim.t_start = new_start;
	anim.t_end = new_end;
}

template <typename Track> void ReverseAnimTrack(Track &track, time_ns t_start, time_ns t_end) {
	for (typename std::deque<typename Track::Key>::iterator i = track.keys.begin(); i != track.keys.end(); ++i)
		i->t = t_end - (i->t - t_start);
	SortAnimTrackKeys(track);
}

void ReverseAnim(Anim &anim, time_ns t_start, time_ns t_end) {
	for (std::vector<AnimTrackT<bool> >::iterator i = anim.bool_tracks.begin(); i != anim.bool_tracks.end(); ++i)
		ReverseAnimTrack(*i, t_start, t_end);
	for (std::vector<AnimTrackT<int> >::iterator i = anim.int_tracks.begin(); i != anim.int_tracks.end(); ++i)
		ReverseAnimTrack(*i, t_start, t_end);
	for (std::vector<AnimTrackHermiteT<float> >::iterator i = anim.float_tracks.begin(); i != anim.float_tracks.end(); ++i)
		ReverseAnimTrack(*i, t_start, t_end);
	for (std::vector<AnimTrackHermiteT<Vec2> >::iterator i = anim.vec2_tracks.begin(); i != anim.vec2_tracks.end(); ++i)
		ReverseAnimTrack(*i, t_start, t_end);
	for (std::vector<AnimTrackHermiteT<Vec3> >::iterator i = anim.vec3_tracks.begin(); i != anim.vec3_tracks.end(); ++i)
		ReverseAnimTrack(*i, t_start, t_end);
	for (std::vector<AnimTrackHermiteT<Vec4> >::iterator i = anim.vec4_tracks.begin(); i != anim.vec4_tracks.end(); ++i)
		ReverseAnimTrack(*i, t_start, t_end);
	for (std::vector<AnimTrackT<Quaternion> >::iterator i = anim.quat_tracks.begin(); i != anim.quat_tracks.end(); ++i)
		ReverseAnimTrack(*i, t_start, t_end);
	for (std::vector<AnimTrackHermiteT<Color> >::iterator i = anim.color_tracks.begin(); i != anim.color_tracks.end(); ++i)
		ReverseAnimTrack(*i, t_start, t_end);
	for (std::vector<AnimTrackT<std::string> >::iterator i = anim.string_tracks.begin(); i != anim.string_tracks.end(); ++i)
		ReverseAnimTrack(*i, t_start, t_end);

	ReverseAnimTrack(anim.instance_anim_track, t_start, t_end);
}

template <typename Track> void QuantizeAnimTrack(Track &track, time_ns t_step) {
	for (typename std::deque<typename Track::Key>::iterator i = track.keys.begin(); i != track.keys.end(); ++i)
		i->t = (i->t / t_step) * t_step;
	SortAnimTrackKeys(track);
}

void QuantizeAnim(Anim &anim, time_ns t_step) {
	for (std::vector<AnimTrackT<bool> >::iterator i = anim.bool_tracks.begin(); i != anim.bool_tracks.end(); ++i)
		QuantizeAnimTrack(*i, t_step);
	for (std::vector<AnimTrackT<int> >::iterator i = anim.int_tracks.begin(); i != anim.int_tracks.end(); ++i)
		QuantizeAnimTrack(*i, t_step);
	for (std::vector<AnimTrackHermiteT<float> >::iterator i = anim.float_tracks.begin(); i != anim.float_tracks.end(); ++i)
		QuantizeAnimTrack(*i, t_step);
	for (std::vector<AnimTrackHermiteT<Vec2> >::iterator i = anim.vec2_tracks.begin(); i != anim.vec2_tracks.end(); ++i)
		QuantizeAnimTrack(*i, t_step);
	for (std::vector<AnimTrackHermiteT<Vec3> >::iterator i = anim.vec3_tracks.begin(); i != anim.vec3_tracks.end(); ++i)
		QuantizeAnimTrack(*i, t_step);
	for (std::vector<AnimTrackHermiteT<Vec4> >::iterator i = anim.vec4_tracks.begin(); i != anim.vec4_tracks.end(); ++i)
		QuantizeAnimTrack(*i, t_step);
	for (std::vector<AnimTrackT<Quaternion> >::iterator i = anim.quat_tracks.begin(); i != anim.quat_tracks.end(); ++i)
		QuantizeAnimTrack(*i, t_step);
	for (std::vector<AnimTrackHermiteT<Color> >::iterator i = anim.color_tracks.begin(); i != anim.color_tracks.end(); ++i)
		QuantizeAnimTrack(*i, t_step);
	for (std::vector<AnimTrackT<std::string> >::iterator i = anim.string_tracks.begin(); i != anim.string_tracks.end(); ++i)
		QuantizeAnimTrack(*i, t_step);

	QuantizeAnimTrack(anim.instance_anim_track, t_step);
}

void ConformAnimTrackKeys(AnimTrackT<Quaternion> &track) {
	// make sure adjacent quaternions use the shortest path when interpolated
	for (size_t i = 1; i < track.keys.size(); ++i) {
		const Quaternion prev_quat = track.keys[i - 1].v;
		const Quaternion curr_quat = track.keys[i].v;

		if (Dot(prev_quat, curr_quat) < 0.f)
			track.keys[i].v = curr_quat * -1.f;
	}
}

//
void MigrateLegacyAnimTracks(Anim &anim) {
	for (std::vector<AnimTrackT<std::string> >::iterator i = anim.string_tracks.begin(); i != anim.string_tracks.end();) {
		const AnimTrackT<std::string> &track = *i;

		if (track.target == "Instance.Anim") { // migrate legacy instance animation track
			if (anim.instance_anim_track.keys.empty()) {
				for (std::deque<AnimKeyT<std::string> >::const_iterator i = track.keys.begin(); i != track.keys.end(); ++i) {
					AnimKeyT<InstanceAnimKey> key;
					key.t = i->t;
					key.v.anim_name = i->v;
					key.v.loop_mode = ALM_Once;
					anim.instance_anim_track.keys.push_back(key);
				}
			} else {
				warn("Not migrating legacy instance animation track as a modern track of this type exists");
			}

			i = anim.string_tracks.erase(i);
		} else {
			++i;
		}
	}
}

//
template <typename AnimTrack> bool AnimTracksHaveKeys(const std::vector<AnimTrack> &tracks) {
	for (typename std::vector<AnimTrack>::const_iterator i = tracks.begin(); i != tracks.end(); ++i)
		if (!i->keys.empty())
			return true;
	return false;
}

bool AnimHasKeys(const Anim &anim) {
	if (AnimTracksHaveKeys(anim.vec3_tracks))
		return true;
	if (AnimTracksHaveKeys(anim.vec4_tracks))
		return true;
	if (AnimTracksHaveKeys(anim.quat_tracks))
		return true;
	if (AnimTracksHaveKeys(anim.color_tracks))
		return true;
	if (AnimTracksHaveKeys(anim.float_tracks))
		return true;
	if (AnimTracksHaveKeys(anim.bool_tracks))
		return true;
	if (AnimTracksHaveKeys(anim.int_tracks))
		return true;
	if (AnimTracksHaveKeys(anim.vec2_tracks))
		return true;
	if (AnimTracksHaveKeys(anim.string_tracks))
		return true;
	if (!anim.instance_anim_track.keys.empty())
		return true;
	return false;
}

//
template <typename AnimTrack> void DeleteEmptyAnimTracks_(std::vector<AnimTrack> &tracks) {
	for (typename std::vector<AnimTrack>::iterator i = tracks.begin(); i != tracks.end(); )
		if (i->keys.empty())
			i = tracks.erase(i);
		else
			++i;
}

void DeleteEmptyAnimTracks(Anim &anim) {
	DeleteEmptyAnimTracks_(anim.vec3_tracks);
	DeleteEmptyAnimTracks_(anim.vec4_tracks);
	DeleteEmptyAnimTracks_(anim.quat_tracks);
	DeleteEmptyAnimTracks_(anim.color_tracks);
	DeleteEmptyAnimTracks_(anim.float_tracks);
	DeleteEmptyAnimTracks_(anim.bool_tracks);
	DeleteEmptyAnimTracks_(anim.int_tracks);
	DeleteEmptyAnimTracks_(anim.vec2_tracks);
	DeleteEmptyAnimTracks_(anim.string_tracks);
}

} // namespace hg
