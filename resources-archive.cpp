#include "resources-archive.h"
#ifndef DEF_ARCHIVER
#include "graphics.hpp"
#endif // DEF_ARCHIVER

namespace archive
{
	//items
	item* items[ITEMS_COUNT]{ nullptr };
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
			case types::texturefile:
				assets[i].asset = new graphics::Texture;
				((graphics::Texture*)assets[i].asset)->LoadFile(assets[i].filename);
				return;
			case types::meshfile:
				assets[i].asset = new graphics::Mesh;
				((graphics::Mesh*)assets[i].asset)->LoadFile(assets[i].filename);
				return;
			case types::meshblendfile:
				assets[i].asset = new graphics::MeshBlend;
				((graphics::MeshBlend*)assets[i].asset)->LoadFile(assets[i].filename);
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

	void ClearMemory()
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