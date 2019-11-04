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
		ScancodeTransfer[scancode::Q] = key::ACTION_A;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_1)] = key::ACTION_B;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_2)] = key::ACTION_C;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_3)] = key::ACTION_D;
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
			if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS)
				Set(key::C_USE);
			else if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_RELEASE)
				Unset(key::C_USE);

			#define JOY_DEADZONE 0.05F

			int count;
			const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_5, &count);
			if (abs(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X]) > JOY_DEADZONE)
				joy_x_a = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
			if (abs(state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]) > JOY_DEADZONE)
				joy_y_a = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

			if (abs(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X]) > JOY_DEADZONE)
				joy_x_b = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
			if (abs(state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]) > JOY_DEADZONE)
				joy_y_b = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
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
		if (action == GLFW_RELEASE)
			if (button == GLFW_MOUSE_BUTTON_LEFT)
				Unset(key::USE);
	}

	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		//camera.ProcessMouseScroll(yoffset);
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
	void SetTo(key::Key2 i, bool b)
	{
		mem::bvsetto(keyBitsHeld, 1ui64 << (btui64)i, b);
		mem::bvsetto(keyBitsHit, 1ui64 << (btui64)i, b);
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
}