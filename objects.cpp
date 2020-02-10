#include "objects.h"
#include "objects_items.h"

#include "graphics.hpp"
#include <glm\gtc\matrix_transform.hpp>

#include "archive.hpp"

#include "core.h"
#include "cfg.h"

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
void Inventory::Destroy(btID item_template)
{
	for (int i = 0; i < items.Size(); ++i)
	{
		if (GETITEM_MISC(items[i])->item_template == item_template) // if we already have a stack of this item
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
		if (items[i] == item_ID) // if we have this item instance
		{
			items.Remove(i); // Remove the item from inventory without deleting the instance
			return;
		}
	}
}
void Inventory::Draw(btui16 active_slot)
{
	int p1_x_start = -(int)graphics::FrameSizeX() / 2;
	int p1_y_start = -(int)graphics::FrameSizeY() / 2;

	const bti32 invspace = 38;

	bti32 offset = p1_x_start + 96 - 16;
	for (btui16 i = 0; i < items.Size(); i++)
	{
		if (items.Used(i))
		{
			if (i == active_slot)
				graphics::DrawGUITexture(&res::GetT(acv::items[GETITEM_MISC(items[i])->item_template]->id_icon), offset + i * invspace, p1_y_start + 24, 64, 64);
			else
				graphics::DrawGUITexture(&res::GetT(acv::items[GETITEM_MISC(items[i])->item_template]->id_icon), offset + i * invspace, p1_y_start + 16, 64, 64);
		}
	}
	guibox_selection.ReGen((offset + active_slot * invspace) - 12, (offset + active_slot * invspace) + 12, p1_y_start + 12, p1_y_start + 36, 8, 8);
	guibox_selection.Draw(&res::GetT(res::t_gui_select_box));
}

