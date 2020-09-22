#include "audio.hpp"

#include "maths.hpp"
// for transform2d only
// TODO: put transform in its own file
#include "entity.h"
#include "index.h"
#include "core.h"
#ifdef DEF_NMP
#include "network.h"
#endif // DEF_NMP

#ifdef DEF_USE_CS
//#define CUTE_SOUND_FORCE_SDL
#define CUTE_SOUND_IMPLEMENTATION
#include "3rdparty\cute_sound.h"
#else
//#define MA_NO_WAV
//#define MA_NO_MP3
//#define MA_NO_FLAC
#define MINIAUDIO_IMPLEMENTATION
#include "3rdparty\miniaudio.h"
#endif

namespace aud
{
	#ifdef DEF_USE_CS

	char* fileNames[FILE_COUNT]
	{
		"snd/fire_smg.wav",
		"snd/swing.wav",
		"snd/hit.wav",
		"snd/footstep_01.wav",
		"snd/footstep_02.wav",
		"snd/effect.wav",
		"snd/hey.wav",
		"snd/hey_spectral.wav",
		"snd/welcome.wav",
		"snd/menu_beep.wav",
		"snd/menu_accept.wav",
	};

	cs_context_t* ctx;
	cs_loaded_sound_t loaded[FILE_COUNT];
	cs_play_sound_def_t def[FILE_COUNT];
	cs_playing_sound_t* sound[FILE_COUNT];

	#if DEF_PROJECT == PROJECT_BC
	cs_loaded_sound_t loaded_music_1;
	cs_loaded_sound_t loaded_music_2;
	cs_play_sound_def_t def_music;
	cs_playing_sound_t* sound_music;

	bool tpptemp = false;
	#endif

	cs_loaded_sound_t loaded_shot;
	cs_play_sound_def_t def_shot;
	cs_playing_sound_t* sound_shot;

	btf32 mstrVol = 0.5f;

	void Init(void* handle)
	{
		ctx = cs_make_context(handle, 44100, 10, 1, 32);
		if (ctx) {
			int breakpoint = 1;
			// Load all of the audio files
			for (int i = 0; i < FILE_COUNT; ++i) {
				//cs_read_mem_wav
				loaded[i] = cs_load_wav(fileNames[i]);
				def[i] = cs_make_def(&loaded[i]);
			}

			#if DEF_PROJECT == PROJECT_BC
			tpptemp = config.b3PP;

			loaded_music_1 = cs_load_wav("snd/mus_morrowind_theme_dj_mix.wav");
			loaded_music_2 = cs_load_wav("snd/mus_fightingroom.wav");
			def_music = cs_make_def(&loaded_music_1);
			sound_music = cs_play_sound(ctx, def_music);
			if (sound_music) {
				sound_music->volume0 = 0.3f;
				sound_music->volume1 = 0.3f;
			}
			cs_loop_sound(sound_music, 1); // loop that banger!!!
			#endif
		}
		else {
			int breakpoint = 1;
		}
	}

	void Update(btf64 dt)
	{
		#if DEF_PROJECT == PROJECT_BC
		if (config.b3PP != tpptemp) {
			tpptemp = config.b3PP;
			cs_stop_sound(sound_music);
			if (tpptemp)
				def_music = cs_make_def(&loaded_music_1);
			else
				def_music = cs_make_def(&loaded_music_2);
			sound_music = cs_play_sound(ctx, def_music);
			if (sound_music) {
				sound_music->volume0 = 0.3f;
				sound_music->volume1 = 0.3f;
			}
			cs_loop_sound(sound_music, 1); // loop that banger!!!
		}
		#endif

		if (ctx) {
			cs_mix(ctx);
		}
	}

	void PlaySnd(AudioFile file, btf32 volume)
	{
		// play the sound		
		sound[file] = cs_play_sound(ctx, def[file]);
		if (sound[file]) {
			sound[file]->volume0 = mstrVol;
			sound[file]->volume1 = mstrVol;
		}
	}

