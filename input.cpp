#include "input.h"
#include "memory.hpp"
#ifdef __cplusplus
extern "C" {
	#endif
	#include <SDL2\SDL.h>
	#ifdef __cplusplus
}
#endif
#ifdef DEF_NMP
#include "network.h"
#endif // DEF_NMP

#define JOY_AXIS_MAX ((bti16)0b0111111111111111)
#define JOY_AXIS_MIN ((bti16)0b1000000000000000)
#define JOY_AXIS_THRESH_MAX ((bti16)0b0000011111111111)
#define JOY_AXIS_THRESH_MIN ((bti16)0b1111100000000000)
#define JOY_AXIS_MAX_F 32768.f

namespace input
{
	namespace ki
	{
		char* scancodeNames[]
		{
			"Error",
			"Escape",
			"1",
			"2",
			"3",
			"4",
			"5",
			"6",
			"7",
			"8",
			"9",
			"0",
			"-",
			"=",
			"Backspace",
			"Tab",
			"Q",
			"W",
			"E",
			"R",
			"T",
			"Y",
			"U",
			"I",
			"O",
			"P",
			"[",
			"]",
			"Enter",
			"Left Control",
			"A",
			"S",
			"D",
			"F",
			"G",
			"H",
			"J",
			"K",
			"L",
			";",
			"'",
			"`",
			"Left Shift",
			"'\'",
			"Z",
			"X",
			"C",
			"V",
			"B",
			"N",
			"M",
			",",
			".",
			"/",
			"Right Shift",
			"Print Screen",
			"Left Alt",
			"Space",
			"Caps Lock",
			"F1",
			"F2",
			"F3",
			"F4",
			"F5",
			"F6",
			"F7",
			"F8",
			"F9",
			"F10",
			"Num Lock",
			"Scroll Lock",
			"Numpad 7",
			"Numpad 8",
			"Numpad 9",
			"Numpad -",
			"Numpad 4",
			"Numpad 5",
			"Numpad 6",
			"Numpad +",
			"Numpad 1",
			"Numpad 2",
			"Numpad 3",
			"Numpad 0",
			"Numpad Del",
			"Alt-SysRq",
			"F11 or F12 or PF1 or FN", // 55
			"Unlabeled Key",
			"F11",
			"F12",
			"Unknown", // Everything at or past this point is not standard
		};
	}

	enum enumJoyButton : btui8 {
		JOY_FACE_A,
		JOY_FACE_B,
		JOY_FACE_Y,
		JOY_FACE_X,
		JOY_BUMPER_L,
		JOY_BUMPER_R,
		JOY_BACK,
		JOY_START,
		JOY_STICK_L,
		JOY_STICK_R,
		JOY_BUTTON_COUNT,
	};
	enum enumJoyAxis : btui8 {
		JOY_AXIS_L_X,
		JOY_AXIS_L_Y,
		JOY_AXIS_TRIGGER_L,
		JOY_AXIS_R_X,
		JOY_AXIS_R_Y,
		JOY_AXIS_TRIGGER_R,
		JOY_AXIS_COUNT,
	};

	key::Key2 ScancodeTransfer[512]{ key::NONE }; // uses btui8 to save space
	key::Key2 JoyButtonTransfer[JOY_BUTTON_COUNT]{ key::NONE };
	//key::Key2 JoyAxisTransfer[9]{ key::NONE };

