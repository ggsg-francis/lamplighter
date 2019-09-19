#include "resources.h"
#include "resources-archive.h"
#include "serializer.h"
#include "memory.hpp"

namespace res
{
	//btf32 anim_curve[16]{
	//	0.f, -0.2f, -0.2f, 0.f, 0.025f, 0.05f, 0.1f, 0.4f,
	//	0.6f, 0.9f, 0.95f, 0.925f, 1.f, 1.2f, 1.2f, 1.f};
	btf32 anim_curve[16]{
		0.f, 0.f, 0.f, 0.f, 0.025f, 0.05f, 0.1f, 0.4f,
		0.6f, 0.9f, 0.95f, 0.925f, 1.f, 1.f, 1.f, 1.f };

	Animation animations[res::aniarr_size];
	AnimMeshSet amset_legs;
	AnimMeshSet amset_arms;
	AnimMeshSet amset_cloak;

	Animation::Animation()
	{
		steps = nullptr;
	}

	Animation::~Animation()
	{
		free(steps);
	}

	void Animation::AddStep(AnimStep step)
	{
		size++;
		steps = (AnimStep*)realloc(steps, sizeof(AnimStep) * size);
		steps[size - 1] = step;
	}

	graphics::Texture& GetBTT(btui32 index)
	{
		return *(graphics::Texture*)archive::assets[index].asset;
	}

	graphics::Mesh& GetBTM(btui32 index)
	{
		return *(graphics::Mesh*)archive::assets[index].asset;
	}

	graphics::MeshBlend& GetBTMB(btui32 index)
	{
		return *(graphics::MeshBlend*)archive::assets[index].asset;
	}