	void PlaySnd3D(AudioFile file, m::Vector3 src)
	{
		#ifdef DEF_NMP
		// TODO: caluclate panning based on proximity to L and R player
		// calculate the distance between this sound and each listener (just one atm)
		btf32 distance = m::Length(m::Vector2(src.x, src.z) - ENTITY(core::players[network::nid])->t.position);
		// get closest distance (temp until using panning)
		// stop this function if the distance is too large
		if (distance > 15.f) return;
		// calculate volume from distance
		btf32 vol = (15.f - distance) / 15.f;
		#else
		// TODO: caluclate panning based on proximity to L and R player
		// calculate the distance between this sound and each listener (just one atm)
		btf32 distance_0 = m::Length(m::Vector2(src.x, src.z) - ENTITY(core::players[0])->t.position);
		btf32 distance_1 = m::Length(m::Vector2(src.x, src.z) - ENTITY(core::players[1])->t.position);
		// get closest distance (temp until using panning)
		btf32 distance = distance_1;
		if (distance_0 <= distance_1) { distance = distance_0; }
		// stop this function if the distance is too large
		if (distance > 15.f) return;
		// calculate volume from distance
		btf32 vol = (15.f - distance) / 15.f;
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

	#else

	char* fileNames[FILE_COUNT]
	{
		"snd/fire_smg.mp3",
		"snd/swing.mp3",
		"snd/hit.mp3",
		"snd/footstep_01.mp3",
		"snd/footstep_02.mp3",
		"snd/effect.mp3",
		"snd/hey.mp3",
		"snd/hey_spectral.mp3",
		"snd/welcome.mp3",
		"snd/menu_beep.mp3",
		"snd/menu_accept.mp3",
		"snd/mus_b2.mp3",
		"snd/mus_fightingroom.mp3",
	};

	ma_result result;
	ma_decoder decoders[FILE_COUNT];
	ma_device_config deviceConfig;
	ma_device device;

	bool sound_playing[FILE_COUNT];
	btf32 sound_volume[FILE_COUNT];

	//ma_event g_stopEvent; /* <-- Signaled by the audio thread, waited on by the main thread. */

	#define CHANNEL_COUNT 2

	//The way mixing works is that we just read into a temporary buffer, then take the contents of that buffer and mix it with the
	//contents of the output buffer by simply adding the samples together. You could also clip the samples to -1..+1, but I'm not
	//doing that in this example.
	ma_uint32 MixS16(ma_decoder* pDecoder, bti16* pOutputS16, ma_uint32 frameCount, btf32 volume) {
		bti16 temp[4096];
		ma_uint32 tempCapInFrames = ma_countof(temp) / CHANNEL_COUNT;
		ma_uint32 totalFramesRead = 0;

		while (totalFramesRead < frameCount) {
			ma_uint32 iSample;
			ma_uint32 framesReadThisIteration;
			ma_uint32 totalFramesRemaining = frameCount - totalFramesRead;
			ma_uint32 framesToReadThisIteration = tempCapInFrames;
			if (framesToReadThisIteration > totalFramesRemaining) {
				framesToReadThisIteration = totalFramesRemaining;
			}

			framesReadThisIteration = (ma_uint32)ma_decoder_read_pcm_frames(pDecoder, temp, framesToReadThisIteration);
			if (framesReadThisIteration == 0) {
				break;
			}

			/* Mix the frames together. */
			for (iSample = 0; iSample < framesReadThisIteration*CHANNEL_COUNT; ++iSample) {
				bti64 output = pOutputS16[totalFramesRead*CHANNEL_COUNT + iSample] + temp[iSample];
				// vol test
				output = (bti64)((btf32)output * volume);
				// Clamp to 16 bit int range
				if (output > (bti64)((bti16)0b0111111111111111))
					output = (bti64)((bti16)0b0111111111111111);
				if (output < (bti64)((bti16)0b1000000000000000))
					output = (bti64)((bti16)0b1000000000000000);
				pOutputS16[totalFramesRead*CHANNEL_COUNT + iSample] = (bti16)output;
			}

			totalFramesRead += framesReadThisIteration;

			if (framesReadThisIteration < framesToReadThisIteration) {
				break;  /* Reached EOF. */
			}
		}

		return totalFramesRead;
	}

	// Playback
	void SoundCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount) {
		// In playback mode copy data to pOutput. In capture mode read data from pInput. In full-duplex mode, both
		// pOutput and pInput will be valid and you can move data from pInput into pOutput. Never process more than
		// frameCount frames.
		bti16* pOutputS16 = (bti16*)pOutput;
		//MA_ASSERT(pDevice->playback.format == SAMPLE_FORMAT);   // <-- Important for this example.
		for (int iDecoder = 0; iDecoder < FILE_COUNT; ++iDecoder) {
			if (!sound_playing[iDecoder]) continue;
			ma_uint32 framesRead = MixS16(&decoders[iDecoder], pOutputS16, frameCount, sound_volume[iDecoder]);
			if (framesRead < frameCount) // Nice! so if we read less than the max number of frames we reached the end
				sound_playing[iDecoder] = false;
		}
		(void)pInput;
	}

