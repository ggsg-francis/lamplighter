#include "objects_entities.h"
#include "objects_items.h"

#include "graphics.hpp"
#include <glm\gtc\matrix_transform.hpp>

#include "archive.hpp"

#include "core.h"
#include "index.h"

#include "cfg.h"

#include "audio.hpp"

#include "collision.h"

#include "nonpc.h"

void Actor_OnHitGround(Actor* chr)
{
	if (chr->foot_state == Actor::eL_DOWN) aud::PlaySnd(aud::FILE_FOOTSTEP_SNOW_A, chr->fpCurrentL);
	else aud::PlaySnd(aud::FILE_FOOTSTEP_SNOW_B, chr->fpCurrentR);
	// Swap feet
	if (chr->foot_state == Actor::eL_DOWN) chr->foot_state = Actor::eR_DOWN;
	else if (chr->foot_state == Actor::eR_DOWN) chr->foot_state = Actor::eL_DOWN;
}
void Entity_Deintersect(Entity* ent, CellSpace& csi)
{
	btf32 offsetx, offsety;
	bool overlapN, overlapS, overlapE, overlapW, touchNS = false, touchEW = false;

	//-------------------------------- ACTOR COLLISION CHECK

	if (ent->properties.get(Entity::eCOLLIDE_ENT))
	{
		for (btcoord x = csi.c[eCELL_I].x - 1u; x < csi.c[eCELL_I].x + 1u; ++x)
		{
			for (btcoord y = csi.c[eCELL_I].y - 1u; y < csi.c[eCELL_I].y + 1u; ++y)
			{
				//de-intersect entities
				for (int e = 0; e < core::CellEntityCount(x, y); e++)
				{
					if (core::CellEntity(x, y, e) != ID_NULL)
					{
						if (GetEntityExists(core::CellEntity(x, y, e)) && ENTITY(core::CellEntity(x, y, e))->properties.get(Entity::eCOLLIDE_ENT))
						{
							m::Vector2 vec = ent->t.position - ENTITY(core::CellEntity(x, y, e))->t.position;
							float dist = m::Length(vec);
							btf32 combined_radius = ent->radius + ENTITY(core::CellEntity(x, y, e))->radius;
							if (dist < combined_radius && dist > 0.f)
							{
								// TEMP! if same type
								if (ent->type == ENTITY(core::CellEntity(x, y, e))->type)
								{
									ent->t.position += m::Normalize(vec) * (combined_radius - dist) * 0.5f;
									ENTITY(core::CellEntity(x, y, e))->t.position -= m::Normalize(vec) * (combined_radius - dist) * 0.5f;
								}

								// consider using some kind of collision callback

								/*
								// knockback effect
								m::Vector2 surface = m::Normalize(vec * -1.f);
								if (ent->type == ENTITY_TYPE_CHARA && m::Length(ent->t.velocity) > 0.02f)
								{
								// if other is also a character
								if (ENTITY(cells[x][y].ents[e])->type == ENTITY_TYPE_CHARA)
								{
								ent->t.velocity = surface * -0.1f; // set my velocity
								ENTITY(cells[x][y].ents[e])->t.velocity = surface * 0.3f; // set their velocity
								}
								}
								*/
							}
						}
					} // End for each entity in cell
				} // End if entity count of this cell is bigger than zero
			} // End for each cell group Y
		} // End for each cell group X
	} // end does collide entities check

	  ///*
	  //-------------------------------- ENVIRONMENTAL COLLISION CHECK (2ND THEREFORE PRIORITIZED)

	offsetx = ent->t.position.x - ent->t.csi.c[eCELL_I].x;
	offsety = ent->t.position.y - ent->t.csi.c[eCELL_I].y;

	overlapN = offsety > 0;
	overlapS = offsety < 0;
	overlapE = offsetx > 0;
	overlapW = offsetx < 0;

	//-------------------------------- STRAIGHT EDGE COLLISION CHECK
	// North
	if (((btf32)env::eCells.terrain_height[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y + 1u] / TERRAIN_HEIGHT_DIVISION)
	>(ent->t.height + 0.5f) && overlapN)
	{
		ent->t.position.y = ent->t.csi.c[eCELL_I].y; // + (1 - radius)
		ent->t.velocity.y = 0.f;
		touchNS = true;
	}
	// South
	if (((btf32)env::eCells.terrain_height[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y - 1u] / TERRAIN_HEIGHT_DIVISION)
		> (ent->t.height + 0.5f) && overlapS)
	{
		ent->t.position.y = ent->t.csi.c[eCELL_I].y; // - (1 - radius)
		ent->t.velocity.y = 0.f;
		touchNS = true;
	}
	// East
	if (((btf32)env::eCells.terrain_height[ent->t.csi.c[eCELL_I].x + 1u][ent->t.csi.c[eCELL_I].y] / TERRAIN_HEIGHT_DIVISION)
	> (ent->t.height + 0.5f) && overlapE)
	{
		ent->t.position.x = ent->t.csi.c[eCELL_I].x; // + (1 - radius)
		ent->t.velocity.x = 0.f;
		touchEW = true;
	}
	// West
	if (((btf32)env::eCells.terrain_height[ent->t.csi.c[eCELL_I].x - 1u][ent->t.csi.c[eCELL_I].y] / TERRAIN_HEIGHT_DIVISION)
	> (ent->t.height + 0.5f) && overlapW)
	{
		ent->t.position.x = ent->t.csi.c[eCELL_I].x; // - (1 - radius)
		ent->t.velocity.x = 0.f;
		touchEW = true;
	}

	//-------------------------------- CORNER COLLISION CHECK

	// North-east
	if (((btf32)env::eCells.terrain_height[ent->t.csi.c[eCELL_I].x + 1u][ent->t.csi.c[eCELL_I].y + 1u] / TERRAIN_HEIGHT_DIVISION)
	> (ent->t.height + 0.5f) && overlapN && overlapE) {
		m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(0.5f, 0.5f);
		if (m::Length(offset) < 0.5f)
			ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
	}
	// North-west
	if (((btf32)env::eCells.terrain_height[ent->t.csi.c[eCELL_I].x - 1u][ent->t.csi.c[eCELL_I].y + 1u] / TERRAIN_HEIGHT_DIVISION)
		> (ent->t.height + 0.5f) && overlapN && overlapW) {
		m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(-0.5f, 0.5f);
		if (m::Length(offset) < 0.5f)
			ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
	}
	// South-east
	if (((btf32)env::eCells.terrain_height[ent->t.csi.c[eCELL_I].x + 1u][ent->t.csi.c[eCELL_I].y - 1u] / TERRAIN_HEIGHT_DIVISION)
		> (ent->t.height + 0.5f) && overlapS && overlapE) {
		m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(0.5f, -0.5f);
		if (m::Length(offset) < 0.5f)
			ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
	}
	// South-west
	if (((btf32)env::eCells.terrain_height[ent->t.csi.c[eCELL_I].x - 1u][ent->t.csi.c[eCELL_I].y - 1u] / TERRAIN_HEIGHT_DIVISION)
		> (ent->t.height + 0.5f) && overlapS && overlapW) {
		m::Vector2 offset = m::Vector2(offsetx, offsety) - m::Vector2(-0.5f, -0.5f);
		if (m::Length(offset) < 0.5f)
			ent->t.position += m::Normalize(offset) * (0.5f - m::Length(offset));
	}
	//*/
}
void Entity_CheckGrounded(Entity* ent)
{
	btf32 th;
	env::GetHeight(th, ent->t.csi);
	
	if (ent->t.height + ent->t.height_velocity >= th + 1.f)
	{
		ent->grounded = false;
		ent->slideVelocity *= 0.f;
		if (ent->type == ENTITY_TYPE_ACTOR)
			((Actor*)ent)->jump_state = Actor::eJUMP_JUMP;
	}
	else if (ent->t.height_velocity > 0.f)
	{
		ent->grounded = false;
		ent->slideVelocity *= 0.f;
	}
	else if (!ent->grounded)
	{
		if (ent->type == ENTITY_TYPE_ACTOR)
			ent->grounded = RayEntity(ent->id, ((Actor*)ent)->aniStandHeight);
		else
			ent->grounded = RayEntity(ent->id, 0.f);
		if (ent->grounded)
		{
			ent->slideVelocity = ent->t.velocity;
			if (ent->type == ENTITY_TYPE_ACTOR) {
				Actor_OnHitGround((Actor*)ent);
			}
		}
	}
}
void Entity_PhysicsTick(Entity* ent, btID id, btf32 dt)
{
	// Regenerate csi

	CellSpace cs_last = ent->t.csi;
	core::GetCellSpaceInfo(ent->t.position, ent->t.csi);
	// If the new CS is different, remove us from the last cell and add us to the new one
	if (cs_last.c[eCELL_I].x != ent->t.csi.c[eCELL_I].x || cs_last.c[eCELL_I].y != ent->t.csi.c[eCELL_I].y)
	{
		core::RemoveEntityCell(cs_last.c[eCELL_I].x, cs_last.c[eCELL_I].y, id);
		core::AddEntityCell(ent->t.csi.c[eCELL_I].x, ent->t.csi.c[eCELL_I].y, id);
	}

	Entity_CheckGrounded(ent);

	// contains ground height...
	btf32 ground_height;

	btf32 distBelowSand;
	// look at how long this is
	switch (acv::props[env::eCells.prop[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y]].floorType)
	{
	case acv::EnvProp::FLOOR_QUICKSAND:
		ent->t.height -= 0.0025f;
		env::GetHeight(ground_height, ent->t.csi);
		// if above the ground
		if (ent->t.height + ent->t.height_velocity > ground_height)
		{
			ent->t.height_velocity -= 0.006f; // Add gravity
			if (ent->properties.get(Entity::ePHYS_DRAG)) ent->t.velocity *= 0.99f;
			ent->slideVelocity *= 0.8f;

			ent->t.position += ent->t.velocity; // Apply velocity
			ent->t.height += ent->t.height_velocity; // Apply velocity
		}
		else
		{
			// remove all velocity
			if (ent->properties.get(Entity::ePHYS_DRAG)) ent->t.velocity *= 0.7f;

			// Multiplier reduces the depth at which you can't move
			distBelowSand = (ground_height - ent->t.height) * 2.5f;
			if (distBelowSand > ent->height)
			{
				if (ent->type == ENTITY_TYPE_RESTING_ITEM)
					int bp = 0;
				ent->state.Damage(1000, 0);
			}
			if (distBelowSand > 1.f) distBelowSand = 1.f;
			ent->t.position += (ent->t.velocity * (1.f - distBelowSand)); // Apply velocity
		}
		break;
	default:
		if (ent->grounded)
		{
			env::GetHeight(ground_height, ent->t.csi);
			if (ent->type == ENTITY_TYPE_ACTOR) {
				ent->t.height = m::Lerp(ent->t.height, ground_height + ((Actor*)ent)->aniStandHeight, 6.f * dt);
			}
			else {
				ent->t.height = ground_height;
				ent->t.velocity *= 0.f; // Remove slide on non actors
			}

			m::Vector2 slope;
			env::GetSlope(slope.x, slope.y, ent->t.csi);

			m::Vector2 surfMod(1.f, 1.f);

			// Don't slide uphill
			if (ent->slideVelocity.x > 0.f && slope.x > 0.f) {
				surfMod.x = 0.5f;
				slope.x = 0.f;
			}
			else if (ent->slideVelocity.x < 0.f && slope.x < 0.f) {
				surfMod.x = 0.5f;
				slope.x = 0.f;
			}
			if (ent->slideVelocity.y > 0.f && slope.y > 0.f) {
				surfMod.y = 0.5f;
				slope.y = 0.f;
			}
			else if (ent->slideVelocity.y < 0.f && slope.y < 0.f) {
				surfMod.y = 0.5f;
				slope.y = 0.f;
			}

			btf32 slide_reduce = 0.18f * dt; // Slide reduction per second multiplied by frame length
			// Linear slide reduction (with slope adjustment)
			m::Vector2 slideMag = (btf32)m::Length(ent->slideVelocity);
			slideMag.y = slideMag.x;
			if (slideMag.x > slide_reduce / (abs(slope.x) + surfMod.x)) {
				slideMag.x -= slide_reduce / (abs(slope.x) + surfMod.x);
			}
			else slideMag.x = 0.f;
			if (slideMag.y > slide_reduce / (abs(slope.y) + surfMod.y)) {
				slideMag.y -= slide_reduce / (abs(slope.y) + surfMod.y);
			}
			else slideMag.y = 0.f;
			ent->slideVelocity = m::Normalize(ent->slideVelocity) * slideMag;

			// No height velocity when on the ground!
			ent->t.height_velocity = 0.f;
		}
		else
		{
			// Add gravity
			ent->t.height_velocity -= 0.20f * dt;
			// Velocity reduction (Air drag)
			if (ent->properties.get(Entity::ePHYS_DRAG)) ent->t.velocity *= 0.99f;
		}
		if (ent->grounded) ent->t.position += ent->slideVelocity;
		ent->t.position += ent->t.velocity; // Apply velocity
		ent->t.height += ent->t.height_velocity; // Apply velocity
		break;
	}

	Entity_Deintersect(ent, ent->t.csi);
}

