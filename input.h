#pragma once

//union SDL_Event;
struct  GLFWwindow;

namespace input
{
	namespace key
	{
		enum key_editor : btui64
		{
			//-------------------------------- KEYBOARD PLAYER INPUT (SPACE FOR 12 HELD, 12 HIT)

			// 0 - 11
			EDITOR_DIR_F_HOLD = 1UI64,
			EDITOR_DIR_B_HOLD = 1UI64 << 1UI64,
			EDITOR_DIR_L_HOLD = 1UI64 << 2UI64,
			EDITOR_DIR_R_HOLD = 1UI64 << 3UI64,
			EDITOR_USE_HOLD = 1UI64 << 4UI64,
			EDITOR_RUN_HOLD = 1UI64 << 5UI64,
			EDITOR_ACTIVATE_HOLD = 1UI64 << 10UI64,
			EDITOR_ACTION_A_HOLD = 1UI64 << 6UI64,
			EDITOR_ACTION_B_HOLD = 1UI64 << 7UI64,
			EDITOR_ACTION_C_HOLD = 1UI64 << 8UI64,
			EDITOR_ACTION_D_HOLD = 1UI64 << 9UI64,

			// 12 - 23
			EDITOR_DIR_F_HIT = 1UI64 << 19UI64,
			EDITOR_DIR_B_HIT = 1UI64 << 20UI64,
			EDITOR_DIR_L_HIT = 1UI64 << 21UI64,
			EDITOR_DIR_R_HIT = 1UI64 << 22UI64,
			EDITOR_USE_HIT = 1UI64 << 12UI64,
			EDITOR_RUN_HIT = 1UI64 << 13UI64,
			EDITOR_ACTIVATE_HIT = 1UI64 << 14UI64,
			EDITOR_ACTION_A_HIT = 1UI64 << 15UI64,
			EDITOR_ACTION_B_HIT = 1UI64 << 16UI64,
			EDITOR_ACTION_C_HIT = 1UI64 << 17UI64,
			EDITOR_ACTION_D_HIT = 1UI64 << 18UI64,

			//-------------------------------- UTILITY HIT

			//48 - 63
			EDITOR_QUIT_HIT = 1UI64 << 48UI64,
			EDITOR_F1_HIT = 1UI64 << 49UI64,
			EDITOR_F2_HIT = 1UI64 << 50UI64,
			EDITOR_F3_HIT = 1UI64 << 51UI64,
			EDITOR_F4_HIT = 1UI64 << 52UI64,
			EDITOR_F5_HIT = 1UI64 << 53UI64,
			EDITOR_F6_HIT = 1UI64 << 54UI64,
			EDITOR_F7_HIT = 1UI64 << 55UI64,
			EDITOR_F8_HIT = 1UI64 << 56UI64,
			EDITOR_F9_HIT = 1UI64 << 57UI64,
			EDITOR_F0_HIT = 1UI64 << 58UI64,
			EEDITOR_SCR_UP_HIT = 1UI64 << 59UI64,
			EEDITOR_SCR_DN_HIT = 1UI64 << 60UI64,
		};
		enum key : btui64
		{
			//-------------------------------- KEYBOARD PLAYER INPUT (space for 12 held, 12 hit)

			// 0 - 11
			eDIR_F_HOLD = 1u,
			eDIR_B_HOLD = 1u << 1u,
			eDIR_L_HOLD = 1u << 2u,
			eDIR_R_HOLD = 1u << 3u,
			eUSE_HOLD = 1u << 4u, // Use equipped item
			eRUN_HOLD = 1u << 5u,
			eACTIVATE_HOLD = 1u << 10u, // Activate other actor
			eACTION_A_HOLD = 1u << 6u,
			eACTION_B_HOLD = 1u << 7u,
			eACTION_C_HOLD = 1u << 8u,
			eACTION_D_HOLD = 1u << 9u,