char* DisplayNameActor(void* ent)
{
	return "Actor";
};
char* DisplayNameRestingItem(void* ent)
{
	//return (char*)acv::items[GETITEM_MISC(((RestingItem*)index::GetEntityPtr(ent))->item_instance)->item_template]->name;
	return (char*)acv::items[GETITEM_MISC(((RestingItem*)ent)->item_instance)->item_template]->name;
};
void DrawRestingItem(void* ent)
{
	RestingItem* item = (RestingItem*)ent;
	// Draw the mesh of our item id
	//DrawMesh(ent, res::GetM(acv::items[index::GetItem(item->item_instance)->item_template]->id_mesh), res::GetT(acv::items[index::GetItem(item->item_instance)->item_template]->id_tex), SS_NORMAL, item->t_item.getMatrix());
	DrawMesh(item->id, res::GetM(acv::items[GETITEM_MISC(item->item_instance)->item_template]->id_mesh), res::GetT(acv::items[GETITEM_MISC(item->item_instance)->item_template]->id_tex), SS_NORMAL, item->matrix);
}
m::Vector3 SetFootPos(m::Vector2 position)
{
	CellSpace cs;
	index::GetCellSpaceInfo(position, cs);
	btf32 height;
	env::GetHeight(height, cs);
	return m::Vector3(position.x, height, position.y);
}
void DrawChara(void* ent)
{
	#define leglen 0.75f
	#define legDClen 1.f
	//#define legDClen 0.75f
	#define velocityStepMult 0.5f

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
	#define t_skin chr->t_skin
	#define viewPitch chr->viewPitch
	#define foot_state chr->foot_state
	#define footPosTargR chr->footPosTargR
	#define footPosTargL chr->footPosTargL
	#define ani_body_lean chr->ani_body_lean
	#define footPosR chr->footPosR
	#define footPosL chr->footPosL
	#define t_head chr->t_head

	Transform3D t_test = t_body;
	t_test.Rotate(glm::radians(m::AngDif(viewYaw.Deg(), t.yaw.Deg())), m::Vector3(0, 1, 0));

	btf32 lerpAmt = 0.05f * speed;

	// draw arms

	graphics::Matrix4x4 matrix;

	m::Vector3 newpos2 = t_body.GetPosition() + t_test.GetUp() * 0.55f;

	m::Vector3 jointPosR = newpos2 + t_test.GetRight() * 0.11f;
	m::Vector3 jointPosL = newpos2 + t_test.GetRight() * -0.11f;

	btf32 distR;
	btf32 distL;

	if (inventory.items.Used(inv_active_slot))
	{
		#define HELDINSTANCE ((HeldItem*)index::GetItemPtr(inventory.items[inv_active_slot]))

		m::Vector3 handPosR = HELDINSTANCE->fpGetRightHandPos(inventory.items[inv_active_slot]);
		m::Vector3 handPosL = HELDINSTANCE->fpGetLeftHandPos(inventory.items[inv_active_slot]);

		m::Vector3 bodyForwardR = m::Normalize((t_test.GetRight() * -1.f) + t_test.GetUp() + t_test.GetForward());
		m::Vector3 bodyForwardL = m::Normalize(t_test.GetRight() + t_test.GetUp() + t_test.GetForward());

		btf32 len = m::Length(jointPosR - handPosR);
		if (len > leglen) len = leglen;
		btf32 lenUp = sqrtf(leglen * leglen - len * len); // Pythagorean theorem
		m::Vector3 vecfw = m::Normalize(handPosR - jointPosR);
		m::Vector3 vecside = m::Normalize(m::Cross(vecfw, bodyForwardR));
		m::Vector3 vecup = m::Normalize(m::Cross(vecfw, vecside));
		graphics::MatrixTransformXFlip(matLegHipR, jointPosR, t_body.GetUp() * -1.f, vecup);
		graphics::MatrixTransformXFlip(matLegUpR, jointPosR, m::Normalize(vecfw * len + vecup * lenUp), vecup);
		graphics::MatrixTransformXFlip(matLegLoR, jointPosR + vecup * lenUp, m::Normalize(vecfw * len - vecup * lenUp), vecup);
		graphics::MatrixTransformXFlip(matLegFootR, jointPosR - vecfw * (leglen - len), vecfw, vecup);
		graphics::SetFrontFaceInverse();
		DrawMeshDeform(chr->id, res::GetMD(res::md_char_arm), res::skin_t[t_skin], SS_CHARA, 4u, matLegHipR, matLegUpR, matLegLoR, matLegFootR);
		graphics::SetFrontFace();

		len = m::Length(jointPosL - handPosL);
		if (len > leglen) len = leglen;
		lenUp = sqrtf(leglen * leglen - len * len); // Pythagorean theorem
		vecfw = m::Normalize(handPosL - jointPosL);
		vecside = m::Normalize(m::Cross(vecfw, bodyForwardL));
		vecup = m::Normalize(m::Cross(vecfw, vecside));
		graphics::MatrixTransform(matLegHipL, jointPosL, t_body.GetUp() * -1.f, vecup);
		graphics::MatrixTransform(matLegUpL, jointPosL, m::Normalize(vecfw * len + vecup * lenUp), vecup);
		graphics::MatrixTransform(matLegLoL, jointPosL + vecup * lenUp, m::Normalize(vecfw * len - vecup * lenUp), vecup);
		graphics::MatrixTransform(matLegFootL, jointPosL - vecfw * (leglen - len), vecfw, vecup);
		DrawMeshDeform(chr->id, res::GetMD(res::md_char_arm), res::skin_t[t_skin], SS_CHARA, 4u, matLegHipL, matLegUpL, matLegLoL, matLegFootL);

		// draw item
		HELDINSTANCE->fpDraw(inventory.items[inv_active_slot], GETITEM_MISC(inventory.items[inv_active_slot])->item_template, t.position, t.height, viewYaw, viewPitch);

		#undef HELDINSTANCE
	}

	// draw legs

	btf32 hip_width = 0.125f;

	//m::Vector3 newpos = m::Vector3(t.position.x, 0.68f + t.height, t.position.y);
	m::Vector3 newpos = t_body.GetPosition();

	jointPosR = newpos + t_body.GetRight() * hip_width;
	jointPosL = newpos + t_body.GetRight() * -hip_width;

	m::Vector3 velocity = m::Normalize(m::Vector3(t.velocity.x, 0.f, t.velocity.y));

	m::Vector2 right = m::Vector2(t_body.GetRight().x, t_body.GetRight().z);

	if (foot_state == eL_DOWN)
	{
		if (m::Length(t.velocity) < 0.025f) // if below a certain speed, switch to standing pose
		{
			footPosTargR = SetFootPos(m::Vector2(footPosTargR.x, footPosTargR.z));
			if (m::Length(jointPosL - footPosTargL) > leglen)
				footPosTargL = SetFootPos(t.position + right * -hip_width);
			//foot_pos_l = m::Vector3(t.position.x, t.height, t.position.y) + t_body.GetRight() * hip_width;
			foot_state = eBOTH_DOWN;
		}
		else // else hang foot
		{
			footPosTargR = m::Vector3(t.position.x, t.height + 0.15f, t.position.y) + t_body.GetRight() * hip_width + velocity * 0.3f;
		}
		// if too far off balance
		if (m::Length(jointPosL - footPosTargL) > legDClen)
		{
			footPosTargL = SetFootPos(t.position + right * hip_width + t.velocity * velocityStepMult);
			//foot_pos_l = m::Vector3(t.position.x, t.height, t.position.y) + t_body.GetRight() * hip_width + velocity * 0.7f;
			foot_state = eR_DOWN;
		}
	}
	else if (foot_state == eR_DOWN)
	{
		if (m::Length(t.velocity) < 0.025f) // if below a certain speed, switch to standing pose
		{
			footPosTargL = SetFootPos(m::Vector2(footPosTargL.x, footPosTargL.z));
			if (m::Length(jointPosR - footPosTargR) > leglen)
				footPosTargR = SetFootPos(t.position + right * hip_width);
			//foot_pos_r = m::Vector3(t.position.x, t.height, t.position.y) + t_body.GetRight() * -hip_width;
			foot_state = eBOTH_DOWN;
		}
		else // else hang foot
		{
			footPosTargL = m::Vector3(t.position.x, t.height + 0.15f, t.position.y) + t_body.GetRight() * -hip_width + velocity * 0.3f;
		}
		// if too far off balance
		if (m::Length(jointPosR - footPosTargR) > legDClen)
		{
			footPosTargL = SetFootPos(t.position + right * -hip_width + t.velocity * velocityStepMult);
			//foot_pos_r = m::Vector3(t.position.x, t.height, t.position.y) + t_body.GetRight() * -hip_width + velocity * 0.7f;
			foot_state = eL_DOWN;
		}
	}
	else
	{
		if (m::Length(t.velocity) > 0.025f) // if above a certain speed, switch to walking pose
		{
			if (ani_body_lean.x < 0.f)
				foot_state = eL_DOWN;
			else
				foot_state = eR_DOWN;
		}
		// should probably switch to: if the average position drifts off-balance
		// move the furthest leg to a new position
		else if (m::Length(jointPosL - footPosTargL) > leglen)
		{
			footPosTargL = SetFootPos(t.position + right * -hip_width + t.velocity * velocityStepMult);
			//foot_pos_r = m::Vector3(t.position.x, t.height, t.position.y) + t_body.GetRight() * -hip_width + velocity * 0.7f;
			foot_state = eR_DOWN;
		}
		else if (m::Length(jointPosR - footPosTargR) > leglen)
		{
			footPosTargR = SetFootPos(t.position + right * hip_width + t.velocity * velocityStepMult);
			//foot_pos_l = m::Vector3(t.position.x, t.height, t.position.y) + t_body.GetRight() * hip_width + velocity * 0.7f;
			foot_state = eL_DOWN;
		}
	}

	footPosR = m::Lerp(footPosR, footPosTargR, lerpAmt);
	footPosL = m::Lerp(footPosL, footPosTargL, lerpAmt);

	btf32 len = m::Length(jointPosR - footPosR);
	if (len > leglen) len = leglen;
	btf32 lenUp = sqrtf(leglen * leglen - len * len); // Pythagorean theorem
	m::Vector3 vecfw = m::Normalize(footPosR - jointPosR);
	m::Vector3 vecside = m::Normalize(m::Cross(vecfw, t_body.GetForward()));
	m::Vector3 vecup = m::Normalize(m::Cross(vecfw, vecside));
	m::Vector3 vecup2 = vecup * -1.f;
	graphics::MatrixTransformXFlip(matLegHipR, jointPosR, t_body.GetUp() * -1.f, vecup2);
	graphics::MatrixTransformXFlip(matLegUpR, jointPosR, m::Normalize(vecfw * len - vecup * lenUp), vecup2);
	graphics::MatrixTransformXFlip(matLegLoR, jointPosR - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup2);
	graphics::MatrixTransformXFlip(matLegFootR, jointPosR - vecfw * (leglen - len), vecfw, vecup2);
	graphics::SetFrontFaceInverse();
	DrawMeshDeform(chr->id, res::GetMD(res::md_char_leg), res::GetT(res::t_equip_legs_robe_01), SS_CHARA, 4u, matLegHipR, matLegUpR, matLegLoR, matLegFootR);
	graphics::SetFrontFace();
	// transform legR for cloak
	//graphics::MatrixTransformForwardUp(matrixLegR, t_body.GetPosition(), m::Normalize(vecfw * len - vecup * lenUp * 0.5f), t_body.GetForward());
	graphics::MatrixTransformForwardUp(matLegUpR, t_body.GetPosition(), footPosR - t_body.GetPosition(), t_body.GetForward());

	len = m::Length(jointPosL - footPosL);
	if (len > leglen) len = leglen;
	lenUp = sqrtf(leglen * leglen - len * len); // Pythagorean theorem
	vecfw = m::Normalize(footPosL - jointPosL);
	vecside = m::Normalize(m::Cross(vecfw, t_body.GetForward()));
	vecup = m::Normalize(m::Cross(vecfw, vecside));
	vecup2 = vecup * -1.f;
	graphics::MatrixTransform(matLegHipL, jointPosL, t_body.GetUp() * -1.f, vecup2);
	graphics::MatrixTransform(matLegUpL, jointPosL, m::Normalize(vecfw * len - vecup * lenUp), vecup2);
	graphics::MatrixTransform(matLegLoL, jointPosL - vecup * lenUp, m::Normalize(vecfw * len + vecup * lenUp), vecup2);
	graphics::MatrixTransform(matLegFootL, jointPosL - vecfw * (leglen - len), vecfw, vecup2);
	DrawMeshDeform(chr->id, res::GetMD(res::md_char_leg), res::GetT(res::t_equip_legs_robe_01), SS_CHARA, 4u, matLegHipL, matLegUpL, matLegLoL, matLegFootL);
	// transform legL for cloak
	//graphics::MatrixTransformForwardUp(matrixLegL, t_body.GetPosition(), m::Normalize(vecfw * len - vecup * lenUp * 0.5f), t_body.GetForward());
	graphics::MatrixTransformForwardUp(matLegUpL, t_body.GetPosition(), footPosL - t_body.GetPosition(), t_body.GetForward());

	// draw head

	//DrawBlendMeshAtTransform(index, res::mb_char_head, 0, t_skin, graphics::shader_blend, t_head);
	DrawBlendMesh(chr->id, res::GetMB(res::mb_char_head), 0, res::skin_t[t_skin], SS_CHARA, t_head.getMatrix());
	//DrawMeshAtTransform(index, res::m_proj_2, res::t_proj_2, graphics::shader_solid, t_head);
	//DrawMesh(index, res::GetM(res::m_equip_head_pickers), res::GetT(res::t_default), SS_NORMAL, t_head.getMatrix());

	// need a good way of knowing own index
	DrawMeshDeform(chr->id, res::GetMD(res::md_chr_body), res::skin_t[t_skin], SS_CHARA, 2u,
		t_body.getMatrix(), t_test.getMatrix(), graphics::Matrix4x4(), graphics::Matrix4x4());
	DrawMeshDeform(chr->id, res::GetMD(res::md_equip_body_robe_01), res::GetT(res::t_equip_body_robe_01), SS_CHARA, 4u,
		t_body.getMatrix(), t_test.getMatrix(), matLegUpL, matLegUpR);

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
	#undef t_skin
	#undef viewPitch
	#undef foot_state
	#undef footPosTargR
	#undef footPosTargL
	#undef ani_body_lean
	#undef footPosR
	#undef footPosL
	#undef t_head
}
void DrawEditorPawn(void* ent)
{
	Chara* chr = (Chara*)ent;

	// need a good way of knowing own index
	DrawMesh(chr->id, res::GetM(res::m_debug_bb), res::skin_t[chr->t_skin], SS_NORMAL, chr->t_body.getMatrix());

	// draw head

	//DrawBlendMesh(index, res::GetMB(res::mb_char_head), 0, res::skin_t[t_skin], SS_CHARA, t_head.getMatrix());
}


