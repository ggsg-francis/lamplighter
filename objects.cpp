#include "objects.h"

#include "graphics.hpp"
#include <glm\gtc\matrix_transform.hpp>

#include "resources.h"
#include "resources-archive.h"

#include "index.h"
#include "cfg.h"

void DrawMeshAtTransform(btID id, res::assetid mdl, res::assetid tex, graphics::Shader& shd, Transform3D transform)
{
	// Enable the shader
	shd.Use();

	// set object ID on shader
	shd.SetBool("idn", id == ID_NULL);
	shd.SetUint("id", id);

	// Set matrices on shader
	shd.setMat4("projection", graphics::GetMatProj());
	shd.setMat4("view", graphics::GetMatView());
	shd.setMat4("model", transform.getModelMatrix());

	// Render the mesh
	//mdl.Draw(tex.id, shd.ID);
	res::GetBTM(mdl).Draw(res::GetBTT(tex).glID, shd.ID);
}

void DrawMesh(btID id, res::assetid mdl, res::assetid tex, graphics::Shader& shd, glm::mat4 matrix)
{
	// Enable the shader
	shd.Use();

	// set object ID on shader
	shd.SetBool("idn", id == ID_NULL);
	shd.SetUint("id", id);

	// Set matrices on shader
	shd.setMat4("projection", graphics::GetMatProj());
	shd.setMat4("view", graphics::GetMatView());
	shd.setMat4("model", matrix);

	// Render the mesh
	res::GetBTM(mdl).Draw(res::GetBTT(tex).glID, shd.ID);
}

void DrawMesh(btID id, res::assetid mdl, res::assetid tex, graphics::Shader& shd, graphics::Matrix4x4 matrix)
{
	// Enable the shader
	shd.Use();

	// set object ID on shader
	shd.SetBool("idn", id == ID_NULL);
	shd.SetUint("id", id);

	// Set matrices on shader
	shd.setMat4("projection", graphics::GetMatProj());
	shd.setMat4("view", graphics::GetMatView());
	shd.setMat4("model", matrix);

	// Render the mesh
	res::GetBTM(mdl).Draw(res::GetBTT(tex).glID, shd.ID);
}

void DrawBlendMeshAtTransform(btID id, res::assetid mdl, btf32 bs, res::assetid tex, graphics::Shader& shd, Transform3D transform)
{
	// Enable the shader
	shd.Use();

	// set object ID on shader
	shd.SetBool("idn", id == ID_NULL);
	shd.SetUint("id", id);

	// Set matrices on shader
	shd.setMat4("projection", graphics::GetMatProj());
	shd.setMat4("view", graphics::GetMatView());
	shd.SetFloat("blendState", bs);
	shd.setMat4("model", transform.getModelMatrix());

	// Render the mesh
	res::GetBTMB(mdl).Draw(res::GetBTT(tex).glID, shd.ID);
}

void DrawBlendMesh(btID id, res::assetid mdl, btf32 bs, res::assetid tex, graphics::Shader& shd, graphics::Matrix4x4 matrix)
{
	// Enable the shader
	shd.Use();

	// set object ID on shader
	shd.SetBool("idn", id == ID_NULL);
	shd.SetUint("id", id);

	// Set matrices on shader
	shd.setMat4("projection", graphics::GetMatProj());
	shd.setMat4("view", graphics::GetMatView());
	shd.SetFloat("blendState", bs);
	shd.setMat4("model", matrix);

	// Render the mesh
	res::GetBTMB(mdl).Draw(res::GetBTT(tex).glID, shd.ID);
}

