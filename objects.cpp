#include "objects.h"
#include "objects_items.h"

#include "graphics.hpp"
#include <glm\gtc\matrix_transform.hpp>

#include "archive.hpp"

#include "index.h"
#include "cfg.h"

graphics::GUIText text_inventory_temp;
graphics::GUIBox guibox_selection;

ItemSlot::ItemSlot(btID _item)
{
	switch (acv::item_types[_item])
	{
	case acv::types::ITEM_EQUIP:
		heldInstance = new HeldItem;
		break;
	case acv::types::ITEM_WPN_MELEE:
		heldInstance = new HeldMel;
		break;
	case acv::types::ITEM_WPN_MATCHGUN:
		heldInstance = new HeldGun;
		//heldInstance = new HeldGunMatchLock;
		break;
	case acv::types::ITEM_WPN_MAGIC:
		heldInstance = new HeldMgc;
		break;
	case acv::types::ITEM_CONS:
		heldInstance = new HeldItem;
		break;
	default:
		heldInstance = new HeldItem;
		break;
	}
	item = _item;
}
ItemSlot::~ItemSlot()
{
	delete heldInstance;
}

inline void Inventory::IncrStack(btui32 index)
{
	++items[index].count;
}
inline void Inventory::DecrStack(btui32 index)
{
	if (items[index].count > 1u)
		--items[index].count;
	else
		items.Remove(index);
}
void Inventory::AddItem(btID itemid)
{
	//bool added = false;
	//for (int i = 0; i < items.Size(); ++i)
	//	if (items.Used(i) && items[i].item == itemid) // if we already have a stack of this item
	//	{
	//		IncrStack(i); added = true; // Add to existing stack
	//	}
	////if (!added) items.Add(new ItemSlot(HeldItem(), itemid));
	//if (!added) items.Add(new ItemSlot(itemid));
	items.Add(new ItemSlot(itemid));
}
void Inventory::RemvItem(btID itemid)
{
	//for (int i = 0; i < items.Size(); ++i)
	//	if (items[i].item == itemid) // if we already have a stack of this item
	//		DecrStack(i);
	for (int i = 0; i < items.Size(); ++i)
		if (items[i].item == itemid) // if we already have a stack of this item
			items.Remove(i);
}
//void Inventory::RemvItemAt(btui32 index) { DecrStack(index); }
void Inventory::RemvItemAt(btui32 index) { items.Remove(index); }
void Inventory::Draw(btui16 active_slot)
{
	int p1_x_start = -(int)graphics::FrameSizeX() / 2;
	int p1_y_start = -(int)graphics::FrameSizeY() / 2;

	const bti32 invspace = 38;

	bti32 offset = p1_x_start + 96 - 16;
	for (btui16 i = 0; i < 10; i++)
	{
		if (i < items.Size() && items.Used(i))
		{
			if (i == active_slot)
				graphics::DrawGUITexture(&res::GetT(acv::items[items[i].item]->id_icon), offset + i * invspace, p1_y_start + 24, 64, 64);
			else
				graphics::DrawGUITexture(&res::GetT(acv::items[items[i].item]->id_icon), offset + i * invspace, p1_y_start + 16, 64, 64);
		}
	}
	guibox_selection.ReGen((offset + active_slot * invspace) - 12, (offset + active_slot * invspace) + 12, p1_y_start + 12, p1_y_start + 36, 8, 8);
	guibox_selection.Draw(&res::GetT(res::t_gui_select_box));
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

void Entity::Tick(btID index, btf32 dt)
{
}

void Entity::Draw(btID index)
{
}

void EItem::Tick(btID index, btf32 dt)
{
	//t.position.y += 0.01f;

	//CellSpace cs_last = csi;

	//index::GetCellSpaceInfo(t.position, csi);

	/*if (cs_last.c[eCELL_I].x != csi.c[eCELL_I].x || cs_last.c[eCELL_I].y != csi.c[eCELL_I].y)
	{
		index::RemoveEntityCell(cs_last.c[eCELL_I].x, cs_last.c[eCELL_I].y, index);
		index::AddEntityCell(csi.c[eCELL_I].x, csi.c[eCELL_I].y, index);
	}*/

	//env::GetHeight(t.height, csi);
	t_item.SetPosition(m::Vector3(t.position.x, t.height + acv::items[itemid]->f_model_height, t.position.y));
	t_item.SetRotation(yaw.Rad());
}

void EItem::Draw(btID index)
{
	// Draw the mesh of our item id
	DrawMesh(index, res::GetM(acv::items[itemid]->id_mesh), res::GetT(acv::items[itemid]->id_tex), SS_NORMAL, t_item.getMatrix());
}

void Actor::PickUpItem(btID id)
{
	EItem* item = (EItem*)index::GetEntity(id);
	inventory.AddItem(item->itemid);
	index::DestroyEntity(id);
}

void Actor::DropItem(btID slot)
{
	if (slot < inventory.items.Size() && inventory.items.Used(slot))
	{
		index::SpawnItem(inventory.items[slot].item, t.position, yaw.Deg());
		inventory.RemvItemAt(slot);
	}
}

void Actor::SetEquipSlot()
{
}

void Actor::IncrEquipSlot()
{
	if (inv_active_slot < inventory.items.Size() - 1u)
	{
		++inv_active_slot;
		if (inventory.items.Used(inv_active_slot))
			inventory.items[inv_active_slot].heldInstance->OnEquip();
	}
}

void Actor::DecrEquipSlot()
{
	if (inv_active_slot > 0u)
	{
		--inv_active_slot;
		if (inventory.items.Used(inv_active_slot))
			inventory.items[inv_active_slot].heldInstance->OnEquip();
	}
}

void Actor::Tick(btID index, btf32 dt)
{
}

void Actor::Draw(btID index)
{
}

void Chara::Tick(btID index, btf32 dt)
{
	bool can_move = true;
	bool can_turn = true;
	if (inventory.items.Used(inv_active_slot))
	{
		#define HELDINSTANCE inventory.items[inv_active_slot].heldInstance
		can_move = !HELDINSTANCE->BlockMove();
		can_turn = !HELDINSTANCE->BlockTurn();
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
			if (can_turn && abs(m::AngDif(yaw.Deg(), viewYaw.Deg())) > 65.f || m::Length(input) > 0.2f)
				yaw.RotateTowards(viewYaw.Deg(), 8.f); // Rotate body towards the target direction
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
			index::RemoveEntityCell(cs_last.c[eCELL_I].x, cs_last.c[eCELL_I].y, index);
			index::AddEntityCell(csi.c[eCELL_I].x, csi.c[eCELL_I].y, index);
		}

		//-------------------------------- SET HEIGHT AND CELL SPACE

		env::GetHeight(t.height, csi);

		//-------------------------------- RUN COLLISION & AI

		if (!cfg::bEditMode)
		{
			index::EntDeintersect(this, csi, viewYaw.Deg(), true);
			if (aiControlled) index::ActorRunAI(index); // Run AI
			else atk_target = index::GetViewTargetEntity(index, 100.f, fac::enemy);
			//atk_target = index::GetViewTargetEntity(index, 100.f, fac::enemy);
		}
	} // End if alive

	if (inventory.items.Used(inv_active_slot))
	{
		#define HELDINSTANCE inventory.items[inv_active_slot].heldInstance
		if (HELDINSTANCE != nullptr) HELDINSTANCE->Tick(this);
		#undef HELDINSTANCE
	}

	//________________________________________________________________
	//------------- SET TRANSFORMATIONS FOR GRAPHICS -----------------

	// Reset transforms
	t_body = Transform3D();
	t_head = Transform3D();

	t_body.SetPosition(m::Vector3(t.position.x, 0.1f + t.height + 0.6f, t.position.y));
	t_body.Rotate(yaw.Rad(), m::Vector3(0, 1, 0));

	ani_body_lean = m::Lerp(ani_body_lean, input * m::Vector2(8.f, 15.f), 0.25f);

	t_body.Rotate(glm::radians(ani_body_lean.y), m::Vector3(1, 0, 0));
	t_body.Rotate(glm::radians(ani_body_lean.x), m::Vector3(0, 0, 1));

	// Set head transform
	t_head.SetPosition(t_body.GetPosition());
	t_head.Rotate(viewYaw.Rad(), m::Vector3(0, 1, 0));
	t_head.Rotate(viewPitch.Rad(), m::Vector3(1, 0, 0));
	t_head.Translate(t_body.GetUp() * 0.7f);
	t_head.SetScale(m::Vector3(0.95f, 0.95f, 0.95f));
}

