#include "input.h"
#include "memory.hpp"
#include <GLFW\glfw3.h>

namespace input
{
	namespace ki
	{
		enum key_intermediate : btui8 // saves space
		{
			none,
			quit,
			fw,
			bk,
			lt,
			rt,
			atk,
			sprint,
			activate,
			action_a,
			action_b,
			action_c,
			action_d,
			key_intermediate_count,
		};
	}

	ki::key_intermediate ScancodeTransfer[256]{ ki::none }; // uses btui8 to save space
	key::modkey KeyTransferPress[ki::key_intermediate_count]{ key::set_none };
	key::modkey KeyTransferRelease[ki::key_intermediate_count]{ key::set_none };

	void Init()
	{
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_ESCAPE)] = ki::quit;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_W)] = ki::fw;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_S)] = ki::bk;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_D)] = ki::rt;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_A)] = ki::lt;
		//ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_SPACE)] = ki::atk;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_LEFT_SHIFT)] = ki::sprint;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_E)] = ki::activate;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_Q)] = ki::action_a;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_1)] = ki::action_b;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_2)] = ki::action_c;
		ScancodeTransfer[glfwGetKeyScancode(GLFW_KEY_3)] = ki::action_d;

		KeyTransferPress[ki::none] = key::set_none;
		KeyTransferPress[ki::quit] = key::set_quit;
		KeyTransferPress[ki::fw] = key::eSET_DIR_F;
		KeyTransferPress[ki::bk] = key::eSET_DIR_B;
		KeyTransferPress[ki::lt] = key::eSET_DIR_L;
		KeyTransferPress[ki::rt] = key::eSET_DIR_R;
		KeyTransferPress[ki::atk] = key::eSET_USE;
		KeyTransferPress[ki::sprint] = key::eSET_RUN;
		KeyTransferPress[ki::activate] = key::eSET_ACTIVATE;
		KeyTransferPress[ki::action_a] = key::eSET_ACTION_A;
		KeyTransferPress[ki::action_b] = key::eSET_ACTION_B;
		KeyTransferPress[ki::action_c] = key::eSET_ACTION_C;
		KeyTransferPress[ki::action_d] = key::eSET_ACTION_D;

		KeyTransferRelease[ki::none] = key::set_none;
		KeyTransferRelease[ki::quit] = key::set_none;
		KeyTransferRelease[ki::fw] = key::eUNS_DIR_F;
		KeyTransferRelease[ki::bk] = key::eUNS_DIR_B;
		KeyTransferRelease[ki::lt] = key::eUNS_DIR_L;
		KeyTransferRelease[ki::rt] = key::eUNS_DIR_R;
		KeyTransferRelease[ki::atk] = key::eUNS_USE;
		KeyTransferRelease[ki::sprint] = key::eUNS_RUN;
		KeyTransferRelease[ki::activate] = key::eUNS_ACTIVATE;
		KeyTransferRelease[ki::action_a] = key::eUNS_ACTION_A;
		KeyTransferRelease[ki::action_b] = key::eUNS_ACTION_B;
		KeyTransferRelease[ki::action_c] = key::eUNS_ACTION_C;
		KeyTransferRelease[ki::action_d] = key::eUNS_ACTION_D;
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
			SetTo(key::set_c_sprint, state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS);
			if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS)
				Set(key::set_c_atk);
			else if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_RELEASE)
				Unset(key::unset_c_atk);

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
		if (action != GLFW_REPEAT && scancode < 256) // Check scancode size because numpad is too large for input array
		{
			if (ScancodeTransfer[scancode] != ki::none)
				if (action == GLFW_PRESS)
					Set(KeyTransferPress[ScancodeTransfer[scancode]]);
				else if (action == GLFW_RELEASE)
					Unset(KeyTransferRelease[ScancodeTransfer[scancode]]);
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
				Set(key::eSET_USE);
		if (action == GLFW_RELEASE)
			if (button == GLFW_MOUSE_BUTTON_LEFT)
				Unset(key::eUNS_USE);
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
		//Unset(key::unset_hits);
		Unset(key::unset_hits);
	}

	void ClearAll()
	{
		//mouse_x = 0.f;
		//mouse_y = 0.f;
		Unset(key::unset_all);
	}

	bool Get(key::key i)
	{
		return mem::bvget(flags2, (btui64)i);
	}
	void SetTo(key::modkey i, bool b)
	{
		mem::bvsetto(flags2, (btui64)i, b);
	}
	void Set(key::modkey i)
	{
		mem::bvset(flags2, (btui64)i);
	}
	void Unset(key::modkey i)
	{
		mem::bvunset(flags2, (btui64)i);
	}

	bool GetHeld(key::key_test i)
	{
		return mem::bvget(keyBitsHeld, (btui64)i);
	}
	bool GetHit(key::key_test i)
	{
		return mem::bvget(keyBitsHit, (btui64)i);
	}
	void SetTo(key::key_test i, bool b)
	{
		mem::bvsetto(keyBitsHeld, (btui64)i, b);
	}
	void Set(key::key_test i)
	{
		mem::bvset(keyBitsHeld, (btui64)i);
		mem::bvset(keyBitsHit, (btui64)i);
	}
	void Unset(key::key_test i)
	{
		mem::bvunset(keyBitsHeld, (btui64)i);
	}
}