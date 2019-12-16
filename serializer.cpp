#include "serializer.h"
#ifdef DEF_ARCHIVER
#include "graphics_convert.h"
#endif
#include "graphics.hpp"
#include "archive.hpp"

#include <fstream>
#include <vector>
#include <iostream>

namespace serializer
{
	btID GetIDFromHandle(char* handle, acv::types::AssetType type)
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

	void debug_output()
	{
		std::cout << "__________________________________________" << std::endl;
		std::cout << "FILENAMES" << std::endl;
		for (btui32 i = 0ui32; i < acv::assetCount; i++)
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
		std::cout << "ITEMS" << std::endl;
		for (int i = 0; i < acv::item_index; i++)
		{
			#define ITEMI ((acv::item*)acv::items[i])
			#define WMELI ((acv::item_w_melee*)acv::items[i])
			#define CONSI ((acv::item_pton*)acv::items[i])

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

			if (acv::item_types[i] == acv::types::ITEM_WPN_MELEE)
			{
				std::cout << "DAMAGE PIERCE        " << WMELI->f_dam_pierce << std::endl;
				std::cout << "DAMAGE SLASH         " << WMELI->f_dam_slash << std::endl;
				std::cout << "DAMAGE SLAM          " << WMELI->f_dam_slam << std::endl;
			}
			if (acv::item_types[i] == acv::types::ITEM_CONS)
			{
				std::cout << "EFFECT               " << CONSI->effect << std::endl;
				std::cout << "EFFECT VALUE         " << CONSI->effect_value << std::endl;
				std::cout << "EFFECT TIME          " << CONSI->effect_time << std::endl;
			}
			#undef ITEMI
			#undef WPNI
			#undef PTNI
		}
	}