	void Init()
	{
		serializer::load_archive("res/archive.UwUa");

		//for now, just load everything
		for (btui32 i = 0u; i < archive::assetCount; i++)
		{
			archive::LoadAsset(i);
		}

		amset_legs.models[res::a_idle] = res::mb_legs;
		amset_legs.models[res::a_step_l] = res::mb_legs;
		amset_legs.models[res::a_step_r] = res::mb_legs;
		amset_legs.models[res::a_run_l] = res::mb_legsrun;
		amset_legs.models[res::a_run_r] = res::mb_legsrun;
		amset_legs.models[res::a_step_back_l] = res::mb_legs;
		amset_legs.models[res::a_step_back_r] = res::mb_legs;
		amset_legs.models[res::a_knockback] = res::mb_legs;
		amset_legs.models[res::a_castmgc] = res::mb_legs;

		amset_arms.models[res::a_idle] = res::mb_armscast;
		amset_arms.models[res::a_step_l] = res::mb_armscast;
		amset_arms.models[res::a_step_r] = res::mb_armscast;
		amset_arms.models[res::a_run_l] = res::mb_armscast;
		amset_arms.models[res::a_run_r] = res::mb_armscast;
		amset_arms.models[res::a_step_back_l] = res::mb_armscast;
		amset_arms.models[res::a_step_back_r] = res::mb_armscast;
		amset_arms.models[res::a_knockback] = res::mb_armscast;
		amset_arms.models[res::a_castmgc] = res::mb_armscast;

		amset_cloak.models[res::a_idle] = res::mb_equip_body_pickers_step;
		amset_cloak.models[res::a_step_l] = res::mb_equip_body_pickers_step;
		amset_cloak.models[res::a_step_r] = res::mb_equip_body_pickers_step;
		amset_cloak.models[res::a_run_l] = res::mb_equip_body_pickers_run;
		amset_cloak.models[res::a_run_r] = res::mb_equip_body_pickers_run;
		amset_cloak.models[res::a_step_back_l] = res::mb_equip_body_pickers_step;
		amset_cloak.models[res::a_step_back_r] = res::mb_equip_body_pickers_step;
		amset_cloak.models[res::a_knockback] = res::mb_equip_body_pickers_step;
		amset_cloak.models[res::a_castmgc] = res::mb_equip_body_pickers_step;

		#define NS res
		#define CAST res::aniflag

		// Idle pose

		animations[res::a_idle].AddStep(res::AnimStep(
			0.0f, 0.0f, 1.0f, 0.0f, 0.f, 0.0f, 0.0f,
			(CAST)(NS::eCAN_CANCEL)
		));

		// Walk left step

		animations[res::a_step_l].AddStep(res::AnimStep(
			0.0f, 0.5f, // Blend values start & end
			0.1f, 0.125f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.0f, // Height offset start & end 
			(CAST)(NS::eCAN_TURN | NS::eRETURN_TO_IDLE)
		));
		animations[res::a_step_l].AddStep(res::AnimStep(
			0.5f, 1.0f, // Blend values start & end
			0.2f, 0.125f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.0f, // Height offset start & end 
			(CAST)(NS::eCAN_TURN | NS::eRETURN_TO_IDLE)
		));
		animations[res::a_step_l].AddStep(res::AnimStep(
			1.0f, 0.0f, // Blend values start & end
			0.2f, 0.25f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.0f, // Height offset start & end 
			(CAST)(NS::eCAN_TURN | NS::eRETURN_TO_IDLE)
		));

		// Walk right step

		animations[res::a_step_r].AddStep(res::AnimStep(
			0.0f, 0.5f, // Blend values start & end
			0.1f, 0.125f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.0f, // Height offset start & end 
			(CAST)(NS::eFLIP_LR | NS::eCAN_TURN | NS::eRETURN_TO_IDLE)
		));
		animations[res::a_step_r].AddStep(res::AnimStep(
			0.5f, 1.0f, // Blend values start & end
			0.2f, 0.125f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.0f, // Height offset start & end 
			(CAST)(NS::eFLIP_LR | NS::eCAN_TURN | NS::eRETURN_TO_IDLE)
		));
		animations[res::a_step_r].AddStep(res::AnimStep(
			1.0f, 0.0f, // Blend values start & end
			0.2f, 0.25f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.0f, // Height offset start & end 
			(CAST)(NS::eFLIP_LR | NS::eCAN_TURN | NS::eRETURN_TO_IDLE)
		));

		// Run left step

		animations[res::a_run_l].AddStep(res::AnimStep(
			0.0f, 1.0f, // Blend values start & end
			0.09f, 0.35f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.2f, // Height offset start & end 
			(CAST)(NS::eCAN_TURN | NS::eINFLICT_KNOCKBACK | NS::eRETURN_TO_IDLE | NS::eSMEAR)
		));
		animations[res::a_run_l].AddStep(res::AnimStep(
			1.0f, 0.8f, // Blend values start & end
			0.2f, 0.5f, 0.f, // Time & forward amount & rotation amount
			0.2f, 0.15f, // Height offset start & end 
			(CAST)(NS::eCAN_TURN | NS::eINFLICT_KNOCKBACK | NS::eRETURN_TO_IDLE)
		));
		animations[res::a_run_l].AddStep(res::AnimStep(
			0.8f, 0.0f, // Blend values start & end
			0.14f, 0.3f, 0.f, // Time & forward amount & rotation amount
			0.15f, 0.0f, // Height offset start & end 
			(CAST)(NS::eCAN_TURN | NS::eINFLICT_KNOCKBACK | NS::eRETURN_TO_IDLE)
		));

		// Run right step

		animations[res::a_run_r].AddStep(res::AnimStep(
			0.0f, 1.0f, // Blend values start & end
			0.09f, 0.35f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.2f, // Height offset start & end 
			(CAST)(NS::eFLIP_LR | NS::eCAN_TURN | NS::eINFLICT_KNOCKBACK | NS::eRETURN_TO_IDLE | NS::eSMEAR)
		));
		animations[res::a_run_r].AddStep(res::AnimStep(
			1.0f, 0.8f, // Blend values start & end
			0.2f, 0.5f, 0.f, // Time & forward amount & rotation amount
			0.2f, 0.15f, // Height offset start & end 
			(CAST)(NS::eFLIP_LR | NS::eCAN_TURN | NS::eINFLICT_KNOCKBACK | NS::eRETURN_TO_IDLE)
		));
		animations[res::a_run_r].AddStep(res::AnimStep(
			0.8f, 0.0f, // Blend values start & end
			0.14f, 0.3f, 0.f, // Time & forward amount & rotation amount
			0.15f, 0.0f, // Height offset start & end 
			(CAST)(NS::eFLIP_LR | NS::eCAN_TURN | NS::eINFLICT_KNOCKBACK | NS::eRETURN_TO_IDLE)
		));

		/*

		// Run left step

		animations[res::a_run_l].AddStep(res::AnimStep(
		0.0f, 1.0f, // Blend values start & end
		0.2f, 0.4f, 0.f, // Time & forward amount & rotation amount
		0.0f, 0.2f, // Height offset start & end
		(CAST)(NS::can_turn | NS::inflict_knockback | NS::return_to_idle)
		));
		animations[res::a_run_l].AddStep(res::AnimStep(
		1.0f, 0.8f, // Blend values start & end
		0.2f, 0.4f, 0.f, // Time & forward amount & rotation amount
		0.2f, 0.15f, // Height offset start & end
		(CAST)(NS::can_turn | NS::inflict_knockback | NS::return_to_idle)
		));
		animations[res::a_run_l].AddStep(res::AnimStep(
		0.8f, 0.0f, // Blend values start & end
		0.2f, 0.4f, 0.f, // Time & forward amount & rotation amount
		0.15f, 0.0f, // Height offset start & end
		(CAST)(NS::can_turn | NS::inflict_knockback | NS::return_to_idle)
		));

		// Run right step

		animations[res::a_run_r].AddStep(res::AnimStep(
		0.0f, 1.0f, // Blend values start & end
		0.2f, 0.4f, 0.f, // Time & forward amount & rotation amount
		0.0f, 0.2f, // Height offset start & end
		(CAST)(NS::flip_lr | NS::can_turn | NS::inflict_knockback | NS::return_to_idle)
		));
		animations[res::a_run_r].AddStep(res::AnimStep(
		1.0f, 0.8f, // Blend values start & end
		0.2f, 0.4f, 0.f, // Time & forward amount & rotation amount
		0.2f, 0.15f, // Height offset start & end
		(CAST)(NS::flip_lr | NS::can_turn | NS::inflict_knockback | NS::return_to_idle)
		));
		animations[res::a_run_r].AddStep(res::AnimStep(
		0.8f, 0.0f, // Blend values start & end
		0.2f, 0.4f, 0.f, // Time & forward amount & rotation amount
		0.15f, 0.0f, // Height offset start & end
		(CAST)(NS::flip_lr | NS::can_turn | NS::inflict_knockback | NS::return_to_idle)
		));

		*/

		// Step back left foot

		animations[res::a_step_back_l].AddStep(res::AnimStep(
			0.0f, 0.5f, // Blend values start & end
			0.1f, -0.125f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.0f, // Height offset start & end 
			(CAST)(NS::eCAN_TURN | NS::eRETURN_TO_IDLE)
		));
		animations[res::a_step_back_l].AddStep(res::AnimStep(
			0.5f, 1.0f, // Blend values start & end
			0.2f, -0.125f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.0f, // Height offset start & end 
			(CAST)(NS::eCAN_TURN | NS::eRETURN_TO_IDLE)
		));
		animations[res::a_step_back_l].AddStep(res::AnimStep(
			1.0f, 0.0f, // Blend values start & end
			0.2f, -0.25f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.0f, // Height offset start & end 
			(CAST)(NS::eCAN_TURN | NS::eRETURN_TO_IDLE)
		));

		// Step back right foot

		animations[res::a_step_back_r].AddStep(res::AnimStep(
			0.0f, 0.5f, // Blend values start & end
			0.1f, -0.125f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.0f, // Height offset start & end 
			(CAST)(NS::eFLIP_LR | NS::eCAN_TURN | NS::eRETURN_TO_IDLE)
		));
		animations[res::a_step_back_r].AddStep(res::AnimStep(
			0.5f, 1.0f, // Blend values start & end
			0.2f, -0.125f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.0f, // Height offset start & end 
			(CAST)(NS::eFLIP_LR | NS::eCAN_TURN | NS::eRETURN_TO_IDLE)
		));
		animations[res::a_step_back_r].AddStep(res::AnimStep(
			1.0f, 0.0f, // Blend values start & end
			0.2f, -0.25f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.0f, // Height offset start & end 
			(CAST)(NS::eFLIP_LR | NS::eCAN_TURN | NS::eRETURN_TO_IDLE)
		));

		// Knockback

		animations[res::a_knockback].AddStep(res::AnimStep(
			0.5f, 0.5f, // Blend values start & end
			0.4f, 0.0f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.0f, // Height offset start & end 
			(CAST)(NS::eNO_FLAG)
		));

		// Attack

		animations[res::a_castmgc].AddStep(res::AnimStep(
			0.0f, 1.0f, // Blend values start & end
			1.f, 0.0f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.0f, // Height offset start & end
			(CAST)(NS::eCAN_TURN | NS::eRETURN_TO_IDLE)
		));
		animations[res::a_castmgc].AddStep(res::AnimStep(
			1.0f, 0.0f, // Blend values start & end
			1.f, 0.0f, 0.f, // Time & forward amount & rotation amount
			0.0f, 0.0f, // Height offset start & end
			(CAST)(NS::eCAST_PROJECTILE | NS::eRETURN_TO_IDLE)
		));

		#undef NS
		#undef CAST
	}
	void End()
	{
		archive::ClearMemory();
	}
}