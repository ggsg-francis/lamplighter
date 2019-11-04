#include "objects.h"
#include "objects_items.h"

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

void Entity::Tick(btID index, btf32 dt)
{
}

void Entity::Draw(btID index)
{
}

void EItem::Tick(btID index, btf32 dt)
{
	index::GetCellSpaceInfo(t.position, csi);
	env::GetHeight(t.height, csi);
	t_item.SetPosition(m::Vector3(t.position.x, t.height, t.position.y));
	t_item.SetRotation(yaw.Rad());
}

void EItem::Draw(btID index)
{
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

void Actor::Tick(btID index, btf32 dt)
{
}

void Actor::Draw(btID index)
{
}

void Chara::Tick(btID index, btf32 dt)
{
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
			input.x = -input.x;
			t.velocity = m::Lerp(t.velocity, m::Rotate(input, viewYaw.Rad()) * m::Vector2(-1.f, 1.f) * dt * speed, 0.2f);
			yaw.RotateTowards(viewYaw.Deg(), 5.f); // Rotate body towards the target direction
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

			//// New way ( four references )
			//// optimize this $hit, also keeps missing for some reason
			//btui32 i;
			//for (i = 0u; i < eCELL_COUNT; ++i)
			//	index::RemoveEntityCell(cs_last.c[i].x, cs_last.c[i].y, index);
			////GetCellSpaceInfo(ePos, ENTITY(index)->csi);
			//for (i = 0u; i < eCELL_COUNT; ++i)
			//	index::AddEntityCell(ENTITY(index)->csi.c[i].x, ENTITY(index)->csi.c[i].y, index);
		}

		//-------------------------------- SET HEIGHT AND CELL SPACE

		env::GetHeight(t.height, csi);

		//-------------------------------- RUN COLLISION & AI

		if (!cfg::bEditMode)
		{
			index::EntDeintersect(this, csi, viewYaw.Deg(), true);
			if (aiControlled) index::ActorRunAI(index); // Run AI
		}
	} // End if alive

	  //if attacking
	if (inputbv.get(Actor::IN_USE) && attack_time < time)
	{
		attack_time = time + 0.25;
		index::ActorCastProj(index);
	}

	state.hp += 0.001f;
	if (state.hp > 1.f) state.hp = 1.f;

	if (heldItem != nullptr) heldItem->Tick(
		inputbv.get(Actor::IN_ATN_A),
		inputbv.get(Actor::IN_ATN_B),
		inputbv.get(Actor::IN_ATN_C),
		inputbv.get(Actor::IN_ATN_D),
		inputbv.get(Actor::IN_USE));

	//________________________________________________________________
	//------------- SET TRANSFORMATIONS FOR GRAPHICS -----------------

	// Reset transforms
	t_body = Transform3D();
	t_head = Transform3D();

	t_body.SetPosition(m::Vector3(t.position.x, 0.1f + t.height + 0.75f, t.position.y));
	t_body.Rotate(yaw.Rad(), m::Vector3(0, 1, 0));

	//CHARA(index)->ani_body_lean = m::Lerp(CHARA(index)->ani_body_lean, f2Input * 15.f, 0.1f);
	ani_body_lean = m::Lerp(ani_body_lean, input * m::Vector2(8.f, 15.f), 0.25f);

	t_body.Rotate(glm::radians(ani_body_lean.y), m::Vector3(1, 0, 0));
	t_body.Rotate(glm::radians(ani_body_lean.x), m::Vector3(0, 0, 1));

	// Set head transform
	t_head.SetPosition(t_body.GetPosition());
	t_head.Rotate(viewYaw.Rad(), m::Vector3(0, 1, 0));
	t_head.Rotate(viewPitch.Rad(), m::Vector3(1, 0, 0));
	t_head.Translate(t_body.GetUp() * 0.7f);
}

void Chara::Draw(btID index)
{
	// need a good way of knowing own index
	DrawBlendMeshAtTransform(index, res::mb_legs, 0, t_skin, graphics::shader_blend, t_body);

	// draw arms

	graphics::Matrix4x4 matrix;

	m::Vector3 newpos2 = m::Vector3(t.position.x, 0.65f + t.height, t.position.y) + t_body.GetUp() * 0.75f;

	m::Vector3 newpos_l = newpos2 + t_body.GetRight() * 0.1f;
	m::Vector3 newpos_r = newpos2 + t_body.GetRight() * -0.1f;

	btf32 dist_l;
	btf32 dist_r;

	if (heldItem != nullptr)
	{
		m::Vector3 hand_pos_l = heldItem->t_item.GetPosition() + heldItem->t_item.GetUp() * -0.08f;
		m::Vector3 hand_pos_r = heldItem->t_item.GetPosition() + heldItem->t_item.GetForward() * 0.5f;

		dist_l = m::Length(newpos_l - hand_pos_l) * 1.5f - 0.5f;
		dist_r = m::Length(newpos_r - hand_pos_r) * 1.5f - 0.5f;

		graphics::MatrixTransform(matrix, newpos_l, hand_pos_l - newpos_l, (t_body.GetRight() * -1.f) + t_body.GetUp());
		DrawBlendMesh(index, res::mb_armscast, dist_l, t_skin, graphics::shader_blend, matrix);
		graphics::MatrixTransform(matrix, newpos_r, hand_pos_r - newpos_r, t_body.GetRight() + t_body.GetUp());
		DrawBlendMesh(index, res::mb_armscast, dist_r, t_skin, graphics::shader_blend, matrix);
	}

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

	if (heldItem != nullptr) heldItem->Draw(t.position, t.height, viewYaw, viewPitch);
}

void EditorPawn::Tick(btID index, btf32 dt)
{
	if (state.properties.get(ActiveState::eALIVE))
	{
		//ENTITY[index]->t.velocity = fw::Lerp(ENTITY[index]->t.velocity, fw::Rotate(f2Input, aViewYaw.Rad()) * fw::Vector2(-1.f, 1.f) * dt * fSpeed, 0.3f);
		input.x = -input.x;
		t.velocity = m::Rotate(input, viewYaw.Rad()) * m::Vector2(-1.f, 1.f) * dt * 5.f;

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
	} // End if alive


	//________________________________________________________________
	//------------- SET TRANSFORMATIONS FOR GRAPHICS -----------------

	// Reset transforms
	t_body = Transform3D();
	t_head = Transform3D();

	t_body.SetPosition(m::Vector3(t.position.x, 0.1f + t.height + 0.75f, t.position.y));
	t_body.Rotate(yaw.Rad(), m::Vector3(0, 1, 0));

	// Set head transform
	t_head.SetPosition(t_body.GetPosition());
	t_head.Rotate(viewYaw.Rad(), m::Vector3(0, 1, 0));
	t_head.Rotate(viewPitch.Rad(), m::Vector3(1, 0, 0));
	t_head.Translate(t_body.GetUp() * 0.7f);
}

void EditorPawn::Draw(btID index)
{
	// need a good way of knowing own index
	DrawBlendMeshAtTransform(index, res::mb_legs, 0, t_skin, graphics::shader_blend, t_body);

	// draw head

	DrawBlendMeshAtTransform(index, res::mb_char_head, 0, t_skin, graphics::shader_blend, t_head);
	DrawMeshAtTransform(index, res::m_equip_head_pickers, res::t_equip_atlas, graphics::shader_solid, t_head);
}