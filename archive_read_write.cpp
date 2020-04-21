#include "archive_read_write.h"
#ifdef DEF_ARCHIVER
#include "archiver_graphics_convert.h"
#endif
#include "graphics.hpp"
#include "archive.hpp"

#include <fstream>
#include <vector>
#include <iostream>

namespace serializer
{
	btID GetAssetIDFromHandle(char* handle, AssetType type)
	{
		btID retid = ID_NULL;
		for (btID i = 0; i < acv::assetCount; ++i)
		{
			if (memcmp(handle, acv::assets[i].handle, 8u) == 0)
			{
				retid = i;
				break;
			}
		}
		return retid; // Return ID of new filename
	}

	btID GetSpellIDFromHandle(char* handle)
	{
		btID retid = ID_NULL;
		for (btID i = 0; i < acv::spell_index; ++i)
		{
			if (memcmp(handle, acv::spells[i].handle, 8u) == 0)
			{
				retid = i;
				break;
			}
		}
		return retid; // Return ID of new filename
	}

	btID GetProjIDFromHandle(char* handle)
	{
		btID retid = ID_NULL;
		for (btID i = 0; i < acv::projectiles_index; ++i)
		{
			if (memcmp(handle, acv::projectiles[i].handle, 8u) == 0)
			{
				retid = i;
				break;
			}
		}
		return retid; // Return ID of new filename
	}

	btID GetItemIDFromHandle(char* handle)
	{
		btID retid = ID_NULL;
		for (btID i = 0; i < acv::projectiles_index; ++i)
		{
			if (memcmp(handle, acv::projectiles[i].handle, 8u) == 0)
			{
				retid = i;
				break;
			}
		}
		return retid; // Return ID of new filename
	}

	void debug_output()
	{
		std::cout << "__________________________________________" << std::endl;
		std::cout << "FILENAMES" << std::endl;
		for (btui32 i = 0u; i < acv::assetCount; i++)
		{
			std::cout << "FILE " << i << " | TYPE " << acv::assets[i].type << " | ADDR " << acv::assets[i].filename << std::endl;
		}
		std::cout << "__________________________________________" << std::endl;
		std::cout << "PROPS" << std::endl;
		for (int i = 0; i < acv::prop_index; i++)
		{
			std::cout << "---------------------" << std::endl;
			std::cout << "MESH ID              " << acv::props[i].idMesh << std::endl;
			std::cout << "TEXTURE ID           " << acv::props[i].idTxtr << std::endl;
		}
		std::cout << "__________________________________________" << std::endl;
		std::cout << "SPELLS" << std::endl;
		for (int i = 0; i < acv::spell_index; i++)
		{
			std::cout << "---------------------" << std::endl;
			std::cout << "HANDLE               ";
			for (btui8 c = 0; c < 8; ++c)
				std::cout << acv::spells[i].handle[c];
			std::cout << std::endl;
			std::cout << "NAME                 " << acv::spells[i].name << std::endl;
			std::cout << "CAST TYPE            " << acv::spells[i].cast_type << std::endl;
			std::cout << "EFFECT ON TARGET     " << acv::spells[i].target_effect_type << std::endl;
			std::cout << "DURATION ON TARGET   " << acv::spells[i].target_effect_duration << std::endl;
			std::cout << "MAGNITUDE ON TARGET  " << acv::spells[i].target_effect_magnitude << std::endl;
		}
		std::cout << "__________________________________________" << std::endl;
		std::cout << "PROJECTILES" << std::endl;
		for (int i = 0; i < acv::projectiles_index; i++)
		{
			std::cout << "---------------------" << std::endl;
			std::cout << "HANDLE               ";
			for (btui8 c = 0; c < 8; ++c)
				std::cout << acv::projectiles[i].handle[c];
			std::cout << std::endl;
			std::cout << "DAMAGE               " << acv::projectiles[i].damage << std::endl;
			std::cout << "SAVE ON HIT          " << acv::projectiles[i].saveOnHit << std::endl;
			std::cout << "AMMUNITION TYPE      " << acv::projectiles[i].ammunition_type << std::endl;
			std::cout << "MESH                 " << acv::projectiles[i].mesh << std::endl;
			std::cout << "TEXTURE              " << acv::projectiles[i].texture << std::endl;
		}
		std::cout << "__________________________________________" << std::endl;
		std::cout << "ITEMS" << std::endl;
		for (int i = 0; i < acv::item_index; i++)
		{
			#define ITEMI ((acv::BaseItem*)acv::items[i])
			#define WMELI ((acv::BaseItemMel*)acv::items[i])
			#define CONSI ((acv::BaseItemCon*)acv::items[i])

			std::cout << "---------------------" << std::endl;
			std::cout << "ID                   " << ITEMI->id << std::endl;
			std::cout << "TYPE                 " << acv::item_types[i] << std::endl;
			std::cout << "ICON                 " << ITEMI->id_icon << std::endl;
			std::cout << "NAME                 " << ITEMI->name << std::endl;
			std::cout << "WEIGHT               " << ITEMI->f_weight << std::endl;
			std::cout << "BASE VALUE           " << ITEMI->f_value_base << std::endl;
			std::cout << "RADIUS               " << ITEMI->f_radius << std::endl;
			std::cout << "MESH ID              " << ITEMI->id_mesh << std::endl;
			std::cout << "TEXTURE ID           " << ITEMI->id_tex << std::endl;

			if (acv::item_types[i] == ITEM_TYPE_WPN_MELEE)
			{
				std::cout << "DAMAGE PIERCE        " << WMELI->f_dam_pierce << std::endl;
				std::cout << "DAMAGE SLASH         " << WMELI->f_dam_slash << std::endl;
				std::cout << "DAMAGE SLAM          " << WMELI->f_dam_slam << std::endl;
			}
			if (acv::item_types[i] == ITEM_TYPE_CONS)
			{
				std::cout << "EFFECT               " << CONSI->id_effect << std::endl;
				std::cout << "PROJECTILE           " << CONSI->id_projectile << std::endl;
			}
			#undef ITEMI
			#undef WPNI
			#undef PTNI
		}
		std::cout << "__________________________________________" << std::endl;
		std::cout << "ACTOR BASES" << std::endl;
		for (int i = 0; i < acv::actor_template_index; i++)
		{
			std::cout << "---------------------" << std::endl;
			std::cout << "HANDLE               ";
			for (btui8 c = 0; c < 8; ++c)
				std::cout << acv::actor_templates[i].handle[c];
			std::cout << std::endl;
			std::cout << "MESH ID HEAD         " << acv::actor_templates[i].m_head << std::endl;
			std::cout << "MESH ID BODY         " << acv::actor_templates[i].m_body << std::endl;
			std::cout << "MESH ID ARM          " << acv::actor_templates[i].m_arm << std::endl;
			std::cout << "MESH ID LEG          " << acv::actor_templates[i].m_leg << std::endl;
			std::cout << "TXTR ID HEAD         " << acv::actor_templates[i].t_head << std::endl;
			std::cout << "TXTR ID BODY         " << acv::actor_templates[i].t_body << std::endl;
			std::cout << "TXTR ID ARM          " << acv::actor_templates[i].t_arm << std::endl;
			std::cout << "TXTR ID LEG          " << acv::actor_templates[i].t_leg << std::endl;
			std::cout << "JPOS ARM FORWARD     " << acv::actor_templates[i].jpos_arm_fw[0] << std::endl;
			std::cout << "JPOS ARM RIGHT       " << acv::actor_templates[i].jpos_arm_rt[0] << std::endl;
			std::cout << "JPOS ARM UP          " << acv::actor_templates[i].jpos_arm_up[0] << std::endl;
			std::cout << "JPOS LEG FORWARD     " << acv::actor_templates[i].jpos_leg_fw[0] << std::endl;
			std::cout << "JPOS LEG RIGHT       " << acv::actor_templates[i].jpos_leg_rt[0] << std::endl;
			std::cout << "JPOS LEG UP          " << acv::actor_templates[i].jpos_leg_up[0] << std::endl;
			std::cout << "ARM LENGTH           " << acv::actor_templates[i].leng_arm[0] << std::endl;
			std::cout << "LEG LENGTH           " << acv::actor_templates[i].leng_leg[0] << std::endl;
			std::cout << "BODY LENGTH          " << acv::actor_templates[i].leng_body[0] << std::endl;
		}
	}