char* DisplayNameActor(void* ent)
{
	//return "Actor";
	return (char*)((Actor*)ent)->name;
};
char* DisplayNameRestingItem(void* ent)
{
	//return (char*)acv::items[GETITEM_MISC(((RestingItem*)index::GetEntityPtr(ent))->item_instance)->item_template]->name;
	return (char*)acv::items[GETITEMINST(((RestingItem*)ent)->item_instance)->id_item_template]->name;
};
void DrawRestingItem(void* ent)
{
	RestingItem* item = (RestingItem*)ent;
	// Draw the mesh of our item id
	//DrawMesh(ent, res::GetM(acv::items[index::GetItem(item->item_instance)->item_template]->id_mesh), res::GetT(acv::items[index::GetItem(item->item_instance)->item_template]->id_tex), SS_NORMAL, item->t_item.getMatrix());
	if (item->id == core::viewtarget[core::activePlayer])
	{
		graphics::GetShader(graphics::S_SOLID).Use();
		graphics::GetShader(graphics::S_SOLID).SetBool(graphics::Shader::bLit_TEMP, false);
		DrawMesh(item->id, res::GetM(acv::items[GETITEMINST(item->item_instance)->id_item_template]->id_mesh), res::GetT(acv::items[GETITEMINST(item->item_instance)->id_item_template]->id_tex), SS_NORMAL, item->matrix);
		graphics::GetShader(graphics::S_SOLID).SetBool(graphics::Shader::bLit_TEMP, true);
	}
	else
	{
		DrawMesh(item->id, res::GetM(acv::items[GETITEMINST(item->item_instance)->id_item_template]->id_mesh), res::GetT(acv::items[GETITEMINST(item->item_instance)->id_item_template]->id_tex), SS_NORMAL, item->matrix);
	}
}
void DrawEditorPawn(void* ent)
{
	EditorPawn* chr = (EditorPawn*)ent;

	// need a good way of knowing own index
	DrawMesh(chr->id, res::GetM(res::m_debug_bb), res::GetT(res::t_col_red), SS_NORMAL, chr->t_body.getMatrix());

	// draw head

	//DrawBlendMesh(index, res::GetMB(res::mb_char_head), 0, res::GetT(chr->t_skin), SS_CHARA, t_head.getMatrix());
}


