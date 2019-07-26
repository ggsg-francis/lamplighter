#pragma once

#include "graphics.hpp"

namespace res
{
	extern float modelOffsets[6];
	extern float modelOffsets_turn[6];

	extern float footstep[6];
	extern float footstep_turn[6];

	// This is designed as a head start towards using an indexed asset system
	// I'd like to say it won't make it into the final game, but I'm probably
	// Much too lazy to replace it.
	enum assetid : btui32 // Maintain with 0000gameassets
	{
		// Compass
		m_compass,
		t_compass,
		// Debug stuff
		m_debugcell,
		// Shadow
		m_shadow,
		t_shadow,
		// Chara textures
		t_skin1,
		t_skin2,
		t_equip_atlas,
		// Chara models
		m_legs,
		m_legsrun,
		m_armscast,
		m_head,
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
		m_bb,
		t_bb,
		m_proj,
		t_proj,
		m_kitfloor,
		t_marshmud,
		t_guired,
	};

	enum anims
	{
		idle,
		step_l,
		step_r,
		run_l,
		run_r,
		step_back_l,
		step_back_r,
		knockback,
		aim,
		anim_count, // hehe, clever huh?
	};

	namespace f
	{
		enum stepflag : btui8
		{
			none					=	0b00000000,
			flip_lr					=	0b00000001,
			can_turn				=	0b00000010,
			cast_projectile			=	0b00000100,
			inflict_knockback		=	0b00001000,
			can_start_moving		=	0b00010000,
			empty2					=	0b00100000,
			empty3					=	0b01000000,
			empty4					=	0b10000000,
		};
	}

	struct AnimStep
	{
		btf32 time = 0.f; // how long the animation goes
		btf32 blend_start = 0.f; // 0 to 1
		btf32 blend_end = 0.f; // 0 to 1
		btf32 height_start = 0.f;
		btf32 height_end = 0.f;
		btf32 move_offset = 0.f;
		btf32 pivot_offset_s = 0.f;
		btf32 pivot_offset_e = 0.f;
		btf32 pivot_offset_lr = 0.f;
		f::stepflag flags;
		AnimStep(
			btf32 _sv, btf32 _ev,
			btf32 _time,
			btf32 _height_start,
			btf32 _height_end,
			btf32 _offset,
			btf32 _pivot_offset_s,
			btf32 _pivot_offset_e,
			btf32 _pivot_offset_lr,
			f::stepflag _flags
			)
		{
			time = _time;
			blend_start = _sv; blend_end = _ev;
			height_start = _height_start;
			height_end = _height_end;
			move_offset = _offset;
			pivot_offset_s = _pivot_offset_s;
			pivot_offset_e = _pivot_offset_e;
			pivot_offset_lr = _pivot_offset_lr;
			flags = _flags;
		}

		bool Flag(f::stepflag flag)
		{
			return flags & flag;
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

	struct AnimatedMesh
	{
		assetid models[anim_count];
	};

	Texture& GetBTT(btui32 index);
	graphics::NewMesh& GetBTM(btui32 index);
	ModelBlend& GetBTMB(btui32 index);
}

//the resources are wrapped in a class so that it's constructed and destructed within the main function automatically
class Resources
{
public:
	Resources();
	~Resources();
	void Init();

	res::Animation anims[res::anim_count];
	res::AnimatedMesh mesh_chara_legs;
	res::AnimatedMesh mesh_cloak;

	//shaders
	Shader shader_solid; // Shader used for drawing solid environment art
	Shader shader_char; // Shader used for drawing characters
	Shader shader_blend;
	Shader shader_gui; // GUI shader

	Shader shader_post;
};

extern Resources* pRes;