void ActiveState::Damage(btf32 amount, btf32 angle)
{
	hp -= amount;
	if (hp <= 0.f)
	{
		properties.unset(ActiveState::eALIVE);
		hp = 0.f;
	}
}
void ActiveState::AddEffect(btID caster, StatusEffectType type, btf32 duration, btui32 magnitude)
{
	StatusEffect effect;
	effect.effect_caster_id = caster;
	effect.effect_type = type;
	effect.effect_duration = duration;
	effect.effect_magnitude = magnitude;
	effects.Add(effect);
}
void ActiveState::TickEffects(btf32 dt)
{
	for (btui32 i = 0; i < effects.Size(); ++i)
	{
		switch (effects[i].effect_type)
		{
		case SE_DAMAGE_HP:
			// magnitude multiplied by delta time so that it functions as a value per second
			Damage((btf32)effects[i].effect_magnitude * dt, 0.f);
			// TODO: include AI 'notify attack' function call here
			break;
		case SE_RESTORE_HP:
			break;
		}
		effects[i].effect_duration -= dt; // tick down the effect timer
		if (effects[i].effect_duration < 0.f) // if the timer has run out
			effects.Remove(i); // remove the effect
	}
}


void TickRestingItem(void* ent, btf32 dt)
{
	RestingItem* chr = (RestingItem*)ent;

	//t.position.y += 0.01f;

	//CellSpace cs_last = csi;

	//index::GetCellSpaceInfo(t.position, csi);

	/*if (cs_last.c[eCELL_I].x != csi.c[eCELL_I].x || cs_last.c[eCELL_I].y != csi.c[eCELL_I].y)
	{
		index::RemoveEntityCell(cs_last.c[eCELL_I].x, cs_last.c[eCELL_I].y, index);
		index::AddEntityCell(csi.c[eCELL_I].x, csi.c[eCELL_I].y, index);
	}*/

	chr->matrix = graphics::Matrix4x4();
	graphics::MatrixTransform(chr->matrix, m::Vector3(chr->t.position.x, chr->t.height, chr->t.position.y), chr->t.yaw.Rad());

	//env::GetHeight(t.height, csi);
	//chr->t_item.SetPosition(m::Vector3(chr->t.position.x, chr->t.height + acv::items[index::GetItem(chr->item_instance)->item_template]->f_model_height, chr->t.position.y));
	//chr->t_item.SetRotation(chr->t.yaw.Rad());
}

