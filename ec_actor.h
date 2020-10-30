#pragma once

#include "ec_common.h"

#include "objects_inventory.h"
#include "animation.h"

enum ActorInputBit : lui16
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
struct ActorInput {
	mem::bv<lui16, ActorInputBit> bits;
	lui16 empty;
	m::Vector2 move;
};
// compare 664
//int iadhfkjhgg2 = sizeof(ECCommon);
// compare: 1736 bytes
//int iadhfkjhgg = sizeof(ECActor);
struct ECActor : public ECCommon
{
	ActorInput input;
	
	m::Angle viewYaw;
	m::Angle viewPitch;

	// Render colours
	m::Vector3 skin_col_a;
	m::Vector3 skin_col_b;
	m::Vector3 skin_col_c;

	// Character stats
	lf32 speed = 2.3f;
	lf32 agility = 1.f; // use agility to determine turning speed?

	Inventory inventory;
	lui32 inv_active_slot = 0u;

	Transform3D t_body, t_head;
	m::Vector3 fpCurrentL, fpCurrentR;
	AnimPlayerVec3 ap_fpCurrentL, ap_fpCurrentR;
	
	lf32 aniStandHeight;
	AnimPlayerVec3 ap_StandHeight;
	
	lf32 aniSlideResponse = 0.f;

	m::Vector3 handPosR;
	AnimPlayerVec3 ap_HandPosR;
	m::Vector3 handPosL;
	AnimPlayerVec3 ap_HandPosL;

	path::Path ai_path;
	lui64 ai_timer = 0u;

	lid ai_target_ent = BUF_NULL;
	lid ai_ally_ent = BUF_NULL;
	lid aniHandHoldTarget = ID_NULL;
	// Attack target - attacking target only
	lid atk_target = BUF_NULL;
	// View target (used to pick up items and stuff)
	lid viewtarget = ID_NULL;

	MaxedStat stamina;

	lui8 actorBase = 0u;
	// Not neat looking but packing small bvs together
	enum FootState : lui8
	{
		eL_DOWN,
		eR_DOWN,
	};
	FootState foot_state = eL_DOWN;

	enum JumpState : lui8
	{
		eJUMP_NEITHER,
		eJUMP_JUMP,
		eJUMP_SPRINT,
		eJUMP_SPRINT_WANTJUMP,
	};
	JumpState jump_state = eJUMP_NEITHER;

	enum CharaAniFlags : lui8
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
	mem::bv<lui8, CharaAniFlags> animationBV;

	bool aiControlled = false;

	bool aniCrouch = false;
	bool aniRun = true;

	lui8 ai_path_current_index = 0u;
	bool ai_pathing = false;
};

void ActorOnHitGround(ECActor* chr);
void ActorTryHoldHand(lid id_self, lid id);
void ActorTakeItem(lid id_self, lid id);
void ActorDropItem(lid id_self, lid slot);
void ActorDropAllItems(lid id_self);
void ActorSetEquipSlot(lid id_self, lui32 slot);
void ActorIncrEquipSlot(lid id_self);
void ActorDecrEquipSlot(lid id_self);
void ActorTick(lid id, void* ent, lf32 dt);
void ActorDraw(lid id, void* ent);
