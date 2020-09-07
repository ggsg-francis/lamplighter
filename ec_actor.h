#pragma once

#include "ec_common.h"

#include "objects_inventory.h"

struct ECActor : public ECCommon
{
	enum ActorInput : btui16
	{
		IN_RUN = 0x1u << 0x0u,
		IN_AIM = 0x1u << 0x1u,
		IN_USE = 0x1u << 0x2u,
		IN_USE_HIT = 0x1u << 0x3u,
		IN_USE_ALT = 0x1u << 0x4u,
		IN_ACTN_A = 0x1u << 0x5u,
		IN_ACTN_B = 0x1u << 0x6u,
		IN_ACTN_C = 0x1u << 0x7u,
		IN_CROUCH = 0x1u << 0x8u,
		IN_JUMP = 0x1u << 0x9u,
	};
	mem::bv<btui16, ActorInput> inputBV;
	m::Vector2 input; // Input vector, might be temporary
	m::Angle viewYaw;
	m::Angle viewPitch;

	//-------------------------------- Actor stuff

	btui8 actorBase = 0u;

	m::Vector3 skin_col_a;
	m::Vector3 skin_col_b;
	m::Vector3 skin_col_c;

	btf32 speed = 2.3f;
	btf32 agility = 0.f; // 0?? use agility to determine turning speed?

	MaxedStat stamina;

	Inventory inventory;
	btui32 inv_active_slot = 0u;

	btID atk_target = BUF_NULL;
	m::Angle atkYaw;

	//-------------------------------- CHARA stuff

	enum CharaStaticProperties : btui8
	{
		eLEFT_HANDED = (0x1u << 0x0u),
	};
	mem::bv<btui8, CharaStaticProperties> staticPropertiesBV;

	enum FootState : btui8
	{
		eL_DOWN,
		eR_DOWN,
	};
	FootState foot_state = eL_DOWN;

	enum JumpState : btui8
	{
		eJUMP_NEITHER,
		eJUMP_JUMP,
		eJUMP_SPRINT,
	};

	JumpState jump_state = eJUMP_NEITHER;

	enum CharaAniFlags : btui8
	{
		eANI_TEMP_1 = 1u,
		eANI_LANDED = 1u << 1u,
		eANI_TEMP_3 = 1u << 3u,
		eANI_TEMP_4 = 1u << 2u,
		eANI_TEMP_5 = 1u << 4u,
		eANI_TEMP_6 = 1u << 5u,
		eANI_TEMP_7 = 1u << 6u,
		eANI_TEMP_8 = 1u << 7u,

		eANI_CLEAR = 0b11111111u,
	};
	mem::bv<btui8, CharaAniFlags> animationBV;

	Transform3D t_body, t_head;
	m::Vector3 fpCurrentL, fpCurrentR;
	m::Vector2 ani_body_lean;
	btf32 aniStandHeight;
	bool aniCrouch = false;
	btf32 aniSlideResponse = 0.f;
	bool aniRun = true;

	btf32 aniTimer = 0.f;
	m::Vector3 lastGroundFootPos;

	btID aniHandHoldTarget = ID_NULL;

	// View target (used to pick up items and stuff)
	btID viewtarget = ID_NULL;

	//-------------------------------- AI stuff

	btf32 ai_vy_target = 0.f;
	btf32 ai_vp_target = 0.f;
	btID ai_target_ent = BUF_NULL;
	btID ai_ally_ent = BUF_NULL;
	bool aiControlled = false;
	path::Path ai_path;
	btui8 ai_path_current_index = 0u;
	bool ai_pathing = false;
};

void ActorOnHitGround(ECActor* chr);
void ActorTryHoldHand(btID id_self, btID id);
void ActorTakeItem(btID id_self, btID id);
void ActorDropItem(btID id_self, btID slot);
void ActorDropAllItems(btID id_self);
void ActorSetEquipSlot(btID id_self, btui32 slot);
void ActorIncrEquipSlot(btID id_self);
void ActorDecrEquipSlot(btID id_self);
char* ActorName(void* ent);
void ActorTick(btID id, void* ent, btf32 dt);
void ActorDraw(btID id, void* ent);
