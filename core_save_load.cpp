#include "core_save_load.h"

#include "core.h"
#include "index.h"
#include "entity.h"
#include "weather.h"
//#include "memory.hpp"

#include <stdio.h>

#define FILE_OUT_VER 001u

#define SIZE_8 1
#define SIZE_16 2
#define SIZE_32 4
#define SIZE_64 8

extern mem::ObjBuf<HeldItem, ItemType, ENTITY_TYPE_NULL, BUF_SIZE>* buf_iteminst;

extern mem::objbuf_caterpillar block_proj; // Projectile buffer
extern Projectile proj[BUF_SIZE];

namespace core
{
	struct ReferenceCell
	{
		mem::idbuf ref_ents;
	};
	extern ReferenceCell refCells[WORLD_SIZE][WORLD_SIZE];
}

bool SaveExists()
{
	FILE* file = fopen("save/save.bin", "rb"); // Open file
	if (file != NULL) {
		fclose(file); // Close file
		return true;
	}
	else
		return false;
}

void SaveState()
{
	printf("SAVE FUNCTION CALLED ON TICK %i\n", tickCount);

	// clean all unused item instances
	// this is a hack and i know it
	for (lid index_item = 0; index_item < buf_iteminst->Size(); index_item++) { // For every item
		if (!buf_iteminst->Used(index_item)) continue;
		lui32 item_reference_count = 0u;
		for (lid index_ent = 0; index_ent < GetEntityArraySize(); index_ent++) { // For every entity
			if (!GetEntityExists(index_ent)) continue;
			// if this entity has an inventory
			if (GetEntityType(index_ent) == ENTITY_TYPE_ACTOR) {
				// for every invntory slot
				for (lui32 inv_slot = 0; inv_slot < ACTOR(index_ent)->inventory.items.Size(); ++inv_slot) {
					// if this slot contains this item
					if (ACTOR(index_ent)->inventory.items[inv_slot] != index_item) continue;
					item_reference_count++;
					goto item_done; // we found our reference so can skip the rest
				}
			}
			else if (GetEntityType(index_ent) == ENTITY_TYPE_RESTING_ITEM && ITEM(index_ent)->item_instance == index_item) {
				item_reference_count++;
				goto item_done; // we found our reference so can skip the rest
			}
		}
	item_done:
		if (item_reference_count == 0u) {
			core::DestroyItem(index_item);
			std::cout << "Destroyed Item with no references!" << std::endl;
		}
	}

	lui32 FILE_VER = FILE_OUT_VER;
	lui32 GAME_VER = VERSION_MAJOR;
	lui32 PROJ_VER = VERSION_PROJECT;
	lui32 TEMP_VER = 0u;

	FILE* file = fopen("save/save.bin", "wb"); // Open file
	if (file != NULL)
	{
		fseek(file, 0, SEEK_SET); // Seek file beginning

		fwrite(&FILE_VER, SIZE_32, 1, file);
		fwrite(&GAME_VER, SIZE_32, 1, file);
		fwrite(&PROJ_VER, SIZE_32, 1, file);
		fwrite(&TEMP_VER, SIZE_32, 1, file);

		// Actual game state
		fwrite(&tickCount, SIZE_64, 1, file);
		fwrite(&core::players, SIZE_16, NUM_PLAYERS, file);
		lui64 temp = 0;
		fwrite(&temp, SIZE_64, 1, file);

		//-------------------------------- ENTITIES

		lui32 ent_count = GetEntityArraySize();
		fwrite(&ent_count, SIZE_32, 1, file);
		for (lid i = 0; i < ent_count; i++) // For every entity
		{
			// Could probably just write the type straight up as type_null means unused now
			bool ent_exists = GetEntityExists(i);
			fwrite(&ent_exists, SIZE_8, 1, file);
			if (ent_exists) {
				EntityType type = GetEntityType(i);
				fwrite(&type, SIZE_8, 1, file);
			}
		}
		for (lid i = 0; i < ent_count; i++) // For every entity
		{
			if (GetEntityExists(i))
			{
				ECCommon* entptr = ENTITY(i);
				fwrite(&entptr->name, 32, 1, file);
				fwrite(&entptr->physicsFlags, SIZE_8, 1, file);
				fwrite(&entptr->faction, SIZE_8, 1, file);
				fwrite(&entptr->damagestate, SIZE_16, 1, file);
				fwrite(&entptr->activeFlags, SIZE_64, 1, file);
				fwrite(&entptr->effects, sizeof(mem::Buffer32<StatusEffect>), 1, file);
				fwrite(&entptr->radius, SIZE_32, 1, file);
				fwrite(&entptr->height, SIZE_32, 1, file);
				fwrite(&entptr->t.position.x, SIZE_32, 1, file);
				fwrite(&entptr->t.position.y, SIZE_32, 1, file);
				fwrite(&entptr->velocity.x, SIZE_32, 1, file);
				fwrite(&entptr->velocity.y, SIZE_32, 1, file);
				fwrite(&entptr->t.altitude, SIZE_32, 1, file);
				fwrite(&entptr->altitude_velocity, SIZE_32, 1, file);
				fwrite(&entptr->t.yaw, SIZE_32, 1, file);
				fwrite(&entptr->t.csi, sizeof(CellSpace), 1, file);
				fwrite(&entptr->slideVelocity.x, SIZE_32, 1, file);
				fwrite(&entptr->slideVelocity.y, SIZE_32, 1, file);
				fwrite(&temp, SIZE_8, 1, file);

				switch (GetEntityType(i))
				{
				case ENTITY_TYPE_RESTING_ITEM:
				{
					fwrite(&ITEM(i)->item_instance, SIZE_16, 1, file);
				}
				break;
				case ENTITY_TYPE_ACTOR:
				{
					ECActor* actptr = ACTOR(i);
					fwrite(&actptr->input.bits, SIZE_16, 1, file);
					fwrite(&actptr->input.move.x, SIZE_32, 1, file);
					fwrite(&actptr->input.move.y, SIZE_32, 1, file);
					fwrite(&actptr->viewYaw, SIZE_32, 1, file);
					fwrite(&actptr->viewPitch, SIZE_32, 1, file);
					fwrite(&actptr->actorBase, SIZE_8, 1, file);
					fwrite(&actptr->skin_col_a, sizeof(m::Vector3), 1, file);
					fwrite(&actptr->skin_col_b, sizeof(m::Vector3), 1, file);
					fwrite(&actptr->skin_col_c, sizeof(m::Vector3), 1, file);
					fwrite(&actptr->speed, SIZE_32, 1, file);
					fwrite(&actptr->agility, SIZE_32, 1, file);
					fwrite(&actptr->stamina, sizeof(MaxedStat), 1, file);
					fwrite(&actptr->inventory, sizeof(Inventory), 1, file);
					fwrite(&actptr->inv_active_slot, SIZE_32, 1, file);
					fwrite(&actptr->atk_target, SIZE_16, 1, file);
					fwrite(&temp, SIZE_8, 1, file);
					fwrite(&actptr->foot_state, SIZE_8, 1, file);
					fwrite(&actptr->jump_state, SIZE_8, 1, file);
					fwrite(&actptr->animationBV, SIZE_8, 1, file);
					fwrite(&actptr->t_body, sizeof(Transform3D), 1, file);
					fwrite(&actptr->t_head, sizeof(Transform3D), 1, file);
					fwrite(&actptr->fpCurrentL, sizeof(m::Vector3), 1, file);
					fwrite(&actptr->fpCurrentR, sizeof(m::Vector3), 1, file);
					fwrite(&actptr->aniStandHeight, SIZE_32, 1, file);
					fwrite(&actptr->aniCrouch, SIZE_8, 1, file);
					fwrite(&actptr->aniSlideResponse, SIZE_32, 1, file);
					fwrite(&actptr->aniRun, SIZE_8, 1, file);
					fwrite(&actptr->aniHandHoldTarget, SIZE_16, 1, file);
					fwrite(&actptr->ai_target_ent, SIZE_16, 1, file);
					fwrite(&actptr->ai_ally_ent, SIZE_16, 1, file);
					fwrite(&actptr->aiControlled, SIZE_8, 1, file);
					fwrite(&actptr->ai_path, sizeof(path::Path), 1, file);
					fwrite(&actptr->ai_path_current_index, SIZE_8, 1, file);
					fwrite(&actptr->ai_pathing, SIZE_8, 1, file);
				}
				break;
				}
			}
		}

		//-------------------------------- ITEMS

		lui32 itemsize = buf_iteminst->Size();
		fwrite(&itemsize, SIZE_16, 1, file);
		fwrite(buf_iteminst->TypeRW(), SIZE_8, (size_t)itemsize, file);

		for (lid i = 0; i < itemsize; i++) // For every item
		{
			if (buf_iteminst->Used(i))
			{
				HeldItem* itemptr = GETITEMINST(i);

				fwrite(&itemptr->id_item_template, SIZE_16, 1, file);

				fwrite(&itemptr->ePose, SIZE_8, 1, file);
				fwrite(&itemptr->loc, SIZE_32 * 3, 1, file);
				fwrite(&itemptr->loc_velocity, SIZE_32 * 3, 1, file);
				fwrite(&itemptr->pitch, SIZE_32, 1, file);
				fwrite(&itemptr->pitch_velocity, SIZE_32, 1, file);
				fwrite(&itemptr->yaw, SIZE_32, 1, file);
				fwrite(&itemptr->yaw_velocity, SIZE_32, 1, file);
				
				fwrite(&itemptr->swinging, SIZE_8, 1, file);
				fwrite(&itemptr->swingState, SIZE_32, 1, file);
				
				fwrite(&itemptr->ang_aim_offset_temp, SIZE_32, 1, file);
				fwrite(&itemptr->ang_aim_pitch, SIZE_32, 1, file);
				fwrite(&itemptr->fire_time, SIZE_64, 1, file);
				fwrite(&itemptr->id_ammoInstance, SIZE_16, 1, file);
				
				fwrite(&itemptr->charge, SIZE_32, 1, file);
				
				fwrite(&itemptr->uses, SIZE_32, 1, file);
			}
		}

		//-------------------------------- PROJECTILES

		fwrite(&block_proj, sizeof(block_proj), 1, file);
		fwrite(&proj, sizeof(proj), 1, file);

		//-------------------------------- WEATHER

		fwrite(&weather::w.col_sun_from, SIZE_32 * 3, 1, file);
		fwrite(&weather::w.col_amb_from, SIZE_32 * 3, 1, file);
		fwrite(&weather::w.col_fog_from, SIZE_32 * 3, 1, file);
		fwrite(&weather::w.col_sun_dest, SIZE_32 * 3, 1, file);
		fwrite(&weather::w.col_amb_dest, SIZE_32 * 3, 1, file);
		fwrite(&weather::w.col_fog_dest, SIZE_32 * 3, 1, file);
		fwrite(&weather::w.fog_level_from, SIZE_32, 1, file);
		fwrite(&weather::w.fog_level_dest, SIZE_32, 1, file);
		fwrite(&weather::w.recalc_ticker, SIZE_32, 1, file);
		fwrite(&weather::w.stat_tod, SIZE_32, 1, file);
		fwrite(&weather::w.stat_overcast_level, SIZE_32, 1, file);

		fclose(file); // Close file
	}
	else
	{
		printf("Couldn't open/make the save file for some fucked reason.");
	}

	core::GUISetMessag(0, "Game saved!");
}
void LoadStateFileV001()
{
	lui32 empty = 0u;

	FILE* file = fopen("save/save.bin", "rb"); // Open file
	if (file != NULL)
	{
		fseek(file, 0, SEEK_SET); // Seek file beginning

		fread(&empty, SIZE_32, 1, file);
		fread(&empty, SIZE_32, 1, file);
		fread(&empty, SIZE_32, 1, file);
		fread(&empty, SIZE_32, 1, file);

		// Actual game state
		fread(&tickCount, SIZE_64, 1, file);
		fread(&core::players, SIZE_16, NUM_PLAYERS, file);
		lui64 temp = 0;
		fread(&temp, SIZE_64, 1, file);

		//-------------------------------- ENTITIES

		lui32 ent_count = 0u;
		fread(&ent_count, SIZE_32, 1, file);
		for (lid i = 0; i < ent_count; i++) // For every entity
		{
			// Could probably just write the type straight up as type_null means unused now
			bool ent_exists = false;
			fread(&ent_exists, SIZE_8, 1, file);
			if (ent_exists) {
				EntityType type = ENTITY_TYPE_NULL;
				fread(&type, SIZE_8, 1, file);
				IndexSpawnEntityFixedID(type, i);
			}
		}

		for (lid i = 0; i < ent_count; i++) // For every entity
		{
			if (GetEntityExists(i))
			{
				ECCommon* entptr = ENTITY(i);
				fread(&entptr->name, 32, 1, file);
				fread(&entptr->physicsFlags, SIZE_8, 1, file);
				fread(&entptr->faction, SIZE_8, 1, file);
				fread(&entptr->damagestate, SIZE_16, 1, file);
				fread(&entptr->activeFlags, SIZE_64, 1, file);
				fread(&entptr->effects, sizeof(mem::Buffer32<StatusEffect>), 1, file);
				fread(&entptr->radius, SIZE_32, 1, file);
				fread(&entptr->height, SIZE_32, 1, file);
				fread(&entptr->t.position.x, SIZE_32, 1, file);
				fread(&entptr->t.position.y, SIZE_32, 1, file);
				fread(&entptr->velocity.x, SIZE_32, 1, file);
				fread(&entptr->velocity.y, SIZE_32, 1, file);
				fread(&entptr->t.altitude, SIZE_32, 1, file);
				fread(&entptr->altitude_velocity, SIZE_32, 1, file);
				fread(&entptr->t.yaw, SIZE_32, 1, file);
				fread(&entptr->t.csi, sizeof(CellSpace), 1, file);
				fread(&entptr->slideVelocity.x, SIZE_32, 1, file);
				fread(&entptr->slideVelocity.y, SIZE_32, 1, file);
				fread(&temp, SIZE_8, 1, file);

				switch (GetEntityType(i))
				{
				case ENTITY_TYPE_RESTING_ITEM:
				{
					fread(&ITEM(i)->item_instance, SIZE_16, 1, file);
				}
				break;
				case ENTITY_TYPE_ACTOR:
				{
					ECActor* actptr = ACTOR(i);
					fread(&actptr->input.bits, SIZE_16, 1, file);
					fread(&actptr->input.move.x, SIZE_32, 1, file);
					fread(&actptr->input.move.y, SIZE_32, 1, file);
					fread(&actptr->viewYaw, SIZE_32, 1, file);
					fread(&actptr->viewPitch, SIZE_32, 1, file);
					fread(&actptr->actorBase, SIZE_8, 1, file);
					fread(&actptr->skin_col_a, sizeof(m::Vector3), 1, file);
					fread(&actptr->skin_col_b, sizeof(m::Vector3), 1, file);
					fread(&actptr->skin_col_c, sizeof(m::Vector3), 1, file);
					fread(&actptr->speed, SIZE_32, 1, file);
					fread(&actptr->agility, SIZE_32, 1, file);
					fread(&actptr->stamina, sizeof(MaxedStat), 1, file);
					fread(&actptr->inventory, sizeof(Inventory), 1, file);
					fread(&actptr->inv_active_slot, SIZE_32, 1, file);
					fread(&actptr->atk_target, SIZE_16, 1, file);
					fread(&temp, SIZE_8, 1, file);
					fread(&actptr->foot_state, SIZE_8, 1, file);
					fread(&actptr->jump_state, SIZE_8, 1, file);
					fread(&actptr->animationBV, SIZE_8, 1, file);
					fread(&actptr->t_body, sizeof(Transform3D), 1, file);
					fread(&actptr->t_head, sizeof(Transform3D), 1, file);
					fread(&actptr->fpCurrentL, sizeof(m::Vector3), 1, file);
					fread(&actptr->fpCurrentR, sizeof(m::Vector3), 1, file);
					fread(&actptr->aniStandHeight, SIZE_32, 1, file);
					fread(&actptr->aniCrouch, SIZE_8, 1, file);
					fread(&actptr->aniSlideResponse, SIZE_32, 1, file);
					fread(&actptr->aniRun, SIZE_8, 1, file);
					fread(&actptr->aniHandHoldTarget, SIZE_16, 1, file);
					fread(&actptr->ai_target_ent, SIZE_16, 1, file);
					fread(&actptr->ai_ally_ent, SIZE_16, 1, file);
					fread(&actptr->aiControlled, SIZE_8, 1, file);
					fread(&actptr->ai_path, sizeof(path::Path), 1, file);
					fread(&actptr->ai_path_current_index, SIZE_8, 1, file);
					fread(&actptr->ai_pathing, SIZE_8, 1, file);
					actptr->ai_timer = 0u;
				}
				break;
				}
			}
		}

		//-------------------------------- ITEMS

		lui32 itemsize = 0u;
		fread(&itemsize, SIZE_16, 1, file);
		buf_iteminst->SetSize(itemsize);
		fread(buf_iteminst->TypeRW(), SIZE_8, (size_t)itemsize, file);

		for (lid i = 0; i < itemsize; i++) // For every entity
		{
			if (buf_iteminst->Used(i))
			{
				lid template_temp;
				fread(&template_temp, SIZE_16, 1, file);

				HeldItem* itemptr = GETITEMINST(i);

				itemptr->id_item_template = template_temp;

				fread(&itemptr->ePose, SIZE_8, 1, file);
				fread(&itemptr->loc, SIZE_32 * 3, 1, file);
				fread(&itemptr->loc_velocity, SIZE_32 * 3, 1, file);
				fread(&itemptr->pitch, SIZE_32, 1, file);
				fread(&itemptr->pitch_velocity, SIZE_32, 1, file);
				fread(&itemptr->yaw, SIZE_32, 1, file);
				fread(&itemptr->yaw_velocity, SIZE_32, 1, file);

				fread(&itemptr->swinging, SIZE_8, 1, file);
				fread(&itemptr->swingState, SIZE_32, 1, file);

				fread(&itemptr->ang_aim_offset_temp, SIZE_32, 1, file);
				fread(&itemptr->ang_aim_pitch, SIZE_32, 1, file);
				fread(&itemptr->fire_time, SIZE_64, 1, file);
				fread(&itemptr->id_ammoInstance, SIZE_16, 1, file);

				fread(&itemptr->charge, SIZE_32, 1, file);

				fread(&itemptr->uses, SIZE_32, 1, file);
			}
		}

		//-------------------------------- PROJECTILES

		fread(&block_proj, sizeof(block_proj), 1, file);
		fread(&proj, sizeof(proj), 1, file);

		//-------------------------------- WEATHER

		fread(&weather::w.col_sun_from, SIZE_32 * 3, 1, file);
		fread(&weather::w.col_amb_from, SIZE_32 * 3, 1, file);
		fread(&weather::w.col_fog_from, SIZE_32 * 3, 1, file);
		fread(&weather::w.col_sun_dest, SIZE_32 * 3, 1, file);
		fread(&weather::w.col_amb_dest, SIZE_32 * 3, 1, file);
		fread(&weather::w.col_fog_dest, SIZE_32 * 3, 1, file);
		fread(&weather::w.fog_level_from, SIZE_32, 1, file);
		fread(&weather::w.fog_level_dest, SIZE_32, 1, file);
		fread(&weather::w.recalc_ticker, SIZE_32, 1, file);
		fread(&weather::w.stat_tod, SIZE_32, 1, file);
		fread(&weather::w.stat_overcast_level, SIZE_32, 1, file);

		fclose(file); // Close file
	}
}
bool LoadState()
{
	lui32 FILE_VER = 0b11111111111111111111111111111111;
	lui32 GAME_VER = 0b11111111111111111111111111111111;
	lui32 PROJ_VER = 0b11111111111111111111111111111111;
	lui32 TEMP_VER = 0b11111111111111111111111111111111;

	FILE* file = fopen("save/save.bin", "rb"); // Open file
	if (file == NULL) goto loadfailed_nofile;

	// Load file versions
	fseek(file, 0, SEEK_SET); // Seek file beginning
	fread(&FILE_VER, SIZE_32, 1, file); // Load file versions
	fread(&GAME_VER, SIZE_32, 1, file);
	fread(&PROJ_VER, SIZE_32, 1, file);
	fread(&TEMP_VER, SIZE_32, 1, file);
	fclose(file); // Close file

	// If the save game was created by an older, incompatible major version
	if (GAME_VER != VERSION_MAJOR) goto loadfailed;
	if (PROJ_VER != VERSION_PROJECT) goto loadfailed;

	core::ClearBuffers();

	switch (FILE_VER) {
	case 001u:
		LoadStateFileV001();
		goto loadsucceeded;
	default:
		goto loadfailed;
	}

loadsucceeded:
	core::RegenCellRefs();
	core::CheckPlayerAI();
	core::GUISetMessag(0, "Game loaded!");
	return true;
loadfailed:
	core::GUISetMessag(0, "The game has been modified since the last save and so your progress must be reset, sorry!");
	return false;
loadfailed_nofile:
	return false;
}

#undef SIZE_8
#undef SIZE_16
#undef SIZE_32
#undef SIZE_64