	#ifdef DEF_ARCHIVER
	void convert_files_src(char* fn)
	{
		printf("  ||||||||                      ||||||||\n||||    ||||                  ||||    ||||\n||||    ||||  ||||      ||||  ||||    ||||\n||||    ||||  ||||  ||  ||||  ||||    ||||\n  ||||||||      ||||||||||      ||||||||\n");

		FILE* file = fopen(fn, "r"); // Open file
		if (file != NULL)
		{
			fseek(file, 0, SEEK_SET); // Seek file beginning

			btui32 index = 0u;

			bool comment = false;

			//read property
			char elem[5];
			char oper;
			fgets(&elem[0], 5, file); // Get element name
			// Extra here: comment returns true or false depending on whether the comparison is ****
			while (strcmp(elem, "item") == 0 || strcmp(elem, "****") == 0) // is it a new item?
			{
				comment = strcmp(elem, "****") == 0;
				if (comment)
				{
					while (true)
					{
						oper = fgetc(file); // Get character
						if (oper == '\n') break;
					}
				}
				else
				{
					//....................................... GET ITEM TYPE

					oper = fgetc(file); // Advance past equals sign
					fgets(&elem[0], 5, file); // Get element name

					//....................................... SET ITEM TYPE

					AssetType type;
					graphics::TextureFilterMode t_filter_mode = graphics::eLINEAR;
					graphics::TextureEdgeMode t_edge_mode = graphics::eREPEAT;
					char cdest[FN_SIZE];
					char csrca[FN_SIZE];
					char csrcb[FN_SIZE];

					if (strcmp(elem, "txtr") == 0) { // Texture
						type = ASSET_TEXTURE_FILE;
					}
					else if (strcmp(elem, "mesh") == 0) { // Mesh
						type = ASSET_MESH_FILE;
					}
					else if (strcmp(elem, "mshb") == 0) { // Mesh Blend
						type = ASSET_MESHBLEND_FILE;
					}
					else if (strcmp(elem, "mshd") == 0) { // Mesh Deform
						type = ASSET_MESHDEFORM_FILE;
					}

					//....................................... READ ITEM PROPERTIES

					oper = fgetc(file); //  Advance past line break
					fgets(&elem[0], 5, file); // Get element name
					while (strcmp(elem, "<<<<") != 0) // While we haven't reached the end of this item's stats
					{
						oper = fgetc(file); // Advance past equals sign
						fpos_t start; fpos_t end; // Start and end points of the value (eg. name = dick; dick is the value)
						fgetpos(file, &start);
						while (true)
						{
							oper = fgetc(file); // Get character
							if (oper == '\n')
							{
								fgetpos(file, &end);
								break;
							}
						}
						fseek(file, (long)start, SEEK_SET);
						char* value = (char*)malloc(end - start); // Allocate space for value string
						fgets(&value[0], (int)(end - start) - 1, file); // Get element name

						if (strcmp(elem, "hndl") == 0) // Destination string
						{
							strcpy(cdest, "res/"); strcat(cdest, value); // set filename?
							memcpy(acv::assets[index].handle, value, 8u); // Copy filename into archive
						}
						else if (strcmp(elem, "srca") == 0) // Source string A
							strcpy(csrca, value);
						else if (strcmp(elem, "srcb") == 0) // Source string B
							strcpy(csrcb, value);
						else if (strcmp(elem, "tfil") == 0) // Texture filter mode
						{
							if (strcmp(value, "nearest") == 0)
								t_filter_mode = graphics::eNEAREST;
							else if (strcmp(value, "nearestmip") == 0)
								t_filter_mode = graphics::eNEAREST_MIPMAP;
							else if (strcmp(value, "linear") == 0)
								t_filter_mode = graphics::eLINEAR;
							else if (strcmp(value, "linearmip") == 0)
								t_filter_mode = graphics::eLINEAR_MIPMAP;
							else if (strcmp(value, "foliage") == 0)
								t_filter_mode = graphics::eFOLIAGE;
						}
						else if (strcmp(elem, "tedg") == 0) // Texture edge mode
						{
							if (strcmp(value, "repeat") == 0)
								t_edge_mode = graphics::eREPEAT;
							else if (strcmp(value, "repeatmir") == 0)
								t_edge_mode = graphics::eREPEAT_MIRROR;
							else if (strcmp(value, "clamp") == 0)
								t_edge_mode = graphics::eCLAMP;
							else if (strcmp(value, "repeatclamp") == 0)
								t_edge_mode = graphics::eREPEAT_X_CLAMP_Y;
							else if (strcmp(value, "clamprepeat") == 0)
								t_edge_mode = graphics::eCLAMP_X_REPEAT_Y;
						}

						free(value); // Free content string

						// Get name of next element
						oper = fgetc(file); //  Advance past line break
						fgets(&elem[0], 5, file); // Get element name
					}

					//....................................... CONVERT AND SAVE ASSETS

					if (type == ASSET_TEXTURE_FILE)
					{
						std::cout << "CONV TEXTURE         [" << cdest << "]" << std::endl;
						graphics::ConvertTex(csrca, cdest, t_filter_mode, t_edge_mode); // Create source file
						std::cout << "SETTING INDEX        [" << index << "]" << std::endl;
						strcpy(acv::assets[index].filename, cdest); // Copy filename into archive
						acv::assets[index].type = ASSET_TEXTURE_FILE;
						std::cout << "---------------------" << std::endl;
					}
					else if (type == ASSET_MESH_FILE)
					{
						std::cout << "CONV MESH            [" << cdest << "]" << std::endl;
						graphics::ConvertMesh(csrca, cdest); // Create source file
						std::cout << "SETTING INDEX        [" << index << "]" << std::endl;
						strcpy(acv::assets[index].filename, cdest); // Copy filename into archive
						acv::assets[index].type = ASSET_MESH_FILE;
						std::cout << "---------------------" << std::endl;
					}
					else if (type == ASSET_MESHBLEND_FILE)
					{
						std::cout << "CONV MESHBLEND       [" << cdest << "]" << std::endl;
						graphics::ConvertMB(csrca, csrcb, cdest); // Create source file
						std::cout << "SETTING INDEX        [" << index << "]" << std::endl;
						strcpy(acv::assets[index].filename, cdest); // Copy filename into archive
						acv::assets[index].type = ASSET_MESHBLEND_FILE;
						std::cout << "---------------------" << std::endl;
					}
					else if (type == ASSET_MESHDEFORM_FILE)
					{
						std::cout << "CONV MESHDEFORM      [" << cdest << "]" << std::endl;
						if (acv::assets[index].handle[0] == 'L' && acv::assets[index].handle[1] == 'e' && acv::assets[index].handle[2] == 'g')
							graphics::ConvertMD(csrca, cdest, graphics::eLEG); // Create source file
						else if (acv::assets[index].handle[0] == 'B' && acv::assets[index].handle[1] == 'o' && acv::assets[index].handle[2] == 'd')
							graphics::ConvertMD(csrca, cdest, graphics::eBODY); // Create source file
						else if (acv::assets[index].handle[0] == 'A' && acv::assets[index].handle[1] == 'r' && acv::assets[index].handle[2] == 'm')
							graphics::ConvertMD(csrca, cdest, graphics::eARM); // Create source file
						else
							graphics::ConvertMD(csrca, cdest, graphics::eDEFAULT); // Create source file
						std::cout << "SETTING INDEX        [" << index << "]" << std::endl;
						strcpy(acv::assets[index].filename, cdest); // Copy filename into archive
						acv::assets[index].type = ASSET_MESHDEFORM_FILE;
						std::cout << "---------------------" << std::endl;
					}

					++index; // We are done with this index

					oper = fgetc(file); //  Advance past line break
				} // End if not comment
				fgets(&elem[0], 5, file); // Get next element name
			}
			acv::assetCount = index; // Conveniently, the index is left equaling the total number of assets
			std::cout << "FINAL ASSET COUNT    [" << acv::assetCount << "]" << std::endl;
			fclose(file); // Close file
		}
	}
	#endif