	void Init()
	{
		ScancodeTransfer[SDL_SCANCODE_ESCAPE] = key::QUIT;
		ScancodeTransfer[SDL_SCANCODE_W] = key::DIR_F;
		ScancodeTransfer[SDL_SCANCODE_S] = key::DIR_B;
		ScancodeTransfer[SDL_SCANCODE_D] = key::DIR_R;
		ScancodeTransfer[SDL_SCANCODE_A] = key::DIR_L;
		ScancodeTransfer[SDL_SCANCODE_LSHIFT] = key::RUN;
		ScancodeTransfer[SDL_SCANCODE_LALT] = key::CROUCH;
		ScancodeTransfer[SDL_SCANCODE_SPACE] = key::JUMP;
		ScancodeTransfer[SDL_SCANCODE_E] = key::ACTIVATE;
		ScancodeTransfer[SDL_SCANCODE_1] = key::ACTION_A;
		ScancodeTransfer[SDL_SCANCODE_2] = key::ACTION_B;
		ScancodeTransfer[SDL_SCANCODE_3] = key::ACTION_C;
		ScancodeTransfer[SDL_SCANCODE_R] = key::DROP_HELD;
		ScancodeTransfer[SDL_SCANCODE_Z] = key::INV_CYCLE_L;
		ScancodeTransfer[SDL_SCANCODE_X] = key::INV_CYCLE_R;
		ScancodeTransfer[SDL_SCANCODE_F1] = key::FUNCTION_1;
		ScancodeTransfer[SDL_SCANCODE_F2] = key::FUNCTION_2;
		ScancodeTransfer[SDL_SCANCODE_F3] = key::FUNCTION_3;
		ScancodeTransfer[SDL_SCANCODE_F4] = key::FUNCTION_4;
		ScancodeTransfer[SDL_SCANCODE_F5] = key::FUNCTION_5;
		ScancodeTransfer[SDL_SCANCODE_F6] = key::FUNCTION_6;
		ScancodeTransfer[SDL_SCANCODE_F7] = key::FUNCTION_7;
		ScancodeTransfer[SDL_SCANCODE_F8] = key::FUNCTION_8;
		ScancodeTransfer[SDL_SCANCODE_F9] = key::FUNCTION_9;
		ScancodeTransfer[SDL_SCANCODE_F10] = key::FUNCTION_10;
		ScancodeTransfer[SDL_SCANCODE_F11] = key::FUNCTION_11;
		ScancodeTransfer[SDL_SCANCODE_F12] = key::FUNCTION_12;

		JoyButtonTransfer[JOY_BUMPER_L] = key::C_RUN;
		JoyButtonTransfer[JOY_BUMPER_R] = key::C_JUMP;
		JoyButtonTransfer[JOY_FACE_A] = key::C_ACTIVATE;
		JoyButtonTransfer[JOY_FACE_B] = key::C_USE_ALT;
		JoyButtonTransfer[JOY_STICK_R] = key::C_CROUCH;
	}

	#ifdef DEF_NMP
	InputBuffer buf[NUM_PLAYERS][INPUT_BUF_SIZE];
	#else
	InputBuffer buf;
	#endif

	void UpdateInput(void * input)
	{
		// rename later so we can make it a pointer
		SDL_Event& e = *(SDL_Event*)input;

		switch (e.type)
		{
		case SDL_KEYDOWN:
			if (e.key.repeat == 0 && ScancodeTransfer[e.key.keysym.scancode] != key::NONE)
				Set(ScancodeTransfer[e.key.keysym.scancode]);
			break;
		case SDL_KEYUP:
			if (e.key.repeat == 0 && ScancodeTransfer[e.key.keysym.scancode] != key::NONE)
				Unset(ScancodeTransfer[e.key.keysym.scancode]);
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (e.button.button == SDL_BUTTON_LEFT)
				Set(key::USE);
			if (e.button.button == SDL_BUTTON_RIGHT)
				Set(key::USE_ALT);
			break;
		case SDL_MOUSEBUTTONUP:
			if (e.button.button == SDL_BUTTON_LEFT)
				Unset(key::USE);
			if (e.button.button == SDL_BUTTON_RIGHT)
				Unset(key::USE_ALT);
			break;
		case SDL_MOUSEMOTION:
			BUF_LOCALSET.mouse_x += e.motion.xrel;
			BUF_LOCALSET.mouse_y += e.motion.yrel;
			break;
		case SDL_MOUSEWHEEL:
			if (e.wheel.y > 0.5f) // Scroll up
				SetHit(key::INV_CYCLE_R);
			else if (e.wheel.y < -0.5f) // Scroll down
				SetHit(key::INV_CYCLE_L);
			break;
		case SDL_JOYAXISMOTION: // Joystick Motion
			if (e.jaxis.axis == 2) { // Left Trigger
				SetTo(key::C_USE, e.jaxis.value > 0);
			}
			if (e.jaxis.axis == 5) { // Right Trigger
				SetTo(key::C_USE, e.jaxis.value > 0);
			}
			if ((e.jaxis.value < JOY_AXIS_THRESH_MIN) || (e.jaxis.value > JOY_AXIS_THRESH_MAX)) {
				if (e.jaxis.axis == JOY_AXIS_L_X) // Left-right left stick
					BUF_LOCALSET.joy_x_a = (btf32)e.jaxis.value / (btf32)JOY_AXIS_MAX_F;
				else if (e.jaxis.axis == JOY_AXIS_L_Y) // Up-Down left stick
					BUF_LOCALSET.joy_y_a = (btf32)e.jaxis.value / (btf32)JOY_AXIS_MAX_F;
				else if (e.jaxis.axis == JOY_AXIS_R_X) // Left-right right stick
					BUF_LOCALSET.joy_x_b = (btf32)e.jaxis.value / (btf32)JOY_AXIS_MAX_F;
				else if (e.jaxis.axis == JOY_AXIS_R_Y) // Up-Down right stick
					BUF_LOCALSET.joy_y_b = (btf32)e.jaxis.value / (btf32)JOY_AXIS_MAX_F;
			}
			else {
				if (e.jaxis.axis == JOY_AXIS_L_X) // Left-right left stick
					BUF_LOCALSET.joy_x_a = 0.f;
				else if (e.jaxis.axis == JOY_AXIS_L_Y) // Up-Down left stick
					BUF_LOCALSET.joy_y_a = 0.f;
				else if (e.jaxis.axis == JOY_AXIS_R_X) // Left-right right stick
					BUF_LOCALSET.joy_x_b = 0.f;
				else if (e.jaxis.axis == JOY_AXIS_R_Y) // Up-Down right stick
					BUF_LOCALSET.joy_y_b = 0.f;
			}
			break;
		case SDL_JOYHATMOTION:
			if (e.jhat.value == SDL_HAT_CENTERED) {
				Unset(key::C_INV_CYCLE_L);
				Unset(key::C_INV_CYCLE_R);
				Unset(key::C_DROP_HELD);
			}
			else if (e.jhat.value == SDL_HAT_RIGHT)
				Set(key::C_INV_CYCLE_R);
			else if (e.jhat.value == SDL_HAT_LEFT)
				Set(key::C_INV_CYCLE_L);
			else if (e.jhat.value == SDL_HAT_DOWN)
				Set(key::C_DROP_HELD);
			break;
		case SDL_JOYBUTTONDOWN:
			if (!GetHeld(JoyButtonTransfer[e.jbutton.button]) && JoyButtonTransfer[e.jbutton.button] != key::NONE)
				Set(JoyButtonTransfer[e.jbutton.button]);
				break;
		case SDL_JOYBUTTONUP:
			if (JoyButtonTransfer[e.jbutton.button] != key::NONE)
				Unset(JoyButtonTransfer[e.jbutton.button]);
			break;
		}
	}