	#ifdef DEF_ARCHIVER
	void convert_files_src(char* fn)
	{
		std::cout <<
			"  ||||||||                      ||||||||" << std::endl <<
			"||||    ||||                  ||||    ||||" << std::endl <<
			"||||    ||||  ||||      ||||  ||||    ||||" << std::endl <<
			"||||    ||||  ||||  ||  ||||  ||||    ||||" << std::endl <<
			"  ||||||||      ||||||||||      ||||||||" << std::endl;

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

					acv::types::AssetType type;
					graphics::TextureFilterMode t_filter_mode = graphics::eLINEAR;
					graphics::TextureEdgeMode t_edge_mode = graphics::eREPEAT;
					char cdest[FN_SIZE];
					char csrca[FN_SIZE];
					char csrcb[FN_SIZE];

					if (strcmp(elem, "txtr") == 0) { // Texture
						type = acv::types::ASSET_TEXTURE_FILE;
					}
					else if (strcmp(elem, "mesh") == 0) { // Mesh
						type = acv::types::ASSET_MESH_FILE;
					}
					else if (strcmp(elem, "mshb") == 0) { // Mesh Blend
						type = acv::types::ASSET_MESHBLEND_FILE;
					}
					else if (strcmp(elem, "mshd") == 0) { // Mesh Deform
						type = acv::types::ASSET_MESHDEFORM_FILE;
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

					if (type == acv::types::ASSET_TEXTURE_FILE)
					{
						std::cout << "CONV TEXTURE         [" << cdest << "]" << std::endl;
						graphics::ConvertTex(csrca, cdest, t_filter_mode, t_edge_mode); // Create source file
						std::cout << "SETTING INDEX        [" << index << "]" << std::endl;
						strcpy(acv::assets[index].filename, cdest); // Copy filename into archive
						acv::assets[index].type = acv::types::ASSET_TEXTURE_FILE;
						std::cout << "---------------------" << std::endl;
					}
					else if (type == acv::types::ASSET_MESH_FILE)
					{
						std::cout << "CONV MESH            [" << cdest << "]" << std::endl;
						graphics::ConvertMesh(csrca, cdest); // Create source file
						std::cout << "SETTING INDEX        [" << index << "]" << std::endl;
						strcpy(acv::assets[index].filename, cdest); // Copy filename into archive
						acv::assets[index].type = acv::types::ASSET_MESH_FILE;
						std::cout << "---------------------" << std::endl;
					}
					else if (type == acv::types::ASSET_MESHBLEND_FILE)
					{
						std::cout << "CONV MESHBLEND       [" << cdest << "]" << std::endl;
						graphics::ConvertMB(csrca, csrcb, cdest); // Create source file
						std::cout << "SETTING INDEX        [" << index << "]" << std::endl;
						strcpy(acv::assets[index].filename, cdest); // Copy filename into archive
						acv::assets[index].type = acv::types::ASSET_MESHBLEND_FILE;
						std::cout << "---------------------" << std::endl;
					}
					else if (type == acv::types::ASSET_MESHDEFORM_FILE)
					{
						std::cout << "CONV MESHDEFORM      [" << cdest << "]" << std::endl;
						graphics::ConvertMD(csrca, cdest); // Create source file
						std::cout << "SETTING INDEX        [" << index << "]" << std::endl;
						strcpy(acv::assets[index].filename, cdest); // Copy filename into archive
						acv::assets[index].type = acv::types::ASSET_MESHDEFORM_FILE;
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

	void interpret_archive_src(char* fn)
	{
		void* item;

		#define ITEM_ITEM ((acv::item*)item)
		#define ITEM_MISC ((acv::item_misc*)item)
		#define ITEM_APR ((acv::item_aprl*)item)
		#define ITEM_W_MELEE ((acv::item_w_melee*)item)
		#define ITEM_PTN ((acv::item_pton*)item)

		FILE* file = fopen(fn, "r"); // Open file
		if (file != NULL)
		{
			fseek(file, 0, SEEK_SET); // Seek file beginning

			//variables for use
			//int id_current = 0;

			//read property
			char elem[5];
			char oper;
			fgets(&elem[0], 5, file); // Get element name
			while (strcmp(elem, "prop") == 0) // is it a new item?
			{
				//....................................... GET ITEM TYPE

				oper = fgetc(file); // Advance past equals sign
				fgets(&elem[0], 5, file); // Get element name

				//....................................... SET ITEM TYPE

				// If it's an environment prop
				if (strcmp(elem, "envp") == 0) ++acv::prop_index;

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

					if (strcmp(elem, "srct") == 0) // Texture
						acv::props[acv::prop_index].idTxtr = GetIDFromHandle(value, acv::types::ASSET_TEXTURE_FILE);
					else if (strcmp(elem, "srcm") == 0) // Mesh
						acv::props[acv::prop_index].idMesh = GetIDFromHandle(value, acv::types::ASSET_MESH_FILE);

					//++acv::prop_index; // Iterate prop count

					free(value); // Free content string

					//get name of next operator
					oper = fgetc(file); //  Advance past line break
					fgets(&elem[0], 5, file); // Get element name
				}
				oper = fgetc(file); //  Advance past line break
				fgets(&elem[0], 5, file); // Get element name
			}
			while (strcmp(elem, "item") == 0) // is it a new item?
			{
				//....................................... GET ITEM TYPE

				oper = fgetc(file); // Advance past equals sign
				fgets(&elem[0], 5, file); // Get element name

				//....................................... SET ITEM TYPE

				if (elem[0] == 'w') // If it's a weapon
				{
					if (strcmp(elem, "wmel") == 0) {
						acv::items[acv::item_index] = new acv::item_w_melee();
						acv::item_types[acv::item_index] = acv::types::ITEM_WPN_MELEE;
					}
					else if (strcmp(elem, "wgun") == 0) {
						acv::items[acv::item_index] = new acv::item_w_gun();
						acv::item_types[acv::item_index] = acv::types::ITEM_WPN_MATCHGUN;
					}
					else if (strcmp(elem, "wmgc") == 0) {
						acv::items[acv::item_index] = new acv::item_w_magic();
						acv::item_types[acv::item_index] = acv::types::ITEM_WPN_MAGIC;
					}
				}
				else
				{
					if (strcmp(elem, "misc") == 0) {
						acv::items[acv::item_index] = new acv::item();
						acv::item_types[acv::item_index] = acv::types::ITEM_ROOT;
					}
					else if (strcmp(elem, "aprl") == 0) {
						acv::items[acv::item_index] = new acv::item_aprl();
						acv::item_types[acv::item_index] = acv::types::ITEM_EQUIP;
					}
					else if (strcmp(elem, "cons") == 0) {
						acv::items[acv::item_index] = new acv::item_pton();
						acv::item_types[acv::item_index] = acv::types::ITEM_CONS;
					}
				}

				//....................................... ASSIGN ITEM ID

				item = acv::items[acv::item_index];
				ITEM_ITEM->id = acv::item_index;
				acv::item_index++;

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

					// Any item stats
					if (strcmp(elem, "type") == 0) // What type of item is it?
					{
						if (strcmp(value, "weapon") == 0)
						{
							int hello = 0;
						}
						//ITEM_ITEM->id_icon = (id_t)atoi(value);
					}
					else if (strcmp(elem, "icon") == 0) // Icon (change to string?)
						//ITEM_ITEM->id_icon = (btID)atoi(value);
						ITEM_ITEM->id_icon = GetIDFromHandle(value, acv::types::ASSET_TEXTURE_FILE);
					else if (strcmp(elem, "name") == 0) // Name
						memcpy(ITEM_ITEM->name, value, end - start);
					else if (strcmp(elem, "wght") == 0) // Carry weight
						ITEM_ITEM->f_weight = (btf32)atof(value);
					else if (strcmp(elem, "muns") == 0) // Money base value
						ITEM_ITEM->f_value_base = (btui32)atoi(value);
					else if (strcmp(elem, "radi") == 0) // Radius when placed
						ITEM_ITEM->f_radius = (btf32)atof(value);
					else if (strcmp(elem, "mdlh") == 0) // Model height when placed
						ITEM_ITEM->f_model_height = (btf32)atof(value);
					else if (strcmp(elem, "srct") == 0) // Texture
						ITEM_ITEM->id_tex = GetIDFromHandle(value, acv::types::ASSET_TEXTURE_FILE);
					else if (strcmp(elem, "srcm") == 0) // Mesh
						ITEM_ITEM->id_mesh = GetIDFromHandle(value, acv::types::ASSET_MESH_FILE);
					else if (strcmp(elem, "scmb") == 0) // Mesh
						ITEM_ITEM->id_mesh = GetIDFromHandle(value, acv::types::ASSET_MESHBLEND_FILE);
					// Weapon value
					else if (strcmp(elem, "held") == 0) // Damage pierce
					{
						//ITEM_WPN->damage_pierce = atof(value);
					}
					else if (strcmp(elem, "dmg1") == 0) // Damage pierce
						ITEM_W_MELEE->f_dam_pierce = (float)atof(value);
					else if (strcmp(elem, "dmg2") == 0) // Damage slash
						ITEM_W_MELEE->f_dam_slash = (float)atof(value);
					else if (strcmp(elem, "dmg3") == 0) // Damage slam
						ITEM_W_MELEE->f_dam_slam = (float)atof(value);
					// Apparel value
					else if (strcmp(elem, "def1") == 0) // Defend pierce damage
						ITEM_APR->block_pierce = (float)atof(value);
					else if (strcmp(elem, "def2") == 0) // Defend slash damage
						ITEM_APR->block_slice = (float)atof(value);
					else if (strcmp(elem, "def3") == 0) // Defend slam damage
						ITEM_APR->block_slam = (float)atof(value);

					free(value); // Free content string

					//get name of next operator
					oper = fgetc(file); //  Advance past line break
					fgets(&elem[0], 5, file); // Get element name
				}
				oper = fgetc(file); //  Advance past line break
				fgets(&elem[0], 5, file); // Get element name
			}
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

			//id_t count;
			//fread(&count, sizeof(id_t), 1, file); // Read filename count
			fread(&acv::assetCount, sizeof(btui32), 1, file); // Read filename count
			//acv::fn_index = count; // Temp

			for (btui32 i = 0ui32; i < acv::assetCount; i++) // Write file types
			{
				fread(&acv::assets[i].type, sizeof(btui8), 1, file);
			}
			for (btui32 i = 0ui32; i < acv::assetCount; i++)
			{
				btui8 sl;
				fread(&sl, sizeof(btui8), 1, file); // Read string length
				fread(acv::assets[i].filename, sl, 1, file); // Read file string
			}

			//-------------------------------- Prop part

			// Read count
			fread(&acv::prop_index, sizeof(btui32), 1, file);
			// Read props
			fread(&acv::props, sizeof(acv::EnvProp), acv::prop_index, file);

			//-------------------------------- Item part

			///*
			fread(&acv::item_index, sizeof(btui32), 1, file); // Read item count
			//acv::item_index = count; // Temp

			for (int i = 0; i < acv::item_index; i++)
			{
				fread(&acv::item_types[i], sizeof(btui8), 1, file);

				switch (acv::item_types[i])
				{
				default: // base (ITEM_ROOT)
					acv::items[i] = new acv::item();
					fread(acv::items[i], sizeof(acv::item), 1, file);
					break;
				case acv::types::ITEM_WPN_MELEE:
					acv::items[i] = new acv::item_w_melee();
					fread(acv::items[i], sizeof(acv::item_w_melee), 1, file);
					break;
				case acv::types::ITEM_WPN_MATCHGUN:
					acv::items[i] = new acv::item_w_gun();
					fread(acv::items[i], sizeof(acv::item_w_gun), 1, file);
					break;
				case acv::types::ITEM_WPN_MAGIC:
					acv::items[i] = new acv::item_w_magic();
					fread(acv::items[i], sizeof(acv::item_w_magic), 1, file);
					break;
				case acv::types::ITEM_CONS:
					acv::items[i] = new acv::item_pton();
					fread(acv::items[i], sizeof(acv::item_pton), 1, file);
					break;
				}
			}
			//*/
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

			for (btui32 i = 0ui32; i < acv::assetCount; i++) // Write file types
			{
				fwrite(&acv::assets[i].type, sizeof(btui8), 1, file);
			}
			for (btui32 i = 0ui32; i < acv::assetCount; i++) // Write file names
			{
				btui8 sl = (btui8)strlen(acv::assets[i].filename);
				fwrite(&sl, sizeof(btui8), 1, file);
				fputs(acv::assets[i].filename, file); // save file address
			}

			//-------------------------------- Prop part

			count = acv::prop_index;
			fwrite(&count, sizeof(btui32), 1, file);

			fwrite(&acv::props, sizeof(acv::EnvProp), count, file);

			//-------------------------------- Item part

			count = acv::item_index;
			fwrite(&count, sizeof(btui32), 1, file);

			for (int i = 0; i < acv::item_index; i++)
			{
				switch (acv::item_types[i])
				{
				default: // base (ITEM_ROOT)
					fwrite(&acv::item_types[i], sizeof(btui8), 1, file);
					fwrite(acv::items[i], sizeof(acv::item), 1, file);
					break;
				case acv::types::ITEM_WPN_MELEE:
					fwrite(&acv::item_types[i], sizeof(btui8), 1, file);
					fwrite(acv::items[i], sizeof(acv::item_w_melee), 1, file);
					break;
				case acv::types::ITEM_WPN_MATCHGUN:
					fwrite(&acv::item_types[i], sizeof(btui8), 1, file);
					fwrite(acv::items[i], sizeof(acv::item_w_gun), 1, file);
					break;
				case acv::types::ITEM_WPN_MAGIC:
					fwrite(&acv::item_types[i], sizeof(btui8), 1, file);
					fwrite(acv::items[i], sizeof(acv::item_w_magic), 1, file);
					break;
				case acv::types::ITEM_CONS:
					fwrite(&acv::item_types[i], sizeof(btui8), 1, file);
					fwrite(acv::items[i], sizeof(acv::item_pton), 1, file);
					break;
				}
			}

			fclose(file); // Close file

			std::cout << "SAVE COMPLETE!" << std::endl;
		}
	}
}