	void InterpretWord(FILE* file, char* value, int* size)
	{
		*size = 1;
		char oper;
		fpos_t start; // Start position of the value (eg. name = dick; dick is the value)
		fgetpos(file, &start);
		while (true)
		{
			++*size;
			oper = fgetc(file); // Get character
			if (oper == '\n')
			{
				fsetpos(file, &start);
				fgets(value, (int)(*size) - 1, file); // Get element name
				break;
			}
		}
	}

	void InterpretCommand(FILE* file, char* value)
	{
		fgets(value, 5, file); // Get element name
	}

	void AdvanceSpace(FILE* file)
	{
		fgetc(file); //  Advance past space
	}

	void InterpretArchiveContents(char* fn)
	{
		void* item;

		#define GET2_ITEM_ITEM ((acv::BaseItem*)item)
		#define GET2_ITEM_APR ((acv::BaseItemEqp*)item)
		#define GET2_ITEM_W_MELEE ((acv::BaseItemMel*)item)
		#define GET2_ITEM_PTN ((acv::BaseItemCon*)item)
		#define GET2_ITEM_GUN ((acv::BaseItemGun*)item)

		FILE* file = fopen(fn, "r"); // Open file
		if (file != NULL)
		{
			fseek(file, 0, SEEK_SET); // Seek file beginning

			char elem[5];
			char oper;

			//-------------------------------- PROPS

		getPropElemName:
			fgets(&elem[0], 5, file); // Get element name
			if (strcmp(elem, "PROP") == 0) // is it a new item?
			{
				// GET TYPE

				AdvanceSpace(file);
				InterpretCommand(file, elem);

				// SET TYPE

				// If it's an environment prop
				if (strcmp(elem, "prop") != 0) goto nextPropElem;

				// READ PROPERTIES

				AdvanceSpace(file);
				InterpretCommand(file, elem);
				while (strcmp(elem, "<<<<") != 0) // While we haven't reached the end of this item's stats
				{
					oper = fgetc(file); // Advance past equals sign

					char value[256]; int value_size = 0;
					InterpretWord(file, value, &value_size);

					if (strcmp(elem, "srct") == 0) // Texture
						acv::props[acv::prop_index].idTxtr = GetAssetIDFromHandle(value, ASSET_TEXTURE_FILE);
					else if (strcmp(elem, "srcm") == 0) // Mesh
						acv::props[acv::prop_index].idMesh = GetAssetIDFromHandle(value, ASSET_MESH_FILE);
					else if (strcmp(elem, "flor") == 0)
						acv::props[acv::prop_index].floorType = (acv::EnvProp::EnvPropFloorMat)atoi(value);
					else if (strcmp(elem, "phys") == 0)
						acv::props[acv::prop_index].physShape = (acv::EnvProp::EnvPropPhysShape)atoi(value);
					else if (strcmp(elem, "flag") == 0)
						acv::props[acv::prop_index].flags = (acv::EnvProp::EnvPropFlags)atoi(value);

					//get name of next operator
					oper = fgetc(file); //  Advance past line break
					InterpretCommand(file, elem);
				}
				++acv::prop_index;
			nextPropElem:
				oper = fgetc(file); //  Advance past line break
				goto getPropElemName;
			}
			else goto skipSpelElemName;

			//-------------------------------- SPELLS

		getSpelElemName:
			fgets(&elem[0], 5, file); // Get element name
		skipSpelElemName:
			while (strcmp(elem, "SPEL") == 0) // is it a new item?
			{
				// GET TYPE

				AdvanceSpace(file);
				InterpretCommand(file, elem);

				// SET TYPE

				// If it's an environment prop
				if (strcmp(elem, "spel") != 0) goto nextSpelElem;

				// READ PROPERTIES

				AdvanceSpace(file);
				InterpretCommand(file, elem);
				while (strcmp(elem, "<<<<") != 0) // While we haven't reached the end of this item's stats
				{
					AdvanceSpace(file);
					char value[256]; int value_size = 0;
					InterpretWord(file, value, &value_size);

					if (strcmp(elem, "hndl") == 0) // Handle
						memcpy(acv::spells[acv::spell_index].handle, value, 8u);
					else if (strcmp(elem, "name") == 0) // Name
						memcpy(acv::spells[acv::spell_index].name, value, value_size);
					else if (strcmp(elem, "eidt") == 0) // Effect on Target
						acv::spells[acv::spell_index].target_effect_type = (btui16)atoi(value);
					else if (strcmp(elem, "edrt") == 0) // Duration on Target
						acv::spells[acv::spell_index].target_effect_duration = (btf32)atof(value);
					else if (strcmp(elem, "emgt") == 0) // Magnitude on Target
						acv::spells[acv::spell_index].target_effect_magnitude = (btui32)atoi(value);

					//#define NEXT_OPERATOR oper=fgetc(file);fgets(&elem[0], 5, file); // Get element name

					//get name of next operator
					oper = fgetc(file); //  Advance past line break
					InterpretCommand(file, elem);
				}
				++acv::spell_index;
			nextSpelElem:
				AdvanceSpace(file);
				goto getSpelElemName;
			}

		getProjElemName:
			while (strcmp(elem, "PROJ") == 0) // is it a new item?
			{
				// GET TYPE

				AdvanceSpace(file);
				InterpretCommand(file, elem);

				// SET TYPE

				// If it's an environment prop
				if (strcmp(elem, "proj") != 0) goto nextProjElem;

				// READ PROPERTIES

				AdvanceSpace(file);
				InterpretCommand(file, elem);
				while (strcmp(elem, "<<<<") != 0) // While we haven't reached the end of this item's stats
				{
					AdvanceSpace(file);
					char value[256]; int value_size = 0;
					InterpretWord(file, value, &value_size);

					if (strcmp(elem, "hndl") == 0) // Handle
						memcpy(acv::projectiles[acv::projectiles_index].handle, value, 8u);
					else if (strcmp(elem, "pdam") == 0) // Damage amount
						acv::projectiles[acv::projectiles_index].damage = (btui32)atoi(value);
					else if (strcmp(elem, "psav") == 0) // Bool save on fire
						acv::projectiles[acv::projectiles_index].saveOnHit = (bool)atoi(value);
					else if (strcmp(elem, "atyp") == 0) // Type
						acv::projectiles[acv::projectiles_index].ammunition_type = value[0];
					else if (strcmp(elem, "srcm") == 0) // Source Mesh
						acv::projectiles[acv::projectiles_index].mesh = GetAssetIDFromHandle(value, ASSET_MESH_FILE);
					else if (strcmp(elem, "srct") == 0) // Source Texture
						acv::projectiles[acv::projectiles_index].texture = GetAssetIDFromHandle(value, ASSET_TEXTURE_FILE);

					//#define NEXT_OPERATOR oper=fgetc(file);fgets(&elem[0], 5, file); // Get element name

					//get name of next operator
					oper = fgetc(file); //  Advance past line break
					InterpretCommand(file, elem);
				}
				++acv::projectiles_index;
			nextProjElem:
				AdvanceSpace(file);
				InterpretCommand(file, elem);
				goto getProjElemName;
			}

			//-------------------------------- ITEMS

			//InterpretCommand(file, elem);

			while (strcmp(elem, "ITEM") == 0) // is it a new item?
			{
				// GET ITEM TYPE

				oper = fgetc(file); // Advance past equals sign
				fgets(&elem[0], 5, file); // Get element name

				// SET ITEM TYPE

				if (elem[0] == 'w') // If it's a weapon
				{
					if (strcmp(elem, "wmel") == 0) {
						acv::items[acv::item_index] = new acv::BaseItemMel();
						acv::item_types[acv::item_index] = ITEM_TYPE_WPN_MELEE;
					}
					else if (strcmp(elem, "wgun") == 0) {
						acv::items[acv::item_index] = new acv::BaseItemGun();
						acv::item_types[acv::item_index] = ITEM_TYPE_WPN_MATCHGUN;
					}
					else if (strcmp(elem, "wmgc") == 0) {
						acv::items[acv::item_index] = new acv::BaseItemMgc();
						acv::item_types[acv::item_index] = ITEM_TYPE_WPN_MAGIC;
					}
				}
				else
				{
					if (strcmp(elem, "misc") == 0) {
						acv::items[acv::item_index] = new acv::BaseItem();
						acv::item_types[acv::item_index] = ITEM_TYPE_MISC;
					}
					else if (strcmp(elem, "aprl") == 0) {
						acv::items[acv::item_index] = new acv::BaseItemEqp();
						acv::item_types[acv::item_index] = ITEM_TYPE_EQUIP;
					}
					else if (strcmp(elem, "cons") == 0) {
						acv::items[acv::item_index] = new acv::BaseItemCon();
						acv::item_types[acv::item_index] = ITEM_TYPE_CONS;
					}
				}

				// ASSIGN ITEM ID

				item = acv::items[acv::item_index];
				GET2_ITEM_ITEM->id = acv::item_index;
				acv::item_index++;

				// READ ITEM PROPERTIES

				oper = fgetc(file); //  Advance past line break
				fgets(&elem[0], 5, file); // Get element name
				while (strcmp(elem, "<<<<") != 0) // While we haven't reached the end of this item's stats
				{
					oper = fgetc(file); // Advance past equals sign
					
					char value[256]; int value_size = 0;
					InterpretWord(file, value, &value_size);

					// Any item stats
					if (strcmp(elem, "hndl") == 0) // Handle
						memcpy(GET2_ITEM_ITEM->handle, value, 8u);
					else if (strcmp(elem, "type") == 0) // What type of item is it?
					{
						if (strcmp(value, "weapon") == 0)
						{
							int hello = 0;
						}
						//ITEM_ITEM->id_icon = (id_t)atoi(value);
					}
					else if (strcmp(elem, "icon") == 0) // Icon (change to string?)
						//ITEM_ITEM->id_icon = (btID)atoi(value);
						GET2_ITEM_ITEM->id_icon = GetAssetIDFromHandle(value, ASSET_TEXTURE_FILE);
					else if (strcmp(elem, "name") == 0) // Name
						memcpy(GET2_ITEM_ITEM->name, value, value_size);
					else if (strcmp(elem, "wght") == 0) // Carry weight
						GET2_ITEM_ITEM->f_weight = (btf32)atof(value);
					else if (strcmp(elem, "muns") == 0) // Money base value
						GET2_ITEM_ITEM->f_value_base = (btui32)atoi(value);
					else if (strcmp(elem, "radi") == 0) // Radius when placed
						GET2_ITEM_ITEM->f_radius = (btf32)atof(value);
					else if (strcmp(elem, "mdlh") == 0) // Model height when placed
						GET2_ITEM_ITEM->f_model_height = (btf32)atof(value);
					else if (strcmp(elem, "srct") == 0) // Texture
						GET2_ITEM_ITEM->id_tex = GetAssetIDFromHandle(value, ASSET_TEXTURE_FILE);
					else if (strcmp(elem, "srcm") == 0) // Mesh
						GET2_ITEM_ITEM->id_mesh = GetAssetIDFromHandle(value, ASSET_MESH_FILE);
					else if (strcmp(elem, "scmb") == 0) // Mesh
						GET2_ITEM_ITEM->id_mesh = GetAssetIDFromHandle(value, ASSET_MESHBLEND_FILE);
					// Weapon value
					else if (strcmp(elem, "held") == 0) // Damage pierce
					{
						//ITEM_WPN->damage_pierce = atof(value);
					}
					else if (strcmp(elem, "dmg1") == 0) // Damage pierce
						GET2_ITEM_W_MELEE->f_dam_pierce = (float)atof(value);
					else if (strcmp(elem, "dmg2") == 0) // Damage slash
						GET2_ITEM_W_MELEE->f_dam_slash = (float)atof(value);
					else if (strcmp(elem, "dmg3") == 0) // Damage slam
						GET2_ITEM_W_MELEE->f_dam_slam = (float)atof(value);
					// Apparel value
					else if (strcmp(elem, "def1") == 0) // Defend pierce damage
						GET2_ITEM_APR->block_pierce = (float)atof(value);
					else if (strcmp(elem, "def2") == 0) // Defend slash damage
						GET2_ITEM_APR->block_slice = (float)atof(value);
					else if (strcmp(elem, "def3") == 0) // Defend slam damage
						GET2_ITEM_APR->block_slam = (float)atof(value);
					// Con Value
					else if (strcmp(elem, "cspl") == 0) // Spell
						GET2_ITEM_PTN->id_effect = GetSpellIDFromHandle(value);
					else if (strcmp(elem, "cprj") == 0) // Spell
						GET2_ITEM_PTN->id_projectile = GetProjIDFromHandle(value);

					else if (strcmp(elem, "atyp") == 0) // Spell
						GET2_ITEM_GUN->ammunition_type = value[0];

					//get name of next operator
					oper = fgetc(file); //  Advance past line break
					fgets(&elem[0], 5, file); // Get element name
				}
				oper = fgetc(file); //  Advance past line break
				fgets(&elem[0], 5, file); // Get element name
			}

			//-------------------------------- ENTITY TEMPLATES
			
			///*
		getEntTElemName:
			//fgets(&elem[0], 5, file); // Get element name
		skipEntTElemName:
			while (strcmp(elem, "ACTT") == 0) // is it a new item?
			{
				// GET TYPE

				oper = fgetc(file); // Advance past equals sign
				fgets(&elem[0], 5, file); // Get element name

				// SET TYPE

				// If it's an environment prop
				if (strcmp(elem, "actt") != 0) goto nextEntTElem;

				// READ PROPERTIES

				AdvanceSpace(file);
				InterpretCommand(file, elem);
				while (strcmp(elem, "<<<<") != 0) // While we haven't reached the end of this item's stats
				{
					AdvanceSpace(file);
					if (strcmp(elem, "hndl") == 0) // Handle
					{
						char value[256]; int value_size = 0;
						InterpretWord(file, value, &value_size);
						memcpy(acv::actor_templates[acv::actor_template_index].handle, value, 8u);

						// Get name of next operator
						AdvanceSpace(file);
						InterpretCommand(file, elem);
					}
					else if (strcmp(elem, "srcm") == 0) // Source Mesh
					{
						InterpretCommand(file, elem);
						AdvanceSpace(file);
						char value[256]; int value_size = 0;
						InterpretWord(file, value, &value_size);

						if (strcmp(elem, "head") == 0) // Head
							acv::actor_templates[acv::actor_template_index].m_head = (btui16)GetAssetIDFromHandle(value, ASSET_MESHDEFORM_FILE);
						if (strcmp(elem, "body") == 0) // Body
							acv::actor_templates[acv::actor_template_index].m_body = (btui16)GetAssetIDFromHandle(value, ASSET_MESHDEFORM_FILE);
						if (strcmp(elem, "larm") == 0) // Arm
							acv::actor_templates[acv::actor_template_index].m_arm = (btui16)GetAssetIDFromHandle(value, ASSET_MESHDEFORM_FILE);
						if (strcmp(elem, "lleg") == 0) // Leg
							acv::actor_templates[acv::actor_template_index].m_leg = (btui16)GetAssetIDFromHandle(value, ASSET_MESHDEFORM_FILE);

						// Get name of next operator
						AdvanceSpace(file);
						InterpretCommand(file, elem);
					}
					else if (strcmp(elem, "srct") == 0) // Source Texture
					{
						InterpretCommand(file, elem);
						AdvanceSpace(file);
						char value[256]; int value_size = 0;
						InterpretWord(file, value, &value_size);

						if (strcmp(elem, "head") == 0) // Head
							acv::actor_templates[acv::actor_template_index].t_head = (btui16)GetAssetIDFromHandle(value, ASSET_TEXTURE_FILE);
						if (strcmp(elem, "body") == 0) // Body
							acv::actor_templates[acv::actor_template_index].t_body = (btui16)GetAssetIDFromHandle(value, ASSET_TEXTURE_FILE);
						if (strcmp(elem, "larm") == 0) // Arm
							acv::actor_templates[acv::actor_template_index].t_arm = (btui16)GetAssetIDFromHandle(value, ASSET_TEXTURE_FILE);
						if (strcmp(elem, "lleg") == 0) // Leg
							acv::actor_templates[acv::actor_template_index].t_leg = (btui16)GetAssetIDFromHandle(value, ASSET_TEXTURE_FILE);

						// Get name of next operator
						AdvanceSpace(file);
						InterpretCommand(file, elem);
					}
					else if (strcmp(elem, "jpos") == 0) // Limb Position
					{
						InterpretCommand(file, elem);
						AdvanceSpace(file);
						if (strcmp(elem, "larm") == 0) // Arm
						{
							InterpretCommand(file, elem);
							AdvanceSpace(file);
							char value[256]; int value_size = 0;
							InterpretWord(file, value, &value_size); 
							
							if (strcmp(elem, "fw__") == 0) // Arm
								acv::actor_templates[acv::actor_template_index].jpos_arm_fw[0] = (btf32)atof(value);
							else if (strcmp(elem, "rt__") == 0) // Arm
								acv::actor_templates[acv::actor_template_index].jpos_arm_rt[0] = (btf32)atof(value);
							else if (strcmp(elem, "up__") == 0) // Arm
								acv::actor_templates[acv::actor_template_index].jpos_arm_up[0] = (btf32)atof(value);

							// Get name of next operator
							AdvanceSpace(file);
							InterpretCommand(file, elem);
						}
						if (strcmp(elem, "lleg") == 0) // Leg
						{
							InterpretCommand(file, elem);
							AdvanceSpace(file);
							char value[256]; int value_size = 0;
							InterpretWord(file, value, &value_size); 
							
							if (strcmp(elem, "fw__") == 0) // Arm
								acv::actor_templates[acv::actor_template_index].jpos_leg_fw[0] = (btf32)atof(value);
							else if (strcmp(elem, "rt__") == 0) // Arm
								acv::actor_templates[acv::actor_template_index].jpos_leg_rt[0] = (btf32)atof(value);
							else if (strcmp(elem, "up__") == 0) // Arm
								acv::actor_templates[acv::actor_template_index].jpos_leg_up[0] = (btf32)atof(value);

							// Get name of next operator
							AdvanceSpace(file);
							InterpretCommand(file, elem);
						}
					}
					else if (strcmp(elem, "leng") == 0) // Limb Length
					{
						InterpretCommand(file, elem);
						AdvanceSpace(file);
						char value[256]; int value_size = 0;
						InterpretWord(file, value, &value_size); 
						
						if (strcmp(elem, "larm") == 0) // Arm
							acv::actor_templates[acv::actor_template_index].leng_arm[0] = (btf32)atof(value);
						else if (strcmp(elem, "lleg") == 0) // Leg
							acv::actor_templates[acv::actor_template_index].leng_leg[0] = (btf32)atof(value);
						else if (strcmp(elem, "body") == 0) // Leg
							acv::actor_templates[acv::actor_template_index].leng_body[0] = (btf32)atof(value);

						// Get name of next operator
						AdvanceSpace(file);
						InterpretCommand(file, elem);
					}
				}
				++acv::actor_template_index;
			nextEntTElem:
				AdvanceSpace(file);
				goto getEntTElemName;
			}
			//*/

			//-------------------------------- END

			debug_output();
			fclose(file); // Close file
		}
		#undef ITEM_ITEM
		#undef ITEM_MISC
		#undef ITEM_APR
		#undef ITEM_WPN
		#undef ITEM_PTN
	}

