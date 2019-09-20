#include "objects.h"

#include "graphics.hpp"
#include <glm\gtc\matrix_transform.hpp>

#include "archive.hpp"

#include "index.h"
#include "cfg.h"

void DrawMeshAtTransform(btID id, assetID mdl, assetID tex, graphics::Shader& shd, Transform3D transform)
{
	// Enable the shader
	shd.Use();

	// set object ID on shader
	shd.SetBool("idn", id == ID_NULL);
	shd.SetUint("id", id);

	// Set matrices on shader
	shd.setMat4("matp", graphics::GetMatProj());
	shd.setMat4("matv", graphics::GetMatView());
	shd.setMat4("matm", transform.getModelMatrix());

	// Render the mesh
	//mdl.Draw(tex.id, shd.ID);
	res::GetMesh(mdl).Draw(res::GetTexture(tex).glID, shd.ID);
}

void DrawMesh(btID id, assetID mdl, assetID tex, graphics::Shader& shd, glm::mat4 matrix)
{
	// Enable the shader
	shd.Use();

	// set object ID on shader
	shd.SetBool("idn", id == ID_NULL);
	shd.SetUint("id", id);

	// Set matrices on shader
	shd.setMat4("matp", graphics::GetMatProj());
	shd.setMat4("matv", graphics::GetMatView());
	shd.setMat4("matm", matrix);

	// Render the mesh
	res::GetMesh(mdl).Draw(res::GetTexture(tex).glID, shd.ID);
}

void DrawMesh(btID id, assetID mdl, assetID tex, graphics::Shader& shd, graphics::Matrix4x4 matrix)
{
	// Enable the shader
	shd.Use();

	// set object ID on shader
	shd.SetBool("idn", id == ID_NULL);
	shd.SetUint("id", id);

	// Set matrices on shader
	shd.setMat4("matp", graphics::GetMatProj());
	shd.setMat4("matv", graphics::GetMatView());
	shd.setMat4("matm", matrix);

	// Render the mesh
	//if (res::IsMesh(mdl))
	res::GetMesh(mdl).Draw(res::GetTexture(tex).glID, shd.ID);
	//else
	//	res::GetMesh(DEFAULT_MESH).Draw(res::GetTexture(DEFAULT_TEXTURE).glID, shd.ID);
}

void DrawMesh(btID id, assetID mdl, graphics::Shader& shd, graphics::Matrix4x4 matrix)
{
	// Enable the shader
	shd.Use();

	// set object ID on shader
	shd.SetBool("idn", id == ID_NULL);
	shd.SetUint("id", id);

	// Set matrices on shader
	shd.setMat4("matp", graphics::GetMatProj());
	shd.setMat4("matv", graphics::GetMatView());
	shd.setMat4("matm", matrix);

	// Render the mesh
	//if (res::IsMesh(mdl))
	res::GetMesh(mdl).Draw(res::GetTexture(res::t_default).glID, shd.ID);
	//else
	//	res::GetMesh(DEFAULT_MESH).Draw(res::GetTexture(DEFAULT_TEXTURE).glID, shd.ID);
}

void DrawBlendMeshAtTransform(btID id, assetID mdl, btf32 bs, assetID tex, graphics::Shader& shd, Transform3D transform)
{
	// Enable the shader
	shd.Use();

	// set object ID on shader
	shd.SetBool("idn", id == ID_NULL);
	shd.SetUint("id", id);

	// Set matrices on shader
	shd.setMat4("matp", graphics::GetMatProj());
	shd.setMat4("matv", graphics::GetMatView());
	shd.SetFloat("blendState", bs);
	shd.setMat4("matm", transform.getModelMatrix());

	// Render the mesh
	res::GetMeshBlend(mdl).Draw(res::GetTexture(tex).glID, shd.ID);
}

void DrawBlendMesh(btID id, assetID mdl, btf32 bs, assetID tex, graphics::Shader& shd, graphics::Matrix4x4 matrix)
{
	// Enable the shader
	shd.Use();

	// set object ID on shader
	shd.SetBool("idn", id == ID_NULL);
	shd.SetUint("id", id);

	// Set matrices on shader
	shd.setMat4("matp", graphics::GetMatProj());
	shd.setMat4("matv", graphics::GetMatView());
	shd.SetFloat("blendState", bs);
	shd.setMat4("matm", matrix);

	// Render the mesh
	res::GetMeshBlend(mdl).Draw(res::GetTexture(tex).glID, shd.ID);
}

