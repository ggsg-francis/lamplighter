#include "objects_entities.h"
#include "objects_items.h"

#include "graphics.hpp"
#include <glm\gtc\matrix_transform.hpp>

#include "archive.hpp"

#include "core.h"
#include "cfg.h"

#include "audio.hpp"

#include "collision.h"


graphics::GUIText text_inventory_temp;
graphics::GUIBox guibox_selection;

void Inventory::AddNew(btID item_template)
{
	btID id = index::SpawnItem(item_template);
	if (id != BUF_NULL) items.Add(id);
}
void Inventory::DestroyIndex(btui32 index)
{
	items.Remove(index);
	index::DestroyItem(items[index]);
}
void Inventory::DestroyID(btID id)
{
	for (int i = 0; i < items.Size(); ++i)
	{
		if (items[i] == id) // if we already have a stack of this item
		{
			DestroyIndex(i);
			return;
		}
	}
}
void Inventory::Destroy(btID item_template)
{
	for (int i = 0; i < items.Size(); ++i)
	{
		if (GETITEM_MISC(items[i])->id_item_template == item_template) // if we already have a stack of this item
		{
			DestroyIndex(i);
			return;
		}
	}
}
void Inventory::TransferItemRecv(btID item_ID)
{
	items.Add(item_ID); // Add the item without creating a new instance
}
void Inventory::TransferItemSendIndex(btui32 index)
{
	items.Remove(index); // Remove the item from inventory without deleting the instance
}
void Inventory::TransferItemSend(btID item_ID)
{
	for (int i = 0; i < items.Size(); ++i)
	{
		if (items.Used(i))
		{
			if (items[i] == item_ID) // if we have this item instance
			{
				items.Remove(i); // Remove the item from inventory without deleting the instance
				return;
			}
		}
	}
}
btID Inventory::GetItemOfTemplate(btID item_template)
{
	for (int i = 0; i < items.Size(); ++i)
	{
		if (items.Used(i))
		{
			if (((HeldItem*)index::GetItemPtr(items[i]))->id_item_template == item_template) // if we have this item instance
			{
				return items[i];
			}
		}
	}
	return ID_NULL;
}
btID Inventory::GetItemOfAmmunitionType(btui8 ammo_type)
{
	for (int i = 0; i < items.Size(); ++i)
	{
		if (items.Used(i))
		{
			if (index::GetItemType(items[i]) == ITEM_TYPE_CONS)
			{
				// TODO: again, this is the fucking worst, i mean jus look at it...
				if (ammo_type == acv::projectiles[((acv::BaseItemCon*)acv::items[((HeldItem*)index::GetItemPtr(items[i]))->id_item_template])->id_projectile].ammunition_type)
				{
					return items[i];
				}
			}
		}
	}
	return ID_NULL;
}
void Inventory::Draw(btui16 active_slot)
{
	int p1_x_start = -(int)graphics::FrameSizeX() / 2;
	int p1_y_start = -(int)graphics::FrameSizeY() / 2;

	const bti32 invspace = 38;

	graphics::GUIText text;

	bti32 offset = p1_x_start + 96 - 16;
	for (btui16 i = 0; i < items.Size(); i++)
	{
		if (items.Used(i))
		{
			if (i == active_slot)
				graphics::DrawGUITexture(&res::GetT(acv::items[GETITEM_MISC(items[i])->id_item_template]->id_icon), offset + i * invspace, p1_y_start + 30, 64, 64);
			else
				graphics::DrawGUITexture(&res::GetT(acv::items[GETITEM_MISC(items[i])->id_item_template]->id_icon), offset + i * invspace, p1_y_start + 24, 64, 64);
		}
	}
	// Draw Count GUI on top
	for (btui16 i = 0; i < items.Size(); i++)
	{
		if (items.Used(i))
		{
			// get item type
			if (index::GetItemType(items[i]) == ITEM_TYPE_CONS)
			{
				char textbuffer[8];
				_itoa(GETITEM_CONS(items[i])->uses, textbuffer, 10);
				text.ReGen(textbuffer, offset + i * invspace - 16, offset + i * invspace + 32, p1_y_start + 20);
				text.Draw(&res::GetT(res::t_gui_font));
			}
		}
	}
	guibox_selection.ReGen((offset + active_slot * invspace) - 12, (offset + active_slot * invspace) + 12, p1_y_start + 12, p1_y_start + 36, 8, 8);
	guibox_selection.Draw(&res::GetT(res::t_gui_select_box));
	//itoa
}