	void LoadArchive(char* fn)
	{
		std::cout << "LOADING FILE...      [" << fn << "]" << std::endl;

		FILE* file = fopen(fn, "rb"); // Open file
		if (file != NULL)
		{
			fseek(file, 0, SEEK_SET); // Seek file beginning

			//-------------------------------- Asset part

			fread(&acv::assetCount, sizeof(btui32), 1, file); // Read filename count
			for (btui32 i = 0u; i < acv::assetCount; i++) // Write file types
			{
				fread(&acv::assets[i].type, sizeof(btui8), 1, file);
			}
			for (btui32 i = 0u; i < acv::assetCount; i++)
			{
				btui8 sl;
				fread(&sl, sizeof(btui8), 1, file); // Read string length
				fread(acv::assets[i].filename, sl, 1, file); // Read file string
			}

			//-------------------------------- Prop part

			fread(&acv::prop_index, sizeof(btui32), 1, file);
			fread(&acv::props, sizeof(acv::EnvProp), acv::prop_index, file);

			//-------------------------------- Spell part

			fread(&acv::spell_index, sizeof(btui32), 1, file);
			fread(&acv::spells, sizeof(acv::Spell), acv::spell_index, file);

			//-------------------------------- Projectile part

			fread(&acv::projectiles_index, sizeof(btui32), 1, file);
			fread(&acv::projectiles, sizeof(acv::ProjectileTemplate), acv::projectiles_index, file);

			//-------------------------------- Item part

			fread(&acv::item_index, sizeof(btui32), 1, file); // Read item count
			for (int i = 0; i < acv::item_index; i++)
			{
				fread(&acv::item_types[i], sizeof(btui8), 1, file);

				switch (acv::item_types[i])
				{
				default: // base (ITEM_ROOT)
					acv::items[i] = new acv::BaseItem();
					fread(acv::items[i], sizeof(acv::BaseItem), 1, file);
					break;
				case ITEM_TYPE_WPN_MELEE:
					acv::items[i] = new acv::BaseItemMel();
					fread(acv::items[i], sizeof(acv::BaseItemMel), 1, file);
					break;
				case ITEM_TYPE_WPN_MATCHGUN:
					acv::items[i] = new acv::BaseItemGun();
					fread(acv::items[i], sizeof(acv::BaseItemGun), 1, file);
					break;
				case ITEM_TYPE_WPN_MAGIC:
					acv::items[i] = new acv::BaseItemMgc();
					fread(acv::items[i], sizeof(acv::BaseItemMgc), 1, file);
					break;
				case ITEM_TYPE_CONS:
					acv::items[i] = new acv::BaseItemCon();
					fread(acv::items[i], sizeof(acv::BaseItemCon), 1, file);
					break;
				}
			}

			//-------------------------------- Entity Template part

			fread(&acv::actor_template_index, sizeof(btui32), 1, file);
			fread(&acv::actor_templates, sizeof(acv::ActorBase), acv::actor_template_index, file);

			fclose(file); // Close file

			std::cout << "LOAD COMPLETE!" << std::endl;
		}
	}

