#include "archive.hpp"
#include "archive_read_write.h"
#include "memory.hpp"

// temp? for generating skin texture
#include "maths.hpp"

#ifndef DEF_ARCHIVER
namespace res
{
	graphics::Texture& GetT(btui32 index)
	{
		if (index < acv::assetCount && acv::assets[index].type == ASSET_TEXTURE_FILE)
			return *(graphics::Texture*)acv::assets[index].asset;
		return *(graphics::Texture*)acv::assets[DEFAULT_TEXTURE].asset;
	}

	graphics::Mesh& GetM(btui32 index)
	{
		if (index < acv::assetCount && acv::assets[index].type == ASSET_MESH_FILE)
			return *(graphics::Mesh*)acv::assets[index].asset;
		return *(graphics::Mesh*)acv::assets[DEFAULT_MESH].asset;
	}

	graphics::MeshBlend& GetMB(btui32 index)
	{
		if (index < acv::assetCount && acv::assets[index].type == ASSET_MESHBLEND_FILE)
			return *(graphics::MeshBlend*)acv::assets[index].asset;
		return *(graphics::MeshBlend*)acv::assets[DEFAULT_MESHBLEND].asset;
	}

	graphics::MeshDeform& GetMD(btui32 index)
	{
		if (index < acv::assetCount && acv::assets[index].type == ASSET_MESHDEFORM_FILE)
			return *(graphics::MeshDeform*)acv::assets[index].asset;
		return *(graphics::MeshDeform*)acv::assets[DEFAULT_MESHDEFORM].asset;
	}

	bool IsTexture(btui32 index)
	{
		return acv::assets[index].type == ASSET_TEXTURE_FILE;
	}

	bool IsMesh(btui32 index)
	{
		return acv::assets[index].type == ASSET_MESH_FILE;
	}

	bool IsMeshBlend(btui32 index)
	{
		return acv::assets[index].type == ASSET_MESHBLEND_FILE;
	}

	graphics::ModifiableTexture skin_t[4];

	void Init()
	{
		serializer::LoadArchive("res/archive.UwUa");

		//for now, just load everything
		for (btui32 i = 0u; i < acv::assetCount; i++)
		{
			acv::LoadAsset(i);
		}
	}
	void End()
	{
		acv::ClearMemory();
	}
}
#endif

namespace acv
{
	// Contents

	BaseItem* items[ITEMS_COUNT]{ nullptr };
	ItemType item_types[ITEMS_COUNT];
	btui32 item_index = 0;

	EnvProp props[PROPS_COUNT];
	btui32 prop_index = 0;

	Spell spells[SPELL_COUNT];
	btui32 spell_index = 0;

	ProjectileTemplate projectiles[PROJECTILE_TEMPLATE_COUNT];
	btui32 projectiles_index;

	ActorBase actor_templates[ACTORBASE_COUNT];
	btui32 actor_template_index = 0;

	// Assets

	archive_asset assets[FN_COUNT];
	btui32 assetCount;

	#ifndef DEF_ARCHIVER

	void LoadAsset(btui32 i)
	{
		if (!assets[i].loaded)
		{
			switch (assets[i].type)
			{
			case ASSET_TEXTURE_FILE:
				assets[i].asset = new graphics::Texture;
				((graphics::Texture*)assets[i].asset)->LoadFile(assets[i].filename);
				assets[i].loaded = true; 
				return;
			case ASSET_MESH_FILE:
				assets[i].asset = new graphics::Mesh;
				((graphics::Mesh*)assets[i].asset)->LoadFile(assets[i].filename, false);
				assets[i].loaded = true;
				return;
			case ASSET_MESHBLEND_FILE:
				assets[i].asset = new graphics::MeshBlend;
				((graphics::MeshBlend*)assets[i].asset)->LoadFile(assets[i].filename);
				assets[i].loaded = true;
				return;
			case ASSET_MESHDEFORM_FILE:
				assets[i].asset = new graphics::MeshDeform;
				((graphics::MeshDeform*)assets[i].asset)->LoadFile(assets[i].filename);
				assets[i].loaded = true;
				return;
			};
		}
	}

	void UnloadAsset(btui32 i)
	{
		// Old (seems to leak)
		if (assets[i].asset != nullptr && assets[i].loaded)
		{
			switch (assets[i].type)
			{
			case ASSET_TEXTURE_FILE:
				return;
			case ASSET_MESH_FILE:
				((graphics::Mesh*)assets[i].asset)->Unload();
				return;
			case ASSET_MESHBLEND_FILE:
				return;
			case ASSET_MESHDEFORM_FILE:
				return;
			};
			assets[i].loaded = false;
			delete assets[i].asset;
		}
	};

	#endif // DEF_ARCHIVER

	void ClearMemory()
	{
		for (int i = 0; i < acv::item_index; i++)
		{
			delete(acv::items[i]);
		}
		#ifndef DEF_ARCHIVER
		for (int i = 0; i < FN_COUNT; i++)
		{
			UnloadAsset(i);
		}
		#endif // DEF_ARCHIVER
	}
}