void Chara::Draw(btID index)
{
	#define leglen 0.75f
	#define legDClen 1.f
	//#define legDClen 0.75f
	#define velocityStepMult 0.5f

	Transform3D t_test = t_body;
	t_test.Rotate(glm::radians(m::AngDif(viewYaw.Deg(), yaw.Deg())), m::Vector3(0, 1, 0));

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
		#define HELDINSTANCE inventory.items[inv_active_slot].heldInstance

		m::Vector3 handPosR = HELDINSTANCE->GetRightHandPos();
		m::Vector3 handPosL = HELDINSTANCE->GetLeftHandPos();

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
		DrawMeshDeform(index, res::GetMD(res::md_char_arm), res::skin_t[t_skin], SS_CHARA, 4u, matLegHipR, matLegUpR, matLegLoR, matLegFootR);
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
		DrawMeshDeform(index, res::GetMD(res::md_char_arm), res::skin_t[t_skin], SS_CHARA, 4u, matLegHipL, matLegUpL, matLegLoL, matLegFootL);

		// draw item
		HELDINSTANCE->Draw(inventory.items[inv_active_slot].item, t.position, t.height, viewYaw, viewPitch);

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
	DrawMeshDeform(index, res::GetMD(res::md_char_leg), res::GetT(res::t_equip_legs_robe_01), SS_CHARA, 4u, matLegHipR, matLegUpR, matLegLoR, matLegFootR);
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
	DrawMeshDeform(index, res::GetMD(res::md_char_leg), res::GetT(res::t_equip_legs_robe_01), SS_CHARA, 4u, matLegHipL, matLegUpL, matLegLoL, matLegFootL);
	// transform legL for cloak
	//graphics::MatrixTransformForwardUp(matrixLegL, t_body.GetPosition(), m::Normalize(vecfw * len - vecup * lenUp * 0.5f), t_body.GetForward());
	graphics::MatrixTransformForwardUp(matLegUpL, t_body.GetPosition(), footPosL - t_body.GetPosition(), t_body.GetForward());

	// draw head

	//DrawBlendMeshAtTransform(index, res::mb_char_head, 0, t_skin, graphics::shader_blend, t_head);
	DrawBlendMesh(index, res::GetMB(res::mb_char_head), 0, res::skin_t[t_skin], SS_CHARA, t_head.getMatrix());
	//DrawMeshAtTransform(index, res::m_proj_2, res::t_proj_2, graphics::shader_solid, t_head);
	//DrawMesh(index, res::GetM(res::m_equip_head_pickers), res::GetT(res::t_default), SS_NORMAL, t_head.getMatrix());

	// need a good way of knowing own index
	DrawMeshDeform(index, res::GetMD(res::md_chr_body), res::skin_t[t_skin], SS_CHARA, 2u,
		t_body.getMatrix(), t_test.getMatrix(), graphics::Matrix4x4(), graphics::Matrix4x4());
	DrawMeshDeform(index, res::GetMD(res::md_equip_body_robe_01), res::GetT(res::t_equip_body_robe_01), SS_CHARA, 4u,
		t_body.getMatrix(), t_test.getMatrix(), matLegUpL, matLegUpR);
}

