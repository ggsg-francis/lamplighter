#include "resources.h"
#include "resources-archive.h"
#include "serializer.h"
#include "memory.hpp"

Resources* pRes;

namespace res
{
	//mem::buffer<GLuint> rbuf; // GL ids for the resource buffer
	//mem::buffer<btui16> rbuf_usage; // for example, if 3 objects are using cloak_01 mesh, cloak_01 index here would be 3

	float modelOffsets[6]{ 0.1f, 0.09f, 0.f, 0.1f, 0.09f, 0.f };
	float modelOffsets_turn[6]{ 0.06f, 0.03f, 0.f, 0.06f, 0.03f, 0.f };

	float footstep[6]{ 1.f, 0.9f, 0.f, -1.f, -0.9f, -0.f };
	float footstep_turn[6]{ 0.3f, 0.5f, 0.f, -0.3f, -0.5f, -0.f };

	Animation::Animation()
	{
		steps = (AnimStep*)malloc(1);
	}

	Animation::~Animation()
	{
		free(steps);
	}

	void Animation::AddStep(AnimStep step)
	{
		size++;
		AnimStep* _steps = (AnimStep*)realloc(steps, sizeof(AnimStep) * size);
		if (_steps) {
			steps = _steps;
		}
		steps[size - 1] = step;
	}

	Texture& GetBTT(btui32 index)
	{
		return *(Texture*)archive::assets[index].asset;
	}

	graphics::NewMesh& GetBTM(btui32 index)
	{
		return *(graphics::NewMesh*)archive::assets[index].asset;
	}

	ModelBlend& GetBTMB(btui32 index)
	{
		return *(ModelBlend*)archive::assets[index].asset;
	}
}

Resources::Resources()
{
	//
}

Resources::~Resources()
{
	archive::clear_memory();
}

