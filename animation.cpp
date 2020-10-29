#include "animation.h"

AnimFramesVec3::AnimFramesVec3(btui32 i, ...) {
	if (i <= 32) {
		frameCount = i;
		va_list args;
		va_start(args, i);
		for (btui32 x = 0; x < i; x++) {
			AnimKeyframeVec3 kf = va_arg(args, AnimKeyframeVec3);
			keyframes[x].value = kf.value;
			keyframes[x].time = kf.time;
		}
		va_end(args);
	}
}
void AnimFramesVec3::SetKeyframes(btui32 i, ...) {
	if (i <= 32) {
		frameCount = i;
		va_list args;
		va_start(args, i);
		for (btui32 x = 0; x < i; x++) {
			AnimKeyframeVec3 kf = va_arg(args, AnimKeyframeVec3);
			keyframes[x].value = kf.value;
			keyframes[x].time = kf.time;
		}
		va_end(args);
	}
}
AnimResult AnimPlayerVec3::Tick(btf32 dt, AnimFramesVec3* anim, m::Vector3* aniTarget) {
	// Detect if the animation has changed
	// if the pointer gets reallocated this will be a false positive
	// but that shouldn't happen in this engine
	if (anim != anim_last) time = 0.f;
	anim_last = anim;

	// get floor, ceil, mod
	btui32 f = (btui32)floorf(time);
	btui32 c = (btui32)ceilf(time) % anim->frameCount;
	btf32 mod = fmodf(time, 1.f);

	btf32 ftime = dt * (1.f / anim->keyframes[f].time);
	btf32 ctime = dt * (1.f / anim->keyframes[c].time);

	//* tryhard ver.
	// check if we're crossing frames
	if (ftime + mod > 1.f) {
		btf32 amount_a = 1.f - mod; // fills remainder of this frame
		btf32 proportion_a = amount_a / ftime;
		btf32 proportion_b = 1.f - proportion_a;
		// apply
		time += ftime * proportion_a;
		time += ctime * proportion_b;
	}
	else // otherwise, just play as usual
		time += ftime; //*/
	/* lazy ver.
	time += dt * anim->keyframes[f].time; //*/
	time = fmodf(time, anim->frameCount); // loop

	// get floor, ceil, mod again
	f = (btui32)floorf(time);
	c = (btui32)ceilf(time) % anim->frameCount;
	mod = fmodf(time, 1.f);
	// done
	*aniTarget = m::Lerp(anim->keyframes[f].value, anim->keyframes[c].value, mod);
	return eANIM_NO_EVENT;
}
