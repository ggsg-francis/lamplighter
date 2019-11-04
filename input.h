#pragma once

//union SDL_Event;
struct  GLFWwindow;

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
			RUN,
			ACTIVATE, // Activate other actor
			ACTION_A,
			ACTION_B,
			ACTION_C,
			ACTION_D,

			//-------------------------------- CONTROLLER PLAYER INPUT

			C_FW, // Are these directionals even needed?
			C_BK,
			C_LT,
			C_RT,
			C_USE, // Use equipped item
			C_RUN,
			C_ACTIVATE, // Activate other actor
			C_ACTION_A,
			C_ACTION_B,
			C_ACTION_C,
			C_ACTION_D,

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

	extern btf32 mouse_x;
	extern btf32 mouse_y;
	extern btf32 joy_x_a;
	extern btf32 joy_y_a;
	extern btf32 joy_x_b;
	extern btf32 joy_y_b;

	void Init();

	void UpdateControllerInput();

	// Whenever the window size changed (by OS or user resize) this callback function executes
	void KeyCallback(GLFWwindow*, int, int, int, int);
	void CharCallback(GLFWwindow*, unsigned int);
	// Whenever the mouse moves, this callback is called
	void CursorCallback(GLFWwindow*, double, double);
	void MouseButtonCallback(GLFWwindow*, int, int, int);
	// Whenever the mouse scroll wheel scrolls, this callback is called
	void ScrollCallback(GLFWwindow*, double, double);

	// Clear hit inputs and mouse delta
	void ClearHitsAndDelta();
	// Clear all inputs and mouse delta
	void ClearAll();

	bool GetHeld(key::Key2 KEY);
	bool GetHit(key::Key2 KEY);
	void SetTo(key::Key2 KEY, bool VALUE);
	void Set(key::Key2 KEY);
	void Unset(key::Key2 KEY);
}