			// 12 - 23
			eDIR_F_HIT = 1u << 19u,
			eDIR_B_HIT = 1u << 20u,
			eDIR_L_HIT = 1u << 21u,
			eDIR_R_HIT = 1u << 22u,
			eUSE_HIT = 1u << 12u, // Use equipped item
			eRUN_HIT = 1u << 13u,
			eACTIVATE_HIT = 1u << 14u, // Activate other actor
			eACTION_A_HIT = 1u << 15u,
			eACTION_B_HIT = 1u << 16u,
			eACTION_C_HIT = 1u << 17u,
			eACTION_D_HIT = 1u << 18u,
			
			//-------------------------------- CONTROLLER PLAYER INPUT (space for 12 held, 12 hit)

			// 24 - 35
			c_fw_held = 1u << 24u, // are these directionals even needed?
			c_bk_held = 1u << 25u,
			c_lt_held = 1u << 26u,
			c_rt_held = 1u << 27u,
			c_atk_held = 1u << 28u,
			c_sprint_held = 1u << 29u,

			eC_DIR_F_HOLD_TEST = 1u << 30u,
			eC_DIR_B_HOLD_TEST = 1u << 31u,
			eC_DIR_L_HOLD_TEST = 1ui64 << 32ui64,
			eC_DIR_R_HOLD_TEST = 1ui64 << 33ui64,

			// 36 - 47
			c_atk_hit = 1ui64 << 36ui64,
			c_sprint_hit = 1ui64 << 37ui64,
			c_activate_hit = 1ui64 << 38ui64,
			c_action_a_hit = 1ui64 << 39ui64,
			c_action_b_hit = 1ui64 << 40ui64,
			c_action_c_hit = 1ui64 << 41ui64,
			c_action_d_hit = 1ui64 << 42ui64,

			//-------------------------------- UTILITY HIT

			//48 - 63
			quit_hit = 1ui64 << 48ui64,
			f1_hit = 1ui64 << 49ui64,
			f2_hit = 1ui64 << 50ui64,
			f3_hit = 1ui64 << 51ui64,
			f4_hit = 1ui64 << 52ui64,
			f5_hit = 1ui64 << 53ui64,
			f6_hit = 1ui64 << 54ui64,
			f7_hit = 1ui64 << 55ui64,
			f8_hit = 1ui64 << 56ui64,
			f9_hit = 1ui64 << 57ui64,
			f0_hit = 1ui64 << 58ui64,
		};
		enum modkey : btui64
		{
			set_none = 0ui64,

			//-------------------------------- KEYBOARD SETS

			eSET_DIR_F = eDIR_F_HOLD | eDIR_F_HIT,
			eUNS_DIR_F = eDIR_F_HOLD,
			eSET_DIR_B = eDIR_B_HOLD | eDIR_B_HIT,
			eUNS_DIR_B = eDIR_B_HOLD,
			eSET_DIR_L = eDIR_L_HOLD | eDIR_L_HIT,
			eUNS_DIR_L = eDIR_L_HOLD,
			eSET_DIR_R = eDIR_R_HOLD | eDIR_R_HIT,
			eUNS_DIR_R = eDIR_R_HOLD,
			eSET_USE = eUSE_HOLD | eUSE_HIT,
			eUNS_USE = eUSE_HOLD,
			eSET_RUN = eRUN_HOLD | eRUN_HIT,
			eUNS_RUN = eRUN_HOLD,
			eSET_ACTIVATE = eACTIVATE_HOLD | eACTIVATE_HIT,
			eUNS_ACTIVATE = eACTIVATE_HOLD,
			eSET_ACTION_A = eACTION_A_HIT | eACTION_A_HOLD,
			eUNS_ACTION_A = eACTION_A_HOLD,
			eSET_ACTION_B = eACTION_B_HIT | eACTION_A_HOLD,
			eUNS_ACTION_B = eACTION_A_HOLD,
			eSET_ACTION_C = eACTION_C_HIT | eACTION_A_HOLD,
			eUNS_ACTION_C = eACTION_A_HOLD,
			eSET_ACTION_D = eACTION_D_HIT | eACTION_A_HOLD,
			eUNS_ACTION_D = eACTION_A_HOLD,

			//-------------------------------- CONTROLLER SETS

