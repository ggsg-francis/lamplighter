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
		FILE_FOOTSTEP_SNOW_A,
		FILE_FOOTSTEP_SNOW_B,
		FILE_EFFECT,
		FILE_TAUNT,
		FILE_HEY_SPECTRAL,
		FILE_WELCOME,
		FILE_GUI_BEEP,
		FILE_GUI_ACCEPT,
		FILE_MUS_01,
		FILE_MUS_02,
		FILE_COUNT,
	};

	void Init(void* handle);
	void Update(btf64 dt);
	void PlaySnd(AudioFile file, btf32 volume = 1.f);
	void PlaySnd3D(AudioFile file, m::Vector3 src);
	void End();
}

#endif
