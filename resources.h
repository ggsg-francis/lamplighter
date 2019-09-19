#pragma once

#include "graphics.hpp"

namespace res
{
	// This is designed as a head start towards using an indexed asset system
	// I'd like to say it won't make it into the final game, but I'm probably much too lazy to replace it.
	// ONLY CONTAINS ASSET IDS DIRECTLY REFERENCED IN THE CODE, THIS LIST SHOULD GET SMALLER OVER TIME
	// AND REQUIRE LESS MAINTENANCE AS THE ARCHIVE IS IMPROVED UPON
	enum assetid : btui8 // Maintain with 0000gameassets
	{
		// Debug stuff
		m_debug_bb, // good name
		t_debug_bb, // good name
		m_debugcell,
		// Compass
		m_compass, // good name
		t_compass, // good name
		// Shadow
		m_shadow, // good name
		t_shadow, // good name
		// sky
		m_skydome, // good name
		m_skystars, // good name
		t_sky, // role is indeterminate so name is vague
		m_skymoon, // good name
		t_skymoon, // good name
		// terrain
		m_terrain_near, // good name
		m_terrain_oob, // good name
		// Chara textures
		t_skin1,
		t_skin2,
		t_equip_atlas,
		// Chara models
		mb_legs,
		mb_legsrun,
		mb_armscast,
		mb_head,
		// Equipment
		m_equip_head_pickers, // good name
		mb_equip_body_pickers_step, // good name
		mb_equip_body_pickers_run, // good name
		// Matchlock gun
		m_item_matchlock_01, // good name
		mb_item_matchlock_01_lever, // good name
		mb_item_matchlock_01_rod, // good name
		mb_item_matchlock_01_pan, // good name
		mb_item_matchlock_01_pan_full, // good name
		t_item_matchlock_01, // good name
		// Other shit
		m_proj,
		t_proj,
		mb_smoke_trail_segment, // good name
		t_smoke_trail, // good name
		m_kitfloor,
		t_terrain_sanddirt, // good name
		t_terrain_scorch, // good name
		// Gui stuff
		t_gui_crosshair, // good name
		t_gui_font, // good name
		t_gui_bar_red, // good name
		t_gui_bar_yellow, // good name
		t_gui_box, // good name
		t_gui_select_box, // good name
		t_gui_inv_slot, // good name
	};

	enum aniarr : btui8
	{
		a_idle,
		a_step_l,
		a_step_r,
		a_run_l,
		a_run_r,
		a_step_back_l,
		a_step_back_r,
		a_knockback,
		a_castmgc,
		aniarr_size, // hehe, clever huh?
	};

	enum aniflag : btui8
	{
		eNO_FLAG = 0ui8,
		eFLIP_LR = 1ui8,
		eCAN_TURN = 1ui8 << 1ui8, // can i turn independently of what's baked into the anim?
		eCAST_PROJECTILE = 1ui8 << 2ui8, // Cast a projectile this step
		eINFLICT_KNOCKBACK = 1ui8 << 3ui8, // If I bump into another character during this animation, they'll be knocked back
		eCAN_CANCEL = 1ui8 << 4ui8, // Can exit the animation at any time
		eRETURN_TO_IDLE = 1ui8 << 5ui8, // Return to idle animation when this animation has ended
		eSMEAR = 1ui8 << 6ui8, // Smear this frame
		eEMPTY = 1ui8 << 7ui8,
	};

	struct AnimStep
	{
		btf32 time = 0.f; // how long the animation goes
		btf32 blend_start = 0.f; // 0 to 1
		btf32 blend_end = 0.f; // 0 to 1
		btf32 height_start = 0.f;
		btf32 height_end = 0.f;
		btf32 mov_offs = 0.f; // Forward move offset
		btf32 rot_offs = 0.f; // Rotation offset
		aniflag flags;
		AnimStep(){};
		AnimStep(btf32 _sv, btf32 _ev, btf32 _time, btf32 _offset, btf32 _rot_offs, btf32 _height_start, btf32 _height_end, aniflag _flags)
		{
			time = _time;
			blend_start = _sv; blend_end = _ev;
			height_start = _height_start;
			height_end = _height_end;
			mov_offs = _offset;
			rot_offs = _rot_offs;
			flags = _flags;
		}
		bool Flag(aniflag flag)
		{
			return (flags & flag) != eNO_FLAG;
		}
	};

	struct Animation
	{
		AnimStep* steps = nullptr;
		btui8 size = 0ui8;
		Animation();
		~Animation();
		void AddStep(AnimStep step);
	};

	struct AnimMeshSet
	{
		assetid models[aniarr_size];
	};

	extern Animation animations[res::aniarr_size];
	extern AnimMeshSet amset_legs;
	extern AnimMeshSet amset_arms;
	extern AnimMeshSet amset_cloak;
	extern btf32 anim_curve[16];

	graphics::Texture& GetBTT(btui32 index);
	graphics::Mesh& GetBTM(btui32 index);
	graphics::MeshBlend& GetBTMB(btui32 index);

	void Init();
	void End();
}