void Musket::Tick()
{
	if (eMusketHoldState == Musket::aim) // IF AIMING
	{
		if (input::Get(input::key::action_a_hit))
			eMusketHoldState = Musket::inspect_pan;
		if (input::Get(input::key::action_b_hit))
			eMusketHoldState = Musket::inspect_barrel;
		bvMusketState.setto(Musket::musket_state::latch_pulled, input::Get(input::key::atk_held));
	}
	else if (eMusketHoldState == Musket::inspect_pan) // IF PAN
	{
		if (input::Get(input::key::action_a_hit))
			eMusketHoldState = Musket::aim;
		if (input::Get(input::key::action_b_hit))
			bvMusketState.toggle(Musket::musket_state::fpan_hatch_open);
		if (bvMusketState.get(Musket::musket_state::fpan_hatch_open)) // if hatch open
			if (input::Get(input::key::action_c_hit)) // input
				bvMusketState.set(Musket::musket_state::fpan_powder_in); // set powder
		bvMusketState.setto(Musket::musket_state::latch_pulled, input::Get(input::key::atk_held));
	}
	else if (eMusketHoldState == Musket::inspect_barrel) // IF BARREL
	{
		// if the hatch is open powder falls out lol
		if (bvMusketState.get(Musket::fpan_hatch_open))
			bvMusketState.unset(Musket::fpan_powder_in);

		if (input::Get(input::key::action_a_hit))
			eMusketHoldState = Musket::aim;

		if (input::Get(input::key::atk_hit)) // set latch float
			bvMusketState.toggle(Musket::musket_state::barrel_rod_in);

		rod = m::Lerp(rod, (btf32)bvMusketState.get(Musket::musket_state::barrel_rod_in), 0.1f);

		if (rod > 0.95f)
			bvMusketState.set(Musket::musket_state::barrel_armed); // unset powder
	}

	fpan = m::Lerp(fpan, (btf32)bvMusketState.get(Musket::musket_state::fpan_hatch_open), 0.3f);
	lever = m::Lerp(lever, (btf32)bvMusketState.get(Musket::musket_state::latch_pulled), 0.2f);

	if (lever > 0.95f
		//&& bvMusketState.get(Musket::get_can_fire))
		&& bvMusketState.get(Musket::barrel_armed)
		&& bvMusketState.get(Musket::fpan_hatch_open)
		&& bvMusketState.get(Musket::fpan_powder_in))
	{
		bvMusketState.unset(Musket::unset_fire); // unset
		//EntityCastProj(index);
	}
	else if (lever > 0.95f
		//&& bvMusketState.get(Musket::get_can_fire))
		&& bvMusketState.get(Musket::fpan_hatch_open)
		&& bvMusketState.get(Musket::fpan_powder_in))
	{
		bvMusketState.unset(Musket::unset_fire); // unset
	}
}

void Musket::Draw(m::Vector2 pos, btf32 height, m::Angle ang)
{
	Transform3D t_item;

	t_item.SetPosition(m::Vector3(pos.x, height, pos.y));
	t_item.Rotate(ang.Rad(), m::Vector3(0, 1, 0));

	switch ((eMusketHoldState))
	{
	case Musket::aim:
		loc = m::Lerp(loc, m::Vector3(0.13f, 0.9f, 0.3f), 0.1f);
		yaw = m::Lerp(yaw, 0.f, 0.1f);
		pitch = m::Lerp(pitch, 0.f, 0.1f);
		break;
	case Musket::inspect_pan:
		loc = m::Lerp(loc, m::Vector3(-0.23f, 1.2f, 0.4f), 0.1f);
		yaw = m::Lerp(yaw, 45.f, 0.1f);
		pitch = m::Lerp(pitch, -15.f, 0.1f);
		break;
	case Musket::inspect_barrel:
		loc = m::Lerp(loc, m::Vector3(0.f, 0.2f, 0.4f), 0.1f);
		yaw = m::Lerp(yaw, 0.f, 0.1f);
		pitch = m::Lerp(pitch, -80.f, 0.1f);
		break;
	}

	t_item.TranslateLocal(loc); // set pose
	t_item.Rotate(glm::radians(yaw), m::Vector3(0.f, 1.f, 0.f));
	t_item.Rotate(glm::radians(pitch), m::Vector3(1.f, 0.f, 0.f));

	DrawMeshAtTransform(ID_NULL, res::m_item_matchlock_01, res::t_item_matchlock_01, graphics::shader_solid, t_item);

	if (bvMusketState.get(Musket::musket_state::fpan_powder_in))
		DrawBlendMeshAtTransform(ID_NULL, res::mb_item_matchlock_01_pan_full, fpan, res::t_item_matchlock_01, graphics::shader_blend, t_item);
	else
		DrawBlendMeshAtTransform(ID_NULL, res::mb_item_matchlock_01_pan, fpan, res::t_item_matchlock_01, graphics::shader_blend, t_item);

	DrawBlendMeshAtTransform(ID_NULL, res::mb_item_matchlock_01_lever, lever, res::t_item_matchlock_01, graphics::shader_blend, t_item);

	if (eMusketHoldState == Musket::inspect_barrel || bvMusketState.get(Musket::musket_state::barrel_rod_in)) // IF BARREL
		DrawBlendMeshAtTransform(ID_NULL, res::mb_item_matchlock_01_rod, rod, res::t_item_matchlock_01, graphics::shader_blend, t_item);
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