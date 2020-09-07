#include "ec_misc.h"

#include "archive.hpp"
#include "index.h"
#include "core.h"
#include "objects_items.h"

#include "ec_actor.h" // Only used for highlighting if selected

char* RestingItemName(void* ent) {
	return (char*)acv::items[GETITEMINST(((ECSingleItem*)ent)->item_instance)->id_item_template]->name;
}

void RestingItemTick(btID id, void* ent, btf32 dt)
{
	ECSingleItem* self = (ECSingleItem*)ent;

	if (self->state.stateFlags.get(ActiveState::eDIED_REPORT))
	{
		//chr->state.stateFlags.unset(ActiveState::eDIED_REPORT);
		core::DestroyEntity(id);
	}

	self->matrix = graphics::Matrix4x4();
	graphics::MatrixTransform(self->matrix, m::Vector3(self->t.position.x, self->t.height + acv::items[((HeldItem*)GetItemInstance(self->item_instance))->id_item_template]->f_model_height, self->t.position.y), self->t.yaw.Rad());

	Entity_PhysicsTick(self, id, dt);
}

void RestingItemDraw(btID id, void* ent)
{
	ECSingleItem* self = (ECSingleItem*)ent;
	// Draw the mesh of our item id
	//DrawMesh(ent, acv::GetM(acv::items[index::GetItem(item->item_instance)->item_template]->id_mesh), acv::GetT(acv::items[index::GetItem(item->item_instance)->item_template]->id_tex), SS_NORMAL, item->t_item.getMatrix());
	if (id == ACTOR(core::players[core::activePlayer])->viewtarget)
	{
		graphics::GetShader(graphics::S_SOLID).Use();
		graphics::GetShader(graphics::S_SOLID).SetBool(graphics::Shader::bLit_TEMP, false);
		DrawMesh(id, acv::GetM(acv::items[GETITEMINST(self->item_instance)->id_item_template]->id_mesh), acv::GetT(acv::items[GETITEMINST(self->item_instance)->id_item_template]->id_tex), SS_NORMAL, self->matrix);
		graphics::GetShader(graphics::S_SOLID).SetBool(graphics::Shader::bLit_TEMP, true);
	}
	else
	{
		if (m::Length(graphics::GetViewPos() - m::Vector3(self->t.position.x, self->t.height, -self->t.position.y)) > 5.f)
			DrawMesh(ID_NULL, acv::GetM(acv::items[GETITEMINST(self->item_instance)->id_item_template]->id_mesh_lod), acv::GetT(acv::items[GETITEMINST(self->item_instance)->id_item_template]->id_tex), SS_NORMAL, self->matrix);
		else
			DrawMesh(ID_NULL, acv::GetM(acv::items[GETITEMINST(self->item_instance)->id_item_template]->id_mesh), acv::GetT(acv::items[GETITEMINST(self->item_instance)->id_item_template]->id_tex), SS_NORMAL, self->matrix);


		//DrawMesh(item->id, acv::GetM(acv::items[GETITEMINST(item->item_instance)->id_item_template]->id_mesh), acv::GetT(acv::items[GETITEMINST(item->item_instance)->id_item_template]->id_tex), SS_NORMAL, item->matrix);
	}
}

void TickEditorPawn(btID id, void* ent, btf32 dt) {}
void DrawEditorPawn(btID id, void* ent) {}