void ActiveState::Damage(btui32 amount, btf32 angle)
{
	//aud::PlaySnd(aud::FILE_SWING_CONNECT, );
	// if it's enough to reduce us to or below zero
	if (amount >= damagestate)
	{
		//stateFlags.unset((ActiveFlags)(eALIVE | eDIED_THIS_TICK));
		stateFlags.unset(eALIVE);
		stateFlags.set(eDIED_REPORT);
		damagestate = 0u;
	}
	else
	{
		damagestate -= amount;
	}
	// TODO: include AI 'notify attack' function call here
}
void ActiveState::AddEffect(btID caster, StatusEffectType type, btf32 duration, btui32 magnitude)
{
	StatusEffect effect;
	effect.effect_caster_id = caster;
	effect.effect_type = type;
	effect.effect_duration = duration;
	effect.effect_magnitude = magnitude;
	effects.Add(effect);

	// TODO: include AI 'notify attack' function call here
}
void ActiveState::AddSpell(btID caster, btID spell)
{
	AddEffect(0, (StatusEffectType)acv::spells[spell].target_effect_type,
		acv::spells[spell].target_effect_duration,
		acv::spells[spell].target_effect_magnitude);
	// TODO: this only works in the case of cast on self, should deal with this properly but dont know how best to yet
	if (core::players[0] == caster)
	{
		char string[64] = "Got ";
		strcat(string, (char*)acv::spells[spell].name);
		core::GUISetMessag(0, string);
	}
	else if (core::players[1] == caster)
	{
		char string[64] = "Got ";
		strcat(string, (char*)acv::spells[spell].name);
		core::GUISetMessag(1, string);
	}
}
void ActiveState::TickEffects(btf32 dt)
{
	int jshdf = effects.Size();
	for (btui32 i = 0; i < effects.Size(); ++i)
	{
		if (effects.Used(i))
		{
			switch (effects[i].effect_type)
			{
			case EFFECT_DAMAGE_HP:
				// magnitude multiplied by delta time so that it functions as a value per second
				Damage(effects[i].effect_magnitude, 0.f);
				break;
			case EFFECT_RESTORE_HP:
				damagestate += (btui16)effects[i].effect_magnitude;
				if (damagestate >= STATE_DAMAGE_MAX) damagestate = STATE_DAMAGE_MAX;
				break;
			}
			effects[i].effect_duration -= dt; // tick down the effect timer
			if (effects[i].effect_duration < 0.f) // if the timer has run out
			{
				effects.Remove(i); // remove the effect
			}
		}
	}
}


void TickRestingItem(void* ent, btf32 dt)
{
	RestingItem* chr = (RestingItem*)ent;

	if (chr->state.stateFlags.get(ActiveState::eDIED_REPORT))
	{
		//chr->state.stateFlags.unset(ActiveState::eDIED_REPORT);
		core::DestroyEntity(chr->id);
	}

	chr->matrix = graphics::Matrix4x4();
	graphics::MatrixTransform(chr->matrix, m::Vector3(chr->t.position.x, chr->t.height + acv::items[((HeldItem*)GetItemPtr(chr->item_instance))->id_item_template]->f_model_height, chr->t.position.y), chr->t.yaw.Rad());

	Entity_PhysicsTick(chr, chr->id, dt);
}

void Actor::TakeItem(btID id)
{
	RestingItem* item = (RestingItem*)GetEntityPtr(id);
	HeldItem* item_held = GETITEMINST(item->item_instance);
	btui32 slot_added = inventory.TransferItemRecv(item->item_instance);
	core::DestroyEntity(id);
	if (slot_added == inv_active_slot)
		ItemOnEquip(inventory.items[inv_active_slot], this);
	if (core::players[0] == this->id)
	{
		char string[64] = "Picked up ";
		strcat(string, (char*)(acv::BaseItem*)acv::items[item_held->id_item_template]->name);
		core::GUISetMessag(0, string);
	}
	else if (core::players[1] == this->id)
	{
		char string[64] = "Picked up ";
		strcat(string, (char*)(acv::BaseItem*)acv::items[item_held->id_item_template]->name);
		core::GUISetMessag(1, string);
	}
}

void Actor::DropItem(btID slot)
{
	if (slot < inventory.items.Size() && inventory.items.Used(slot))
	{
		m::Vector2 throwDir = m::AngToVec2(t.yaw.Rad());
		btID item_entity = core::SpawnEntityItem(inventory.items[slot], t.position + (throwDir * radius), t.yaw.Deg());
		//btID item_entity = index::SpawnEntityItem(inventory.items[slot], t.position + (throwDir * (radius + 0.5f)), t.yaw.Deg());
		ENTITY(item_entity)->t.velocity = throwDir * 0.05f;
		inventory.TransferItemSendIndex(slot);
		DecrEquipSlot();
	}
}

void Actor::DropAllItems()
{
	for (btui32 slot = 0u; slot < INV_SIZE; ++slot)
	{
		if (slot < inventory.items.Size() && inventory.items.Used(slot))
		{
			core::SpawnEntityItem(inventory.items[slot], m::Vector2(
				m::Random(t.position.x - 0.5f, t.position.x + 0.5f),
				m::Random(t.position.y - 0.5f, t.position.y + 0.5f)),
				m::Random(0.f, 360.f));
			inventory.TransferItemSendIndex(slot);
			DecrEquipSlot();
		}
	}
}

void Actor::SetEquipSlot(btui32 index)
{
	if (index < inventory.items.Size())
	{
		inv_active_slot = index;
		if (inventory.items.Used(inv_active_slot))
			ItemOnEquip(inventory.items[inv_active_slot], this);
	}
}

void Actor::IncrEquipSlot()
{
	if (inv_active_slot < inventory.items.Size() - 1u)
	{
		++inv_active_slot;
		if (inventory.items.Used(inv_active_slot))
			ItemOnEquip(inventory.items[inv_active_slot], this);
	}
}

void Actor::DecrEquipSlot()
{
	if (inv_active_slot > 0u)
	{
		--inv_active_slot;
		if (inventory.items.Used(inv_active_slot))
			ItemOnEquip(inventory.items[inv_active_slot], this);
	}
}

//#define LEGLEN(a,x) 0.75f
//#define ARMLEN(a,x) 0.75f
#define ARMLEN(a,x) acv::actor_templates[a].leng_arm[x]
#define LEGLEN(a,x) acv::actor_templates[a].leng_leg[x]
// leg disconnect length? not sure
#define legDClen ARMLEN(0,0)

