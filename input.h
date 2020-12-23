#ifndef INPUT_H
#define INPUT_H

#include "global.h"

struct InputBuffer
{
	// Binary state inputs
	lui64 keyBitsHeld = 0u;
	lui64 keyBitsHit = 0u;
	// Non-binary inputs ;3
	lf32 mouse_x = 0.f;
	lf32 mouse_y = 0.f;
	lf32 mouse_last_x = 0.f;
	lf32 mouse_last_y = 0.f;
	lf32 joy_x_a = 0.f;
	lf32 joy_y_a = 0.f;
	lf32 joy_x_b = 0.f;
	lf32 joy_y_b = 0.f;
};

namespace input
{
	namespace key {
		enum InputBit : lui8 {
			NONE, // Empty input
			//-------------------------------- KEYBOARD PLAYER INPUT
			DIR_F,
			DIR_B,
			DIR_L,
			DIR_R,
			USE, // Use equipped item
			USE_ALT, // Use equipped item
			RUN,
			CROUCH,
			JUMP,
			ACTIVATE, // Activate other actor
			ACTION_A, // Perform action A on item
			ACTION_B, // Perform action B on item
			ACTION_C, // Perform action C on item
			DROP_HELD,
			INV_CYCLE_L, // Select previous inventory slot
			INV_CYCLE_R, // Select next inventory slot
			//-------------------------------- CONTROLLER PLAYER INPUT
			C_FW, // Are these directionals even needed?
			C_BK,
			C_LT,
			C_RT,
			C_USE, // Use equipped item
			C_USE_ALT, // Use equipped item
			C_RUN,
			C_CROUCH,
			C_JUMP,
			C_ACTIVATE, // Activate other actor
			C_ACTION_A, // Perform action A on item
			C_ACTION_B, // Perform action B on item
			C_ACTION_C, // Perform action C on item
			C_DROP_HELD,
			C_INV_CYCLE_L, // Select previous inventory slot
			C_INV_CYCLE_R, // Select next inventory slot
			//-------------------------------- UTILITY
			QUIT,
			FUNCTION_1,
			FUNCTION_2,
			FUNCTION_3,
			FUNCTION_4,
			FUNCTION_5,
			FUNCTION_6,
			FUNCTION_7,
			FUNCTION_8,
			FUNCTION_9,
			FUNCTION_10,
			FUNCTION_11,
			FUNCTION_12,
			// How many keys
			KEY_COUNT,
			// How many bitvectors are needed to cover all inputs
			// A rewrite will be needed when this is > 1
			NUM_BUFFERS = CEILING(KEY_COUNT, 64u),
		};
	}

	#ifdef DEF_NMP
	extern InputBuffer input_buffer[NUM_PLAYERS];
	#define BUF_LOCALSET input_buffer[network::nid]
	#define BUF_LOCALGET input_buffer[network::nid]
	#else
	extern InputBuffer input_buffer;
	#define BUF_LOCALSET input_buffer
	#define BUF_LOCALGET input_buffer
	#endif

	void Init();
	char* GetScancodeName(lui32 scancode);
	char* GetInputName(key::InputBit input);
	void ResetKeyBindings();
	void BindKeyToInput(lui32 scancode, key::InputBit input);
	void SaveFile();
	void LoadFile();

	void UpdateInput(void* input);

	// Clear hit inputs and mouse delta
	void ClearHitsAndDelta();
	// Clear all inputs and mouse delta
	void ClearAll();

	#ifdef DEF_NMP
	bool GetHeld(lui32 index, key::Key2 input);
	bool GetHit(lui32 index, key::Key2 input);
	#endif
	bool GetHeld(key::InputBit input);
	bool GetHit(key::InputBit input);
	bool GetAnyHit();

	void Set(key::InputBit input);
	void Unset(key::InputBit input);
	void SetTo(key::InputBit input, bool value);
	void SetHit(key::InputBit input);
}

#endif
