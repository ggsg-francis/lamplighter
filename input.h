#pragma once

//union SDL_Event;

struct InputBuffer
{
	// new test keys
	btui64 keyBitsHeld = 0ui64;
	btui64 keyBitsHit = 0ui64;

	btf32 mouse_x = 0.f;
	btf32 mouse_y = 0.f;
	btf32 mouse_last_x = 0.f;
	btf32 mouse_last_y = 0.f;
	btf32 joy_x_a = 0.f;
	btf32 joy_y_a = 0.f;
	btf32 joy_x_b = 0.f;
	btf32 joy_y_b = 0.f;
};

namespace input
{
	namespace scancode
	{
		enum ScanCode : btui32
		{
			Q = 0X10,
			W = 0X11,
			E = 0X12,
			R = 0X13,
			T = 0X14,
			Y = 0X15,
			U = 0X16,
			I = 0X17,
			O = 0X18,
			P = 0X19,
			A = 0X1E,
			S = 0X1F,
			D = 0X20,
			F = 0X21,
			G = 0X22,
			H = 0X23,
			J = 0X24,
			K = 0X25,
			L = 0X26,
			Z = 0X2C,
			X = 0X2D,
			C = 0X2E,
			V = 0X2F,
			B = 0X30,
			N = 0X31,
			M = 0X32,
			DEL = 0x53,
		};
	}

	namespace key
	{
		enum Key2 : btui8
		{
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
			C_ACTIVATE, // Activate other actor
			C_ACTION_A, // Perform action A on item
			C_ACTION_B, // Perform action B on item
			C_ACTION_C, // Perform action C on item
			C_DROP_HELD,
			C_INV_CYCLE_L, // Select previous inventory slot
			C_INV_CYCLE_R, // Select next inventory slot

			//-------------------------------- UTILITY HIT

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

			KEY_COUNT,
			// How many bitvectors are needed to cover all inputs
			// A rewrite will be needed when this is > 1
			NUM_BUFFERS = CEILING(KEY_COUNT, 64u),
		};
	}

	#ifdef DEF_NMP
	#define INPUT_BUF_SIZE 2
	#define INPUT_BUF_GET 0 // First element
	#define INPUT_BUF_SET 1 // Last element
	extern InputBuffer buf[NUM_PLAYERS][INPUT_BUF_SIZE];
	#define BUF_LOCALSET buf[network::nid][INPUT_BUF_SET]
	#define BUF_LOCALGET buf[network::nid][INPUT_BUF_GET]
	#else
	extern InputBuffer buf;
	#define BUF_LOCALSET buf
	#define BUF_LOCALGET buf
	#endif

	void Init();

	void UpdateInput(void* input);

	// Clear hit inputs and mouse delta
	void ClearHitsAndDelta();
	// Clear all inputs and mouse delta
	void ClearAll();

	#ifdef DEF_NMP
	// Swap uhhhhhhhhhhhh
	void CycleBuffers();
	#endif

	#ifdef DEF_NMP
	bool GetHeld(btui32 INDEX, key::Key2 KEY);
	bool GetHit(btui32 INDEX, key::Key2 KEY);
	#endif
	bool GetHeld(key::Key2 KEY);
	bool GetHit(key::Key2 KEY);

	void Set(key::Key2 KEY);
	void Unset(key::Key2 KEY);
	void SetTo(key::Key2 KEY, bool VALUE);
	void SetHit(key::Key2 KEY);
}