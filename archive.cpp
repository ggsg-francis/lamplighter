#include "archive.hpp"
#include "archive_read_write.h"
#include "memory.hpp"

// temp? for generating skin texture
#include "maths.hpp"

#if defined DEF_ARCHIVE_IN_CODE && !defined DEF_ARCHIVER
#include "archive_data.h"
#endif

lui64 useCount = 0u;

namespace acv
{
	// Contents

	ItemRecord* items[ITEM_RECORD_COUNT]{ nullptr };
	ItemType item_types[ITEM_RECORD_COUNT];
	lui32 item_index = 0u;

	PropRecord props[PROP_RECORD_COUNT];
	lui32 prop_index = 0u;

	SpellRecord spells[SPELL_RECORD_COUNT];
	lui32 spell_index = 0u;

	ProjectileRecord projectiles[PROJECTILE_RECORD_COUNT];
	lui32 projectiles_index = 0u;

	ActorRecord actor_templates[ACTOR_RECORD_COUNT];
	lui32 actor_template_index = 0u;

	ActivatorRecord activators[ACTIVATOR_RECORD_COUNT];
	lui32 activator_index = 0u;

	// Assets

	mem::Lump<AssetType, ASSET_NONE, FN_COUNT, ARCHIVE_MAX_LOADED_DATA>* assetLump;
	Resource assets[FN_COUNT];
	lui32 assetCount = 0u;
	lui64 asset_loaded_size = 0u;

	//#ifndef DEF_ARCHIVER

	FILE* fileARCHIVE;

	void Init() {
		// Get sums
		//lui8 sum = 0ui8;
		//GetSum(ARCHIVE_FILENAME, &sum);
		//GetSum(ARCHIVE_DATA_FILENAME, &sum);

		// initialize archive memory
		assetLump = new mem::Lump<AssetType, ASSET_NONE, FN_COUNT, ARCHIVE_MAX_LOADED_DATA>();

		serializer::LoadArchive(ARCHIVE_FILENAME);

		fileARCHIVE = fopen(ARCHIVE_DATA_FILENAME, "rb");
	}
	void End() {
		ClearMemory();
		if (fileARCHIVE != NULL)
			fclose(fileARCHIVE);
		delete assetLump;
		assetLump = nullptr;
	}

	void LoadAsset(lui32 i) {
		if (!assets[i].loaded && fileARCHIVE != NULL) {
			int err = fseek(fileARCHIVE, (long)assets[i].file_pos, SEEK_SET); // Seek file beginning
			switch (assets[i].type) {
			case ASSET_TEXTURE_FILE:
				assets[i].asset = assetLump->AddEnt(sizeof(graphics::Texture), ASSET_TEXTURE_FILE);
				((graphics::Texture*)assetLump->GetEnt(assets[i].asset))->LoadFile(fileARCHIVE);
				break;
			case ASSET_MESH_FILE:
				assets[i].asset = assetLump->AddEnt(sizeof(graphics::Mesh), ASSET_MESH_FILE);
				((graphics::Mesh*)assetLump->GetEnt(assets[i].asset))->LoadFile(fileARCHIVE, false);
				break;
			case ASSET_MESHBLEND_FILE:
				assets[i].asset = assetLump->AddEnt(sizeof(graphics::MeshBlend), ASSET_MESHBLEND_FILE);
				((graphics::MeshBlend*)assetLump->GetEnt(assets[i].asset))->LoadFile(fileARCHIVE);
				break;
			case ASSET_MESHDEFORM_FILE:
				assets[i].asset = assetLump->AddEnt(sizeof(graphics::MeshDeform), ASSET_MESHDEFORM_FILE);
				((graphics::MeshDeform*)assetLump->GetEnt(assets[i].asset))->LoadFile(fileARCHIVE);
				break;
			};
			assets[i].loaded = true;
			asset_loaded_size += assets[i].file_size;
			lf64 use_amount = (lf64)asset_loaded_size / (lf64)ARCHIVE_MAX_LOADED_DATA;
			printf("Loaded asset %u | Usage: %u of %u , %f\n", i, (lui32)asset_loaded_size, ARCHIVE_MAX_LOADED_DATA, (lf32)use_amount * 100.f);
		}
	}