//#define bodylen 0.65
#define BODYLEN acv::actor_templates[0].leng_body[0]

//#define hip_width 0.125f
#define hip_width acv::actor_templates[0].jpos_arm_rt[0]

#define velocityStepMult (8.f * LEGLEN(chr->actorBase,0)) // How far to place our foot ahead when walking

m::Vector3 Actor_SetFootPos(m::Vector2 position)
{
	CellSpace cs;
	core::GetCellSpaceInfo(position, cs);
	btf32 height;
	env::GetHeight(height, cs);
	return m::Vector3(position.x, height + 0.075f, position.y);
}
void Actor_ClampLegs(Actor* chr)
{
	// duplicate
	m::Vector3 newpos = chr->t_body.GetPosition();
	m::Vector3 jointPosR = newpos + chr->t_body.GetRight() * hip_width;
	m::Vector3 jointPosL = newpos + chr->t_body.GetRight() * -hip_width;
	// Foot position distance check
	if (m::Length(chr->fpCurrentL - jointPosL) > LEGLEN(chr->actorBase, 0))
		chr->fpCurrentL = jointPosL + m::Normalize(chr->fpCurrentL - jointPosL) * LEGLEN(chr->actorBase, 0);
	if (m::Length(chr->fpCurrentR - jointPosR) > LEGLEN(chr->actorBase, 0))
		chr->fpCurrentR = jointPosR + m::Normalize(chr->fpCurrentR - jointPosR) * LEGLEN(chr->actorBase, 0);
}
void Actor_AnimateLegs(Actor* chr)
{
	m::Vector2 vecfw = m::AngToVec2(chr->t.yaw.Rad());
	m::Vector2 vecrt = m::Vector2(-vecfw.y, vecfw.x);

	bool isInputting = m::Length(chr->input) >= 0.01f;

	m::Vector3 tempFPL(0.f);
	m::Vector3 tempFPR(0.f);

	m::Vector2 velocityOffset = chr->t.velocity - chr->slideVelocity;
	m::Vector2 voNorm = m::Normalize(velocityOffset);

	// If on the ground
	if (chr->grounded) {
		chr->lastGroundFootPos = Actor_SetFootPos(chr->t.position);
		// If we're standing still, play idle anim
		if (!isInputting) {
			if (chr->foot_state == Actor::eL_DOWN) {
				tempFPL = Actor_SetFootPos(chr->t.position + (vecrt * 0.15f) + (vecfw * -0.15f));
				tempFPR = Actor_SetFootPos(chr->t.position + (vecrt * -0.15f) + (vecfw * 0.15f));
				chr->aniTimer = 0.5f;
			}
			if (chr->foot_state == Actor::eR_DOWN) {
				tempFPL = Actor_SetFootPos(chr->t.position + (vecrt * 0.15f) + (vecfw * 0.15f));
				tempFPR = Actor_SetFootPos(chr->t.position + (vecrt * -0.15f) + (vecfw * -0.15f));
				chr->aniTimer = 1.5f;
			}
		}
		// If walking
		else {
			#define STEP_LEN 0.35f
			// Tick animation timer
			chr->aniTimer += (0.5f / STEP_LEN) * m::Length(velocityOffset);
			if (chr->aniTimer > 2.f) chr->aniTimer -= 2.f;
			// Create foot target positions
			if (chr->aniTimer < 1.f) {
				tempFPL = Actor_SetFootPos(chr->t.position + (vecrt * 0.05f) + (voNorm * m::Lerp(-STEP_LEN, STEP_LEN, chr->aniTimer)));
				tempFPR = Actor_SetFootPos(chr->t.position + (vecrt * -0.05f) + (voNorm * m::Lerp(STEP_LEN, -STEP_LEN, chr->aniTimer)));
				tempFPL.y += m::QuadraticFootstep(0.3f, chr->aniTimer);
				chr->foot_state = Actor::eR_DOWN;
			}
			else {
				tempFPL = Actor_SetFootPos(chr->t.position + (vecrt * 0.05f) + (voNorm * m::Lerp(STEP_LEN, -STEP_LEN, chr->aniTimer - 1.f)));
				tempFPR = Actor_SetFootPos(chr->t.position + (vecrt * -0.05f) + (voNorm * m::Lerp(-STEP_LEN, STEP_LEN, chr->aniTimer - 1.f)));
				tempFPR.y += m::QuadraticFootstep(0.3f, chr->aniTimer - 1.f);
				chr->foot_state = Actor::eL_DOWN;
			}
			#undef STEP_LEN
		}
	}
	// If in the air (can be due to jumping, falling, or running)
	else {
		// Set left foot
		if (chr->foot_state == Actor::eL_DOWN) {
			if (chr->t.height_velocity > 0.f)
				tempFPR = chr->lastGroundFootPos;
			else
				tempFPR = chr->t_body.GetPosition() + m::Vector3(
					chr->t.velocity.x * -velocityStepMult * 0.5f,
					LEGLEN(chr->actorBase, 0) * -0.4f,
					chr->t.velocity.y * -velocityStepMult * 0.5f);
			tempFPL = chr->t_body.GetPosition() + m::Vector3(
				chr->t.velocity.x * -velocityStepMult * -0.5f,
				LEGLEN(chr->actorBase, 0) * -0.65f,
				chr->t.velocity.y * -velocityStepMult * -0.5f);
			chr->aniTimer = 0.f;
		}
		// Set right foot
		else {
			if (chr->t.height_velocity > 0.f)
				tempFPL = chr->lastGroundFootPos;
			else
				tempFPL = chr->t_body.GetPosition() + m::Vector3(
					chr->t.velocity.x * -velocityStepMult * 0.5f,
					LEGLEN(chr->actorBase, 0) * -0.4f,
					chr->t.velocity.y * -velocityStepMult * 0.5f);
			tempFPR = chr->t_body.GetPosition() + m::Vector3(
				chr->t.velocity.x * -velocityStepMult * -0.5f,
				LEGLEN(chr->actorBase, 0) * -0.65f,
				chr->t.velocity.y * -velocityStepMult * -0.5f);
			chr->aniTimer = 1.f;
		}
	}

	// Apply new positions (interpolated)
	chr->fpCurrentL = m::Lerp(chr->fpCurrentL, tempFPL, 0.5f);
	chr->fpCurrentR = m::Lerp(chr->fpCurrentR, tempFPR, 0.5f);

	// Clamp positions so they never exceed the length of a leg in distance
	Actor_ClampLegs(chr);
}
void TickChara(void* ent, btf32 dt)
{
	Actor* chr = (Actor*)ent;

	chr->input = m::Normalize(chr->input);

	// apply crouch input
	if (chr->inputBV.get(Actor::IN_CROUCH))
		chr->aniCrouch = !chr->aniCrouch;
	chr->aniRun = chr->inputBV.get(Actor::IN_RUN);

	if (chr->aniCrouch)
		chr->aniStandHeight = m::Lerp(chr->aniStandHeight, LEGLEN(chr->actorBase, 0) * 0.4f, 0.4f);
	else
		chr->aniStandHeight = m::Lerp(chr->aniStandHeight, LEGLEN(chr->actorBase, 0) * 0.9f, 0.4f);

	btf32 slide = (m::Length(chr->slideVelocity) / dt) * 0.5f;
	if (slide > 1.f) chr->aniSlideResponse = m::Lerp(chr->aniSlideResponse, 1.f, 0.3f);
	else chr->aniSlideResponse = m::Lerp(chr->aniSlideResponse, slide, 0.3f);

	// Jump
	if (chr->inputBV.get(Actor::IN_JUMP)) {
		if (chr->grounded) {
			// Enter jump
			chr->t.height_velocity = 0.1f;
			chr->t.velocity = chr->input * dt * chr->speed + chr->slideVelocity;
			chr->aniCrouch = false;
			chr->jump_state = Actor::eJUMP_JUMP;
		}
		/* // enable to turn on hover jumping
		else if (chr->jump_state == Actor::eJUMP_JUMP)
			chr->t.height_velocity += 0.005f; // hover
		*/
		else if (chr->jump_state == Actor::eJUMP_SPRINT)
			chr->t.height_velocity -= 0.01f; // anti-hover
	}
	// Sprint
	else if (chr->grounded && chr->aniRun && m::Length(chr->t.velocity) > 0.5f * dt) {
		// Enter sprint jump
		chr->t.height_velocity = 0.038f; // enter jump
		chr->t.velocity = chr->input * dt * (chr->speed * 1.1f) + (chr->slideVelocity * 0.9f);
		chr->aniCrouch = false;
		chr->jump_state = Actor::eJUMP_SPRINT;
	}

	bool can_move = true;
	bool can_turn = true;
	if (chr->inventory.items.Used(chr->inv_active_slot))
	{
		can_move = !ItemBlockMove(chr->inventory.items[chr->inv_active_slot]);
		can_turn = !ItemBlockMove(chr->inventory.items[chr->inv_active_slot]);
	}

	if (chr->state.stateFlags.get(ActiveState::eDIED_REPORT))
	{
		chr->DropAllItems();
		chr->state.stateFlags.unset(ActiveState::eDIED_REPORT);
	}
	if (chr->state.stateFlags.get(ActiveState::eALIVE))
	{
		if (chr->grounded)
		{
			if (can_move)
			{
				if (!chr->aniCrouch)
					chr->t.velocity = m::Lerp(chr->t.velocity, chr->input * dt * chr->speed, 0.3f);
				else
					chr->t.velocity = m::Lerp(chr->t.velocity, chr->input * dt * chr->speed * 0.5f, 0.3f);
			}
			else
			{
				chr->input = m::Vector2(0.f, 0.f);
				chr->t.velocity = m::Lerp(chr->t.velocity, m::Vector2(0.f, 0.f), 0.2f);
			}
		}
		if (can_turn)
		{
			btf32 angdif = abs(m::AngDif(chr->t.yaw.Deg(), chr->viewYaw.Deg()));
			if (angdif > 85.f)
				if (angdif - 85.f < 8.f)
					chr->t.yaw.RotateTowards(chr->viewYaw.Deg(), angdif - 85.f); // Rotate body towards the target direction
				else
					chr->t.yaw.RotateTowards(chr->viewYaw.Deg(), 8.f); // Rotate body towards the target direction
			else if (m::Length(chr->input) > 0.2f)
			{
				chr->t.yaw.RotateTowards(chr->viewYaw.Deg(), 8.f); // Rotate body towards the target direction
			}
		}

		//-------------------------------- APPLY MOVEMENT

		Entity_PhysicsTick(chr, chr->id, dt);

		//-------------------------------- RUN AI FUNCTION

		// if AI controlled run the AI function
		if (chr->aiControlled) NPCTick(chr->id);
		// if player controlled, just aim at whatever's in front of us
		else chr->atk_target = core::GetViewTargetEntity(chr->id, 100.f, fac::enemy);

		//-------------------------------- RUN ITEM TICK

		if (chr->inventory.items.Used(chr->inv_active_slot))
			ItemTick(chr->inventory.items[chr->inv_active_slot], dt, chr);
	} // End if alive

	//________________________________________________________________
	//------------- SET TRANSFORMATIONS FOR GRAPHICS -----------------

	if (chr->state.stateFlags.get(ActiveState::eALIVE))
	{
		// Reset transforms
		chr->t_body = Transform3D();
		chr->t_head = Transform3D();

		// Set head transform
		if (!chr->aniCrouch)
			chr->t_head.SetPosition(m::Vector3(chr->t.position.x, 0.1f + chr->t.height - (chr->aniSlideResponse * 0.25f) + BODYLEN * 0.99f, chr->t.position.y));
		else
			chr->t_head.SetPosition(m::Vector3(chr->t.position.x, 0.1f + chr->t.height - (chr->aniSlideResponse * 0.25f) + BODYLEN * 0.85f, chr->t.position.y));
		
		chr->t_head.Rotate(chr->viewYaw.Rad(), m::Vector3(0, 1, 0));
		chr->t_head.Rotate(chr->viewPitch.Rad(), m::Vector3(1, 0, 0));
		chr->t_head.SetScale(m::Vector3(0.95f, 0.95f, 0.95f));

		chr->ani_body_lean = m::Lerp(chr->ani_body_lean, chr->t.position, 0.3f);

		// Set body transform
		//t_body.SetPosition(m::Vector3(chr->t.position.x, 0.1f + chr->t.height + 0.6f - (m::Length(chr->slideVelocity) * 0.5f), chr->t.position.y));
		chr->t_body.SetPosition(m::Vector3(chr->ani_body_lean.x, 0.1f + chr->t.height - (chr->aniSlideResponse * 0.125f), chr->ani_body_lean.y));
		chr->t_body.Rotate(chr->t.yaw.Rad(), m::Vector3(0, 1, 0));


		// Head position distance check
		//if (m::Length(chr->t_body.GetPosition() - chr->t_head.GetPosition()) > bodylen)
		//	chr->t_head.SetPosition(chr->t_body.GetPosition() + m::Normalize(chr->t_body.GetPosition() - chr->t_head.GetPosition()) * bodylen);
		if (m::Length(chr->t_body.GetPosition() - chr->t_head.GetPosition()) > BODYLEN)
			chr->t_head.SetPosition(m::Lerp(chr->t_head.GetPosition(), chr->t_body.GetPosition() + m::Normalize(chr->t_body.GetPosition() - chr->t_head.GetPosition()) * BODYLEN, 0.001f));

		//-------------------------------- HANDLE ANIMATION

		Actor_AnimateLegs(chr);
	}
	else
	{
		// Reset transforms
		chr->t_body = Transform3D();
		chr->t_head = Transform3D();

		chr->t_body.SetPosition(m::Vector3(chr->t.position.x, chr->t.height, chr->t.position.y));
		chr->t_body.Rotate(chr->t.yaw.Rad(), m::Vector3(0, 1, 0));
		chr->t_body.Rotate(glm::radians(90.f), m::Vector3(1, 0, 0));

		// Set head transform
		chr->t_head.SetPosition(chr->t_body.GetPosition());
		chr->t_head.Rotate(chr->viewYaw.Rad(), m::Vector3(0, 1, 0));
		//t_head.Rotate(viewPitch.Rad(), m::Vector3(1, 0, 0));
		chr->t_head.Translate(chr->t_body.GetUp() * BODYLEN);
	}
}
void DrawChara(void* ent)
{
	Actor* chr = (Actor*)ent;

	graphics::Matrix4x4 matLegHipR, matLegUpR, matLegLoR, matLegFootR;
	graphics::Matrix4x4 matLegHipL, matLegUpL, matLegLoL, matLegFootL;
	graphics::Matrix4x4 matBodyUp, matBodyLo;

	#define tr_body chr->t_body
	#define viewYaw chr->viewYaw
	#define speed chr->speed
	#define inventory chr->inventory
	#define inv_active_slot chr->inv_active_slot
	#define viewPitch chr->viewPitch
	#define foot_state chr->foot_state
	#define ani_body_lean chr->ani_body_lean
	#define tr_head chr->t_head

	Transform3D t_upperbody = tr_body;
	t_upperbody.Rotate(glm::radians(m::AngDif(viewYaw.Deg(), chr->t.yaw.Deg())), m::Vector3(0, 1, 0));

	btf32 lerpAmt = 0.05f * speed;

	//-------------------------------- SET ACTOR SHADER PARAMETERS

	graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA).Use();
	graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA).setVec3(graphics::Shader::Colour_A, chr->skin_col_a.x, chr->skin_col_a.y, chr->skin_col_a.z);
	graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA).setVec3(graphics::Shader::Colour_B, chr->skin_col_b.x, chr->skin_col_b.y, chr->skin_col_b.z);
	graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA).setVec3(graphics::Shader::Colour_C, chr->skin_col_c.x, chr->skin_col_c.y, chr->skin_col_c.z);

	//-------------------------------- DRAW BODY

	btf32 len = m::Length(tr_body.GetPosition() - tr_head.GetPosition());
	if (len > BODYLEN) len = BODYLEN;
	btf32 lenUp = sqrtf(BODYLEN * BODYLEN - len * len); // Pythagorean theorem
	m::Vector3 vecfw = m::Normalize(tr_body.GetPosition() - tr_head.GetPosition());
	m::Vector3 vecside = m::Normalize(m::Cross(vecfw, tr_body.GetForward()));
	m::Vector3 vecup = m::Normalize(m::Cross(vecfw, vecside));
	m::Vector3 vecside_upper = m::Normalize(m::Cross(vecfw, tr_head.GetForward()));
	m::Vector3 vecup_upper = (m::Normalize(m::Cross(vecfw, vecside_upper)) + vecup);
	graphics::MatrixTransform(matBodyLo, tr_body.GetPosition(), m::Normalize(vecfw * len - vecup * lenUp), vecup * -1.f);
	graphics::MatrixTransform(matBodyUp,
		tr_body.GetPosition() + (vecup * lenUp),
			m::Normalize(vecfw * len + vecup * lenUp),
			vecup_upper * -1.f);
	DrawMeshDeform(chr->id, res::GetMD(acv::actor_templates[chr->actorBase].m_body), res::GetT(acv::actor_templates[chr->actorBase].t_body), SS_CHARA, 2u,
		matBodyLo, matBodyUp, graphics::Matrix4x4(), graphics::Matrix4x4());

	#ifndef DEF_NMP
	if (chr->state.stateFlags.get(ActiveState::eALIVE))
	{
		//DrawMeshDeform(chr->id, res::GetMD(res::md_equip_body_robe_01), res::GetT(res::t_equip_body_robe_01), SS_NORMAL, 4u,
		//matBodyLo, matBodyUp, matLegUpL, matLegUpR);
	}
	#endif

	//-------------------------------- DRAW ARMS

	graphics::Matrix4x4 matrix;

	//m::Vector3 newpos2 = t_body.GetPosition() + t_body.GetUp() * len;
	m::Vector3 newpos2 = graphics::MatrixGetPosition(matBodyUp) - graphics::MatrixGetForward(matBodyUp) * (BODYLEN - 0.1f);

	btf32 shoulder_width = 0.12f;

	m::Vector3 jointPosR = newpos2 + graphics::MatrixGetRight(matBodyUp) * shoulder_width;
	m::Vector3 jointPosL = newpos2 + graphics::MatrixGetRight(matBodyUp) * -shoulder_width;

	//-------------------------------- DRAW ARMS

	if (inventory.items.Used(inv_active_slot))
	{
		HeldItem* heldItem = ((HeldItem*)GetItemPtr(inventory.items[inv_active_slot]));

		// Draw held item
		ItemDraw(inventory.items[inv_active_slot],
			GETITEMINST(inventory.items[inv_active_slot])->id_item_template,
			chr->t.position, chr->t.height + 0.3f, viewYaw, viewPitch);

		// Get hand positions
		m::Vector3 handPosR = ItemRHPos(inventory.items[inv_active_slot]);
		m::Vector3 handPosL = ItemLHPos(inventory.items[inv_active_slot]);

		// Arm orientation vectors
		#define bodyForwardR m::Normalize((t_upperbody.GetRight() * -1.f) + t_upperbody.GetUp() * 0.75f + t_upperbody.GetForward() * 0.5f)
		#define bodyForwardL m::Normalize(t_upperbody.GetRight() + t_upperbody.GetUp() * 0.75f + t_upperbody.GetForward() * 0.5f)
		//#define bodyForwardR m::Normalize((graphics::MatrixGetRight(matBodyUp) * -1.f) + graphics::MatrixGetUp(matBodyUp) * 0.75f + graphics::MatrixGetForward(matBodyUp)* 0.5f)
		//#define bodyForwardL m::Normalize(graphics::MatrixGetRight(matBodyUp) * 1.f +    graphics::MatrixGetUp(matBodyUp) * 0.75f + graphics::MatrixGetForward(matBodyUp)* 0.5f)


		// Arm right
		len = m::Length(jointPosR - handPosR);
		if (len > ARMLEN(chr->actorBase,0)) len = ARMLEN(chr->actorBase,0);
		lenUp = sqrtf(ARMLEN(chr->actorBase,0) * ARMLEN(chr->actorBase,0) - len * len); // Pythagorean theorem
		m::Vector3 vecfw = m::Normalize(handPosR - jointPosR);
		m::Vector3 vecside = m::Normalize(m::Cross(vecfw, bodyForwardR));
		m::Vector3 vecup = m::Normalize(m::Cross(vecfw, vecside)) * -1.f;
		graphics::MatrixTransformXFlip(matLegUpR, jointPosR, m::Normalize(vecfw * len - vecup * lenUp), vecup);
		graphics::MatrixTransformXFlip(matLegLoR, jointPosR - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup);
		graphics::MatrixTransformXFlip(matLegFootR, jointPosR - vecfw * (ARMLEN(chr->actorBase,0) - len), vecfw, vecup);
		// Draw arm
		graphics::SetFrontFaceInverse();
		DrawMeshDeform(chr->id, res::GetMD(acv::actor_templates[chr->actorBase].m_arm), res::GetT(acv::actor_templates[chr->actorBase].t_arm), SS_CHARA, 4u, matBodyUp, matLegUpR, matLegLoR, matLegFootR);
		graphics::SetFrontFace();

		// Arm left
		len = m::Length(jointPosL - handPosL);
		if (len > ARMLEN(chr->actorBase,0)) len = ARMLEN(chr->actorBase,0);
		lenUp = sqrtf(ARMLEN(chr->actorBase,0) * ARMLEN(chr->actorBase,0) - len * len); // Pythagorean theorem
		vecfw = m::Normalize(handPosL - jointPosL);
		vecside = m::Normalize(m::Cross(vecfw, bodyForwardL));
		vecup = m::Normalize(m::Cross(vecfw, vecside) * -1.f);
		graphics::MatrixTransform(matLegUpL, jointPosL, m::Normalize(vecfw * len - vecup * lenUp), vecup);
		graphics::MatrixTransform(matLegLoL, jointPosL - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup);
		graphics::MatrixTransform(matLegFootL, jointPosL - vecfw * (ARMLEN(chr->actorBase,0) - len), vecfw, vecup);
		// Draw arm
		DrawMeshDeform(chr->id, res::GetMD(acv::actor_templates[chr->actorBase].m_arm), res::GetT(acv::actor_templates[chr->actorBase].t_arm), SS_CHARA, 4u, matBodyUp, matLegUpL, matLegLoL, matLegFootL);

		#undef bodyForwardR 
		#undef bodyForwardL
	}
	else
	{
		m::Vector2 fpoffs1 = chr->t.position - m::Vector2(chr->fpCurrentL.x, chr->fpCurrentL.z);
		m::Vector2 fpoffs2 = chr->t.position - m::Vector2(chr->fpCurrentR.x, chr->fpCurrentR.z);

		m::Vector3 armoffsL;
		m::Vector3 armoffsR;
		if (!chr->aniCrouch)
		{
			armoffsL = m::Normalize(m::Vector3(fpoffs1.x, -0.9f, fpoffs1.y) - graphics::MatrixGetRight(matBodyUp) * 0.25f) * ARMLEN(chr->actorBase,0) * 0.98f;
			armoffsR = m::Normalize(m::Vector3(fpoffs2.x, -0.9f, fpoffs2.y) + graphics::MatrixGetRight(matBodyUp) * 0.25f) * ARMLEN(chr->actorBase,0) * 0.98f;
		}
		else
		{
			armoffsL = m::Normalize(m::Vector3(fpoffs1.x, -0.9f, fpoffs1.y) - graphics::MatrixGetRight(matBodyUp) * 0.6f) * ARMLEN(chr->actorBase,0) * 0.75f;
			armoffsR = m::Normalize(m::Vector3(fpoffs2.x, -0.9f, fpoffs2.y) + graphics::MatrixGetRight(matBodyUp) * 0.6f) * ARMLEN(chr->actorBase,0) * 0.75f;
		}

		// Make hand positions
		m::Vector3 handPosR = jointPosR + armoffsR;
		m::Vector3 handPosL = jointPosL + armoffsL;

		// Arm orientation vectors
		#define bodyForwardR m::Normalize(graphics::MatrixGetRight(matBodyUp) * -0.25f + graphics::MatrixGetUp(matBodyUp) * 0.75f + graphics::MatrixGetForward(matBodyUp)* 0.5f)
		#define bodyForwardL m::Normalize(graphics::MatrixGetRight(matBodyUp) *  0.25f + graphics::MatrixGetUp(matBodyUp) * 0.75f + graphics::MatrixGetForward(matBodyUp)* 0.5f)

		// Arm right
		len = m::Length(jointPosR - handPosR);
		if (len > ARMLEN(chr->actorBase,0)) len = ARMLEN(chr->actorBase,0);
		lenUp = sqrtf(ARMLEN(chr->actorBase,0) * ARMLEN(chr->actorBase,0) - len * len); // Pythagorean theorem
		m::Vector3 vecfw = m::Normalize(handPosR - jointPosR);
		m::Vector3 vecside = m::Normalize(m::Cross(vecfw, bodyForwardR));
		m::Vector3 vecup = m::Normalize(m::Cross(vecfw, vecside)) * -1.f;
		graphics::MatrixTransformXFlip(matLegUpR, jointPosR, m::Normalize(vecfw * len - vecup * lenUp), vecup);
		graphics::MatrixTransformXFlip(matLegLoR, jointPosR - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup);
		graphics::MatrixTransformXFlip(matLegFootR, jointPosR - vecfw * (ARMLEN(chr->actorBase,0) - len), vecfw, vecup);
		// Draw arm
		graphics::SetFrontFaceInverse();
		DrawMeshDeform(chr->id, res::GetMD(acv::actor_templates[chr->actorBase].m_arm), res::GetT(acv::actor_templates[chr->actorBase].t_arm), SS_CHARA, 4u, matBodyUp, matLegUpR, matLegLoR, matLegFootR);
		graphics::SetFrontFace();

		// Arm left
		len = m::Length(jointPosL - handPosL);
		if (len > ARMLEN(chr->actorBase,0)) len = ARMLEN(chr->actorBase,0);
		lenUp = sqrtf(ARMLEN(chr->actorBase,0) * ARMLEN(chr->actorBase,0) - len * len); // Pythagorean theorem
		vecfw = m::Normalize(handPosL - jointPosL);
		vecside = m::Normalize(m::Cross(vecfw, bodyForwardL));
		vecup = m::Normalize(m::Cross(vecfw, vecside) * -1.f);
		graphics::MatrixTransform(matLegUpL, jointPosL, m::Normalize(vecfw * len - vecup * lenUp), vecup);
		graphics::MatrixTransform(matLegLoL, jointPosL - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup);
		graphics::MatrixTransform(matLegFootL, jointPosL - vecfw * (ARMLEN(chr->actorBase,0) - len), vecfw, vecup);
		// Draw arm
		DrawMeshDeform(chr->id, res::GetMD(acv::actor_templates[chr->actorBase].m_arm), res::GetT(acv::actor_templates[chr->actorBase].t_arm), SS_CHARA, 4u, matBodyUp, matLegUpL, matLegLoL, matLegFootL);

		#undef bodyForwardR 
		#undef bodyForwardL
	}

	//-------------------------------- DRAW LEGS

	if (chr->state.stateFlags.get(ActiveState::eALIVE))
	{
		btf32 velocityAmt = m::Length(chr->t.velocity - chr->slideVelocity);

		m::Vector3 newpos = tr_body.GetPosition();

		jointPosR = newpos + graphics::MatrixGetRight(matBodyLo) * hip_width;
		jointPosL = newpos + graphics::MatrixGetRight(matBodyLo) * -hip_width;

		// generate matrices

		// Leg right
		btf32 len = m::Length(jointPosR - chr->fpCurrentR);
		if (len > LEGLEN(chr->actorBase,0)) len = LEGLEN(chr->actorBase,0);
		btf32 lenUp = sqrtf(LEGLEN(chr->actorBase,0) * LEGLEN(chr->actorBase,0) - len * len); // Pythagorean theorem
		m::Vector3 vecfw = m::Normalize(chr->fpCurrentR - jointPosR);
		m::Vector3 vecside = m::Normalize(m::Cross(vecfw, (m::Normalize(tr_body.GetForward() + tr_body.GetRight() * 0.23f))));
		m::Vector3 vecup = m::Normalize(m::Cross(vecfw, vecside));
		m::Vector3 vecup_inv = vecup * -1.f;
		graphics::MatrixTransformXFlip(matLegHipR, jointPosR, graphics::MatrixGetForward(matBodyLo), graphics::MatrixGetUp(matBodyLo));
		graphics::MatrixTransformXFlip(matLegUpR, jointPosR, m::Normalize(vecfw * len - vecup * lenUp), vecup_inv);
		graphics::MatrixTransformXFlip(matLegLoR, jointPosR - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup_inv);
		graphics::MatrixTransformXFlip(matLegFootR, chr->fpCurrentR + m::Vector3(0.f, LEGLEN(chr->actorBase,0), 0.f), m::Vector3(0.f, -1.f, 0.f), vecup_inv);
		// Draw leg
		graphics::SetFrontFaceInverse();
		DrawMeshDeform(chr->id, res::GetMD(acv::actor_templates[chr->actorBase].m_leg), res::GetT(res::t_equip_legs_robe_01), SS_NORMAL, 4u, matLegHipR, matLegUpR, matLegLoR, matLegFootR);
		graphics::SetFrontFace();
		// transform legR for cloak
		//graphics::MatrixTransformForwardUp(matLegUpR, t_body.GetPosition(), chr->fpCurrentR - t_body.GetPosition(), t_body.GetForward());

		// Leg left
		len = m::Length(jointPosL - chr->fpCurrentL);
		if (len > LEGLEN(chr->actorBase,0)) len = LEGLEN(chr->actorBase,0);
		lenUp = sqrtf(LEGLEN(chr->actorBase,0) * LEGLEN(chr->actorBase,0) - len * len); // Pythagorean theorem
		vecfw = m::Normalize(chr->fpCurrentL - jointPosL);
		vecside = m::Normalize(m::Cross(vecfw, (m::Normalize(tr_body.GetForward() + tr_body.GetRight() * -0.23f))));
		vecup = m::Normalize(m::Cross(vecfw, vecside));
		vecup_inv = vecup * -1.f;
		graphics::MatrixTransform(matLegHipL, jointPosL, graphics::MatrixGetForward(matBodyLo), graphics::MatrixGetUp(matBodyLo));
		graphics::MatrixTransform(matLegUpL, jointPosL, m::Normalize(vecfw * len - vecup * lenUp), vecup_inv);
		graphics::MatrixTransform(matLegLoL, jointPosL - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup_inv);
		graphics::MatrixTransform(matLegFootL, chr->fpCurrentL + m::Vector3(0.f, LEGLEN(chr->actorBase,0), 0.f), m::Vector3(0.f, -1.f, 0.f), vecup_inv);
		// Draw leg
		DrawMeshDeform(chr->id, res::GetMD(acv::actor_templates[chr->actorBase].m_leg), res::GetT(res::t_equip_legs_robe_01), SS_NORMAL, 4u, matLegHipL, matLegUpL, matLegLoL, matLegFootL);
		// transform legL for cloak
		//graphics::MatrixTransformForwardUp(matLegUpL, t_body.GetPosition(), chr->fpCurrentL - t_body.GetPosition(), t_body.GetForward());

	}

	//-------------------------------- DRAW HEAD

	Transform3D t2;
	t2.SetPosition(tr_body.GetPosition());
	t2.SetRotation(t_upperbody.GetRotation());
	t2.TranslateLocal(m::Vector3(0.f, 0.7f, 0.f));
	//DrawMeshDeform(chr->id, res::GetMD(res::md_char_head), res::GetT(chr->t_skin), SS_CHARA, 4u,
	//	t2.getMatrix(), t_head.getMatrix(), t_head.getMatrix(), t_head.getMatrix());
	DrawMeshDeform(chr->id, res::GetMD(acv::actor_templates[chr->actorBase].m_head), res::GetT(acv::actor_templates[chr->actorBase].t_head), SS_CHARA, 4u,
		t2.getMatrix(), tr_head.getMatrix(), tr_head.getMatrix(), tr_head.getMatrix());

	#undef t_body
	#undef viewYaw
	#undef t
	#undef speed
	#undef inventory
	#undef inv_active_slot
	#undef viewPitch
	#undef foot_state
	#undef footPosTargR
	#undef footPosTargL
	#undef ani_body_lean
	#undef t_head
}