			set_c_atk = c_atk_held | c_atk_hit,
			unset_c_atk = c_atk_held,
			set_c_sprint = c_sprint_held | c_sprint_hit,
			unset_c_sprint = c_sprint_held,
			set_c_activate = c_activate_hit,
			set_c_action_a = c_action_a_hit,
			set_c_action_b = c_action_b_hit,
			set_c_action_c = c_action_c_hit,
			set_c_action_d = c_action_d_hit,

			//-------------------------------- CONTROLLER HAT

			hat_all = c_fw_held | c_bk_held | c_rt_held | c_lt_held,
			hat_u = c_fw_held,
			hat_d = c_bk_held,
			hat_r = c_rt_held,
			hat_l = c_lt_held,
			hat_ru = c_fw_held | c_rt_held,
			hat_rd = c_bk_held | c_lt_held,
			hat_lu = c_fw_held | c_lt_held,
			hat_ld = c_bk_held | c_rt_held,

			//-------------------------------- UTILITY SETS

			set_quit = quit_hit,
			set_f1 = f1_hit,
			set_f2 = f2_hit,
			set_f3 = f3_hit,
			set_f4 = f4_hit,
			set_f5 = f5_hit,
			set_f6 = f6_hit,
			set_f7 = f7_hit,
			set_f8 = f8_hit,
			set_f9 = f9_hit,
			set_f0 = f0_hit,

			// Zones       |Utility        | P2 Hit    | P2 Held   | P1 Hit    | P1 Held
			unset_hits = 0b1111111111111111111111111111000000000000111111111111000000000000, // Every hit input
			unset_util = 0b1111111111111111111111111111111100000000000000000000000000000000, // Every utility input
			unset_all  = 0b1111111111111111111111111111111111111111111111111111111111111111, // Every input
		};
		enum key_test : btui64
		{
			//-------------------------------- KEYBOARD PLAYER INPUT (space for 12 held, 12 hit)

			// 0 - 23
			EDIR_F = 1U << 19U,
			EDIR_B = 1U << 20U,
			EDIR_L = 1U << 21U,
			EDIR_R = 1U << 22U,
			EUSE = 1U << 12U, // Use equipped item
			ERUN = 1U << 13U,
			EACTIVATE = 1U << 14U, // Activate other actor
			EACTION_A = 1U << 15U,
			EACTION_B = 1U << 16U,
			EACTION_C = 1U << 17U,
			EACTION_D = 1U << 18U,

			//-------------------------------- CONTROLLER PLAYER INPUT (SPACE FOR 12 HELD, 12 HIT)

			// 24 - 47
			C_FW = 1U << 24U, // Are these directionals even needed?
			C_BK = 1U << 25U,
			C_LT = 1U << 26U,
			C_RT = 1U << 27U,
			C_ATK = 1U << 28U,
			C_SPRINT = 1U << 29U,
			C_ACTIVATE = 1UI64 << 38UI64,
			C_ACTION_A = 1UI64 << 39UI64,
			C_ACTION_B = 1UI64 << 40UI64,
			C_ACTION_C = 1UI64 << 41UI64,
			C_ACTION_D = 1UI64 << 42UI64,

			//-------------------------------- UTILITY HIT

			//48 - 63
			QUIT = 1UI64 << 48UI64,
			F1 = 1UI64 << 49UI64,
			F2 = 1UI64 << 50UI64,
			F3 = 1UI64 << 51UI64,
			F4 = 1UI64 << 52UI64,
			F5 = 1UI64 << 53UI64,
			F6 = 1UI64 << 54UI64,
			F7 = 1UI64 << 55UI64,
			F8 = 1UI64 << 56UI64,
			F9 = 1UI64 << 57UI64,
			F0 = 1UI64 << 58UI64,

			/*
			
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

			C_DIR_F, // Are these directionals even needed?
			C_DIR_B,
			C_DIR_L,
			C_DIR_R,
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
			FUNCTION_0,

			*/
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

	bool Get(key::key MASK);
	void SetTo(key::modkey MASK, bool VALUE);
	void Set(key::modkey MASK);
	void Unset(key::modkey MASK);

	bool GetHeld(key::key_test KEY);
	bool GetHit(key::key_test KEY);
	void SetTo(key::key_test KEY, bool VALUE);
	void Set(key::key_test KEY);
	void Unset(key::key_test KEY);
}