m::Vector3 Chara::SetFootPos(m::Vector2 position)
{
	CellSpace cs;
	index::GetCellSpaceInfo(position, cs);
	btf32 height;
	env::GetHeight(height, cs);
	return m::Vector3(position.x, height, position.y);
}

void EditorPawn::Tick(btID index, btf32 dt)
{
	bool can_move = true;
	bool can_turn = true;
	if (inventory.items.Used(inv_active_slot))
	{
		#define HELDINSTANCE inventory.items[inv_active_slot].heldInstance
		can_move = !HELDINSTANCE->BlockMove();
		can_turn = !HELDINSTANCE->BlockTurn();
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
			if (can_turn && abs(m::AngDif(yaw.Deg(), viewYaw.Deg())) > 65.f || m::Length(input) > 0.2f)
				yaw.RotateTowards(viewYaw.Deg(), 8.f); // Rotate body towards the target direction
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
			index::RemoveEntityCell(cs_last.c[eCELL_I].x, cs_last.c[eCELL_I].y, index);
			index::AddEntityCell(csi.c[eCELL_I].x, csi.c[eCELL_I].y, index);
		}

		//-------------------------------- SET HEIGHT AND CELL SPACE

		env::GetHeight(t.height, csi);

		//-------------------------------- RUN COLLISION & AI

		if (!cfg::bEditMode)
		{
			index::EntDeintersect(this, csi, viewYaw.Deg(), true);
			if (aiControlled) index::ActorRunAI(index); // Run AI
			else atk_target = index::GetViewTargetEntity(index, 100.f, fac::enemy);
			//atk_target = index::GetViewTargetEntity(index, 100.f, fac::enemy);
		}
	} // End if alive

	if (inventory.items.Used(inv_active_slot))
	{
		#define HELDINSTANCE inventory.items[inv_active_slot].heldInstance
		if (HELDINSTANCE != nullptr) HELDINSTANCE->Tick(this);
		#undef HELDINSTANCE
	}

	//________________________________________________________________
	//------------- SET TRANSFORMATIONS FOR GRAPHICS -----------------

	// Reset transforms
	t_body = Transform3D();
	t_head = Transform3D();

	t_body.SetPosition(m::Vector3(t.position.x, 0.1f + t.height + 0.7f, t.position.y));
	t_body.Rotate(yaw.Rad(), m::Vector3(0, 1, 0));

	// Set head transform
	t_head.SetPosition(t_body.GetPosition());
	t_head.Rotate(viewYaw.Rad(), m::Vector3(0, 1, 0));
	t_head.Rotate(viewPitch.Rad(), m::Vector3(1, 0, 0));
	t_head.Translate(t_body.GetUp() * 0.7f);
	t_head.SetScale(m::Vector3(0.95f, 0.95f, 0.95f));
}

void EditorPawn::Draw(btID index)
{
	// need a good way of knowing own index
	DrawMesh(index, res::GetM(res::m_debug_bb), res::skin_t[t_skin], SS_NORMAL, t_body.getMatrix());

	// draw head

	//DrawBlendMesh(index, res::GetMB(res::mb_char_head), 0, res::skin_t[t_skin], SS_CHARA, t_head.getMatrix());
}
