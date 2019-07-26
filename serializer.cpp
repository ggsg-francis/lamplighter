#include "serializer.h"
#include "serializer-graphics.h"
#include "resources-archive.h"

#include <fstream>
#include <vector>
#include <iostream>

namespace serializer
{
	btID get_id_from_fn(char* fn, archive::types::asset_type type)
	{
		btID retid;
		for (int i = 0; i < archive::fn_index_OBSOLETE; i++) // Check all currently registered filenames
			if (strcmp(fn, archive::filenames_OBSOLETE[i]) == 0) // If this filename is already registered
				return (btID)i; // Return ID of matching filename
		strcpy(archive::filenames_OBSOLETE[archive::fn_index_OBSOLETE], fn); // Else, this filename is new, so copy into it
		retid = archive::fn_index_OBSOLETE;
		archive::fn_types_OBSOLETE[archive::fn_index_OBSOLETE] = type; // Set file type, for loading
		archive::fn_index_OBSOLETE++;
		return retid; // Return ID of new filename
	}

	void debug_output()
	{
		std::cout << "#################################################" << std::endl;
		std::cout << "FILENAMES" << std::endl;
		for (int i = 0; i < archive::assetCount; i++)
		{
			std::cout << "FILE " << i << ": TYPE: " << archive::assets[i].type << " ADDR: " << archive::assets[i].filename << std::endl;
		}
		std::cout << "#################################################" << std::endl;
		std::cout << "ITEMS" << std::endl;
		for (int i = 0; i < archive::item_index; i++)
		{
			#define ITEMI ((archive::item*)archive::items[i])
			#define WPNI ((archive::item_wpon*)archive::items[i])
			#define PTNI ((archive::item_pton*)archive::items[i])

			std::cout << "-------------------------------------------------" << std::endl;
			std::cout << "ID             " << ITEMI->id << std::endl;
			//std::cout << "TYPE           " << ITEMI->type << std::endl;
			std::cout << "TYPE           " << archive::item_types[i] << std::endl;
			std::cout << "ICON           " << ITEMI->id_icon << std::endl;
			std::cout << "NAME           " << ITEMI->name << std::endl;
			std::cout << "WEIGHT         " << ITEMI->f_weight << std::endl;
			std::cout << "BASE VALUE     " << ITEMI->f_value_base << std::endl;
			std::cout << "RADIUS         " << ITEMI->f_radius << std::endl;
			std::cout << "MESH ID        " << ITEMI->id_mesh << std::endl;
			std::cout << "TEXTURE ID     " << ITEMI->id_tex << std::endl;
			//if (ITEMI->type == types::weapon)
			if (archive::item_types[i] == archive::types::weapon)
			{
				std::cout << "DAMAGE PIERCE  " << WPNI->f_dam_pierce << std::endl;
				std::cout << "DAMAGE SLASH   " << WPNI->f_dam_slash << std::endl;
				std::cout << "DAMAGE SLAM    " << WPNI->f_dam_slam << std::endl;
			}
			//if (ITEMI->type == types::potion)
			if (archive::item_types[i] == archive::types::potion)
			{
				std::cout << "EFFECT         " << PTNI->effect << std::endl;
				std::cout << "EFFECT VALUE   " << PTNI->effect_value << std::endl;
				std::cout << "EFFECT TIME    " << PTNI->effect_time << std::endl;
			}
			#undef ITEMI
			#undef WPNI
			#undef PTNI
		}
	}

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
						if (oper == '\n')
						{
							break;
						}
					}
				}
				else
				{
					//....................................... GET ITEM TYPE

					oper = fgetc(file); // Advance past equals sign
					fgets(&elem[0], 5, file); // Get element name

					//....................................... SET ITEM TYPE

					archive::types::asset_type type;
					char cdest[FN_SIZE];
					char csrca[FN_SIZE];
					char csrcb[FN_SIZE];

					if (strcmp(elem, "btt_") == 0) { // Texture
						type = archive::types::btt;
					}
					if (strcmp(elem, "btm_") == 0) { // Mesh
						type = archive::types::btm;
					}
					if (strcmp(elem, "btmb") == 0) { // Mesh Blend
						type = archive::types::btmb;
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
						fseek(file, start, SEEK_SET);
						char* value = (char*)malloc(end - start); // Allocate space for value string
						fgets(&value[0], end - start - 1, file); // Get element name

						if (strcmp(elem, "dest") == 0) // Destination string
							strcpy(cdest, value);
						if (strcmp(elem, "srca") == 0) // Source string A
							strcpy(csrca, value);
						if (strcmp(elem, "srcb") == 0) // Source string B
							strcpy(csrcb, value);

						free(value); // Free content string

						// Get name of next element
						oper = fgetc(file); //  Advance past line break
						fgets(&elem[0], 5, file); // Get element name
					}

					//....................................... CONVERT AND SAVE ASSETS

					if (type == archive::types::btt)
					{
						std::cout << "GENERATING TEXTURE   [" << cdest << "]" << std::endl;
						serializer_graphics::ConvertTex(csrca, cdest); // Create source file
						std::cout << "SETTING INDEX        [" << index << "]" << std::endl;
						strcpy(archive::assets[index].filename, cdest); // Copy filename into archive
						archive::assets[index].type = archive::types::btt;
						std::cout << "---------------------" << std::endl;
					}
					else if (type == archive::types::btm)
					{
						std::cout << "GENERATING MESH      [" << cdest << "]" << std::endl;
						serializer_graphics::ConvertMesh(csrca, cdest); // Create source file
						std::cout << "SETTING INDEX        [" << index << "]" << std::endl;
						strcpy(archive::assets[index].filename, cdest); // Copy filename into archive
						archive::assets[index].type = archive::types::btm;
						std::cout << "---------------------" << std::endl;
					}
					else if (type == archive::types::btmb)
					{
						std::cout << "GENERATING MESHBLEND [" << cdest << "]" << std::endl;
						serializer_graphics::ConvertMB(csrca, csrcb, cdest); // Create source file
						std::cout << "SETTING INDEX        [" << index << "]" << std::endl;
						strcpy(archive::assets[index].filename, cdest); // Copy filename into archive
						archive::assets[index].type = archive::types::btmb;
						std::cout << "---------------------" << std::endl;
					}

					++index; // We are done with this index

					oper = fgetc(file); //  Advance past line break
				} // End if not comment
				fgets(&elem[0], 5, file); // Get next element name
			}
			archive::assetCount = index; // Conveniently, the index is left equaling the total number of assets
			std::cout << "FINAL ASSET COUNT    [" << archive::assetCount << "]" << std::endl;
			fclose(file); // Close file
		}
	}

	void interpret_archive_src(char* fn)
	{
		void* item;

		#define ITEM_ITEM ((archive::item*)item)
		#define ITEM_MISC ((archive::item_misc*)item)
		#define ITEM_APR ((archive::item_aprl*)item)
		#define ITEM_WPN ((archive::item_wpon*)item)
		#define ITEM_PTN ((archive::item_pton*)item)

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
			while (strcmp(elem, "item") == 0) // is it a new item?
			{
				//....................................... GET ITEM TYPE

				oper = fgetc(file); // Advance past equals sign
				fgets(&elem[0], 5, file); // Get element name

				//....................................... SET ITEM TYPE

				if (strcmp(elem, "misc") == 0) {
					archive::items[archive::item_index] = new archive::item_misc();
					archive::item_types[archive::item_index] = archive::types::misc;
				}
				else if (strcmp(elem, "aprl") == 0) {
					archive::items[archive::item_index] = new archive::item_aprl();
					archive::item_types[archive::item_index] = archive::types::apparel;
				}
				else if (strcmp(elem, "wpon") == 0) {
					archive::items[archive::item_index] = new archive::item_wpon();
					archive::item_types[archive::item_index] = archive::types::weapon;
				}
				else if (strcmp(elem, "pton") == 0) {
					archive::items[archive::item_index] = new archive::item_pton();
					archive::item_types[archive::item_index] = archive::types::potion;
				}

				//....................................... ASSIGN ITEM ID

				item = archive::items[archive::item_index];
				ITEM_ITEM->id = archive::item_index;
				archive::item_index++;

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
					fseek(file, start, SEEK_SET);
					char* value = (char*)malloc(end - start); // Allocate space for value string
					fgets(&value[0], end - start - 1, file); // Get element name

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
						ITEM_ITEM->id_icon = (btID)atoi(value);
					else if (strcmp(elem, "name") == 0) // Name
						memcpy(ITEM_ITEM->name, value, end - start);
					else if (strcmp(elem, "wght") == 0) // Carry weight
						ITEM_ITEM->f_weight = atof(value);
					else if (strcmp(elem, "muns") == 0) // Money base value
						ITEM_ITEM->f_value_base = atof(value);
					else if (strcmp(elem, "radi") == 0) // Radius when placed
						ITEM_ITEM->f_radius = atof(value);
					else if (strcmp(elem, "txtr") == 0) // Texture
						ITEM_ITEM->id_tex = get_id_from_fn(value, archive::types::btt);
					else if (strcmp(elem, "mesh") == 0) // Mesh
						ITEM_ITEM->id_mesh = get_id_from_fn(value, archive::types::btm);
					else if (strcmp(elem, "mshb") == 0) // Mesh
						ITEM_ITEM->id_mesh = get_id_from_fn(value, archive::types::btmb);
					// Weapon value
					else if (strcmp(elem, "held") == 0) // Damage pierce
					{
						//ITEM_WPN->damage_pierce = atof(value);
					}
					else if (strcmp(elem, "dmg1") == 0) // Damage pierce
						ITEM_WPN->f_dam_pierce = atof(value);
					else if (strcmp(elem, "dmg2") == 0) // Damage slash
						ITEM_WPN->f_dam_slash = atof(value);
					else if (strcmp(elem, "dmg3") == 0) // Damage slam
						ITEM_WPN->f_dam_slam = atof(value);
					// Apparel value
					else if (strcmp(elem, "def1") == 0) // Defend pierce damage
						ITEM_APR->block_pierce = atof(value);
					else if (strcmp(elem, "def2") == 0) // Defend slash damage
						ITEM_APR->block_slice = atof(value);
					else if (strcmp(elem, "def3") == 0) // Defend slam damage
						ITEM_APR->block_slam = atof(value);

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

	void load_archive(char* fn)
	{
		std::cout << "LOADING FILE...      [" << fn << "]" << std::endl;

		FILE* file = fopen(fn, "rb"); // Open file
		if (file != NULL)
		{
			fseek(file, 0, SEEK_SET); // Seek file beginning

			//id_t count;
			//fread(&count, sizeof(id_t), 1, file); // Read filename count
			fread(&archive::assetCount, sizeof(btID), 1, file); // Read filename count
			//archive::fn_index = count; // Temp

			for (int i = 0; i < archive::assetCount; i++) // Write file types
			{
				fread(&archive::assets[i].type, sizeof(btui8), 1, file);
			}
			for (int i = 0; i < archive::assetCount; i++)
			{
				btui8 sl;
				fread(&sl, sizeof(btui8), 1, file); // Read string length
				fread(archive::assets[i].filename, sl, 1, file); // Read file string
			}

			///*
			fread(&archive::item_index, sizeof(btID), 1, file); // Read item count
			//archive::item_index = count; // Temp

			for (int i = 0; i < archive::item_index; i++)
			{
				fread(&archive::item_types[i], sizeof(btui8), 1, file);

				if (archive::item_types[i] == archive::types::weapon)
				{
					archive::items[i] = new archive::item_wpon();
					fread(archive::items[i], sizeof(archive::item_wpon), 1, file);
				}
				else if (archive::item_types[i] == archive::types::potion)
				{
					archive::items[i] = new archive::item_pton();
					fread(archive::items[i], sizeof(archive::item_pton), 1, file);
				}
			}
			//*/
			debug_output();
			fclose(file); // Close file

			std::cout << "LOAD COMPLETE!" << std::endl;
		}
	}

	void save_archive(char* fn)
	{
		std::cout << "SAVING FILE...       [" << fn << "]" << std::endl;

		FILE* file = fopen(fn, "wb"); // Open file
		if (file != NULL)
		{
			fseek(file, 0, SEEK_SET); // Seek file beginning

			// Asset part

			btID count = archive::assetCount;
			fwrite(&count, sizeof(btID), 1, file); // Write number of filenames

			for (int i = 0; i < archive::assetCount; i++) // Write file types
			{
				fwrite(&archive::assets[i].type, sizeof(btui8), 1, file);
			}
			for (int i = 0; i < archive::assetCount; i++) // Write file names
			{
				btui8 sl = (btui8)strlen(archive::assets[i].filename);
				fwrite(&sl, sizeof(btui8), 1, file);
				fputs(archive::assets[i].filename, file); // save file address
			}

			// Item part

			///*
			count = archive::item_index;
			fwrite(&count, sizeof(btID), 1, file);

			for (int i = 0; i < archive::item_index; i++)
			{
				if (archive::item_types[i] == archive::types::weapon)
				{
					fwrite(&archive::item_types[i], sizeof(btui8), 1, file);
					fwrite(archive::items[i], sizeof(archive::item_wpon), 1, file);
				}
				if (archive::item_types[i] == archive::types::potion)
				{
					fwrite(&archive::item_types[i], sizeof(btui8), 1, file);
					fwrite(archive::items[i], sizeof(archive::item_pton), 1, file);
				}
			}//*/

			fclose(file); // Close file

			std::cout << "SAVE COMPLETE!" << std::endl;
		}

		/*
		FILE* file = fopen(fn, "wb"); // Open file
		if (file != NULL)
		{
			fseek(file, 0, SEEK_SET); // Seek file beginning

			btID count = archive::fn_index_OBSOLETE;
			fwrite(&count, sizeof(btID), 1, file); // Write number of filenames

			for (int i = 0; i < archive::fn_index_OBSOLETE; i++) // Write file types
			{
				fwrite(&archive::fn_types_OBSOLETE[i], sizeof(btui8), 1, file);
			}
			for (int i = 0; i < archive::fn_index_OBSOLETE; i++) // Write file names
			{
				btui8 sl = (btui8)strlen(archive::filenames_OBSOLETE[i]);
				fwrite(&sl, sizeof(btui8), 1, file);
				fputs(archive::filenames_OBSOLETE[i], file); // save file address
			}

			count = archive::item_index;
			fwrite(&count, sizeof(btID), 1, file);

			for (int i = 0; i < archive::item_index; i++)
			{
				if (archive::item_types[i] == archive::types::weapon)
				{
					fwrite(&archive::item_types[i], sizeof(btui8), 1, file);
					fwrite(archive::items[i], sizeof(archive::item_wpon), 1, file);
				}
				if (archive::item_types[i] == archive::types::potion)
				{
					fwrite(&archive::item_types[i], sizeof(btui8), 1, file);
					fwrite(archive::items[i], sizeof(archive::item_pton), 1, file);
				}
			}

			fclose(file); // Close file
		}//*/
	}
}