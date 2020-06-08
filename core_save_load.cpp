#include "core_save_load.h"

#include "core.h"
#include "index.h"
#include "objects_entities.h"
#include "weather.h"

#include <stdio.h>

#define SIZE_8 1
#define SIZE_16 2
#define SIZE_32 4
#define SIZE_64 8

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
	return file != NULL;
}

void SaveState()
{
	printf("SAVE FUNCTION CALLED ON TICK %i\n", tickCount_temp);

	// clean all unused item instances
	for (btID index_item = 0; index_item <= block_item.index_end; index_item++) // For every item
	{
		if (block_item.used[index_item])
		{
			btui32 item_reference_count = 0u;
			for (btID index_ent = 0; index_ent <= block_entity.index_end; index_ent++) // For every entity
			{
				if (block_entity.used[index_ent])
				{
					// if this entity has an inventory
					if (ENTITY(index_ent)->type == ENTITY_TYPE_CHARA)
					{
						// for every invntory slot
						for (btui32 inv_slot = 0; inv_slot < ACTOR(index_ent)->inventory.items.Size(); ++inv_slot)
						{
							// if this slot contains this item
							if (ACTOR(index_ent)->inventory.items[inv_slot] == index_item)
							{
								item_reference_count++;
								goto item_done; // we found our reference so can skip the rest
							}
						}
					}
					else if (ENTITY(index_ent)->type == ENTITY_TYPE_RESTING_ITEM)
					{
						// if this entity hold this item
						if (ITEM(index_ent)->item_instance == index_item)
						{
							item_reference_count++;
							goto item_done; // we found our reference so can skip the rest
						}
					}
				}
			}
		item_done:
			if (item_reference_count == 0u)
			{
				core::DestroyItem(index_item);
				std::cout << "Destroyed Item with no references!" << std::endl;
			}
		}
	}

	btui32 FILE_VER = 001u;

	FILE* file = fopen("save/save.bin", "wb"); // Open file
	if (file != NULL)
	{
		fseek(file, 0, SEEK_SET); // Seek file beginning

		fwrite(&FILE_VER, SIZE_32, 1, file);

		// Actual game state
		fwrite(&core::players, SIZE_16, NUM_PLAYERS, file);
		fwrite(&core::spawnz_time_temp, SIZE_64, 1, file);

		//-------------------------------- ENTITIES

		fwrite(&block_entity.index_end, SIZE_16, 1, file);
		fwrite(&block_entity.used, SIZE_8, (size_t)(block_entity.index_end + 1u), file);
		//unneeded...
		//fwrite(&block_entity_data, 4ui64, (size_t)(block_entity.index_end + 1u), file);

		for (btID i = 0; i <= block_entity.index_end; i++) // For every entity
		{
			if (block_entity.used[i])
			{
				Entity* entptr = ENTITY(i);

				fwrite(&entptr->type, SIZE_8, 1, file);

				fwrite(&entptr->radius, SIZE_32, 1, file);
				fwrite(&entptr->height, SIZE_32, 1, file);
				fwrite(&entptr->properties, SIZE_8, 1, file);
				fwrite(&entptr->faction, SIZE_8, 1, file);
				fwrite(&entptr->state.damagestate, SIZE_16, 1, file); // duplicated due to it used to being a 32 bit number
				fwrite(&entptr->state.damagestate, SIZE_16, 1, file); // duplicated due to it used to being a 32 bit number
				fwrite(&entptr->state.stateFlags, SIZE_64, 1, file);
				fwrite(&entptr->state.effects, sizeof(mem::Buffer32<StatusEffect>), 1, file);
				fwrite(&entptr->t.position.x, SIZE_32, 1, file);
				fwrite(&entptr->t.position.y, SIZE_32, 1, file);
				fwrite(&entptr->t.height, SIZE_32, 1, file);
				fwrite(&entptr->t.yaw, SIZE_32, 1, file);

				switch (ENTITY(i)->type)
				{
				case ENTITY_TYPE_RESTING_ITEM:
					fwrite(&ITEM(i)->item_instance, SIZE_16, 1, file);
					break;
				case ENTITY_TYPE_CHARA:
					fwrite(&ACTOR(i)->name, 32, 1, file);
					fwrite(&ACTOR(i)->viewYaw, SIZE_32, 1, file);
					fwrite(&ACTOR(i)->viewPitch, SIZE_32, 1, file);
					fwrite(&ACTOR(i)->speed, SIZE_32, 1, file);
					fwrite(&ACTOR(i)->agility, SIZE_32, 1, file);
					fwrite(&ACTOR(i)->inventory, sizeof(Inventory), 1, file);
					fwrite(&ACTOR(i)->inv_active_slot, SIZE_32, 1, file);
					fwrite(&ACTOR(i)->aiControlled, SIZE_8, 1, file);
					fwrite(&ACTOR(i)->skin_col_a, sizeof(m::Vector3), 1, file);
					fwrite(&ACTOR(i)->skin_col_b, sizeof(m::Vector3), 1, file);
					fwrite(&ACTOR(i)->skin_col_c, sizeof(m::Vector3), 1, file);
					break;
				}
			}
		}

		//-------------------------------- ITEMS

		fwrite(&block_item.index_end, SIZE_16, 1, file);
		fwrite(&block_item.used, SIZE_8, (size_t)(block_item.index_end + 1u), file);

		for (btID i = 0; i <= block_item.index_end; i++) // For every item
		{
			if (block_item.used[i])
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
}
void LoadStateFileV001()
{
	btui32 FILE_VER = 0b11111111111111111111111111111111;

	FILE* file = fopen("save/save.bin", "rb"); // Open file
	if (file != NULL)
	{
		fseek(file, 0, SEEK_SET); // Seek file beginning

		fread(&FILE_VER, SIZE_32, 1, file);

		// Actual game state
		fread(&core::players, SIZE_16, NUM_PLAYERS, file);
		fread(&core::spawnz_time_temp, SIZE_64, 1, file);

		//-------------------------------- ENTITIES

		fread(&block_entity.index_end, SIZE_16, 1, file);
		fread(&block_entity.used, SIZE_8, (size_t)(block_entity.index_end + 1u), file);
		//fread(&block_entity_data, 4ui64, (size_t)(block_entity.index_end + 1u), file);

		for (btID i = 0; i <= block_entity.index_end; i++) // For every entity
		{
			if (block_entity.used[i])
			{
				EntityType type_temp;
				fread(&type_temp, SIZE_8, 1, file);

				// initialize entity here (eg. new) 
				IndexInitEntity(i, type_temp);

				ENTITY(i)->type = type_temp;
				fread(&ENTITY(i)->radius, SIZE_32, 1, file);
				fread(&ENTITY(i)->height, SIZE_32, 1, file);
				fread(&ENTITY(i)->properties, SIZE_8, 1, file);
				fread(&ENTITY(i)->faction, SIZE_8, 1, file);
				fread(&ENTITY(i)->state.damagestate, SIZE_16, 1, file); // duplicated due to it used to being a 32 bit number
				fread(&ENTITY(i)->state.damagestate, SIZE_16, 1, file); // duplicated due to it used to being a 32 bit number
				fread(&ENTITY(i)->state.stateFlags, SIZE_64, 1, file);
				fread(&ENTITY(i)->state.effects, sizeof(mem::Buffer32<StatusEffect>), 1, file);
				fread(&ENTITY(i)->t.position.x, SIZE_32, 1, file);
				fread(&ENTITY(i)->t.position.y, SIZE_32, 1, file);
				fread(&ENTITY(i)->t.height, SIZE_32, 1, file);
				fread(&ENTITY(i)->t.yaw, SIZE_32, 1, file);

				switch (ENTITY(i)->type)
				{
				case ENTITY_TYPE_RESTING_ITEM:
					fread(&ITEM(i)->item_instance, SIZE_16, 1, file);
					break;
				case ENTITY_TYPE_CHARA:
					fread(&ACTOR(i)->name, 32, 1, file);
					fread(&ACTOR(i)->viewYaw, SIZE_32, 1, file);
					fread(&ACTOR(i)->viewPitch, SIZE_32, 1, file);
					fread(&ACTOR(i)->speed, SIZE_32, 1, file);
					fread(&ACTOR(i)->agility, SIZE_32, 1, file);
					fread(&ACTOR(i)->inventory, sizeof(Inventory), 1, file);
					fread(&ACTOR(i)->inv_active_slot, SIZE_32, 1, file);
					fread(&ACTOR(i)->aiControlled, SIZE_8, 1, file);
					ACTOR(i)->ai_target_ent = ID_NULL; // temp
					ACTOR(i)->ai_ally_ent = ID_NULL; // temp
					fread(&ACTOR(i)->skin_col_a, sizeof(m::Vector3), 1, file);
					fread(&ACTOR(i)->skin_col_b, sizeof(m::Vector3), 1, file);
					fread(&ACTOR(i)->skin_col_c, sizeof(m::Vector3), 1, file);
					break;
				}

				std::cout << "Loaded entity ID " << i << std::endl;
			}
		}

		//-------------------------------- ITEMS

		fread(&block_item.index_end, SIZE_16, 1, file);
		fread(&block_item.used, SIZE_8, (size_t)(block_item.index_end + 1u), file);

		for (btID i = 0; i <= block_item.index_end; i++) // For every entity
		{
			if (block_item.used[i])
			{
				btID template_temp;
				fread(&template_temp, SIZE_16, 1, file);

				IndexInitItemInstance(i, acv::item_types[template_temp]);

				GETITEMINST(i)->id_item_template = template_temp;

				fread(&GETITEMINST(i)->ePose, SIZE_8, 1, file);
				fread(&GETITEMINST(i)->loc, SIZE_32 * 3, 1, file);
				fread(&GETITEMINST(i)->loc_velocity, SIZE_32 * 3, 1, file);
				fread(&GETITEMINST(i)->pitch, SIZE_32, 1, file);
				fread(&GETITEMINST(i)->pitch_velocity, SIZE_32, 1, file);
				fread(&GETITEMINST(i)->yaw, SIZE_32, 1, file);
				fread(&GETITEMINST(i)->yaw_velocity, SIZE_32, 1, file);

				fread(&GETITEMINST(i)->swinging, SIZE_8, 1, file);
				fread(&GETITEMINST(i)->swingState, SIZE_32, 1, file);

				fread(&GETITEMINST(i)->ang_aim_offset_temp, SIZE_32, 1, file);
				fread(&GETITEMINST(i)->ang_aim_pitch, SIZE_32, 1, file);
				fread(&GETITEMINST(i)->fire_time, SIZE_64, 1, file);
				fread(&GETITEMINST(i)->id_ammoInstance, SIZE_16, 1, file);

				fread(&GETITEMINST(i)->charge, SIZE_32, 1, file);

				fread(&GETITEMINST(i)->uses, SIZE_32, 1, file);

				std::cout << "Loaded item ID " << i << std::endl;
			}
		}

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
void LoadState()
{
	btui32 FILE_VER = 0b11111111111111111111111111111111;

	FILE* file = fopen("save/save.bin", "rb"); // Open file
	if (file != NULL)
	{
		core::ClearBuffers();

		fseek(file, 0, SEEK_SET); // Seek file beginning
		fread(&FILE_VER, SIZE_32, 1, file); // Load file version
		fclose(file); // Close file

		switch (FILE_VER)
		{
		case 001u:
			LoadStateFileV001();
			break;
		}
	}
}

#undef SIZE_8
#undef SIZE_16
#undef SIZE_32
#undef SIZE_64