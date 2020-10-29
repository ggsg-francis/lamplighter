#ifndef ANIMATION_H
#define ANIMATION_H

#include "memory.hpp"
#include "maths.hpp"

// I considered templating these but at this point i'd rather have
// more code than more compile time

struct AnimKeyframeVec3 {
	m::Vector3 value;
	btf32 time;
	AnimKeyframeVec3() : value{ m::Vector3() }, time{ 0.f } {};
	AnimKeyframeVec3(m::Vector3 _value, btf32 _time) : value{ _value }, time{ _time } {};
};
struct AnimFramesVec3 {
	AnimKeyframeVec3 keyframes[32];
	btui32 frameCount = 0u;
	AnimFramesVec3(btui32 i, ...); // temporary constructor
	void SetKeyframes(btui32 i, ...);
};
enum AnimResult {
	eANIM_NO_EVENT,
	eANIM_LOOPED,
};
struct AnimPlayerVec3 {
private:
	AnimFramesVec3* anim_last;
	btf32 time = 0.f;
public:
	AnimResult Tick(btf32 dt, AnimFramesVec3* anim, m::Vector3* aniTarget);
};

#endif
