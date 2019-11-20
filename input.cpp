#include "input.h"
#include "memory.hpp"
#include <GLFW\glfw3.h>

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

	key::Key2 ScancodeTransfer[512]{ key::NONE }; // uses btui8 to save space

	void Init()
	{
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_ESCAPE)] = key::QUIT;
		ScancodeTransfer[scancode::W] = key::DIR_F;
		ScancodeTransfer[scancode::S] = key::DIR_B;
		ScancodeTransfer[scancode::D] = key::DIR_R;
		ScancodeTransfer[scancode::A] = key::DIR_L;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_LEFT_SHIFT)] = key::RUN;
		ScancodeTransfer[scancode::E] = key::ACTIVATE;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_1)] = key::ACTION_A;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_2)] = key::ACTION_B;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_3)] = key::ACTION_C;
		ScancodeTransfer[scancode::R] = key::DROP_HELD;
		ScancodeTransfer[scancode::Z] = key::INV_CYCLE_L;
		ScancodeTransfer[scancode::X] = key::INV_CYCLE_R;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_F1)] = key::FUNCTION_1;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_F2)] = key::FUNCTION_2;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_F3)] = key::FUNCTION_3;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_F4)] = key::FUNCTION_4;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_F5)] = key::FUNCTION_5;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_F6)] = key::FUNCTION_6;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_F7)] = key::FUNCTION_7;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_F8)] = key::FUNCTION_8;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_F9)] = key::FUNCTION_9;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_F10)] = key::FUNCTION_10;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_F11)] = key::FUNCTION_11;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_F12)] = key::FUNCTION_12;
	}

	namespace rawinput
	{
		enum e : btui8
		{
			joy_bumper_l = 4u,
			joy_bumper_r = 5u,
		};
	}

	btui64 flags2 = 0ui64;

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

	GLFWgamepadstate state;

	/*
	else if (e.type == SDL_JOYHATMOTION)
	{
	//neutral
	if (e.jhat.value == SDL_HAT_CENTERED)
	{
	Unset(key::hat_all);
	}
	//normal
	else if (e.jhat.value == SDL_HAT_UP)
	{
	Unset(key::hat_all);
	Set(key::hat_u);
	}
	else if (e.jhat.value == SDL_HAT_DOWN)
	{
	Unset(key::hat_all);
	Set(key::hat_d);
	}
	else if (e.jhat.value == SDL_HAT_RIGHT)
	{
	Unset(key::hat_all);
	Set(key::hat_r);
	}
	else if (e.jhat.value == SDL_HAT_LEFT)
	{
	Unset(key::hat_all);
	Set(key::hat_l);
	}
	//diagonal
	else if (e.jhat.value == SDL_HAT_RIGHTUP)
	{
	Unset(key::hat_all);
	Set(key::hat_ru);
	}
	else if (e.jhat.value == SDL_HAT_LEFTDOWN)
	{
	Unset(key::hat_all);
	Set(key::hat_ld);
	}
	else if (e.jhat.value == SDL_HAT_RIGHTDOWN)
	{
	Unset(key::hat_all);
	Set(key::hat_rd);
	}
	else if (e.jhat.value == SDL_HAT_LEFTUP)
	{
	Unset(key::hat_all);
	Set(key::hat_lu);
	}
	}
	*/

	void UpdateControllerInput()
	{
		if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1) && glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
		{
			SetTo(key::C_RUN, state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS);

			if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS && !GetHeld(key::C_USE))
				Set(key::C_USE);
			else if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_RELEASE)
				Unset(key::C_USE);

			if (state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS && !GetHeld(key::C_USE_ALT))
				Set(key::C_USE_ALT);
			else if (state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_RELEASE)
				Unset(key::C_USE_ALT);

			if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_PRESS && !GetHeld(key::C_INV_CYCLE_L))
				Set(key::C_INV_CYCLE_L);
			else if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT] == GLFW_RELEASE)
				Unset(key::C_INV_CYCLE_L);

			if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_PRESS && !GetHeld(key::C_INV_CYCLE_R))
				Set(key::C_INV_CYCLE_R);
			else if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT] == GLFW_RELEASE)
				Unset(key::C_INV_CYCLE_R);

			if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS && !GetHeld(key::C_ACTIVATE))
				Set(key::C_ACTIVATE);
			else if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_RELEASE)
				Unset(key::C_ACTIVATE);

			if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS && !GetHeld(key::C_DROP_HELD))
				Set(key::C_DROP_HELD);
			else if (state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_RELEASE)
				Unset(key::C_DROP_HELD);

			if (state.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS && !GetHeld(key::C_ACTION_A))
				Set(key::C_ACTION_A);
			else if (state.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_RELEASE)
				Unset(key::C_ACTION_A);

			if (state.buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS && !GetHeld(key::C_ACTION_B))
				Set(key::C_ACTION_B);
			else if (state.buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_RELEASE)
				Unset(key::C_ACTION_B);

			if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS && !GetHeld(key::C_ACTION_C))
				Set(key::C_ACTION_C);
			else if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_RELEASE)
				Unset(key::C_ACTION_C);

			#define JOY_DEADZONE 0.06F
			#define JOY_MULT (1.0F / (1.0F - JOY_DEADZONE))

			int count;
			const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_5, &count);
			/*
			btf32 length_left = sqrt(
				state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] *
				state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] +
				state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] *
				state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);
			if (length_left > JOY_DEADZONE)
			{
				// Left X
				if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] > 0.f)
					joy_x_a = (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] - JOY_DEADZONE) * JOY_MULT;
				else if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] < -0.f)
					joy_x_a = (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] + JOY_DEADZONE) * JOY_MULT;
				// Left Y
				if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] > 0.f)
					joy_y_a = (state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] - JOY_DEADZONE) * JOY_MULT;
				else if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] < -0.f)
					joy_y_a = (state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] + JOY_DEADZONE) * JOY_MULT;
			}
			btf32 length_right = sqrt(
				state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] *
				state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] +
				state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] *
				state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]);
			if (length_right > JOY_DEADZONE)
			{
				// Right X
				if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] > 0.f)
					joy_x_b = (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] - JOY_DEADZONE) * JOY_MULT;
				else if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] < -0.f)
					joy_x_b = (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] + JOY_DEADZONE) * JOY_MULT;
				// Right Y
				if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] > 0.f)
					joy_y_b = (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] - JOY_DEADZONE) * JOY_MULT;
				else if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] < -0.f)
					joy_y_b = (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] + JOY_DEADZONE) * JOY_MULT;
			}
			*/
			// Left X
			if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] > JOY_DEADZONE)
				joy_x_a = (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] * JOY_MULT) - JOY_DEADZONE;
			else if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] < -JOY_DEADZONE)
				joy_x_a = (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] * JOY_MULT) + JOY_DEADZONE;
			// Left Y
			if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] > JOY_DEADZONE)
				joy_y_a = (state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] * JOY_MULT) - JOY_DEADZONE;
			else if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] < -JOY_DEADZONE)
				joy_y_a = (state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] * JOY_MULT) + JOY_DEADZONE;
			// Right X
			if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] > JOY_DEADZONE)
				joy_x_b = (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] * JOY_MULT) - JOY_DEADZONE;
			else if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] < -JOY_DEADZONE)
				joy_x_b = (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] * JOY_MULT) + JOY_DEADZONE;
			// Right Y
			if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] > JOY_DEADZONE)
				joy_y_b = (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] * JOY_MULT) - JOY_DEADZONE;
			else if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] < -JOY_DEADZONE)
				joy_y_b = (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] * JOY_MULT) + JOY_DEADZONE;

			#undef JOY_DEADZONE
			#undef JOY_MULT
		}
	}

	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		if (action != GLFW_REPEAT) // Check scancode size because numpad is too large for input array
		{
			if (ScancodeTransfer[scancode] != key::NONE)
				if (action == GLFW_PRESS)
					Set(ScancodeTransfer[scancode]);
				else if (action == GLFW_RELEASE)
					Unset(ScancodeTransfer[scancode]);
		}
	}

	void CharCallback(GLFWwindow* window, unsigned int codepoint)
	{

	}

	void CursorCallback(GLFWwindow* window, double xpos, double ypos)
	{
		// Calculate mouse delta
		mouse_x += (float)xpos - mouse_last_x;
		mouse_y += (float)ypos - mouse_last_y;
		// Set last positions for calculating next frame's delta
		mouse_last_x = (float)xpos;
		mouse_last_y = (float)ypos;
	}

	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
	{
		if (action == GLFW_PRESS)
			if (button == GLFW_MOUSE_BUTTON_LEFT)
				Set(key::USE);
			else if (button == GLFW_MOUSE_BUTTON_RIGHT)
				Set(key::USE_ALT);
		if (action == GLFW_RELEASE)
			if (button == GLFW_MOUSE_BUTTON_LEFT)
				Unset(key::USE);
			else if (button == GLFW_MOUSE_BUTTON_RIGHT)
				Unset(key::USE_ALT);
	}

	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (yoffset < -0.5f) SetHit(key::INV_CYCLE_L);
		else if (yoffset > 0.5f) SetHit(key::INV_CYCLE_R);
	}

	void ClearHitsAndDelta()
	{
		mouse_x = 0.f;
		mouse_y = 0.f;
		joy_x_a = 0.f;
		joy_y_a = 0.f;
		joy_x_b = 0.f;
		joy_y_b = 0.f;
		keyBitsHit = 0b0000000000000000000000000000000000000000000000000000000000000000UI64;
	}

	void ClearAll()
	{
		//mouse_x = 0.f;
		//mouse_y = 0.f;
		keyBitsHit = 0b0000000000000000000000000000000000000000000000000000000000000000UI64;
		keyBitsHeld = 0b0000000000000000000000000000000000000000000000000000000000000000UI64;
	}

	bool GetHeld(key::Key2 i)
	{
		return mem::bvget(keyBitsHeld, 1ui64 << (btui64)i);
	}
	bool GetHit(key::Key2 i)
	{
		return mem::bvget(keyBitsHit, 1ui64 << (btui64)i);
	}
	void Set(key::Key2 i)
	{
		mem::bvset(keyBitsHeld, 1ui64 << (btui64)i);
		mem::bvset(keyBitsHit, 1ui64 << (btui64)i);
	}
	void Unset(key::Key2 i)
	{
		mem::bvunset(keyBitsHeld, 1ui64 << (btui64)i);
	}
	void SetTo(key::Key2 i, bool b)
	{
		mem::bvsetto(keyBitsHeld, 1ui64 << (btui64)i, b);
		mem::bvsetto(keyBitsHit, 1ui64 << (btui64)i, b);
	}
	void SetHit(key::Key2 i)
	{
		mem::bvset(keyBitsHit, 1ui64 << (btui64)i);
	}
}