#include "audio.hpp"

//#include <SDL2\SDL.h>

#include "maths.hpp"
// for transform2d only
// TODO: put transform in its own file
#include "objects_entities.h"
#include "index.h"
#ifdef DEF_NMP
#include "network.h"
#endif // DEF_NMP

#ifdef DEF_USE_CS
//#define CUTE_SOUND_FORCE_SDL
#define CUTE_SOUND_IMPLEMENTATION
#include "3rdparty\cute_sound.h"
#endif

namespace aud
{
	#ifdef DEF_USE_CS

	char* fileNames[]
	{
		"snd/fire_smg.wav",
		"snd/swing.wav",
		//"snd/swing_connect.wav",
		"snd/hitMW.wav",
		"snd/hey.wav",
		/*"snd/footstep_snow_a.wav",
		"snd/footstep_snow_b.wav",*/
		"snd/footstep_01.wav",
		"snd/footstep_02.wav",
	};

	cs_context_t* ctx;
	cs_loaded_sound_t loaded[FILE_COUNT];
	cs_play_sound_def_t def[FILE_COUNT];
	cs_playing_sound_t* sound[FILE_COUNT];

	cs_loaded_sound_t loaded_shot;
	cs_play_sound_def_t def_shot;
	cs_playing_sound_t* sound_shot;

	btf32 mstrVol = 0.2f;
	//btf32 mstrVol = 1.f;

	void Init(void* handle)
	{
		//ctx = cs_make_context(handle, 44100, 8192, 0, 32);
		//ctx = cs_make_context(handle, 100, 1, 1000, 32); 
		//ctx = cs_make_context(handle, 44100, 8192, 1, 32);
		ctx = cs_make_context(handle, 44100, 10, 1, 32);
		if (ctx)
		{
			int breakpoint = 1;
			//do things
			for (int i = 0; i < FILE_COUNT; ++i)
			{
				loaded[i] = cs_load_wav(fileNames[i]);
				def[i] = cs_make_def(&loaded[i]);
			}

			//sound[FILE_TAUNT] = cs_play_sound(ctx, def[FILE_TAUNT]);

			//loaded_shot = cs_load_wav("snd/fire_smg.wav");
			//def_shot = cs_make_def(&loaded_shot);
		}
		else
		{
			int breakpoint = 1;
		}
	}

	void Update(btf64 dt)
	{
		if (ctx)
		{
			cs_mix(ctx);
		}
	}

	void PlaySnd(AudioFile file, m::Vector3 src)
	{
		#ifdef DEF_NMP
		// TODO: caluclate panning based on proximity to L and R player
		// calculate the distance between this sound and each listener (just one atm)
		btf32 distance = m::Length(m::Vector2(src.x, src.z) - ENTITY(index::players[network::nid])->t.position);
		// get closest distance (temp until using panning)
		// stop this function if the distance is too large
		if (distance > 15.f) return;
		// calculate volume from distance
		btf32 vol = (15.f - distance) / 15.f;
		// play the sound		
		sound[file] = cs_play_sound(ctx, def[file]);
		if (sound[file])
		{
			sound[file]->volume0 = vol * mstrVol;
			sound[file]->volume1 = vol * mstrVol;
		}
		#else
		// TODO: caluclate panning based on proximity to L and R player
		// calculate the distance between this sound and each listener (just one atm)
		btf32 distance_0 = m::Length(m::Vector2(src.x, src.z) - ENTITY(index::players[0])->t.position);
		btf32 distance_1 = m::Length(m::Vector2(src.x, src.z) - ENTITY(index::players[1])->t.position);
		// get closest distance (temp until using panning)
		btf32 distance = distance_1;
		if (distance_0 <= distance_1) { distance = distance_0; }
		// stop this function if the distance is too large
		if (distance > 15.f) return;
		// calculate volume from distance
		btf32 vol = (15.f - distance) / 15.f;
		// play the sound		
		sound[file] = cs_play_sound(ctx, def[file]);
		if (sound[file])
		{
			sound[file]->volume0 = vol * mstrVol;
			sound[file]->volume1 = vol * mstrVol;
		}
		#endif // DEF_NMP
	}

	void End()
	{
		for (int i = 0; i < FILE_COUNT; ++i)
		{
			cs_free_sound(&loaded[i]);
		}
		free(ctx);
	}

	#else

	void Init(void* handle)
	{
	}

	void Update(btf64 dt)
	{
	}

	void PlaySnd(AudioFile file, m::Vector3 src)
	{
	}

	void End()
	{
	}

	#endif
}
