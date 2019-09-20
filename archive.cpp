#include "archive.hpp"
#include "serializer.h"
#include "memory.hpp"

#ifndef DEF_ARCHIVER
namespace res
{
	graphics::Texture& GetTexture(btui32 index)
	{
		if (archive::assets[index].type == archive::types::texturefile)
			return *(graphics::Texture*)archive::assets[index].asset;
		else return *(graphics::Texture*)archive::assets[DEFAULT_TEXTURE].asset;
	}

	graphics::Mesh& GetMesh(btui32 index)
	{
		if (archive::assets[index].type == archive::types::meshfile)
			return *(graphics::Mesh*)archive::assets[index].asset;
		else return *(graphics::Mesh*)archive::assets[DEFAULT_MESH].asset;
	}

	graphics::MeshBlend& GetMeshBlend(btui32 index)
	{
		if (archive::assets[index].type == archive::types::meshblendfile)
			return *(graphics::MeshBlend*)archive::assets[index].asset;
		else return *(graphics::MeshBlend*)archive::assets[DEFAULT_MESHBLEND].asset;
	}

	bool IsTexture(btui32 index)
	{
		return archive::assets[index].type == archive::types::texturefile;
	}

	bool IsMesh(btui32 index)
	{
		return archive::assets[index].type == archive::types::meshfile;
	}

	bool IsMeshBlend(btui32 index)
	{
		return archive::assets[index].type == archive::types::meshblendfile;
	}

	void Init()
	{
		serializer::load_archive("res/archive.UwUa");

		//for now, just load everything
		for (btui32 i = 0u; i < archive::assetCount; i++)
		{
			archive::LoadAsset(i);
		}
	}
	void End()
	{
		archive::ClearMemory();
	}
}
#endif

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