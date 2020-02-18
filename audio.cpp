#include "audio.hpp"

//#include <SDL2\SDL.h>


#ifdef DEF_USE_CS
#define CUTE_SOUND_IMPLEMENTATION
#include "3rdparty\cute_sound.h"
#else
#include <irrklang/irrKlang.h>
using namespace irrklang;
#endif // DEF_USE_CS


namespace aud
{
	#ifdef DEF_USE_CS

	cs_context_t* ctx;
	cs_loaded_sound_t loaded;
	cs_play_sound_def_t def;
	cs_playing_sound_t* sound;

	void Init(void* handle)
	{
		ctx = cs_make_context(handle, 100, 1, 1000, 32);

		loaded = cs_load_wav("resources/hey.wav");
		def = cs_make_def(&loaded);
		sound = cs_play_sound(ctx, def);

		//SoundEngine = createIrrKlangDevice();
	}

	void PlaySnd()
	{
		//SoundEngine->play2D("resources/mus_beep.mp3", false);
	}

	void End()
	{
		cs_shutdown_context(ctx);
	}

	#else

	ISoundEngine *SoundEngine;

	void Init()
	{
		SoundEngine = createIrrKlangDevice();
		SoundEngine->setSoundVolume(0.08f);
	}

	void PlaySnd()
	{
		SoundEngine->play2D("snd/hey.wav", false);
	}

	void PlayGunshotTemp(bool shot)
	{
		if (shot) SoundEngine->play2D("snd/fire_smg.wav", false);
		else SoundEngine->play2D("snd/fire_no_shot.wav", false);
		//SoundEngine->play3D();
	}

	void End()
	{
		
	}

	#endif // DEF_USE_CS
}