void EntityTransformTick(Entity* ent, btID id, btf32 x, btf32 y, btf32 z)
{
	// Regenerate csi

	CellSpace cs_last = ent->t.csi;
	index::GetCellSpaceInfo(ent->t.position, ent->t.csi);
	// If the new CS is different, remove us from the last cell and add us to the new one
	if (cs_last.c[eCELL_I].x != ent->t.csi.c[eCELL_I].x || cs_last.c[eCELL_I].y != ent->t.csi.c[eCELL_I].y)
	{
		index::RemoveEntityCell(cs_last.c[eCELL_I].x, cs_last.c[eCELL_I].y, id);
		index::AddEntityCell(ent->t.csi.c[eCELL_I].x, ent->t.csi.c[eCELL_I].y, id);
	}

	ent->t.position += ent->slideVelocity;

	// Modify position
	btf32 eheight;

	btf32 distBelowSand;
	// look at how long this is
	switch (acv::props[env::eCells.prop[ent->t.csi.c[eCELL_I].x][ent->t.csi.c[eCELL_I].y]].floorType)
	{
	case acv::EnvProp::FLOOR_QUICKSAND:
		ent->t.height -= 0.0025f;
		env::GetHeight(eheight, ent->t.csi);
		// if above the ground
		if (ent->t.height + ent->t.height_velocity > eheight)
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
			distBelowSand = (eheight - ent->t.height) * 2.5f;
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
		if (!ent->grounded)
		{
			ent->t.height_velocity -= 0.006f; // Add gravity
			env::GetHeight(eheight, ent->t.csi);
			if (eheight > ent->t.height + ent->t.height_velocity)
			{
				ent->t.height = eheight;
				ent->t.height_velocity = 0.f;
				if (ent->properties.get(Entity::ePHYS_DRAG)) ent->t.velocity *= 0.f;
			}
			if (ent->properties.get(Entity::ePHYS_DRAG)) ent->t.velocity *= 0.99f;
			ent->slideVelocity *= 0.8f;
		}
		else
		{

		}
		ent->t.position += ent->t.velocity; // Apply velocity
		ent->t.height += ent->t.height_velocity; // Apply velocity
		break;
	}

	index::EntDeintersect(ent, ent->t.csi);
	
	CollideEntity(ent->id);
}

char* DisplayNameActor(void* ent)
{
	//return "Actor";
	return (char*)((Actor*)ent)->name;
};
char* DisplayNameRestingItem(void* ent)
{
	//return (char*)acv::items[GETITEM_MISC(((RestingItem*)index::GetEntityPtr(ent))->item_instance)->item_template]->name;
	return (char*)acv::items[GETITEM_MISC(((RestingItem*)ent)->item_instance)->id_item_template]->name;
};
void DrawRestingItem(void* ent)
{
	RestingItem* item = (RestingItem*)ent;
	// Draw the mesh of our item id
	//DrawMesh(ent, res::GetM(acv::items[index::GetItem(item->item_instance)->item_template]->id_mesh), res::GetT(acv::items[index::GetItem(item->item_instance)->item_template]->id_tex), SS_NORMAL, item->t_item.getMatrix());
	if (item->id == index::viewtarget[index::activePlayer])
	{
		graphics::GetShader(graphics::S_SOLID).Use();
		graphics::GetShader(graphics::S_SOLID).SetBool(graphics::Shader::bLit_TEMP, false);
		DrawMesh(item->id, res::GetM(acv::items[GETITEM_MISC(item->item_instance)->id_item_template]->id_mesh), res::GetT(acv::items[GETITEM_MISC(item->item_instance)->id_item_template]->id_tex), SS_NORMAL, item->matrix);
		graphics::GetShader(graphics::S_SOLID).SetBool(graphics::Shader::bLit_TEMP, true);
	}
	else
	{
		DrawMesh(item->id, res::GetM(acv::items[GETITEM_MISC(item->item_instance)->id_item_template]->id_mesh), res::GetT(acv::items[GETITEM_MISC(item->item_instance)->id_item_template]->id_tex), SS_NORMAL, item->matrix);
	}
}
m::Vector3 SetFootPos(m::Vector2 position)
{
	CellSpace cs;
	index::GetCellSpaceInfo(position, cs);
	btf32 height;
	env::GetHeight(height, cs);
	return m::Vector3(position.x, height + 0.125f, position.y);
}
void DrawEditorPawn(void* ent)
{
	Chara* chr = (Chara*)ent;

	// need a good way of knowing own index
	DrawMesh(chr->id, res::GetM(res::m_debug_bb), res::GetT(chr->t_skin), SS_NORMAL, chr->t_body.getMatrix());

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

	/*char string[64] = "Received ";

	switch (effect.effect_type)
	{
	case EFFECT_DAMAGE_HP:
		strcat_s(string, "Damage HP");
		break;
	case EFFECT_RESTORE_HP:
		strcat(string, "Restore HP");
		break;
	}

	index::GUISetMessag(0, string);*/
}
void ActiveState::AddSpell(btID caster, btID spell)
{
	AddEffect(0, (StatusEffectType)acv::spells[spell].target_effect_type,
		acv::spells[spell].target_effect_duration,
		acv::spells[spell].target_effect_magnitude);
	// TODO: this only works in the case of cast on self, should deal with this properly but dont know how best to yet
	if (index::players[0] == caster)
	{
		char string[64] = "Got ";
		strcat(string, (char*)acv::spells[spell].name);
		index::GUISetMessag(0, string);
	}
	else if (index::players[1] == caster)
	{
		char string[64] = "Got ";
		strcat(string, (char*)acv::spells[spell].name);
		index::GUISetMessag(1, string);
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
				damagestate += (btf32)effects[i].effect_magnitude;
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
		index::DestroyEntity(chr->id);
	}

	chr->matrix = graphics::Matrix4x4();
	graphics::MatrixTransform(chr->matrix, m::Vector3(chr->t.position.x, chr->t.height + acv::items[((HeldItem*)index::GetItemPtr(chr->item_instance))->id_item_template]->f_model_height, chr->t.position.y), chr->t.yaw.Rad());

	EntityTransformTick(chr, chr->id, 0.f, 0.f, 0.f);
}

