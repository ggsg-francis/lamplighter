#include "resources-archive.h"
#ifndef DEF_ARCHIVER
#include "graphics.hpp"
#endif // DEF_ARCHIVER

namespace archive
{
	//files
	char filenames_OBSOLETE[FN_SIZE][FN_COUNT];
	types::asset_type fn_types_OBSOLETE[FN_COUNT];
	btID fn_index_OBSOLETE = 0;
	//items
	void* items[ITEMS_COUNT]{ nullptr };
	types::item_type item_types[ITEMS_COUNT];
	btID item_index = 0;

	// NEW STUFF !!!!!!!!!!!!!!!!!!!!!

	archive_asset assets[FN_COUNT];
	btui32 assetCount;

	#ifndef DEF_ARCHIVER

	void LoadAsset(btui32 i)
	{
		if (!assets[i].loaded)
		{
			switch (assets[i].type)
			{
			case types::btt:
				assets[i].asset = new Texture;
				((Texture*)assets[i].asset)->id = graphics::TxLoadBTT(assets[i].filename);
				return;
			case types::btm:
				assets[i].asset = new graphics::NewMesh;
				((graphics::NewMesh*)assets[i].asset)->LoadFile(assets[i].filename);
				return;
			case types::btmb:
				assets[i].asset = new ModelBlend;
				((ModelBlend*)assets[i].asset)->LoadFile(assets[i].filename);
				return;
			};
		}
	}

	void UnloadAsset(btui32 i)
	{
		if (assets[i].asset != nullptr && assets[i].loaded)
			delete assets[i].asset;
	};

	#endif // DEF_ARCHIVER

	void clear_memory()
	{
		for (int i = 0; i < archive::item_index; i++)
		{
			delete(archive::items[i]);
		}
		#ifndef DEF_ARCHIVER
		for (int i = 0; i < archive::item_index; i++)
		{
			UnloadAsset(i);
		}
		#endif // DEF_ARCHIVER
	}
}