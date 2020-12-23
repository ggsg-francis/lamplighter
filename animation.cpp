#include "animation.h"

AnimFrames::AnimFrames(lui32 i, ...) {
	if (i <= 32) {
		frameCount = i;
		va_list args;
		va_start(args, i);
		for (lui32 x = 0; x < i; x++) {
			AnimKeyframe kf = va_arg(args, AnimKeyframe);
			keyframes[x].key_start = kf.key_start;
			keyframes[x].key_end = kf.key_end;
			keyframes[x].time = kf.time;
		}
		va_end(args);
	}
}
void AnimFrames::SetKeyframes(lui32 i, ...) {
	if (i <= 32) {
		frameCount = i;
		va_list args;
		va_start(args, i);
		for (lui32 x = 0; x < i; x++) {
			AnimKeyframe kf = va_arg(args, AnimKeyframe);
			keyframes[x].key_start = kf.key_start;
			keyframes[x].key_end = kf.key_end;
			keyframes[x].time = kf.time;
		}
		va_end(args);
	}
}

AnimResult AnimPlayer::Tick(lf32 dt, AnimFrames* anim, lf32* aniTarget) {
	// Detect if the animation has changed
	// if the pointer gets reallocated this will be a false positive
	// but that shouldn't happen in this engine
	if (anim != anim_last) time = 0.f;
	anim_last = anim;

	// get floor, ceil, mod
	lui32 f = (lui32)floorf(time);
	lui32 c = (lui32)ceilf(time) % anim->frameCount;
	lf32 mod = fmodf(time, 1.f);

	// Set key
	*aniTarget = m::Lerp(anim->keyframes[f].key_start, anim->keyframes[f].key_end, mod);

	// Now advance time
	lf32 ftime = dt * (1.f / anim->keyframes[f].time);
	lf32 ctime = dt * (1.f / anim->keyframes[c].time);
	//* tryhard ver.
	// check if we're crossing frames
	if (ftime + mod > 1.f) {
		lf32 amount_a = 1.f - mod; // fills remainder of this frame
		lf32 proportion_a = amount_a / ftime;
		lf32 proportion_b = 1.f - proportion_a;
		// apply
		time += ftime * proportion_a;
		time += ctime * proportion_b;
	}
	else // otherwise, just play as usual
		time += ftime; //*/
	/* lazy ver.
	time += dt * anim->keyframes[f].time; //*/

	// Handle looping
	lf32 timebak = time;
	time = fmodf(time, anim->frameCount);
	if (time == timebak) return eANIM_NO_EVENT;
	else return eANIM_LOOPED;
}




AnimFramesVec3::AnimFramesVec3(lui32 i, ...) {
	if (i <= 32) {
		frameCount = i;
		va_list args;
		va_start(args, i);
		for (lui32 x = 0; x < i; x++) {
			AnimKeyframeVec3 kf = va_arg(args, AnimKeyframeVec3);
			keyframes[x].value = kf.value;
			keyframes[x].time = kf.time;
		}
		va_end(args);
	}
}
void AnimFramesVec3::SetKeyframes(lui32 i, ...) {
	if (i <= 32) {
		frameCount = i;
		va_list args;
		va_start(args, i);
		for (lui32 x = 0; x < i; x++) {
			AnimKeyframeVec3 kf = va_arg(args, AnimKeyframeVec3);
			keyframes[x].value = kf.value;
			keyframes[x].time = kf.time;
		}
		va_end(args);
	}
}
AnimResult AnimPlayerVec3::Tick(lf32 dt, AnimFramesVec3* anim, m::Vector3* aniTarget) {
	// Detect if the animation has changed
	// if the pointer gets reallocated this will be a false positive
	// but that shouldn't happen in this engine
	if (anim != anim_last) time = 0.f;
	anim_last = anim;

	// get floor, ceil, mod
	lui32 f = (lui32)floorf(time);
	lui32 c = (lui32)ceilf(time) % anim->frameCount;
	lf32 mod = fmodf(time, 1.f);

	lf32 ftime = dt * (1.f / anim->keyframes[f].time);
	lf32 ctime = dt * (1.f / anim->keyframes[c].time);

	//* tryhard ver.
	// check if we're crossing frames
	if (ftime + mod > 1.f) {
		lf32 amount_a = 1.f - mod; // fills remainder of this frame
		lf32 proportion_a = amount_a / ftime;
		lf32 proportion_b = 1.f - proportion_a;
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
	f = (lui32)floorf(time);
	c = (lui32)ceilf(time) % anim->frameCount;
	mod = fmodf(time, 1.f);
	// done
	*aniTarget = m::Lerp(anim->keyframes[f].value, anim->keyframes[c].value, mod);
	return eANIM_NO_EVENT;
}