void TickEditorPawn(void* ent, btf32 dt)
{
	EditorPawn* chr = (EditorPawn*)ent;

	#define t_body chr->t_body
	#define viewYaw chr->viewYaw
	#define speed chr->speed
	#define inventory chr->inventory
	#define inv_active_slot chr->inv_active_slot
	#define t_skin chr->t_skin
	#define viewPitch chr->viewPitch
	#define t_head chr->t_head
	#define state chr->state
	#define input chr->input
	#define aiControlled chr->aiControlled
	#define atk_target chr->atk_target

	bool can_move = true;
	bool can_turn = true;

	if (state.stateFlags.get(ActiveState::eALIVE))
	{
		chr->t.velocity = input * dt * 5.f;

		//-------------------------------- APPLY MOVEMENT

		//moving = (m::Length(chr->t.velocity) > 0.016f);
		m::Vector2 oldpos = chr->t.position;
		chr->t.position += chr->t.velocity; // Apply velocity

		CellSpace cs_last = chr->t.csi;

		//regenerate csi
		core::GetCellSpaceInfo(chr->t.position, chr->t.csi);

		//I don't want this to be here
		if (cs_last.c[eCELL_I].x != chr->t.csi.c[eCELL_I].x || cs_last.c[eCELL_I].y != chr->t.csi.c[eCELL_I].y)
		{
			core::RemoveEntityCell(cs_last.c[eCELL_I].x, cs_last.c[eCELL_I].y, chr->id);
			core::AddEntityCell(chr->t.csi.c[eCELL_I].x, chr->t.csi.c[eCELL_I].y, chr->id);
		}

		//-------------------------------- SET HEIGHT AND CELL SPACE

		btf32 height2;
		env::GetHeight(height2, chr->t.csi);
		chr->t.height = m::Lerp(chr->t.height, height2, 0.05f);

		//-------------------------------- RUN COLLISION & AI

	} // End if alive

	//________________________________________________________________
	//------------- SET TRANSFORMATIONS FOR GRAPHICS -----------------

	// Reset transforms
	t_body = Transform3D();
	t_head = Transform3D();

	t_body.SetPosition(m::Vector3(chr->t.position.x, 0.1f + chr->t.height + 0.7f, chr->t.position.y));
	t_body.Rotate(chr->t.yaw.Rad(), m::Vector3(0, 1, 0));

	// Set head transform
	t_head.SetPosition(t_body.GetPosition());
	t_head.Rotate(viewYaw.Rad(), m::Vector3(0, 1, 0));
	t_head.Rotate(viewPitch.Rad(), m::Vector3(1, 0, 0));
	t_head.Translate(t_body.GetUp() * 0.7f);
	t_head.SetScale(m::Vector3(0.95f, 0.95f, 0.95f));

	#undef t_body
	#undef viewYaw
	#undef speed
	#undef inventory
	#undef inv_active_slot
	#undef matLegHipR
	#undef matLegUpR
	#undef matLegLoR
	#undef matLegFootR
	#undef matLegHipL
	#undef matLegUpL
	#undef matLegLoL
	#undef matLegFootL
	#undef t_skin
	#undef viewPitch
	#undef foot_state
	#undef footPosTargR
	#undef footPosTargL
	#undef ani_body_lean
	#undef footPosR
	#undef footPosL
	#undef t_head
	#undef state
	#undef input
	//#undef moving
	#undef csi
	#undef aiControlled
	#undef atk_target
}