void HeldGun::Tick()
{
	// State switch
	///*
	if (input::Get(input::key::eACTION_A_HIT))
	{
		btui8 i = eMusketHoldState;
		++i;
		if (i == eHOLD_STATE_COUNT)
			i = 0ui8;
		eMusketHoldState = (musket_hold_state)i;
	}
	//*/

	if (eMusketHoldState == HeldGun::aim) // IF AIMING
	{
		/*if (input::Get(input::key::eACTION_A_HIT))
			eMusketHoldState = HeldGun::inspect_pan;
		if (input::Get(input::key::eACTION_B_HIT))
			eMusketHoldState = HeldGun::inspect_barrel;*/
		bvMusketState.setto(HeldGun::musket_state::latch_pulled, input::Get(input::key::eUSE_HOLD));
	}
	else if (eMusketHoldState == HeldGun::inspect_pan) // IF PAN
	{
		/*if (input::Get(input::key::eACTION_A_HIT))
			eMusketHoldState = HeldGun::aim;*/
		if (input::Get(input::key::eACTION_B_HIT))
			bvMusketState.toggle(HeldGun::musket_state::fpan_hatch_open);
		if (bvMusketState.get(HeldGun::musket_state::fpan_hatch_open)) // if hatch open
			if (input::Get(input::key::eACTION_C_HIT)) // input
				bvMusketState.set(HeldGun::musket_state::fpan_powder_in); // set powder
		bvMusketState.setto(HeldGun::musket_state::latch_pulled, input::Get(input::key::eUSE_HOLD));
	}
	else if (eMusketHoldState == HeldGun::inspect_barrel) // IF BARREL
	{
		// if the hatch is open powder falls out lol
		if (bvMusketState.get(HeldGun::fpan_hatch_open))
			bvMusketState.unset(HeldGun::fpan_powder_in);

		/*if (input::Get(input::key::eACTION_A_HIT))
			eMusketHoldState = HeldGun::aim;*/

		if (input::Get(input::key::eACTION_B_HIT)) // set latch float
			bvMusketState.toggle(HeldGun::musket_state::barrel_rod_in);

		rod = m::Lerp(rod, (btf32)bvMusketState.get(HeldGun::musket_state::barrel_rod_in), 0.1f);

		if (rod > 0.95f)
			bvMusketState.set(HeldGun::musket_state::barrel_armed); // unset powder
	}

	fpan = m::Lerp(fpan, (btf32)bvMusketState.get(HeldGun::musket_state::fpan_hatch_open), 0.3f);
	lever = m::Lerp(lever, (btf32)bvMusketState.get(HeldGun::musket_state::latch_pulled), 0.2f);

	if (lever > 0.95f
		//&& bvMusketState.get(Musket::get_can_fire))
		&& bvMusketState.get(HeldGun::barrel_armed)
		&& bvMusketState.get(HeldGun::fpan_hatch_open)
		&& bvMusketState.get(HeldGun::fpan_powder_in))
	{
		bvMusketState.unset(HeldGun::unset_fire); // unset
		//EntityCastProj(index);
	}
	else if (lever > 0.95f
		//&& bvMusketState.get(Musket::get_can_fire))
		&& bvMusketState.get(HeldGun::fpan_hatch_open)
		&& bvMusketState.get(HeldGun::fpan_powder_in))
	{
		bvMusketState.unset(HeldGun::unset_fire); // unset
	}
}

