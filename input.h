#pragma once

union SDL_Event;

namespace input
{
	namespace key
	{
		#ifdef DEF_EDITOR
		/*
		enum key2 : btui64
		{
			// If a button is held, use these
			// player 1 bindings
			//fw = (0x1 << 0x0),
			fw_held = ((btui64)0x1 << (btui64)0x38),
			bk_held = (0x1 << 0x1),
			lt_held = (0x1 << 0x2),
			rt_held = (0x1 << 0x3),
			atk_held = (0x1 << 0x4),
			sprint_held = (0x1 << 0x5),
			//activate = (0x1 << 0x6),
			caml = (0x1 << 0x7),
			aim = (0x1 << 0x8),
			// additional buttons
			quit = (0x1 << 0xf),

			// For reading if an input is hit this frame
			fw_hit = ((btui64)0x1 << (btui64)0x20),
			bk_hit = ((btui64)0x1 << (btui64)0x21),
			lt_hit = ((btui64)0x1 << (btui64)0x22),
			rt_hit = ((btui64)0x1 << (btui64)0x23),
			atk_hit = ((btui64)0x1 << (btui64)0x24),
			sprint_hit = ((btui64)0x1 << (btui64)0x25),
			activate_hit = ((btui64)0x1 << (btui64)0x26),
			camlhit = ((btui64)0x1 << (btui64)0x27),
		};*/
		#else

		#endif
		enum key : btui64
		{
			//******************************** KEYBOARD INPUT HOLD

			fw_held = (0x1 << 0x0),
			//fw_held = ((btui64)0x1 << (btui64)0x38),
			bk_held = (0x1 << 0x1),
			lt_held = (0x1 << 0x2),
			rt_held = (0x1 << 0x3),
			atk_held = (0x1 << 0x4),
			sprint_held = (0x1 << 0x5),
			aim_held = (0x1 << 0x6),
			// additional buttons
			quit = (0x1 << 0xf),
			
			//******************************** CONTROLLER INPUT HOLD

			fw_held_c = (0x1 << 0x10),
			bk_held_c = (0x1 << 0x11),
			lt_held_c = (0x1 << 0x12),
			rt_held_c = (0x1 << 0x13),
			atkb = (0x1 << 0x14),
			c_sprint_held = (0x1 << 0x15),
			camrb = (0x1 << 0x16),
			camlb = (0x1 << 0x17),

			//******************************** KEYBOARD INPUT HIT (32 ONWARDS)

			fw_hit = ((btui64)0x1 << (btui64)0x20),
			bk_hit = ((btui64)0x1 << (btui64)0x21),
			lt_hit = ((btui64)0x1 << (btui64)0x22),
			rt_hit = ((btui64)0x1 << (btui64)0x23),
			atk_hit = ((btui64)0x1 << (btui64)0x24),
			sprint_hit = ((btui64)0x1 << (btui64)0x25),
			activate_hit = ((btui64)0x1 << (btui64)0x26),
			action_a_hit = ((btui64)0x1 << (btui64)0x27),
			action_b_hit = ((btui64)0x1 << (btui64)0x28),
			action_c_hit = ((btui64)0x1 << (btui64)0x29),
			action_d_hit = ((btui64)0x1 << (btui64)0x2a),

			//******************************** CONTROLLER INPUT HIT

			fwhitb = ((btui64)0x1 << (btui64)0x30),
			bkhitb = ((btui64)0x1 << (btui64)0x31),
			lthitb = ((btui64)0x1 << (btui64)0x32),
			rthitb = ((btui64)0x1 << (btui64)0x33),
			atkhitb = ((btui64)0x1 << (btui64)0x34),
			c_sprint_hit = ((btui64)0x1 << (btui64)0x35),
			camrhitb = ((btui64)0x1 << (btui64)0x36),
			camlhitb = ((btui64)0x1 << (btui64)0x37),
		};
		enum modkey : btui64
		{
			//utility stuff
			set_quit = quit,

			// For setting inputs for the first frame (on key down)
			set_fw = fw_held | fw_hit,
			set_bk = bk_held | bk_hit,
			set_lt = lt_held | lt_hit,
			set_rt = rt_held | rt_hit,
			set_atk = atk_held | atk_hit,
			set_sprint = sprint_held | sprint_hit,
			set_aim = aim_held,
			set_activate = activate_hit,
			set_action_a = action_a_hit,
			set_action_b = action_b_hit,
			set_action_c = action_c_hit,
			set_action_d = action_d_hit,
			// Same, but unset. Arguably a waste of space, but it prevents me from making any stupid mistakes
			unset_fw = fw_held,
			unset_bk = bk_held,
			unset_lt = lt_held,
			unset_rt = rt_held,
			unset_atk = atk_held,
			unset_sprint = sprint_held,
			//unset_activate = activate,
			unset_aim = aim_held,
			// For setting inputs for the first frame (on key down)
			set_fwb = fw_held_c | fwhitb,
			set_bkb = bk_held_c | bkhitb,
			set_ltb = lt_held_c | lthitb,
			set_rtb = rt_held_c | rthitb,
			set_atkb = atkb | atkhitb,
			set_c_sprint = c_sprint_held | c_sprint_hit,
			set_camlb = camlb | camlhitb,
			set_camrb = camrb | camrhitb,
			// Same, but unset. Arguably a waste of space, but it prevents me from making any stupid mistakes
			unset_fwb = fw_held_c,
			unset_bkb = bk_held_c,
			unset_ltb = lt_held_c,
			unset_rtb = rt_held_c,
			unset_atkb = atkb,
			unset_focusb = c_sprint_held,
			unset_camlb = camlb,
			unset_camrb = camrb,
			//hat set
			hat_all = fw_held_c | bk_held_c | rt_held_c | lt_held_c,
			hat_u = fw_held_c,
			hat_d = bk_held_c,
			hat_r = rt_held_c,
			hat_l = lt_held_c,
			hat_ru = fw_held_c | rt_held_c,
			hat_rd = bk_held_c | lt_held_c,
			hat_lu = fw_held_c | lt_held_c,
			hat_ld = bk_held_c | rt_held_c,
			// Utility unsets
			unset_hits = 0b1111111111111111111111111111111100000000000000000000000000000000, // Every hit input
			unset_all  = 0b1111111111111111111111111111111111111111111111111111111111111111, // Every input
		};
	}

	extern btf32 mouse_x;
	extern btf32 mouse_y;
	extern btf32 joy_x_a;
	extern btf32 joy_y_a;
	extern btf32 joy_x_b;
	extern btf32 joy_y_b;

	void UpdateInput(SDL_Event& e);
	void ResetInput();

	bool Get(key::key ADDR);
	void Set(key::modkey ADDR);
	void Unset(key::modkey ADDR);
}