	void Init(void* handle)
	{
		// Load all of the audio files
		for (int i = 0; i < FILE_COUNT; ++i) {
			result = ma_decoder_init_file(fileNames[i], NULL, &decoders[i]);
			sound_playing[i] = false;
			if (result != MA_SUCCESS) return;
		}

		deviceConfig = ma_device_config_init(ma_device_type_playback);
		deviceConfig.playback.format = decoders[0].outputFormat; // Set to ma_format_unknown to use the device's native format.
		deviceConfig.playback.channels = decoders[0].outputChannels; // Set to 0 to use the device's native channel count.
		deviceConfig.sampleRate = decoders[0].outputSampleRate; // Set to 0 to use the device's native sample rate.
		deviceConfig.dataCallback = SoundCallback; // This function will be called when miniaudio needs more data.
		deviceConfig.pUserData = NULL; // Can be accessed from the device object (device.pUserData).

		// Initialize sound device
		if (ma_device_init(NULL, &deviceConfig, &device) != MA_SUCCESS) {
			printf("Failed to open playback device.\n");
			for (int i = 0; i < FILE_COUNT; ++i)
				ma_decoder_uninit(&decoders[i]);
			return;
		}

		/*
		We can't stop in the audio thread so we instead need to use an event.
		We wait on this thread in the main thread, and signal it in the audio thread.
		This needs to be done before starting the device. We need a context to
		initialize the event, which we can get from the device. Alternatively you can
		initialize a context separately, but we don't need to do that for this example.
		*/
		//ma_event_init(&g_stopEvent);

		// The device is sleeping by default so you'll need to start it manually.
		if (ma_device_start(&device) != MA_SUCCESS) {
			printf("Failed to start playback device.\n");
			ma_device_uninit(&device);
			for (int i = 0; i < FILE_COUNT; ++i)
				ma_decoder_uninit(&decoders[i]);
			return;
		}
	}

	void Update(btf64 dt)
	{
		// Do something here. Probably your program's main loop.
		if (config.b3PP) {
			if (!sound_playing[FILE_MUS_01]) {
				PlaySnd(FILE_MUS_01, 0.4f);
			}
			sound_playing[FILE_MUS_02] = false;
		}
		else {
			if (!sound_playing[FILE_MUS_02]) {
				PlaySnd(FILE_MUS_02, 0.5f);
			}
			sound_playing[FILE_MUS_01] = false;
		}
	}

	void PlaySnd(AudioFile file, btf32 volume)
	{
		// reset file to beginning
		ma_decoder_seek_to_pcm_frame(&decoders[file], 0);
		sound_playing[file] = true;
		sound_volume[file] = volume;
	}

	void PlaySnd3D(AudioFile file, m::Vector3 src)
	{
		#ifdef DEF_NMP
		// TODO: caluclate panning based on proximity to L and R player
		// calculate the distance between this sound and each listener (just one atm)
		btf32 distance = m::Length(m::Vector2(src.x, src.z) - ENTITY(core::players[network::nid])->t.position);
		// get closest distance (temp until using panning)
		// stop this function if the distance is too large
		if (distance > 15.f) return;
		// calculate volume from distance
		btf32 vol = (15.f - distance) / 15.f;
		#else
		// TODO: caluclate panning based on proximity to L and R player
		// calculate the distance between this sound and each listener (just one atm)
		btf32 distance_0 = m::Length(m::Vector2(src.x, src.z) - ENTITY(core::players[0])->t.position);
		btf32 distance_1 = m::Length(m::Vector2(src.x, src.z) - ENTITY(core::players[1])->t.position);
		// get closest distance (temp until using panning)
		btf32 distance = distance_1;
		if (distance_0 <= distance_1) { distance = distance_0; }
		// stop this function if the distance is too large
		if (distance > 15.f) return;
		// calculate volume from distance
		btf32 vol = (15.f - distance) / 15.f;
		#endif // DEF_NMP
		// play the sound	
		ma_decoder_seek_to_pcm_frame(&decoders[file], 0);
		sound_playing[file] = true;
		sound_volume[file] = vol;
		// volume = vol * mstrvol;
	}

	void End()
	{
		ma_device_uninit(&device);    // This will stop the device so no need to do that manually.
		for (int i = 0; i < FILE_COUNT; ++i)
			ma_decoder_uninit(&decoders[i]);
	}

	#endif
}
