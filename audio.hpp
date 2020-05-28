#ifndef AUDIO_H
#define AUDIO_H

#include "global.h"

namespace m
{
	class Vector3;
}
class Transform2D;

namespace aud
{
	enum AudioFile
	{
		FILE_SHOT_SMG,
		FILE_SWING,
		FILE_SWING_CONNECT,
		FILE_TAUNT,
		FILE_FOOTSTEP_SNOW_A,
		FILE_FOOTSTEP_SNOW_B,
		FILE_COUNT,
	};

	void Init(void* handle);
	void Update(btf64 dt);
	void PlaySnd(AudioFile file, m::Vector3 src);
	void End();
}

#endif