void Actor::PickUpItem(btID id)
{
	RestingItem* item = (RestingItem*)index::GetEntityPtr(id);
	inventory.TransferItemRecv(item->item_instance);
	index::DestroyEntity(id);
}

void Actor::DropItem(btID slot)
{
	if (slot < inventory.items.Size() && inventory.items.Used(slot))
	{
		index::SpawnEntityItem(inventory.items[slot], t.position, t.yaw.Deg());
		inventory.TransferItemSendIndex(slot);
		DecrEquipSlot();
	}
}

void Actor::SetEquipSlot(btui32 index)
{
	if (index < inventory.items.Size())
	{
		inv_active_slot = index;
		if (inventory.items.Used(inv_active_slot))
			GETITEM_MISC(inventory.items[inv_active_slot])->fpOnEquip(inventory.items[inv_active_slot]);
	}
}

void Actor::IncrEquipSlot()
{
	if (inv_active_slot < inventory.items.Size() - 1u)
	{
		++inv_active_slot;
		if (inventory.items.Used(inv_active_slot))
			GETITEM_MISC(inventory.items[inv_active_slot])->fpOnEquip(inventory.items[inv_active_slot]);
	}
}

void Actor::DecrEquipSlot()
{
	if (inv_active_slot > 0u)
	{
		--inv_active_slot;
		if (inventory.items.Used(inv_active_slot))
			GETITEM_MISC(inventory.items[inv_active_slot])->fpOnEquip(inventory.items[inv_active_slot]);
	}
}

