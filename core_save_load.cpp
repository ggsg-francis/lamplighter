#include "core_save_load.h"

#include "core.h"
#include "objects.h"

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
				fwrite(&ENTITY(i)->state.hp, SIZE_32, 1, file);
				fwrite(&ENTITY(i)->state.properties, SIZE_64, 1, file);
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

		for (btID i = 0; i <= index::block_item.index_end; i++) // For every entity
		{
			if (index::block_item.used[i])
			{
				//fwrite(&index::items[i]->item_template, SIZE_16, 1, file);
				fwrite(&GETITEM_MISC(i)->item_template, SIZE_16, 1, file);
			}
		}

		//-------------------------------- CELL CACHE

		// TODO: there's a glitch in the regemeration of the environment cells, we should
		// formalize this to make sure there are no glitches in the future
		// until then, this is a quick fix
		// POTENTIAL FIX: in the loop where we iterate all tick functions, incl. a function call
		// that regenerates the cells of X entity, so that i never have to worry about it again
		fwrite(&index::cells, sizeof(index::cells), 1, file);

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
				fread(&ENTITY(i)->state.hp, SIZE_32, 1, file);
				fread(&ENTITY(i)->state.properties, SIZE_64, 1, file);
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

				//index::items[i]->item_template = template_temp;
				GETITEM_MISC(i)->item_template = template_temp;

				std::cout << "Loaded item ID " << i << std::endl;
			}
		}

		//-------------------------------- CELL CACHE

		fread(&index::cells, sizeof(index::cells), 1, file);

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