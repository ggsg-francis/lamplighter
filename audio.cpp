#include "audio.hpp"

#include "cfg.h"
#include "maths.hpp"
// for transform2d only
// TODO: put transform in its own file
#include "entity.h"
#include "index.h"
#include "core.h"
#ifdef DEF_NMP
#include "network.h"
#endif // DEF_NMP

//#define CUTE_SOUND_FORCE_SDL
#include "3rdparty/stb_vorbis.h"
#define CUTE_SOUND_IMPLEMENTATION
#include "3rdparty/cute_sound.h"

#define DEF_SOUNDTRACK 0
#define DEF_OGG 1

namespace aud
{
	#if !DEF_OGG
	char* fileNames[FILE_COUNT]
	{
		"snd_wav/fire_smg.wav",
		"snd_wav/swing.wav",
		"snd_wav/hit.wav",
		"snd_wav/footstep_01.wav",
		"snd_wav/footstep_02.wav",
		"snd_wav/effect.wav",
		"snd_wav/hey.wav",
		"snd_wav/hey_spectral.wav",
		"snd_wav/welcome.wav",
		"snd_wav/menu_beep.wav",
		"snd_wav/menu_accept.wav",
		"snd_wav/mus_b2.wav",
		"snd_wav/mus_fightingroom.wav",
	};
	#else
	char* fileNames[FILE_COUNT]
	{
		"snd/fire_smg.ogg",
		"snd/swing.ogg",
		"snd/hit.ogg",
		"snd/footstep_01.ogg",
		"snd/footstep_02.ogg",
		"snd/effect.ogg",
		"snd/hey.ogg",
		"snd/hey_spectral.ogg",
		"snd/welcome.ogg",
		"snd/menu_beep.ogg",
		"snd/menu_accept.ogg",
		"snd/mus_b2.ogg",
		"snd/mus_fightingroom.ogg",
	};
	#endif

	cs_context_t* ctx;
	cs_loaded_sound_t loaded[FILE_COUNT];
	cs_play_sound_def_t def[FILE_COUNT];
	cs_playing_sound_t* sound[FILE_COUNT];

	#if DEF_SOUNDTRACK
	bool tpptemp = false;
	#endif

	cs_loaded_sound_t loaded_shot;
	cs_play_sound_def_t def_shot;
	cs_playing_sound_t* sound_shot;

	lf32 mstrVol = 0.5f;

	void Init(void* handle)
	{
		mstrVol = config.fVolume;

		ctx = cs_make_context(handle, 44100, 10, 1, 32);
		if (ctx) {
			int breakpoint = 1;
			// Load all of the audio files
			for (int i = 0; i < FILE_COUNT; ++i) {
				#if !DEF_OGG
				loaded[i] = cs_load_wav(fileNames[i]);
				#else
				loaded[i] = cs_load_ogg(fileNames[i]);
				#endif
				def[i] = cs_make_def(&loaded[i]);
			}

			#if DEF_SOUNDTRACK
			tpptemp = config.b3PP;
			sound[FILE_MUS_01] = cs_play_sound(ctx, def[FILE_MUS_01]);
			if (sound[FILE_MUS_01]) {
				sound[FILE_MUS_01]->volume0 = 0.3f;
				sound[FILE_MUS_01]->volume1 = 0.3f;
				cs_loop_sound(sound[FILE_MUS_01], 1); // loop that banger!!!
			}
			#endif
		}
	}

	void Update(lf64 dt)
	{
		#if DEF_SOUNDTRACK
		if (config.b3PP != tpptemp) {
			tpptemp = config.b3PP;
			if (tpptemp) {
				cs_stop_sound(sound[FILE_MUS_02]);
				sound[FILE_MUS_01] = cs_play_sound(ctx, def[FILE_MUS_01]);
				if (sound[FILE_MUS_01]) {
					sound[FILE_MUS_01]->volume0 = 0.3f;
					sound[FILE_MUS_01]->volume1 = 0.3f;
					cs_loop_sound(sound[FILE_MUS_01], 1); // loop that banger!!!
				}
			}
			else {
				cs_stop_sound(sound[FILE_MUS_01]);
				sound[FILE_MUS_02] = cs_play_sound(ctx, def[FILE_MUS_02]);
				if (sound[FILE_MUS_02]) {
					sound[FILE_MUS_02]->volume0 = 0.3f;
					sound[FILE_MUS_02]->volume1 = 0.3f;
					cs_loop_sound(sound[FILE_MUS_02], 1); // loop that banger!!!
				}
			}
		}
		#endif

		if (ctx) {
			cs_mix(ctx);
		}
	}

	void PlaySnd(AudioFile file, lf32 volume)
	{
		// play the sound		
		sound[file] = cs_play_sound(ctx, def[file]);
		if (sound[file]) {
			sound[file]->volume0 = volume * mstrVol;
			sound[file]->volume1 = volume * mstrVol;
		}
	}

	void PlaySnd3D(AudioFile file, m::Vector3 src)
	{
		#ifdef DEF_NMP
		// TODO: caluclate panning based on proximity to L and R player
		// calculate the distance between this sound and each listener (just one atm)
		lf32 distance = m::Length(m::Vector2(src.x, src.z) - ENTITY(core::players[network::nid])->t.position);
		// get closest distance (temp until using panning)
		// stop this function if the distance is too large
		if (distance > 15.f) return;
		// calculate volume from distance
		lf32 vol = (15.f - distance) / 15.f;
		#else
		// TODO: caluclate panning based on proximity to L and R player
		// calculate the distance between this sound and each listener (just one atm)
		lf32 distance_0 = m::Length(m::Vector2(src.x, src.z) - ENTITY(core::players[0])->t.position);
		lf32 distance_1 = m::Length(m::Vector2(src.x, src.z) - ENTITY(core::players[1])->t.position);
		// get closest distance (temp until using panning)
		lf32 distance = distance_1;
		if (distance_0 <= distance_1) { distance = distance_0; }
		// stop this function if the distance is too large
		if (distance > 15.f) return;
		// calculate volume from distance
		lf32 vol = (15.f - distance) / 15.f;
		#endif // DEF_NMP
		// play the sound		
		sound[file] = cs_play_sound(ctx, def[file]);
		if (sound[file]) {
			sound[file]->volume0 = vol * mstrVol;
			sound[file]->volume1 = vol * mstrVol;
		}
	}

	void End()
	{
		for (int i = 0; i < FILE_COUNT; ++i) {
			cs_free_sound(&loaded[i]);
		}
		free(ctx);
	}
}