void Resources::Init()
{
	serializer::load_archive("resources/tsoa.UwUa");

	//for now, just load everything
	for (btui32 i = 0u; i < archive::assetCount; i++)
	{
		archive::LoadAsset(i);
	}

	mesh_chara_legs.models[res::idle] = res::m_legs;
	mesh_chara_legs.models[res::step_l] = res::m_legs;
	mesh_chara_legs.models[res::step_r] = res::m_legs;
	mesh_chara_legs.models[res::run_l] = res::m_legsrun;
	mesh_chara_legs.models[res::run_r] = res::m_legsrun;
	mesh_chara_legs.models[res::step_back_l] = res::m_legs;
	mesh_chara_legs.models[res::step_back_r] = res::m_legs;
	mesh_chara_legs.models[res::knockback] = res::m_legs;
	mesh_chara_legs.models[res::aim] = res::m_legs;

	mesh_cloak.models[res::idle] = res::mb_equip_body_pickers_step;
	mesh_cloak.models[res::step_l] = res::mb_equip_body_pickers_step;
	mesh_cloak.models[res::step_r] = res::mb_equip_body_pickers_step;
	mesh_cloak.models[res::run_l] = res::mb_equip_body_pickers_run;
	mesh_cloak.models[res::run_r] = res::mb_equip_body_pickers_run;
	mesh_cloak.models[res::step_back_l] = res::mb_equip_body_pickers_step;
	mesh_cloak.models[res::step_back_r] = res::mb_equip_body_pickers_step;
	mesh_cloak.models[res::knockback] = res::mb_equip_body_pickers_step;
	mesh_cloak.models[res::aim] = res::mb_equip_body_pickers_step;

	// in case I move it
	#define ns res::f
	#define cast res::f::stepflag

	// Idle pose

	anims[res::idle].AddStep(res::AnimStep(
		0.f, 0.f, 1.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 
		ns::can_start_moving
	));

	// Walk left step

	anims[res::step_l].AddStep(res::AnimStep(
		0.f, 0.5f, // blend values start & end
		0.1f, // time
		0.f, 0.f, // height 
		0.125f, // offset
		0.f, 0.5f, -0.2f, //offset lr
		ns::can_turn
	));
	anims[res::step_l].AddStep(res::AnimStep(
		0.5f, 1.f, // blend values start & end
		0.2f, // time
		0.f, 0.f, // height 
		0.125f, // offset
		0.f, 0.5f, -0.2f, //offset lr
		ns::can_turn
	));
	anims[res::step_l].AddStep(res::AnimStep(
		1.f, 0.f, // blend values start & end
		0.2f, // time
		0.f, 0.f, // height 
		0.25f, // offset
		-0.5f, 0.f, 0.2f, //offset lr
		ns::can_turn
	));

	// Walk right step

	anims[res::step_r].AddStep(res::AnimStep(
		0.f, 0.5f, // blend values start & end
		0.1f, // time
		0.f, 0.f, // height 
		0.125f, // offset
		0.f, 0.5f, 0.2f, //offset lr
		(cast)(ns::flip_lr | ns::can_turn)
	));
	anims[res::step_r].AddStep(res::AnimStep(
		0.5f, 1.f, // blend values start & end
		0.2f, // time
		0.f, 0.f, // height 
		0.125f, // offset
		0.f, 0.5f, 0.2f, //offset lr
		(cast)(ns::flip_lr | ns::can_turn)
	));
	anims[res::step_r].AddStep(res::AnimStep(
		1.f, 0.f, // blend values start & end
		0.2f, // time
		0.f, 0.f, // height 
		0.25f, // offset
		-0.5f, 0.f, -0.2f, //offset lr
		(cast)(ns::flip_lr | ns::can_turn)
	));

	// Run left step

	anims[res::run_l].AddStep(res::AnimStep(
		0.f, 1.f, // blend values start & end
		0.3f, // time
		0.1f, 0.2f, // height 
		0.6f, // offset
		0.f, 0.5f, -0.2f, //offset lr
		(cast)(ns::can_turn | ns::inflict_knockback)
	));
	anims[res::run_l].AddStep(res::AnimStep(
		1.f, 0.f, // blend values start & end
		0.2f, // time
		0.1f, 0.f, // height 
		0.6f, // offset
		-0.5f, 0.f, 0.2f, //offset lr
		(cast)(ns::can_turn | ns::cast_projectile | ns::inflict_knockback)
	));

	// Run right step

	anims[res::run_r].AddStep(res::AnimStep(
		0.f, 1.f, // blend values start & end
		0.3f, // time
		0.1f, 0.2f, // height 
		0.6f, // offset
		0.f, 0.5f, 0.2f, //offset lr
		(cast)(ns::flip_lr | ns::can_turn | ns::inflict_knockback)
	));
	anims[res::run_r].AddStep(res::AnimStep(
		1.f, 0.f, // blend values start & end
		0.2f, // time
		0.1f, 0.f, // height 
		0.6f, // offset
		-0.5f, 0.f, -0.2f, //offset lr
		(cast)(ns::flip_lr | ns::can_turn | ns::cast_projectile | ns::inflict_knockback)
	));

	// Step back left foot

	anims[res::step_back_l].AddStep(res::AnimStep(
		0.f, 0.5f, // blend values start & end
		0.1f, // time
		0.f, 0.f, // height 
		-0.125f, // Move offset
		0.f, 0.5f, -0.2f, // Pivot offset
		ns::can_turn
	));
	anims[res::step_back_l].AddStep(res::AnimStep(
		0.5f, 1.f, // blend values start & end
		0.2f, // time
		0.f, 0.f, // height 
		-0.125f, // offset
		0.f, 0.5f, -0.2f, //offset lr
		ns::can_turn
	));
	anims[res::step_back_l].AddStep(res::AnimStep(
		1.f, 0.f, // blend values start & end
		0.2f, // time
		0.f, 0.f, // height 
		-0.25f, // offset
		-0.5f, 0.f, 0.2f, //offset lr
		ns::can_turn
	));

	// Step back right foot

	anims[res::step_back_r].AddStep(res::AnimStep(
		0.f, 0.5f, // blend values start & end
		0.1f, // time
		0.f, 0.f, // height 
		-0.125f, // offset
		0.f, 0.5f, 0.2f, //offset lr
		(cast)(ns::flip_lr | ns::can_turn)
	));
	anims[res::step_back_r].AddStep(res::AnimStep(
		0.5f, 1.f, // blend values start & end
		0.2f, // time
		0.f, 0.f, // height 
		-0.125f, // offset
		0.f, 0.5f, 0.2f, //offset lr
		(cast)(ns::flip_lr | ns::can_turn)
	));
	anims[res::step_back_r].AddStep(res::AnimStep(
		1.f, 0.f, // blend values start & end
		0.2f, // time
		0.f, 0.f, // height 
		-0.25f, // offset
		-0.5f, 0.f, -0.2f, //offset lr
		(cast)(ns::flip_lr | ns::can_turn)
	));

	// Knockback

	anims[res::knockback].AddStep(res::AnimStep(
		0.5f, 0.5f, // blend values start & end
		0.4f, // time
		0.f, 0.f, // height 
		0.f, // offset
		0.f, 0.f, 0.f, //offset lr
		ns::none
	));

	// Aiming

	anims[res::aim].AddStep(res::AnimStep(
		0.5f, 0.5f, // blend values start & end
		1.f, // time
		0.f, 0.f, // height start & end
		0.f, // offset
		0.f, 0.f, 0.f, //offset lr
		(cast)(ns::can_turn | ns::can_start_moving)
	));

	#undef ns
	#undef cast

	// too many shaders!
	shader_solid = Shader("shaders/3d.vs", "shaders/3d_env.fs");
	shader_char = Shader("shaders/3d.vs", "shaders/3d_ent.fs");
	shader_blend = Shader("shaders/3d_blend.vs", "shaders/3d_ent.fs");
	shader_gui = Shader("shaders/gui.vs", "shaders/gui_h.fs");
	shader_post = Shader("shaders/fb.vs", "shaders/fb.fs");
}