void Actor::TakeItem(btID id)
{
	RestingItem* item = (RestingItem*)index::GetEntityPtr(id);
	HeldItem* item_held = GETITEM_MISC(item->item_instance);
	inventory.TransferItemRecv(item->item_instance);
	index::DestroyEntity(id);
	if (index::players[0] == this->id)
	{
		char string[64] = "Picked up ";
		strcat(string, (char*)(acv::BaseItem*)acv::items[item_held->id_item_template]->name);
		index::GUISetMessag(0, string);
	}
	else if (index::players[1] == this->id)
	{
		char string[64] = "Picked up ";
		strcat(string, (char*)(acv::BaseItem*)acv::items[item_held->id_item_template]->name);
		index::GUISetMessag(1, string);
	}
}

void Actor::DropItem(btID slot)
{
	if (slot < inventory.items.Size() && inventory.items.Used(slot))
	{
		m::Vector2 throwDir = m::AngToVec2(t.yaw.Rad());
		btID item_entity = index::SpawnEntityItem(inventory.items[slot], t.position + (throwDir * radius), t.yaw.Deg());
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
			index::SpawnEntityItem(inventory.items[slot], m::Vector2(
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
			GETITEM_MISC(inventory.items[inv_active_slot])->fpOnEquip(inventory.items[inv_active_slot], this);
	}
}

void Actor::IncrEquipSlot()
{
	if (inv_active_slot < inventory.items.Size() - 1u)
	{
		++inv_active_slot;
		if (inventory.items.Used(inv_active_slot))
			GETITEM_MISC(inventory.items[inv_active_slot])->fpOnEquip(inventory.items[inv_active_slot], this);
	}
}

void Actor::DecrEquipSlot()
{
	if (inv_active_slot > 0u)
	{
		--inv_active_slot;
		if (inventory.items.Used(inv_active_slot))
			GETITEM_MISC(inventory.items[inv_active_slot])->fpOnEquip(inventory.items[inv_active_slot], this);
	}
}

void TickChara(void* ent, btf32 dt)
{
	Chara* chr = (Chara*)ent;

	if (chr->inputBV.get(Actor::IN_CROUCH))
	{
		chr->aniCrouch = !chr->aniCrouch;
	}

	if (chr->aniCrouch)
		chr->grounded = RayEntity(chr->id, 0.3f);
	else
		chr->grounded = RayEntity(chr->id, 0.6f);

	#define t_body chr->t_body
	#define viewYaw chr->viewYaw
	#define speed chr->speed
	#define inventory chr->inventory
	#define inv_active_slot chr->inv_active_slot
	#define viewPitch chr->viewPitch
	#define foot_state chr->foot_state
	#define footPosTargR chr->footPosTargR
	#define footPosTargL chr->footPosTargL
	#define ani_body_lean chr->ani_body_lean
	#define t_head chr->t_head
	#define state chr->state
	#define input chr->input
	#define csi chr->t.csi
	#define aiControlled chr->aiControlled
	#define atk_target chr->atk_target

	bool can_move = true;
	bool can_turn = true;
	if (inventory.items.Used(inv_active_slot))
	{
		#define HELDINSTANCE ((HeldItem*)index::GetItemPtr(inventory.items[inv_active_slot]))
		can_move = !HELDINSTANCE->fpBlockMove(inventory.items[inv_active_slot]);
		can_turn = !HELDINSTANCE->fpBlockTurn(inventory.items[inv_active_slot]);
		#undef HELDINSTANCE
	}
	//if (!chr->grounded) can_move = false;

	if (state.stateFlags.get(ActiveState::eDIED_REPORT))
	{
		chr->DropAllItems();
		state.stateFlags.unset(ActiveState::eDIED_REPORT);
	}
	if (state.stateFlags.get(ActiveState::eALIVE))
	{
		if (chr->grounded)
		{
			if (can_move)
			{
				input.x = -input.x;
				if (!chr->aniCrouch)
					chr->t.velocity = m::Lerp(chr->t.velocity, m::Rotate(input, viewYaw.Rad()) * m::Vector2(-1.f, 1.f) * dt * speed, 0.2f);
				else
					chr->t.velocity = m::Lerp(chr->t.velocity, m::Rotate(input, viewYaw.Rad()) * m::Vector2(-1.f, 1.f) * dt * speed * 0.5f, 0.2f);
			}
			else
			{
				input = m::Vector2(0.f, 0.f);
				chr->t.velocity = m::Lerp(chr->t.velocity, m::Vector2(0.f, 0.f), 0.2f);
			}
		}
		if (can_turn && abs(m::AngDif(chr->t.yaw.Deg(), viewYaw.Deg())) > 65.f || m::Length(input) > 0.2f)
			chr->t.yaw.RotateTowards(viewYaw.Deg(), 8.f); // Rotate body towards the target direction

		//-------------------------------- APPLY MOVEMENT

		EntityTransformTick(chr, chr->id, 0, 0, 0);

		//-------------------------------- RUN AI FUNCTION

		// if AI controlled run the AI function
		if (aiControlled) index::ActorRunAI(chr->id);
		// if player controlled, just aim at whatever's in front of us
		else atk_target = index::GetViewTargetEntity(chr->id, 100.f, fac::enemy);

		//-------------------------------- RUN ITEM TICK

		if (inventory.items.Used(inv_active_slot))
		{
			#define HELDINSTANCE ((HeldItem*)index::GetItemPtr(inventory.items[inv_active_slot]))
			if (HELDINSTANCE != nullptr) HELDINSTANCE->fpTick(inventory.items[inv_active_slot], dt, chr);
			#undef HELDINSTANCE
		}
	} // End if alive

	//________________________________________________________________
	//------------- SET TRANSFORMATIONS FOR GRAPHICS -----------------

	if (state.stateFlags.get(ActiveState::eALIVE))
	{
		// Reset transforms
		t_body = Transform3D();
		t_head = Transform3D();

		//t_body.SetPosition(m::Vector3(chr->t.position.x, 0.1f + chr->t.height + 0.6f - (m::Length(chr->slideVelocity) * 0.5f), chr->t.position.y));
		t_body.SetPosition(m::Vector3(chr->t.position.x, 0.1f + chr->t.height- (m::Length(chr->slideVelocity) * 0.5f), chr->t.position.y));
		t_body.Rotate(chr->t.yaw.Rad(), m::Vector3(0, 1, 0));

		if (!chr->aniCrouch)
			ani_body_lean = m::Lerp(ani_body_lean, input * m::Vector2(8.f, 15.f), 0.25f);
		else 
			ani_body_lean = m::Lerp(ani_body_lean, input * m::Vector2(8.f, 15.f) + m::Vector2(0.f, 20.f), 0.25f);

		t_body.Rotate(glm::radians(ani_body_lean.y), m::Vector3(1, 0, 0));
		t_body.Rotate(glm::radians(ani_body_lean.x), m::Vector3(0, 0, 1));

		// Set head transform
		t_head.SetPosition(t_body.GetPosition());
		t_head.Rotate(viewYaw.Rad(), m::Vector3(0, 1, 0));
		t_head.Rotate(viewPitch.Rad(), m::Vector3(1, 0, 0));
		//t_head.Translate(t_body.GetUp() * 0.7f);
		if (!chr->aniCrouch)
			t_head.Translate(t_body.GetUp() * 0.63f);
		else
			t_head.Translate(t_body.GetUp() * 0.5f);
		t_head.SetScale(m::Vector3(0.95f, 0.95f, 0.95f));

		//-------------------------------- HANDLE ANIMATION

		chr->footPosL += m::Vector3(chr->slideVelocity.x, 0.f, chr->slideVelocity.y);
		chr->footPosR += m::Vector3(chr->slideVelocity.x, 0.f, chr->slideVelocity.y);
		footPosTargL += m::Vector3(chr->slideVelocity.x, 0.f, chr->slideVelocity.y);
		footPosTargR += m::Vector3(chr->slideVelocity.x, 0.f, chr->slideVelocity.y);
	}
	else
	{
		// Reset transforms
		t_body = Transform3D();
		t_head = Transform3D();

		t_body.SetPosition(m::Vector3(chr->t.position.x, chr->t.height, chr->t.position.y));
		t_body.Rotate(chr->t.yaw.Rad(), m::Vector3(0, 1, 0));
		t_body.Rotate(glm::radians(90.f), m::Vector3(1, 0, 0));

		// Set head transform
		t_head.SetPosition(t_body.GetPosition());
		t_head.Rotate(viewYaw.Rad(), m::Vector3(0, 1, 0));
		//t_head.Rotate(viewPitch.Rad(), m::Vector3(1, 0, 0));
		t_head.Translate(t_body.GetUp() * 0.65f);
	}
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
	#undef state
	#undef input
	#undef csi
	#undef aiControlled
	#undef atk_target
}
void DrawChara(void* ent)
{
	//#define leglen 0.75f
	#define leglen 0.625f
	#define armlen 0.75f
	// leg disconnect length? not sure
	//#define legDClen 0.85f
	#define legDClen 0.675f
	//#define legDClen 0.75f
	//#define velocityStepMult 0.5f
	#define velocityStepMult 10.f

	Chara* chr = (Chara*)ent;

	#define t_body chr->t_body
	#define viewYaw chr->viewYaw
	#define t chr->t
	#define speed chr->speed
	#define inventory chr->inventory
	#define inv_active_slot chr->inv_active_slot
	#define matLegHipR chr->matLegHipR
	#define matLegUpR chr->matLegUpR
	#define matLegLoR chr->matLegLoR
	#define matLegFootR chr->matLegFootR
	#define matLegHipL chr->matLegHipL
	#define matLegUpL chr->matLegUpL
	#define matLegLoL chr->matLegLoL
	#define matLegFootL chr->matLegFootL
	#define viewPitch chr->viewPitch
	#define foot_state chr->foot_state
	#define footPosTargR chr->footPosTargR
	#define footPosTargL chr->footPosTargL
	#define ani_body_lean chr->ani_body_lean
	#define t_head chr->t_head

	Transform3D t_upperbody = t_body;
	t_upperbody.Rotate(glm::radians(m::AngDif(viewYaw.Deg(), t.yaw.Deg())), m::Vector3(0, 1, 0));

	btf32 lerpAmt = 0.05f * speed;

	//-------------------------------- DRAW ARMS

	graphics::Matrix4x4 matrix;

	m::Vector3 newpos2 = t_body.GetPosition() + t_upperbody.GetUp() * 0.55f;
	
	btf32 shoulder_width = 0.16f;

	m::Vector3 jointPosR = newpos2 + t_upperbody.GetRight() * shoulder_width;
	m::Vector3 jointPosL = newpos2 + t_upperbody.GetRight() * -shoulder_width;

	btf32 distR;
	btf32 distL;

	graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA).Use();
	graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA).setVec3(graphics::Shader::Colour_A, chr->skin_col_a.x, chr->skin_col_a.y, chr->skin_col_a.z);
	graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA).setVec3(graphics::Shader::Colour_B, chr->skin_col_b.x, chr->skin_col_b.y, chr->skin_col_b.z);
	graphics::GetShader(graphics::S_SOLID_DEFORM_CHARA).setVec3(graphics::Shader::Colour_C, chr->skin_col_c.x, chr->skin_col_c.y, chr->skin_col_c.z);

	if (inventory.items.Used(inv_active_slot))
	{
		HeldItem* heldItem = ((HeldItem*)index::GetItemPtr(inventory.items[inv_active_slot]));

		// draw item
		heldItem->fpDraw(inventory.items[inv_active_slot],
			GETITEM_MISC(inventory.items[inv_active_slot])->id_item_template,
			t.position, t.height + 0.3f, viewYaw, viewPitch);

		m::Vector3 handPosR = heldItem->fpGetRightHandPos(inventory.items[inv_active_slot]);
		m::Vector3 handPosL = heldItem->fpGetLeftHandPos(inventory.items[inv_active_slot]);

		m::Vector3 bodyForwardR = m::Normalize((t_upperbody.GetRight() * -1.f) + t_upperbody.GetUp() + t_upperbody.GetForward());
		m::Vector3 bodyForwardL = m::Normalize(t_upperbody.GetRight() + t_upperbody.GetUp() + t_upperbody.GetForward());

		// arm 1
		btf32 len = m::Length(jointPosR - handPosR);
		if (len > armlen) len = armlen;
		btf32 lenUp = sqrtf(armlen * armlen - len * len); // Pythagorean theorem
		m::Vector3 vecfw = m::Normalize(handPosR - jointPosR);
		m::Vector3 vecside = m::Normalize(m::Cross(vecfw, bodyForwardR));
		m::Vector3 vecup = m::Normalize(m::Cross(vecfw, vecside));
		graphics::MatrixTransform(matLegHipR, jointPosR, t_body.GetUp() * -1.f, vecup);
		graphics::MatrixTransform(matLegUpR, jointPosR, m::Normalize(vecfw * len + vecup * lenUp), vecup);
		graphics::MatrixTransform(matLegLoR, jointPosR + vecup * lenUp, m::Normalize(vecfw * len - vecup * lenUp), vecup);
		graphics::MatrixTransform(matLegFootR, jointPosR - vecfw * (armlen - len), vecfw, vecup);
		DrawMeshDeform(chr->id, res::GetMD(res::md_char_arm), res::GetT(chr->t_skin), SS_CHARA, 4u, matLegHipR, matLegUpR, matLegLoR, matLegFootR);
		// arm 2
		len = m::Length(jointPosL - handPosL);
		if (len > armlen) len = armlen;
		lenUp = sqrtf(armlen * armlen - len * len); // Pythagorean theorem
		vecfw = m::Normalize(handPosL - jointPosL);
		vecside = m::Normalize(m::Cross(vecfw, bodyForwardL));
		vecup = m::Normalize(m::Cross(vecfw, vecside));
		graphics::MatrixTransformXFlip(matLegHipL, jointPosL, t_body.GetUp() * -1.f, vecup);
		graphics::MatrixTransformXFlip(matLegUpL, jointPosL, m::Normalize(vecfw * len + vecup * lenUp), vecup);
		graphics::MatrixTransformXFlip(matLegLoL, jointPosL + vecup * lenUp, m::Normalize(vecfw * len - vecup * lenUp), vecup);
		graphics::MatrixTransformXFlip(matLegFootL, jointPosL - vecfw * (armlen - len), vecfw, vecup);
		graphics::SetFrontFaceInverse();
		DrawMeshDeform(chr->id, res::GetMD(res::md_char_arm), res::GetT(chr->t_skin), SS_CHARA, 4u, matLegHipL, matLegUpL, matLegLoL, matLegFootL);
		graphics::SetFrontFace();
	}

	//-------------------------------- DRAW LEGS

	if (chr->state.stateFlags.get(ActiveState::eALIVE))
	{
		btf32 hip_width = 0.125f;

		//m::Vector3 newpos = m::Vector3(t.position.x, 0.68f + t.height, t.position.y);
		m::Vector3 newpos = t_body.GetPosition();

		jointPosR = newpos + t_body.GetRight() * hip_width;
		jointPosL = newpos + t_body.GetRight() * -hip_width;

		m::Vector3 velocity = m::Normalize(m::Vector3(t.velocity.x, 0.f, t.velocity.y));

		m::Vector2 right = m::Vector2(t_body.GetRight().x, t_body.GetRight().z);

		// TODO: instead measure difference between base velocity and footslide velocity
		btf32 velocityAmt = m::Length(t.velocity - chr->slideVelocity);

		if (foot_state == eL_DOWN)
		{
			// if moving ignore leg length
			if (velocityAmt > 0.025f)
			{
				if (chr->aniStepAmountR > 0.9f) // if other step nearly done
				{
					chr->footPosL = footPosTargL;
					chr->aniStepAmountL = 0.f;
					aud::PlaySnd(aud::FILE_FOOTSTEP_SNOW_A, m::Vector3(t.position.x, t.height, t.position.y));
					footPosTargL = SetFootPos(t.position + right * -hip_width + t.velocity * velocityStepMult);
					foot_state = eR_DOWN;
				}
			}
			// else if too far off balance
			else if (m::Length(jointPosL - footPosTargL) > legDClen && chr->aniStepAmountL == 1.f)
			{
				chr->footPosL = footPosTargL;
				chr->aniStepAmountL = 0.f;
				footPosTargL = SetFootPos(t.position + right * -hip_width + t.velocity * velocityStepMult);
				foot_state = eR_DOWN;
			}
		}
		else if (foot_state == eR_DOWN)
		{
			// if moving ignore leg length
			if (velocityAmt > 0.025f)
			{
				if (chr->aniStepAmountL > 0.9f) // if other step nearly done
				{
					chr->footPosR = footPosTargR;
					chr->aniStepAmountR = 0.f;
					aud::PlaySnd(aud::FILE_FOOTSTEP_SNOW_B, m::Vector3(t.position.x, t.height, t.position.y));
					footPosTargR = SetFootPos(t.position + right * hip_width + t.velocity * velocityStepMult);
					foot_state = eL_DOWN;
				}
			}
			// else if too far off balance
			else if (m::Length(jointPosR - footPosTargR) > legDClen && chr->aniStepAmountR == 1.f)
			{
				chr->footPosR = footPosTargR;
				chr->aniStepAmountR = 0.f;
				footPosTargR = SetFootPos(t.position + right * hip_width + t.velocity * velocityStepMult);
				foot_state = eL_DOWN;
			}
		}

		btf32 myspeed = m::Length(t.velocity);

		// set step positions
		btf32 anispeed = 0.4f * myspeed;
		if (anispeed < 0.018f) anispeed = 0.018f;

		chr->aniStepAmountL += anispeed;
		if (chr->aniStepAmountL > 1.f)
			chr->aniStepAmountL = 1.f;
		chr->aniStepAmountR += anispeed;
		if (chr->aniStepAmountR > 1.f)
			chr->aniStepAmountR = 1.f;

		btf32 fpHeightL = m::QuadraticFootstep(velocityAmt * 6.f, chr->aniStepAmountL * 2.f - 1.f);
		btf32 fpHeightR = m::QuadraticFootstep(velocityAmt * 6.f, chr->aniStepAmountR * 2.f - 1.f);

		m::Vector3 fpCurrentL = m::Lerp(chr->footPosL, footPosTargL, chr->aniStepAmountL) + m::Vector3(0.f, fpHeightL, 0.f);
		m::Vector3 fpCurrentR = m::Lerp(chr->footPosR, footPosTargR, chr->aniStepAmountR) + m::Vector3(0.f, fpHeightR, 0.f);

		// generate matrices

		btf32 len = m::Length(jointPosR - fpCurrentR);
		if (len > leglen) len = leglen;
		btf32 lenUp = sqrtf(leglen * leglen - len * len); // Pythagorean theorem
		m::Vector3 vecfw = m::Normalize(fpCurrentR - jointPosR);
		m::Vector3 vecside = m::Normalize(m::Cross(vecfw, t_body.GetForward()));
		m::Vector3 vecup = m::Normalize(m::Cross(vecfw, vecside));
		m::Vector3 vecup_inv = vecup * -1.f;
		graphics::MatrixTransformXFlip(matLegHipR, jointPosR, t_body.GetUp() * -1.f, vecup_inv);
		graphics::MatrixTransformXFlip(matLegUpR, jointPosR, m::Normalize(vecfw * len - vecup * lenUp), vecup_inv);
		graphics::MatrixTransformXFlip(matLegLoR, jointPosR - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup_inv);
		graphics::MatrixTransformXFlip(matLegFootR, jointPosR - vecfw * (leglen - len), vecfw, vecup_inv);
		graphics::SetFrontFaceInverse();
		#ifdef DEF_NMP
		DrawMeshDeform(chr->id, res::GetMD(res::md_char_leg), res::GetT(t_skin), SS_CHARA, 4u, matLegHipR, matLegUpR, matLegLoR, matLegFootR);
		#else
		DrawMeshDeform(chr->id, res::GetMD(res::md_char_leg), res::GetT(res::t_equip_legs_robe_01), SS_NORMAL, 4u, matLegHipR, matLegUpR, matLegLoR, matLegFootR);
		#endif
		graphics::SetFrontFace();
		// transform legR for cloak
		//graphics::MatrixTransformForwardUp(matrixLegR, t_body.GetPosition(), m::Normalize(vecfw * len - vecup * lenUp * 0.5f), t_body.GetForward());
		graphics::MatrixTransformForwardUp(matLegUpR, t_body.GetPosition(), fpCurrentR - t_body.GetPosition(), t_body.GetForward());

		len = m::Length(jointPosL - fpCurrentL);
		if (len > leglen) len = leglen;
		lenUp = sqrtf(leglen * leglen - len * len); // Pythagorean theorem
		vecfw = m::Normalize(fpCurrentL - jointPosL);
		vecside = m::Normalize(m::Cross(vecfw, t_body.GetForward()));
		vecup = m::Normalize(m::Cross(vecfw, vecside));
		vecup_inv = vecup * -1.f;
		graphics::MatrixTransform(matLegHipL, jointPosL, t_body.GetUp() * -1.f, vecup_inv);
		graphics::MatrixTransform(matLegUpL, jointPosL, m::Normalize(vecfw * len - vecup * lenUp), vecup_inv);
		graphics::MatrixTransform(matLegLoL, jointPosL - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup_inv);
		graphics::MatrixTransform(matLegFootL, jointPosL - vecfw * (leglen - len), vecfw, vecup_inv);
		//graphics::MatrixTransform(matLegFootL, fpCurrentL - m::Vector3(0.f, leglen, 0.f), m::Vector3(0.f, 1.f, 0.f), m::Vector3(1.f, 0.f, 0.f));
		#ifdef DEF_NMP
		DrawMeshDeform(chr->id, res::GetMD(res::md_char_leg), res::GetT(t_skin), SS_CHARA, 4u, matLegHipL, matLegUpL, matLegLoL, matLegFootL);
		#else
		DrawMeshDeform(chr->id, res::GetMD(res::md_char_leg), res::GetT(res::t_equip_legs_robe_01), SS_NORMAL, 4u, matLegHipL, matLegUpL, matLegLoL, matLegFootL);
		#endif
		// transform legL for cloak
		//graphics::MatrixTransformForwardUp(matrixLegL, t_body.GetPosition(), m::Normalize(vecfw * len - vecup * lenUp * 0.5f), t_body.GetForward());
		graphics::MatrixTransformForwardUp(matLegUpL, t_body.GetPosition(), fpCurrentL - t_body.GetPosition(), t_body.GetForward());

	}

	//-------------------------------- DRAW BODY

	{ // scope
		#define bodylen 0.65

		graphics::Matrix4x4 matBodyUp;
		graphics::Matrix4x4 matBodyLo;

		btf32 len = m::Length(t_body.GetPosition() - t_head.GetPosition());
		if (len > bodylen) len = bodylen;
		btf32 lenUp = sqrtf(bodylen * bodylen - len * len); // Pythagorean theorem
		m::Vector3 vecfw = m::Normalize(t_body.GetPosition() - t_head.GetPosition());
		m::Vector3 vecside = m::Normalize(m::Cross(vecfw, t_body.GetForward()));
		m::Vector3 vecup = m::Normalize(m::Cross(vecfw, vecside));
		m::Vector3 vecside_upper = m::Normalize(m::Cross(vecfw, t_head.GetForward()));
		m::Vector3 vecup_upper = m::Normalize(m::Cross(vecfw, vecside_upper));
		graphics::MatrixTransformForwardUp(matBodyLo, t_body.GetPosition(), m::Normalize(vecfw * len - vecup * lenUp), vecup * -1.f);
		graphics::MatrixTransformForwardUp(matBodyUp, t_body.GetPosition() + vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup_upper * -1.f);

		DrawMeshDeform(chr->id, res::GetMD(res::md_chr_body), res::GetT(chr->t_skin), SS_CHARA, 2u,
			matBodyLo, matBodyUp, graphics::Matrix4x4(), graphics::Matrix4x4());

		#ifndef DEF_NMP
		if (chr->state.stateFlags.get(ActiveState::eALIVE))
		{
			//DrawMeshDeform(chr->id, res::GetMD(res::md_equip_body_robe_01), res::GetT(res::t_equip_body_robe_01), SS_NORMAL, 4u,
				//matBodyLo, matBodyUp, matLegUpL, matLegUpR);
		}
		#endif
	}

	//-------------------------------- DRAW HEAD

	Transform3D t2;
	t2.SetPosition(t_body.GetPosition());
	t2.SetRotation(t_upperbody.GetRotation());
	t2.TranslateLocal(m::Vector3(0.f, 0.7f, 0.f));
	DrawMeshDeform(chr->id, res::GetMD(res::md_char_head), res::GetT(chr->t_skin), SS_CHARA, 4u,
		t2.getMatrix(), t_head.getMatrix(), t_head.getMatrix(), t_head.getMatrix());

	#undef t_body
	#undef viewYaw
	#undef t
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
	//#define moving chr->moving
	#define csi chr->t.csi
	#define aiControlled chr->aiControlled
	#define atk_target chr->atk_target

	bool can_move = true;
	bool can_turn = true;
	if (inventory.items.Used(inv_active_slot))
	{
		#define HELDINSTANCE ((HeldItem*)index::GetItemPtr(inventory.items[inv_active_slot]))
		can_move = !HELDINSTANCE->fpBlockMove(inventory.items[inv_active_slot]);
		can_turn = !HELDINSTANCE->fpBlockTurn(inventory.items[inv_active_slot]);
		#undef HELDINSTANCE
	}

	if (state.stateFlags.get(ActiveState::eALIVE))
	{
		if (cfg::bEditMode)
		{
			//ENTITY[index]->t.velocity = fw::Lerp(ENTITY[index]->t.velocity, fw::Rotate(f2Input, aViewYaw.Rad()) * fw::Vector2(-1.f, 1.f) * dt * fSpeed, 0.3f);
			input.x = -input.x;
			chr->t.velocity = m::Rotate(input, viewYaw.Rad()) * m::Vector2(-1.f, 1.f) * dt * 5.f;
		}
		else
		{
			if (can_move)
			{
				input.x = -input.x;
				chr->t.velocity = m::Lerp(chr->t.velocity, m::Rotate(input, viewYaw.Rad()) * m::Vector2(-1.f, 1.f) * dt * speed, 0.2f);
			}
			else
			{
				input = m::Vector2(0.f, 0.f);
				chr->t.velocity = m::Lerp(chr->t.velocity, m::Vector2(0.f, 0.f), 0.2f);
			}
			if (can_turn && abs(m::AngDif(chr->t.yaw.Deg(), viewYaw.Deg())) > 65.f || m::Length(input) > 0.2f)
				chr->t.yaw.RotateTowards(viewYaw.Deg(), 8.f); // Rotate body towards the target direction
		}

		//-------------------------------- APPLY MOVEMENT

		//moving = (m::Length(chr->t.velocity) > 0.016f);
		m::Vector2 oldpos = chr->t.position;
		chr->t.position += chr->t.velocity; // Apply velocity

		CellSpace cs_last = csi;

		//regenerate csi
		index::GetCellSpaceInfo(chr->t.position, csi);

		//I don't want this to be here
		if (cs_last.c[eCELL_I].x != csi.c[eCELL_I].x || cs_last.c[eCELL_I].y != csi.c[eCELL_I].y)
		{
			index::RemoveEntityCell(cs_last.c[eCELL_I].x, cs_last.c[eCELL_I].y, chr->id);
			index::AddEntityCell(csi.c[eCELL_I].x, csi.c[eCELL_I].y, chr->id);
		}

		//-------------------------------- SET HEIGHT AND CELL SPACE

		btf32 height2;
		env::GetHeight(height2, csi);
		chr->t.height = m::Lerp(chr->t.height, height2, 0.05f);

		//-------------------------------- RUN COLLISION & AI

	} // End if alive

	if (inventory.items.Used(inv_active_slot))
	{
		#define HELDINSTANCE ((HeldItem*)index::GetItemPtr(inventory.items[inv_active_slot]))
		if (HELDINSTANCE != nullptr) HELDINSTANCE->fpTick(inventory.items[inv_active_slot], dt, chr);
		#undef HELDINSTANCE
	}

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
