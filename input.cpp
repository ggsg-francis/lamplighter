#include "input.h"
#include "memory.hpp"
#include <SDL2\SDL.h>

namespace input
{

	namespace rawinput
	{
		enum e : btui8
		{
			joy_bumper_l = 4u,
			joy_bumper_r = 5u,
		};
	}

	btui64 flags2 = 0ui64;

	btf32 mouse_x;
	btf32 mouse_y;
	btf32 joy_x_a;
	btf32 joy_y_a;
	btf32 joy_x_b;
	btf32 joy_y_b;

	void UpdateInput(SDL_Event& e)
	{
		//If user clicks the mouse
			if (e.type == SDL_MOUSEBUTTONDOWN) {
				if (e.button.which == 1)
					Set(key::set_aim);
			}
			else if (e.type == SDL_MOUSEBUTTONUP) {
				if (e.button.which == 1)
					Unset(key::set_aim);
			}
			else if (e.type == SDL_MOUSEMOTION) // Mouse Motion
			{
				mouse_x += e.motion.xrel;
				mouse_y += e.motion.yrel;
			}
			else if (e.type == SDL_JOYAXISMOTION) // Joystick Motion
			{
				if ((e.jaxis.value < -3200) || (e.jaxis.value > 3200))
				{
					if (e.jaxis.axis == 0) // Left-right left stick
						joy_x_a = (btf32)e.jaxis.value / (btf32)32768.f;
					else if (e.jaxis.axis == 1) // Up-Down left stick
						joy_y_a = (btf32)e.jaxis.value / (btf32)32768.f;
					else if (e.jaxis.axis == 3) // Left-right right stick
						joy_x_b = (btf32)e.jaxis.value / (btf32)32768.f;
					else if (e.jaxis.axis == 4) // Up-Down right stick
						joy_y_b = (btf32)e.jaxis.value / (btf32)32768.f;
				}
				else
				{
					if (e.jaxis.axis == 0) // Left-right left stick
						joy_x_a = 0.f;
					else if (e.jaxis.axis == 1) // Up-Down left stick
						joy_y_a = 0.f;
					else if (e.jaxis.axis == 3) // Left-right right stick
						joy_x_b = 0.f;
					else if (e.jaxis.axis == 4) // Up-Down right stick
						joy_y_b = 0.f;
				}
			}
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
			else if (e.type == SDL_JOYBUTTONDOWN)
			{
				if (e.jbutton.button == rawinput::joy_bumper_l)
					Set(key::set_camlb);
				if (e.jbutton.button == rawinput::joy_bumper_r)
					Set(key::set_camrb);

				if (e.jbutton.button == 1) // Face button B
					Set(key::set_c_sprint);
				//Set(key::set_atkb);
			}
			else if (e.type == SDL_JOYBUTTONUP)
			{
				if (e.jbutton.button == rawinput::joy_bumper_l)
					Unset(key::unset_camlb);
				if (e.jbutton.button == rawinput::joy_bumper_r)
					Unset(key::unset_camrb);
				
				if (e.jbutton.button == 1)
					Unset(key::set_c_sprint);
				//Unset(key::set_atkb);
			}
			else if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
			{
				if (e.key.keysym.sym == SDLK_ESCAPE)
					Set(key::set_quit);
				//WASD
				if (e.key.keysym.sym == SDLK_w)
					Set(key::set_fw);
				else if (e.key.keysym.sym == SDLK_s)
					Set(key::set_bk);
				else if (e.key.keysym.sym == SDLK_d)
					Set(key::set_rt);
				else if (e.key.keysym.sym == SDLK_a)
					Set(key::set_lt);
				else if (e.key.keysym.sym == SDLK_SPACE)
					Set(key::set_atk);
				else if (e.key.keysym.sym == SDLK_LSHIFT)
					Set(key::set_sprint);
				else if (e.key.keysym.sym == SDLK_e)
					Set(key::set_activate);
				else if (e.key.keysym.sym == SDLK_1)
					Set(key::set_action_a);
				else if (e.key.keysym.sym == SDLK_2)
					Set(key::set_action_b);
				else if (e.key.keysym.sym == SDLK_3)
					Set(key::set_action_c);
				else if (e.key.keysym.sym == SDLK_4)
					Set(key::set_action_d);
				//arrow keys
				if (e.key.keysym.sym == SDLK_UP)
					Set(key::set_fwb);
				else if (e.key.keysym.sym == SDLK_DOWN)
					Set(key::set_bkb);
				else if (e.key.keysym.sym == SDLK_RIGHT)
					Set(key::set_rtb);
				else if (e.key.keysym.sym == SDLK_LEFT)
					Set(key::set_ltb);
				else if (e.key.keysym.sym == SDLK_KP_ENTER)
					Set(key::set_atkb);
				else if (e.key.keysym.sym == SDLK_KP_0)
					Set(key::set_c_sprint);
			}
			else if (e.type == SDL_KEYUP && e.key.repeat == 0)
			{
				//WASD
				if (e.key.keysym.sym == SDLK_w)
					Unset(key::set_fw);
				else if (e.key.keysym.sym == SDLK_s)
					Unset(key::set_bk);
				else if (e.key.keysym.sym == SDLK_d)
					Unset(key::set_rt);
				else if (e.key.keysym.sym == SDLK_a)
					Unset(key::set_lt);
				else if (e.key.keysym.sym == SDLK_SPACE)
					Unset(key::set_atk);
				else if (e.key.keysym.sym == SDLK_LSHIFT)
					Unset(key::set_sprint);
				#ifdef DEF_CAM_ROTATE
				else if (e.key.keysym.sym == SDLK_e)
					Unset(key::set_activate);
				else if (e.key.keysym.sym == SDLK_q)
					Unset(key::set_action_a);
				#endif
				//arrow keys
				if (e.key.keysym.sym == SDLK_UP)
					Unset(key::set_fwb);
				else if (e.key.keysym.sym == SDLK_DOWN)
					Unset(key::set_bkb);
				else if (e.key.keysym.sym == SDLK_RIGHT)
					Unset(key::set_rtb);
				else if (e.key.keysym.sym == SDLK_LEFT)
					Unset(key::set_ltb);
				else if (e.key.keysym.sym == SDLK_KP_ENTER)
					Unset(key::set_atkb);
				else if (e.key.keysym.sym == SDLK_KP_0)
					Unset(key::set_c_sprint);
			}
			else if (e.type == SDL_MOUSEMOTION)
			{
				//mDeltaX += e.motion.xrel;
				//mDeltaY += e.motion.yrel;
			}
	}

	void ResetInput()
	{
		mouse_x = 0.f;
		mouse_y = 0.f;
		Unset(key::unset_hits);
	}

	bool Get(key::key i)
	{
		return mem::bvget(flags2, (btui64)i);
	}
	void Set(key::modkey i)
	{
		mem::bvset(flags2, (btui64)i);
	}
	void Unset(key::modkey i)
	{
		mem::bvunset(flags2, (btui64)i);
	}
}