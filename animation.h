#ifndef ANIMATION_H
#define ANIMATION_H

#include "memory.hpp"
#include "maths.hpp"

// I considered templating these but at this point i'd rather have
// more code than more compile time

enum AnimResult {
	eANIM_NO_EVENT,
	eANIM_LOOPED,
};

struct AnimKeyframe {
	lf32 key_start;
	lf32 key_end;
	lf32 time;
	AnimKeyframe() : key_start{ 0.f }, key_end{ 0.f }, time{ 0.f } {};
	AnimKeyframe(lf32 _key_start, lf32 _key_end, lf32 _time) :
		key_start{ _key_start }, key_end{ _key_end }, time{ _time } {};
};
struct AnimFrames {
	AnimKeyframe keyframes[32];
	lui32 frameCount = 0u;
	AnimFrames(lui32 i, ...); // temporary constructor
	void SetKeyframes(lui32 i, ...);
};
struct AnimPlayer {
private:
	AnimFrames* anim_last;
	lf32 time = 0.f;
public:
	AnimResult Tick(lf32 dt, AnimFrames* anim, lf32* aniTarget);
};

struct AnimKeyframeVec3 {
	m::Vector3 value;
	lf32 time;
	AnimKeyframeVec3() : value{ m::Vector3() }, time{ 0.f } {};
	AnimKeyframeVec3(m::Vector3 _value, lf32 _time) : value{ _value }, time{ _time } {};
};
struct AnimFramesVec3 {
	AnimKeyframeVec3 keyframes[32];
	lui32 frameCount = 0u;
	AnimFramesVec3(lui32 i, ...); // temporary constructor
	void SetKeyframes(lui32 i, ...);
};
struct AnimPlayerVec3 {
private:
	AnimFramesVec3* anim_last;
	lf32 time = 0.f;
public:
	AnimResult Tick(lf32 dt, AnimFramesVec3* anim, m::Vector3* aniTarget);
};

#endif
