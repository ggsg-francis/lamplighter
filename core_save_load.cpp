#include "core_save_load.h"

#include "core.h"
#include "objects_entities.h"

#include <stdio.h>

#define SIZE_8 1
#define SIZE_16 2
#define SIZE_32 4
#define SIZE_64 8

namespace index
{
	struct cell
	{
		mem::idbuf ents;
	};
	extern cell cells[WORLD_SIZE][WORLD_SIZE];
}

bool SaveExists()
{
	FILE* file = fopen("save/save.bin", "rb"); // Open file
	return file != NULL;
}

void SaveState()
{
	// clean all unused item instances
	for (btID index_item = 0; index_item <= index::block_item.index_end; index_item++) // For every item
	{
		if (index::block_item.used[index_item])
		{
			btui32 item_reference_count = 0u;
			for (btID index_ent = 0; index_ent <= index::block_entity.index_end; index_ent++) // For every entity
			{
				if (index::block_entity.used[index_ent])
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
				index::DestroyItem(index_item);
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
		fwrite(&index::players, SIZE_16, 2, file);

		//-------------------------------- ENTITIES

		fwrite(&index::block_entity.index_end, SIZE_16, 1, file);
		fwrite(&index::block_entity.used, SIZE_8, (size_t)(index::block_entity.index_end + 1ui16), file);

		for (btID i = 0; i <= index::block_entity.index_end; i++) // For every entity
		{
			if (index::block_entity.used[i])
			{
				fwrite(&ENTITY(i)->type, SIZE_8, 1, file);

				fwrite(&ENTITY(i)->radius, SIZE_32, 1, file);
				fwrite(&ENTITY(i)->height, SIZE_32, 1, file);
				fwrite(&ENTITY(i)->properties, SIZE_8, 1, file);
				fwrite(&ENTITY(i)->faction, SIZE_8, 1, file);
				fwrite(&ENTITY(i)->state.damagestate, SIZE_16, 1, file); // duplicated due to it used to being a 32 bit number
				fwrite(&ENTITY(i)->state.damagestate, SIZE_16, 1, file); // duplicated due to it used to being a 32 bit number
				fwrite(&ENTITY(i)->state.stateFlags, SIZE_64, 1, file);
				fwrite(&ENTITY(i)->state.effects, sizeof(mem::Buffer32<StatusEffect>), 1, file);
				fwrite(&ENTITY(i)->t.position.x, SIZE_32, 1, file);
				fwrite(&ENTITY(i)->t.position.y, SIZE_32, 1, file);
				fwrite(&ENTITY(i)->t.height, SIZE_32, 1, file);
				fwrite(&ENTITY(i)->t.yaw, SIZE_32, 1, file);

				switch (ENTITY(i)->type)
				{
				case ENTITY_TYPE_RESTING_ITEM:
					fwrite(&ITEM(i)->item_instance, SIZE_16, 1, file);
				case ENTITY_TYPE_CHARA:
					fwrite(&ACTOR(i)->viewYaw, SIZE_32, 1, file);
					fwrite(&ACTOR(i)->viewPitch, SIZE_32, 1, file);
					fwrite(&ACTOR(i)->t_skin, SIZE_16, 1, file);
					fwrite(&ACTOR(i)->speed, SIZE_32, 1, file);
					fwrite(&ACTOR(i)->agility, SIZE_32, 1, file);
					fwrite(&ACTOR(i)->inventory, sizeof(Inventory), 1, file);
					fwrite(&ACTOR(i)->inv_active_slot, SIZE_32, 1, file);
					fwrite(&ACTOR(i)->aiControlled, SIZE_8, 1, file);
				}
			}
		}

		//-------------------------------- ITEMS

		fwrite(&index::block_item.index_end, SIZE_16, 1, file);
		fwrite(&index::block_item.used, SIZE_8, (size_t)(index::block_item.index_end + 1ui16), file);

		for (btID i = 0; i <= index::block_item.index_end; i++) // For every item
		{
			if (index::block_item.used[i])
			{
				fwrite(&GETITEM_MISC(i)->item_template, SIZE_16, 1, file);

				switch (index::GetItemType(i))
				{
				case ITEM_TYPE_CONS:
					fwrite(&GETITEM_CONS(i)->uses, SIZE_32, 1, file);
				}
			}
		}

		fclose(file); // Close file
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
		fread(&index::players, SIZE_16, 2, file);

		//-------------------------------- ENTITIES

		fread(&index::block_entity.index_end, SIZE_16, 1, file);
		fread(&index::block_entity.used, SIZE_8, (size_t)(index::block_entity.index_end + 1ui16), file);

		for (btID i = 0; i <= index::block_entity.index_end; i++) // For every entity
		{
			if (index::block_entity.used[i])
			{
				EntityType type_temp;
				fread(&type_temp, SIZE_8, 1, file);

				// initialize entity here (eg. new) 
				index::IndexInitEntity(i, type_temp);

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
				case ENTITY_TYPE_CHARA:
					fread(&ACTOR(i)->viewYaw, SIZE_32, 1, file);
					fread(&ACTOR(i)->viewPitch, SIZE_32, 1, file);
					fread(&ACTOR(i)->t_skin, SIZE_16, 1, file);
					fread(&ACTOR(i)->speed, SIZE_32, 1, file);
					fread(&ACTOR(i)->agility, SIZE_32, 1, file);
					fread(&ACTOR(i)->inventory, sizeof(Inventory), 1, file);
					fread(&ACTOR(i)->inv_active_slot, SIZE_32, 1, file);
					fread(&ACTOR(i)->aiControlled, SIZE_8, 1, file);
				}

				std::cout << "Loaded entity ID " << i << std::endl;
			}
		}

		//-------------------------------- ITEMS

		fread(&index::block_item.index_end, SIZE_16, 1, file);
		fread(&index::block_item.used, SIZE_8, (size_t)(index::block_item.index_end + 1ui16), file);

		for (btID i = 0; i <= index::block_item.index_end; i++) // For every entity
		{
			if (index::block_item.used[i])
			{
				btID template_temp;
				fread(&template_temp, SIZE_16, 1, file);

				index::IndexInitItem(i, acv::item_types[template_temp]);

				GETITEM_MISC(i)->item_template = template_temp;

				switch (index::GetItemType(i))
				{
				case ITEM_TYPE_CONS:
					fread(&GETITEM_CONS(i)->uses, SIZE_32, 1, file);
				}

				std::cout << "Loaded item ID " << i << std::endl;
			}
		}

		fclose(file); // Close file
	}
}
void LoadState()
{
	btui32 FILE_VER = 0b11111111111111111111111111111111;

	FILE* file = fopen("save/save.bin", "rb"); // Open file
	if (file != NULL)
	{
		index::ClearBuffers();

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