	void ClearHitsAndDelta()
	{
		BUF_LOCALSET.mouse_x = 0.f;
		BUF_LOCALSET.mouse_y = 0.f;
		BUF_LOCALSET.keyBitsHit = 0b0000000000000000000000000000000000000000000000000000000000000000u;
	}

	void ClearAll()
	{
		BUF_LOCALSET.keyBitsHit = 0b0000000000000000000000000000000000000000000000000000000000000000u;
		BUF_LOCALSET.keyBitsHeld = 0b0000000000000000000000000000000000000000000000000000000000000000u;
	}

	#ifdef DEF_NMP
	void CycleBuffers()
	{
		buf[network::nid][0] = buf[network::nid][1];
	}
	#endif

	#ifdef DEF_NMP
	bool GetHeld(btui32 index, key::Key2 i)
	{
		return mem::bvget<btui64>(buf[index][INPUT_BUF_GET].keyBitsHeld, 1u << (btui64)i);
	}
	bool GetHit(btui32 index, key::Key2 i)
	{
		return mem::bvget<btui64>(buf[index][INPUT_BUF_GET].keyBitsHit, 1u << (btui64)i);
	}
	#endif
	bool GetHeld(key::Key2 i)
	{
		return mem::bvget<btui64>(BUF_LOCALGET.keyBitsHeld, (btui64)1u << (btui64)i);
	}
	bool GetHit(key::Key2 i)
	{
		return mem::bvget<btui64>(BUF_LOCALGET.keyBitsHit, (btui64)1u << (btui64)i);
	}
	void Set(key::Key2 i)
	{
		mem::bvset<btui64>(BUF_LOCALSET.keyBitsHeld, (btui64)1u << (btui64)i);
		mem::bvset<btui64>(BUF_LOCALSET.keyBitsHit, (btui64)1u << (btui64)i);
	}
	void Unset(key::Key2 i)
	{
		mem::bvunset<btui64>(BUF_LOCALSET.keyBitsHeld, (btui64)1u << (btui64)i);
	}
	void SetTo(key::Key2 i, bool b)
	{
		mem::bvsetto<btui64>(BUF_LOCALSET.keyBitsHeld, (btui64)1u << (btui64)i, b);
		mem::bvsetto<btui64>(BUF_LOCALSET.keyBitsHit, (btui64)1u << (btui64)i, b);
	}
	void SetHit(key::Key2 i)
	{
		mem::bvset<btui64>(BUF_LOCALSET.keyBitsHit, (btui64)1u << (btui64)i);
	}
}