	void UnloadAsset(lui32 i) {
		if (assets[i].asset != ID_NULL && assets[i].loaded) {
			switch (assets[i].type) {
			case ASSET_TEXTURE_FILE:
				((graphics::Texture*)assetLump->GetEnt(assets[i].asset))->Unload();
				break;
			case ASSET_MESH_FILE:
				((graphics::Mesh*)assetLump->GetEnt(assets[i].asset))->Unload();
				break;
			case ASSET_MESHBLEND_FILE:
				((graphics::MeshBlend*)assetLump->GetEnt(assets[i].asset))->Unload();
				break;
			case ASSET_MESHDEFORM_FILE:
				((graphics::MeshDeform*)assetLump->GetEnt(assets[i].asset))->Unload();
				break;
			};
			assets[i].loaded = false;
			asset_loaded_size -= assets[i].file_size;
			assetLump->RmvEnt(assets[i].asset);
			assets[i].asset = ID_NULL;
		}
	};

	bool IsLoaded(lui32 i) {
		return assets[i].loaded;
	}

	lui32 AssetCount() {
		return assetCount;
	}

	//#endif // DEF_ARCHIVER

	void ClearMemory() {
		for (lui32 i = 0; i < item_index; i++) {
			delete(items[i]);
		}
		#ifndef DEF_ARCHIVER
		for (lui32 i = 0; i < FN_COUNT; i++) {
			UnloadAsset(i);
		}
		#endif // DEF_ARCHIVER
	}

	//#ifndef DEF_ARCHIVER

	forceinline void AssetAccessCheck(lui32 index) {
		// Load the asset if needed
		if (!assets[index].loaded)
			LoadAsset(index);
		assets[index].tickLastAccessed = useCount;
		++useCount;
	}

	graphics::Texture& GetT(lui32 index) {
		if (index < assetCount && assets[index].type == ASSET_TEXTURE_FILE) {
			AssetAccessCheck(index);
			return *(graphics::Texture*)assetLump->GetEnt(assets[index].asset);
		}
		AssetAccessCheck(DEFAULT_TEXTURE);
		return *(graphics::Texture*)assetLump->GetEnt(assets[DEFAULT_TEXTURE].asset);
	}

	graphics::Mesh& GetM(lui32 index) {
		if (index < assetCount && assets[index].type == ASSET_MESH_FILE) {
			AssetAccessCheck(index);
			return *(graphics::Mesh*)assetLump->GetEnt(assets[index].asset);
		}
		AssetAccessCheck(DEFAULT_MESH);
		return *(graphics::Mesh*)assetLump->GetEnt(assets[DEFAULT_MESH].asset);
	}

	graphics::MeshBlend& GetMB(lui32 index) {
		if (index < assetCount && assets[index].type == ASSET_MESHBLEND_FILE) {
			AssetAccessCheck(index);
			return *(graphics::MeshBlend*)assetLump->GetEnt(assets[index].asset);
		}
		AssetAccessCheck(DEFAULT_MESHBLEND);
		return *(graphics::MeshBlend*)assetLump->GetEnt(assets[DEFAULT_MESHBLEND].asset);
	}

	graphics::MeshDeform& GetMD(lui32 index) {
		if (index < assetCount && assets[index].type == ASSET_MESHDEFORM_FILE) {
			AssetAccessCheck(index);
			return *(graphics::MeshDeform*)assetLump->GetEnt(assets[index].asset);
		}
		AssetAccessCheck(DEFAULT_MESHDEFORM);
		return *(graphics::MeshDeform*)assetLump->GetEnt(assets[DEFAULT_MESHDEFORM].asset);
	}

	bool IsTexture(lui32 index) {
		return assets[index].type == ASSET_TEXTURE_FILE;
	}

	bool IsMesh(lui32 index) {
		return assets[index].type == ASSET_MESH_FILE;
	}

	bool IsMeshBlend(lui32 index) {
		return assets[index].type == ASSET_MESHBLEND_FILE;
	}

	// put in network code
	void GetSum(char* fn, lui8* out_sum)
	{
		lui8 read = 0ui8;
		FILE* file = fopen(fn, "rb"); // Open file
		if (file != NULL) {
			fseek(file, 0, SEEK_END); // Seek file end
			lui64 bytecount = ftell(file); // Get file size
			fseek(file, 0, SEEK_SET); // Seek file beginning
			// Add bytes
			for (lui64 i = 0; i < bytecount; ++i) {
				fread(&read, 1, 1, file);
				*out_sum += read;
			}
			fclose(file);
		}
	}
	//#endif
}
