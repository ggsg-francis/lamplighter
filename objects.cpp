#include "objects.h"

#include "graphics.hpp"
#include <glm\gtc\matrix_transform.hpp>

#include "resources.h"

#include "cfg.h"

void DrawMeshAtTransform(res::assetid mdl, res::assetid tex, Shader& shd, Transform3D transform)
{
	// Enable the shader
	shd.Use();

	// Set matrices on shader
	shd.setMat4("projection", graphics::GetMatProj());
	shd.setMat4("view", graphics::GetMatView());
	shd.setMat4("model", transform.getModelMatrix());

	// Render the mesh
	//mdl.Draw(tex.id, shd.ID);
	res::GetBTM(mdl).Draw(res::GetBTT(tex).id, shd.ID);
}

void DrawBlendMeshAtTransform(res::assetid mdl, btf32 bs, res::assetid tex, Shader& shd, Transform3D transform)
{
	// Enable the shader
	shd.Use();

	// Set matrices on shader
	shd.setMat4("projection", graphics::GetMatProj());
	shd.setMat4("view", graphics::GetMatView());
	shd.SetFloat("blendState", bs);
	shd.setMat4("model", transform.getModelMatrix());

	// Render the mesh
	//mdl.Draw(tex.id, shd.ID);
	res::GetBTMB(mdl).Draw(res::GetBTT(tex).id, shd.ID);
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

		rod = fw::Lerp(rod, (btf32)bvMusketState.get(Musket::musket_state::barrel_rod_in), 0.1f);

		if (rod > 0.95f)
			bvMusketState.set(Musket::musket_state::barrel_armed); // unset powder
	}

	fpan = fw::Lerp(fpan, (btf32)bvMusketState.get(Musket::musket_state::fpan_hatch_open), 0.3f);
	lever = fw::Lerp(lever, (btf32)bvMusketState.get(Musket::musket_state::latch_pulled), 0.2f);

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

void Musket::Draw(fw::Vector2 pos, btf32 height, m::Angle ang)
{
	Transform3D t_item;

	t_item.SetPosition(fw::Vector3(pos.x, height, pos.y));
	t_item.Rotate(ang.Rad(), fw::Vector3(0, 1, 0));

	switch ((eMusketHoldState))
	{
	case Musket::aim:
		loc = fw::Lerp(loc, fw::Vector3(0.13f, 0.9f, 0.3f), 0.1f);
		yaw = fw::Lerp(yaw, 0.f, 0.1f);
		pitch = fw::Lerp(pitch, 0.f, 0.1f);
		break;
	case Musket::inspect_pan:
		loc = fw::Lerp(loc, fw::Vector3(-0.23f, 1.2f, 0.4f), 0.1f);
		yaw = fw::Lerp(yaw, 45.f, 0.1f);
		pitch = fw::Lerp(pitch, -15.f, 0.1f);
		break;
	case Musket::inspect_barrel:
		loc = fw::Lerp(loc, fw::Vector3(0.f, 0.2f, 0.4f), 0.1f);
		yaw = fw::Lerp(yaw, 0.f, 0.1f);
		pitch = fw::Lerp(pitch, -80.f, 0.1f);
		break;
	}

	t_item.TranslateLocal(loc); // set pose
	t_item.Rotate(glm::radians(yaw), fw::Vector3(0.f, 1.f, 0.f));
	t_item.Rotate(glm::radians(pitch), fw::Vector3(1.f, 0.f, 0.f));

	DrawMeshAtTransform(res::m_item_matchlock_01, res::t_item_matchlock_01, pRes->shader_solid, t_item);

	if (bvMusketState.get(Musket::musket_state::fpan_powder_in))
		DrawBlendMeshAtTransform(res::mb_item_matchlock_01_pan_full, fpan, res::t_item_matchlock_01, pRes->shader_blend, t_item);
	else
		DrawBlendMeshAtTransform(res::mb_item_matchlock_01_pan, fpan, res::t_item_matchlock_01, pRes->shader_blend, t_item);

	DrawBlendMeshAtTransform(res::mb_item_matchlock_01_lever, lever, res::t_item_matchlock_01, pRes->shader_blend, t_item);

	if (eMusketHoldState == Musket::inspect_barrel || bvMusketState.get(Musket::musket_state::barrel_rod_in)) // IF BARREL
		DrawBlendMeshAtTransform(res::mb_item_matchlock_01_rod, rod, res::t_item_matchlock_01, pRes->shader_blend, t_item);

}