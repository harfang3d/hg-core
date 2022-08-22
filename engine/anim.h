// HARFANG(R) Copyright (C) 2022 NWNC. Released under GPL/LGPL/Commercial Licence, see licence.txt for details.

#pragma once

#include "foundation/cext.h"
#include "foundation/color.h"
#include "foundation/math.h"
#include "foundation/quaternion.h"
#include "foundation/rw_interface.h"
#include "foundation/time.h"
#include "foundation/vector2.h"
#include "foundation/vector3.h"
#include "foundation/vector4.h"

#include <algorithm>
#include <deque>
#include <map>
#include <rapidjson/document.h>
#include <set>
#include <string>
#include <vector>

namespace hg {

enum AnimLoopMode { ALM_Once, ALM_Infinite, ALM_Loop };

template <typename T> struct AnimKeyT {
	time_ns t;
	T v;
};

template <typename T> struct AnimTrackT {
	typedef AnimKeyT<T> Key;
	std::string target;
	std::deque<Key> keys;
};

//
template <typename T> struct AnimKeyHermiteT {
	AnimKeyHermiteT() : tension(0), bias(0) {}
	time_ns t;
	T v;
	float tension, bias;
};

template <typename T> struct AnimTrackHermiteT {
	typedef AnimKeyHermiteT<T> Key;
	std::string target;
	std::deque<Key> keys;
};

static const int InvalidKeyIdx = -1;

// AnimTrackT is expected to be sorted
template <typename AnimTrack> int GetKey(const AnimTrack &track, time_ns t) {
	const size_t key_count = track.keys.size();
	if (key_count == 0)
		return InvalidKeyIdx;

	// FIXME below a certain number of keys a linear search might be faster

	int lo = 0, hi = numeric_cast<int>(key_count) - 1;

	while (true) {
		const int mid = (lo + hi) / 2;

		if (track.keys[mid].t == t)
			return mid; // match

		if (mid == lo) { // won't converge any further
			if (track.keys[hi].t == t)
				return hi; // match
			break;
		}

		if (t < track.keys[mid].t)
			hi = mid;
		else
			lo = mid;
	}

	return InvalidKeyIdx;
}

template <typename AnimTrack, typename T> void SetKey(AnimTrack &track, time_ns t, const T &v) {
	const int idx = GetKey(track, t);

	if (idx != InvalidKeyIdx) {
		track.keys[idx].v = v;
	} else {
		typename std::deque<typename AnimTrack::Key>::iterator i = track.keys.begin();
		for (; i != track.keys.end(); ++i)
			if (i->t > t)
				break;

		typename AnimTrack::Key key;
		key.t = t;
		key.v = v;
		track.keys.insert(i, key);
	}
}

template <typename AnimTrack> void DeleteKey(AnimTrack &track, time_ns t) {
	const int idx = GetKey(track, t);

	if (idx != InvalidKeyIdx)
		track.keys.erase(track.keys.begin() + idx);
}

//
template <typename AnimTrack, typename T> bool GetIntervalKeys(const AnimTrack &track, time_ns t, int &kf0, int &kf1) {
	const int key_count = numeric_cast<int>(track.keys.size());

	int i = 0;
	for (; i < key_count; ++i)
		if (track.keys[i].t > t)
			break;

	if (i == 0) {
		kf0 = 0;
		return false;
	} else if (i == key_count) {
		kf0 = i - 1;
		return false;
	}

	kf0 = i - 1;
	kf1 = i;
	return true;
}

template <typename AnimTrack, typename T> bool EvaluateStep(const AnimTrack &track, time_ns t, T &v) {
	if (track.keys.empty())
		return false;

	int kf0, kf1;
	GetIntervalKeys<AnimTrack, T>(track, t, kf0, kf1);
	v = track.keys[kf0].v;

	return true;
}

template <typename AnimTrack, typename T> bool EvaluateLinear(const AnimTrack &track, time_ns t, T &v) {
	if (track.keys.empty())
		return false;

	int kf0, kf1;
	if (GetIntervalKeys<AnimTrack, T>(track, t, kf0, kf1)) {
		const float k = time_to_sec_f(t - track.keys[kf0].t) / time_to_sec_f(track.keys[kf1].t - track.keys[kf0].t);
		v = LinearInterpolate(track.keys[kf0].v, track.keys[kf1].v, k);
	} else {
		v = track.keys[kf0].v;
	}
	return true;
}

template <typename T> bool EvaluateHermite(const AnimTrackHermiteT<T> &track, time_ns t, T &v) {
	if (track.keys.empty())
		return false;

	int kf1, kf2;

	if (GetIntervalKeys<AnimTrackHermiteT<T>, T>(track, t, kf1, kf2)) {
		const float u = time_to_sec_f(t - track.keys[kf1].t) / time_to_sec_f(track.keys[kf2].t - track.keys[kf1].t);
		const int kf0 = Max<int>(kf1 - 1, 0), kf3 = Min<int>(kf2 + 1, numeric_cast<int>(track.keys.size()) - 1);
		v = HermiteInterpolate(track.keys[kf0].v, track.keys[kf1].v, track.keys[kf2].v, track.keys[kf3].v, u, track.keys[kf1].tension, track.keys[kf1].bias);
	} else {
		v = track.keys[kf1].v;
	}
	return true;
}

//
template <typename Key> struct CompareKeyTimeIsLess {
	bool operator()(const Key &a, const Key &b) {
		return a.t < b.t;
	}
};

template <typename Key> struct CompareKeyTimeIsEqual {
	bool operator()(const Key &a, const Key &b) {
		return a.t == b.t;
	}
};

template <typename Track> void SortAnimTrackKeys(Track &track) {
	std::sort(track.keys.begin(), track.keys.end(), CompareKeyTimeIsLess<typename Track::Key>());
	track.keys.erase(std::unique(track.keys.begin(), track.keys.end(), CompareKeyTimeIsEqual<typename Track::Key>()), track.keys.end());
}

template <typename Track> void ConformAnimTrackKeys(Track &track) {}

void ConformAnimTrackKeys(AnimTrackT<Quaternion> &track);

//
template <typename T> bool Evaluate(const AnimTrackT<T> &track, time_ns t, T &v) {
	return EvaluateLinear<AnimTrackT<T>, T>(track, t, v);
}
template <typename T> bool Evaluate(const AnimTrackHermiteT<T> &track, time_ns t, T &v) {
	return EvaluateHermite<T>(track, t, v);
}

template <> bool Evaluate(const AnimTrackT<bool> &track, time_ns t, bool &v);
template <> bool Evaluate(const AnimTrackT<std::string> &track, time_ns t, std::string &v);

//
struct InstanceAnimKey {
	InstanceAnimKey() : loop_mode(ALM_Once), t_scale(1.f) {}
	std::string anim_name;
	AnimLoopMode loop_mode;
	float t_scale;
};

template <> bool Evaluate(const AnimTrackT<InstanceAnimKey> &track, time_ns t, InstanceAnimKey &v);

//
static const uint8_t AF_UseQuaternionForRotation = 0x01;

// non-serialized flags
static const uint8_t AF_Instantiated = 0x10;

struct Anim {
	Anim() : t_start(0), t_end(0), flags(0) {}
	std::vector<AnimTrackT<bool> > bool_tracks;
	std::vector<AnimTrackT<int> > int_tracks;
	std::vector<AnimTrackHermiteT<float> > float_tracks;
	std::vector<AnimTrackHermiteT<Vec2> > vec2_tracks;
	std::vector<AnimTrackHermiteT<Vec3> > vec3_tracks;
	std::vector<AnimTrackHermiteT<Vec4> > vec4_tracks;
	std::vector<AnimTrackT<Quaternion> > quat_tracks;
	std::vector<AnimTrackHermiteT<Color> > color_tracks;
	std::vector<AnimTrackT<std::string> > string_tracks;
	AnimTrackT<InstanceAnimKey> instance_anim_track;
	time_ns t_start, t_end;
	uint8_t flags;
};

void SaveAnimToJson(rapidjson::Document &jd, rapidjson::Value &js, const Anim &anim);
void LoadAnimFromJson(const rapidjson::Value &js, Anim &anim);

void SaveAnimToBinary(const Writer &iw, const Handle &h, const Anim &anim);
void LoadAnimFromBinary(const Reader &ir, const Handle &h, Anim &anim);

//
template <typename Track> void ResampleAnimTrack(Track &track, time_ns old_start, time_ns new_start, time_ns scale, time_ns frame_duration) {
	// resample/quantize
	for (typename std::deque<typename Track::Key>::iterator i = track.keys.begin(); i != track.keys.end(); ++i) {
		i->t = ((i->t - old_start) * scale) / 1000000 + new_start;
		i->t = time_ns((i->t + frame_duration / 2) / frame_duration) * frame_duration;
	}

	// remove duplicate timecode
	for (typename std::deque<typename Track::Key>::iterator i = track.keys.begin(); i != track.keys.end();) {
		if ((i != track.keys.begin()) && (i->t == (i - 1)->t))
			i = track.keys.erase(i);
		else
			++i;
	}
}

void ResampleAnim(Anim &anim, time_ns old_start, time_ns old_end, time_ns new_start, time_ns new_end, time_ns frame_duration);
void ReverseAnim(Anim &anim, time_ns t_start, time_ns t_end);
void QuantizeAnim(Anim &anim, time_ns t_step);

static bool CompareKeyValue(const Vec3 &v_a, const Vec3 &v_b, float epsilon) {
	return Len(v_a - v_b) <= epsilon;
}

static bool CompareKeyValue(const Quaternion &v_a, const Quaternion &v_b, float epsilon) {
	return Abs(ACos(Dot(v_a, v_b))) <= epsilon * 0.5f;
}

static bool CompareKeyValue(const Color &v_a, const Color &v_b, float epsilon) {
	return Max(Abs(v_a.r - v_b.r), Max(Abs(v_a.g - v_b.g), Max(Abs(v_a.b - v_b.b), Abs(v_a.a - v_b.a)))) <= epsilon;
}

//
template <typename AnimTrack, typename T> size_t SimplifyAnimTrackT(AnimTrack &track, float epsilon) {
	AnimTrack track_ref = track;
	track.keys.clear();

	// copy keys that can't be interpolated from the others
	int last_copied = 0;
	for (int i = 0; i < track_ref.keys.size(); i++) {
		if (i == 0 || i == track_ref.keys.size() - 1) {
			track.keys.push_back(track_ref.keys[i]);
			last_copied = i;
		} else {
			const typename std::deque<typename AnimTrack::Key> &prev = track_ref.keys[last_copied];
			const typename std::deque<typename AnimTrack::Key> &next = track_ref.keys[i + 1];
			for (int j = last_copied + 1; j <= i; j++) {
				const typename AnimTrack::Key &src_key = track_ref.keys[j];
				T interpolated;
				Evaluate(track, src_key.t, interpolated);
				if (!CompareKeyValue(interpolated, src_key.v, epsilon)) {
					track.keys.push_back(src_key);
					last_copied = i;
					break;
				}
			}
		}
	}

	// erase last key if it's the same as the first and there's just the two of them
	if (track.keys.size() == 2 && CompareKeyValue(track.keys[0].v, track.keys[1].v, epsilon)) {
		track.keys.pop_back();
	}

	HG_ASSERT(track.keys.size() <= track_ref.keys.size());
	return track_ref.keys.size() - track.keys.size();
}

//
void MigrateLegacyAnimTracks(Anim &anim);

//
bool AnimHasKeys(const Anim &anim);
void DeleteEmptyAnimTracks(Anim &anim);

} // namespace hg