void TickChara(void* ent, btf32 dt)
{
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
	#define t_skin chr->t_skin
	#define viewPitch chr->viewPitch
	#define foot_state chr->foot_state
	#define footPosTargR chr->footPosTargR
	#define footPosTargL chr->footPosTargL
	#define ani_body_lean chr->ani_body_lean
	#define footPosR chr->footPosR
	#define footPosL chr->footPosL
	#define t_head chr->t_head
	#define state chr->state
	#define input chr->input
	#define moving chr->moving
	#define csi chr->csi
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

	if (state.properties.get(ActiveState::eALIVE))
	{
		if (cfg::bEditMode)
		{
			//ENTITY[index]->t.velocity = fw::Lerp(ENTITY[index]->t.velocity, fw::Rotate(f2Input, aViewYaw.Rad()) * fw::Vector2(-1.f, 1.f) * dt * fSpeed, 0.3f);
			input.x = -input.x;
			t.velocity = m::Rotate(input, viewYaw.Rad()) * m::Vector2(-1.f, 1.f) * dt * 5.f;
		}
		else
		{
			if (can_move)
			{
				input.x = -input.x;
				t.velocity = m::Lerp(t.velocity, m::Rotate(input, viewYaw.Rad()) * m::Vector2(-1.f, 1.f) * dt * speed, 0.2f);
			}
			else
			{
				input = m::Vector2(0.f, 0.f);
				t.velocity = m::Lerp(t.velocity, m::Vector2(0.f, 0.f), 0.2f);
			}
			if (can_turn && abs(m::AngDif(t.yaw.Deg(), viewYaw.Deg())) > 65.f || m::Length(input) > 0.2f)
				t.yaw.RotateTowards(viewYaw.Deg(), 8.f); // Rotate body towards the target direction
		}

		//-------------------------------- APPLY MOVEMENT

		moving = (m::Length(t.velocity) > 0.016f);
		m::Vector2 oldpos = t.position;
		t.position += t.velocity; // Apply velocity

		CellSpace cs_last = csi;

		//regenerate csi
		index::GetCellSpaceInfo(t.position, csi);

		//I don't want this to be here
		if (cs_last.c[eCELL_I].x != csi.c[eCELL_I].x || cs_last.c[eCELL_I].y != csi.c[eCELL_I].y)
		{
			index::RemoveEntityCell(cs_last.c[eCELL_I].x, cs_last.c[eCELL_I].y, chr->id);
			index::AddEntityCell(csi.c[eCELL_I].x, csi.c[eCELL_I].y, chr->id);
		}

		//-------------------------------- SET HEIGHT AND CELL SPACE

		env::GetHeight(t.height, csi);

		//-------------------------------- RUN COLLISION & AI

		if (!cfg::bEditMode)
		{
			index::EntDeintersect(chr, csi, viewYaw.Deg(), true);
			if (aiControlled) index::ActorRunAI(chr->id); // Run AI
			else atk_target = index::GetViewTargetEntity(chr->id, 100.f, fac::enemy);
			//atk_target = index::GetViewTargetEntity(index, 100.f, fac::enemy);
		}
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

	t_body.SetPosition(m::Vector3(t.position.x, 0.1f + t.height + 0.6f, t.position.y));
	t_body.Rotate(t.yaw.Rad(), m::Vector3(0, 1, 0));

	ani_body_lean = m::Lerp(ani_body_lean, input * m::Vector2(8.f, 15.f), 0.25f);

	t_body.Rotate(glm::radians(ani_body_lean.y), m::Vector3(1, 0, 0));
	t_body.Rotate(glm::radians(ani_body_lean.x), m::Vector3(0, 0, 1));

	// Set head transform
	t_head.SetPosition(t_body.GetPosition());
	t_head.Rotate(viewYaw.Rad(), m::Vector3(0, 1, 0));
	t_head.Rotate(viewPitch.Rad(), m::Vector3(1, 0, 0));
	t_head.Translate(t_body.GetUp() * 0.7f);
	t_head.SetScale(m::Vector3(0.95f, 0.95f, 0.95f));

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
	#undef moving
	#undef csi
	#undef aiControlled
	#undef atk_target
}

void TickEditorPawn(void* ent, btf32 dt)
{
	EditorPawn* chr = (EditorPawn*)ent;

	#define t_body chr->t_body
	#define viewYaw chr->viewYaw
	#define t chr->t
	#define speed chr->speed
	#define inventory chr->inventory
	#define inv_active_slot chr->inv_active_slot
	#define t_skin chr->t_skin
	#define viewPitch chr->viewPitch
	#define t_head chr->t_head
	#define state chr->state
	#define input chr->input
	#define moving chr->moving
	#define csi chr->csi
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

	if (state.properties.get(ActiveState::eALIVE))
	{
		if (cfg::bEditMode)
		{
			//ENTITY[index]->t.velocity = fw::Lerp(ENTITY[index]->t.velocity, fw::Rotate(f2Input, aViewYaw.Rad()) * fw::Vector2(-1.f, 1.f) * dt * fSpeed, 0.3f);
			input.x = -input.x;
			t.velocity = m::Rotate(input, viewYaw.Rad()) * m::Vector2(-1.f, 1.f) * dt * 5.f;
		}
		else
		{
			if (can_move)
			{
				input.x = -input.x;
				t.velocity = m::Lerp(t.velocity, m::Rotate(input, viewYaw.Rad()) * m::Vector2(-1.f, 1.f) * dt * speed, 0.2f);
			}
			else
			{
				input = m::Vector2(0.f, 0.f);
				t.velocity = m::Lerp(t.velocity, m::Vector2(0.f, 0.f), 0.2f);
			}
			if (can_turn && abs(m::AngDif(t.yaw.Deg(), viewYaw.Deg())) > 65.f || m::Length(input) > 0.2f)
				t.yaw.RotateTowards(viewYaw.Deg(), 8.f); // Rotate body towards the target direction
		}

		//-------------------------------- APPLY MOVEMENT

		moving = (m::Length(t.velocity) > 0.016f);
		m::Vector2 oldpos = t.position;
		t.position += t.velocity; // Apply velocity

		CellSpace cs_last = csi;

		//regenerate csi
		index::GetCellSpaceInfo(t.position, csi);

		//I don't want this to be here
		if (cs_last.c[eCELL_I].x != csi.c[eCELL_I].x || cs_last.c[eCELL_I].y != csi.c[eCELL_I].y)
		{
			index::RemoveEntityCell(cs_last.c[eCELL_I].x, cs_last.c[eCELL_I].y, chr->id);
			index::AddEntityCell(csi.c[eCELL_I].x, csi.c[eCELL_I].y, chr->id);
		}

		//-------------------------------- SET HEIGHT AND CELL SPACE

		env::GetHeight(t.height, csi);

		//-------------------------------- RUN COLLISION & AI

		if (!cfg::bEditMode)
		{
			index::EntDeintersect(chr, csi, viewYaw.Deg(), true);
			if (aiControlled) index::ActorRunAI(chr->id); // Run AI
			else atk_target = index::GetViewTargetEntity(chr->id, 100.f, fac::enemy);
			//atk_target = index::GetViewTargetEntity(index, 100.f, fac::enemy);
		}
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

	t_body.SetPosition(m::Vector3(t.position.x, 0.1f + t.height + 0.7f, t.position.y));
	t_body.Rotate(t.yaw.Rad(), m::Vector3(0, 1, 0));

	// Set head transform
	t_head.SetPosition(t_body.GetPosition());
	t_head.Rotate(viewYaw.Rad(), m::Vector3(0, 1, 0));
	t_head.Rotate(viewPitch.Rad(), m::Vector3(1, 0, 0));
	t_head.Translate(t_body.GetUp() * 0.7f);
	t_head.SetScale(m::Vector3(0.95f, 0.95f, 0.95f));

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
	#undef moving
	#undef csi
	#undef aiControlled
	#undef atk_target
}