void HeldGun::Draw(m::Vector2 pos, btf32 height, m::Angle ang, m::Angle pitch2)
{
	t_item.SetPosition(m::Vector3(pos.x, height, pos.y));
	t_item.SetRotation(0.f);
	t_item.Rotate(ang.Rad(), m::Vector3(0, 1, 0));

	switch ((eMusketHoldState))
	{
	case HeldGun::aim:
		loc = m::Lerp(loc, m::Vector3(0.13f, 1.3f, 0.3f), 0.1f);
		yaw = m::Lerp(yaw, 0.f, 0.1f);
		pitch = m::Lerp(pitch, pitch2.Deg(), 0.1f);
		break;
	case HeldGun::inspect_pan:
		/*loc = m::Lerp(loc, m::Vector3(-0.13f, 1.2f, 0.6f), 0.1f);
		yaw = m::Lerp(yaw, 65.f, 0.1f);
		pitch = m::Lerp(pitch, -15.f, 0.1f);*/
		loc = m::Lerp(loc, m::Vector3(0.f, 1.f, 0.4f), 0.1f);
		yaw = m::Lerp(yaw, -15.f, 0.1f);
		pitch = m::Lerp(pitch, -30.f, 0.1f);
		break;
	case HeldGun::inspect_barrel:
		loc = m::Lerp(loc, m::Vector3(0.f, 0.2f, 0.4f), 0.1f);
		yaw = m::Lerp(yaw, 0.f, 0.1f);
		pitch = m::Lerp(pitch, -80.f, 0.1f);
		break;
	}

	t_item.TranslateLocal(loc); // set pose
	t_item.Rotate(glm::radians(yaw), m::Vector3(0.f, 1.f, 0.f));
	t_item.Rotate(glm::radians(pitch), m::Vector3(1.f, 0.f, 0.f));

	DrawMeshAtTransform(ID_NULL, res::m_item_matchlock_01, res::t_item_matchlock_01, graphics::shader_solid, t_item);

	if (bvMusketState.get(HeldGun::musket_state::fpan_powder_in))
		DrawBlendMeshAtTransform(ID_NULL, res::mb_item_matchlock_01_pan_full, fpan, res::t_item_matchlock_01, graphics::shader_blend, t_item);
	else
		DrawBlendMeshAtTransform(ID_NULL, res::mb_item_matchlock_01_pan, fpan, res::t_item_matchlock_01, graphics::shader_blend, t_item);

	DrawBlendMeshAtTransform(ID_NULL, res::mb_item_matchlock_01_lever, lever, res::t_item_matchlock_01, graphics::shader_blend, t_item);

	if (eMusketHoldState == HeldGun::inspect_barrel || bvMusketState.get(HeldGun::musket_state::barrel_rod_in)) // IF BARREL
		DrawBlendMeshAtTransform(ID_NULL, res::mb_item_matchlock_01_rod, rod, res::t_item_matchlock_01, graphics::shader_blend, t_item);
}

void Entity::Tick()
{
}

void Entity::Draw()
{
}

void EItem::Tick()
{
}

void EItem::Draw()
{
	btID index = 0u;
	// Draw the mesh of our item id
	DrawMeshAtTransform(index, (res::AssetConstantID)archive::items[itemid]->id_mesh, (res::AssetConstantID)archive::items[itemid]->id_tex, graphics::shader_solid, t_item);
}

void Actor::PickUpItem(btID id)
{
	EItem* item = (EItem*)index::GetEntity(id);
	inventory.AddItem(item->itemid);
	index::DestroyEntity(id);
}

void Actor::DropItem(btID slot)
{
	if (slot < inventory.Get()->invSize)
	{
		index::SpawnItem(inventory.Get()->items[slot], t.position, yaw.Deg());
		inventory.RemvItemAt(slot);
		//inventory.RemvItem(itemID);
	}
}

void Actor::Tick()
{
}

void Actor::Draw()
{
}

void Chara::Tick()
{
}

