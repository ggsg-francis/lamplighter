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

	if (self->activeFlags.get(ECCommon::eDIED_REPORT))
	{
		//chr->state.stateFlags.unset(ECCommon::eDIED_REPORT);
		core::DestroyEntity(id);
	}

	self->matrix = graphics::Matrix4x4();
	// upright version
	//graphics::MatrixTransform(self->matrix, m::Vector3(self->t.position.x, self->t.height + acv::items[((HeldItem*)GetItemInstance(self->item_instance))->id_item_template]->f_model_height, self->t.position.y), self->t.yaw.Rad());

	Entity_PhysicsTick(self, id, dt);

	// TODO: finding tri is done again in physics tick, reuse it somehow

	btf32 ground_height;
	m::Vector2 slope(0.f, 0.f);
	#if DEF_GRID
	env::GetHeight(ground_height, self ->t.csi);
	env::GetSlope(slope.x, slope.y, self->t.csi);
	#else
	env::EnvTri* triptr = nullptr;
	env::GetNearestSurfaceHeight(ground_height, &triptr, self->t.csi, self->t.altitude);
	if (triptr != nullptr && self->Grounded())
		slope = triptr->slope;
	#endif

	// Construct normals from the triangle slope
	m::Vector3 nor = m::NormalFromSlope(slope);
	m::Vector2 dir = m::AngToVec2(self->t.yaw.Rad());
	m::Vector3 cross = m::Cross(nor, m::Vector3(-dir.y, 0.f, dir.x));

	// TODO: definitions are temporary, this should be an item property

	// align to surface
	#if DEF_PROJECT == PROJECT_BC
	graphics::MatrixTransform(self->matrix, m::Vector3(self->t.position.x,
		self->t.altitude + acv::items[((HeldItem*)GetItemInstance(self->item_instance))->id_item_template]->f_model_height,
		self->t.position.y), cross, nor);
	#endif
	// align to surface sideways
	#if DEF_PROJECT == PROJECT_EX
	graphics::MatrixTransform(self->matrix, m::Vector3(self->t.position.x,
		self->t.altitude + acv::items[((HeldItem*)GetItemInstance(self->item_instance))->id_item_template]->f_model_height,
		self->t.position.y), cross, m::Cross(cross, nor));
	#endif
}

void RestingItemDraw(btID id, void* ent)
{
	ECSingleItem* self = (ECSingleItem*)ent;
	// Draw the mesh of our item id
	//DrawMesh(ent, acv::GetM(acv::items[index::GetItem(item->item_instance)->item_template]->id_mesh), acv::GetT(acv::items[index::GetItem(item->item_instance)->item_template]->id_tex), SS_NORMAL, item->t_item.getMatrix());
	if (m::Length(graphics::GetViewPos() - m::Vector3(self->t.position.x, self->t.altitude, -self->t.position.y)) > 5.f)
		DrawMesh(ID_NULL, acv::GetM(acv::items[GETITEMINST(self->item_instance)->id_item_template]->id_mesh_lod), acv::GetT(acv::items[GETITEMINST(self->item_instance)->id_item_template]->id_tex), SS_NORMAL, self->matrix);
	else
		DrawMesh(ID_NULL, acv::GetM(acv::items[GETITEMINST(self->item_instance)->id_item_template]->id_mesh), acv::GetT(acv::items[GETITEMINST(self->item_instance)->id_item_template]->id_tex), SS_NORMAL, self->matrix);
}

void TarBubbleTick(btID id, void* ent, btf32 dt)
{
	
}

void TarBubbleDraw(btID id, void* ent)
{

}

void WallBugTick(btID id, void* ent, btf32 dt)
{

}

void WallBugDraw(btID id, void* ent)
{

}