	void SaveArchive(char* fn)
	{
		std::cout << "SAVING FILE...       [" << fn << "]" << std::endl;

		FILE* file = fopen(fn, "wb"); // Open file
		if (file != NULL)
		{
			fseek(file, 0, SEEK_SET); // Seek file beginning

			//-------------------------------- Asset part

			btui32 count = acv::assetCount;
			fwrite(&count, sizeof(btui32), 1, file); // Write number of filenames

			for (btui32 i = 0u; i < acv::assetCount; i++) // Write file types
			{
				fwrite(&acv::assets[i].type, sizeof(btui8), 1, file);
			}
			for (btui32 i = 0u; i < acv::assetCount; i++) // Write file names
			{
				btui8 sl = (btui8)strlen(acv::assets[i].filename);
				fwrite(&sl, sizeof(btui8), 1, file);
				fputs(acv::assets[i].filename, file); // save file address
			}

			//-------------------------------- Prop part

			count = acv::prop_index;
			fwrite(&count, sizeof(btui32), 1, file);
			fwrite(&acv::props, sizeof(acv::EnvProp), count, file);

			//-------------------------------- Spell part

			count = acv::spell_index;
			fwrite(&count, sizeof(btui32), 1, file);
			fwrite(&acv::spells, sizeof(acv::Spell), count, file);

			//-------------------------------- Projectile part

			count = acv::projectiles_index;
			fwrite(&count, sizeof(btui32), 1, file);
			fwrite(&acv::projectiles, sizeof(acv::ProjectileTemplate), count, file);

			//-------------------------------- Item part

			count = acv::item_index;
			fwrite(&count, sizeof(btui32), 1, file);
			for (int i = 0; i < acv::item_index; i++)
			{
				switch (acv::item_types[i])
				{
				default: // base (ITEM_ROOT)
					fwrite(&acv::item_types[i], sizeof(btui8), 1, file);
					fwrite(acv::items[i], sizeof(acv::BaseItem), 1, file);
					break;
				case ITEM_TYPE_WPN_MELEE:
					fwrite(&acv::item_types[i], sizeof(btui8), 1, file);
					fwrite(acv::items[i], sizeof(acv::BaseItemMel), 1, file);
					break;
				case ITEM_TYPE_WPN_MATCHGUN:
					fwrite(&acv::item_types[i], sizeof(btui8), 1, file);
					fwrite(acv::items[i], sizeof(acv::BaseItemGun), 1, file);
					break;
				case ITEM_TYPE_WPN_MAGIC:
					fwrite(&acv::item_types[i], sizeof(btui8), 1, file);
					fwrite(acv::items[i], sizeof(acv::BaseItemMgc), 1, file);
					break;
				case ITEM_TYPE_CONS:
					fwrite(&acv::item_types[i], sizeof(btui8), 1, file);
					fwrite(acv::items[i], sizeof(acv::BaseItemCon), 1, file);
					break;
				}
			}

			//-------------------------------- Actor Template part

			count = acv::actor_template_index;
			fwrite(&count, sizeof(btui32), 1, file);
			fwrite(&acv::actor_templates, sizeof(acv::ActorBase), count, file);

			fclose(file); // Close file

			std::cout << "SAVE COMPLETE!" << std::endl;
		}
	}
}