void Chara::Draw()
{
	// draw body
	btID index = 0u;

	// need a good way of knowing own index
	DrawBlendMeshAtTransform(index, res::mb_legs, 0, t_skin, graphics::shader_blend, t_body);

	// draw arms

	graphics::Matrix4x4 matrix;

	m::Vector3 newpos2 = m::Vector3(t.position.x, 0.65f + t.height, t.position.y) + t_body.GetUp() * 0.75f;

	m::Vector3 newpos_l = newpos2 + t_body.GetRight() * 0.1f;
	m::Vector3 newpos_r = newpos2 + t_body.GetRight() * -0.1f;

	m::Vector3 hand_pos_l = heldItem.t_item.GetPosition() + heldItem.t_item.GetUp() * -0.08f;
	m::Vector3 hand_pos_r = heldItem.t_item.GetPosition() + heldItem.t_item.GetForward() * 0.5f;

	btf32 dist_l = m::Length(newpos_l - hand_pos_l) * 1.5f - 0.5f;
	btf32 dist_r = m::Length(newpos_r - hand_pos_r) * 1.5f - 0.5f;

	graphics::MatrixTransform(matrix, newpos_l, hand_pos_l - newpos_l, (t_body.GetRight() * -1.f) + t_body.GetUp());
	DrawBlendMesh(index, res::mb_armscast, dist_l, t_skin, graphics::shader_blend, matrix);
	graphics::MatrixTransform(matrix, newpos_r, hand_pos_r - newpos_r, t_body.GetRight() + t_body.GetUp());
	DrawBlendMesh(index, res::mb_armscast, dist_r, t_skin, graphics::shader_blend, matrix);

	// draw legs

	btf32 hip_width = 0.125f;

	m::Vector3 newpos = m::Vector3(t.position.x, 0.68f + t.height, t.position.y);

	m::Vector3 newpos_hip_l = newpos + t_body.GetRight() * hip_width;
	m::Vector3 newpos_hip_r = newpos + t_body.GetRight() * -hip_width;

	m::Vector3 velocity = m::Normalize(m::Vector3(t.velocity.x, 0.f, t.velocity.y));

	if (foot_state == eL_DOWN)
	{
		if (m::Length(t.velocity) < 0.025f) // if below a certain speed, switch to standing pose
		{
			foot_pos_l = m::Vector3(t.position.x, t.height, t.position.y) + t_body.GetRight() * hip_width;
			foot_state = eBOTH_DOWN;
		}
		else // else hang foot
		{
			foot_pos_l = m::Vector3(t.position.x, t.height + 0.15f, t.position.y) + t_body.GetRight() * hip_width + velocity * 0.3f;
		}
		// if too far off balance
		if (m::Length(newpos_hip_r - foot_pos_r) > 1.f)
		{
			foot_pos_l = m::Vector3(t.position.x, t.height, t.position.y) + t_body.GetRight() * hip_width + velocity * 0.7f;
			foot_state = eR_DOWN;
		}
	}
	else if (foot_state == eR_DOWN)
	{
		if (m::Length(t.velocity) < 0.025f) // if below a certain speed, switch to standing pose
		{
			foot_pos_r = m::Vector3(t.position.x, t.height, t.position.y) + t_body.GetRight() * -hip_width;
			foot_state = eBOTH_DOWN;
		}
		else // else hang foot
		{
			foot_pos_r = m::Vector3(t.position.x, t.height + 0.15f, t.position.y) + t_body.GetRight() * -hip_width + velocity * 0.3f;
		}
		// if too far off balance
		if (m::Length(newpos_hip_l - foot_pos_l) > 1.f)
		{
			foot_pos_r = m::Vector3(t.position.x, t.height, t.position.y) + t_body.GetRight() * -hip_width + velocity * 0.7f;
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
		else if (m::Length(newpos_hip_r - foot_pos_r) > 1.f)
		{
			foot_pos_r = m::Vector3(t.position.x, t.height, t.position.y) + t_body.GetRight() * -hip_width + velocity * 0.7f;
			foot_state = eR_DOWN;
		}
		else if (m::Length(newpos_hip_l - foot_pos_l) > 1.f)
		{
			foot_pos_l = m::Vector3(t.position.x, t.height, t.position.y) + t_body.GetRight() * hip_width + velocity * 0.7f;
			foot_state = eL_DOWN;
		}
	}

	//foot_pos_l_interp = m::Lerp(foot_pos_l_interp, foot_pos_l, 0.15f);
	//foot_pos_r_interp = m::Lerp(foot_pos_r_interp, foot_pos_r, 0.15f);
	foot_pos_l_interp = m::Lerp(foot_pos_l_interp, foot_pos_l, 0.2f);
	foot_pos_r_interp = m::Lerp(foot_pos_r_interp, foot_pos_r, 0.2f);

	dist_l = m::Length(newpos_hip_l - foot_pos_l_interp) * 2.f - 1.f;
	dist_r = m::Length(newpos_hip_r - foot_pos_r_interp) * 2.f - 1.f;

	graphics::MatrixTransformXFlip(matrix, newpos_hip_l, foot_pos_l_interp - newpos_hip_l, t_body.GetForward());
	graphics::SetFrontFaceInverse();
	DrawBlendMesh(index, res::mb_char_leg, dist_l, t_skin, graphics::shader_blend, matrix);
	graphics::SetFrontFace();
	graphics::MatrixTransform(matrix, newpos_hip_r, foot_pos_r_interp - newpos_hip_r, t_body.GetForward());
	DrawBlendMesh(index, res::mb_char_leg, dist_r, t_skin, graphics::shader_blend, matrix);

	// draw head

	DrawBlendMeshAtTransform(index, res::mb_char_head, 0, t_skin, graphics::shader_blend, t_head);
	DrawMeshAtTransform(index, res::m_equip_head_pickers, res::t_equip_atlas, graphics::shader_solid, t_head);

	// draw item

	heldItem.Draw(t.position, t.height, viewYaw, viewPitch);
}