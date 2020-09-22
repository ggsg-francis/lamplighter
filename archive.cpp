#include "archive.hpp"
#include "archive_read_write.h"
#include "memory.hpp"

// temp? for generating skin texture
#include "maths.hpp"

#if defined DEF_ARCHIVE_IN_CODE && !defined DEF_ARCHIVER
#include "archive_data.h"
#endif

// hmmmmmmmmmm
// from core
extern btui64 tickCount;

namespace acv
{
	// Contents

	ItemRecord* items[ITEM_RECORD_COUNT]{ nullptr };
	ItemType item_types[ITEM_RECORD_COUNT];
	btui32 item_index = 0u;

	PropRecord props[PROP_RECORD_COUNT];
	btui32 prop_index = 0u;

	SpellRecord spells[SPELL_RECORD_COUNT];
	btui32 spell_index = 0u;

	ProjectileRecord projectiles[PROJECTILE_RECORD_COUNT];
	btui32 projectiles_index = 0u;

	ActorRecord actor_templates[ACTOR_RECORD_COUNT];
	btui32 actor_template_index = 0u;

	ActivatorRecord activators[ACTIVATOR_RECORD_COUNT];
	btui32 activator_index = 0u;

	// Assets

	Resource assets[FN_COUNT];
	btui32 assetCount = 0u;
	btui64 asset_loaded_size = 0u;

	//#ifndef DEF_ARCHIVER

	FILE* fileARCHIVE;

	void Init() {
		// Get sums
		//btui8 sum = 0ui8;
		//GetSum(ARCHIVE_FILENAME, &sum);
		//GetSum(ARCHIVE_DATA_FILENAME, &sum);

		serializer::LoadArchive(ARCHIVE_FILENAME);

		fileARCHIVE = fopen(ARCHIVE_DATA_FILENAME, "rb");
	}
	void End() {
		acv::ClearMemory();
		if (fileARCHIVE != NULL)
			fclose(fileARCHIVE);
	}

	void LoadAsset(btui32 i) {
		if (!assets[i].loaded) {
			if (fileARCHIVE != NULL) {
				int err = fseek(fileARCHIVE, assets[i].file_pos, SEEK_SET); // Seek file beginning
				switch (assets[i].type) {
				case ASSET_TEXTURE_FILE:
					assets[i].asset = new graphics::Texture;
					((graphics::Texture*)assets[i].asset)->LoadFile(fileARCHIVE);
					goto finish;
				case ASSET_MESH_FILE:
					assets[i].asset = new graphics::Mesh;
					((graphics::Mesh*)assets[i].asset)->LoadFile(fileARCHIVE, false);
					goto finish;
				case ASSET_MESHBLEND_FILE:
					assets[i].asset = new graphics::MeshBlend;
					((graphics::MeshBlend*)assets[i].asset)->LoadFile(fileARCHIVE);
					goto finish;
				case ASSET_MESHDEFORM_FILE:
					assets[i].asset = new graphics::MeshDeform;
					((graphics::MeshDeform*)assets[i].asset)->LoadFile(fileARCHIVE);
					goto finish;
				};
			finish:
				assets[i].loaded = true;
				asset_loaded_size += assets[i].file_size;
				btf64 use_amount = (btf64)asset_loaded_size / (btf64)ARCHIVE_MAX_LOADED_DATA;
				printf("Loaded asset %i | Usage: %i of %i , %f\n", i, asset_loaded_size, ARCHIVE_MAX_LOADED_DATA, (btf32)use_amount * 100.f);
			}
		}
	}

	void UnloadAsset(btui32 i) {
		if (assets[i].asset != nullptr && assets[i].loaded) {
			switch (assets[i].type) {
			case ASSET_TEXTURE_FILE:
				((graphics::Texture*)assets[i].asset)->Unload();
				return;
			case ASSET_MESH_FILE:
				((graphics::Mesh*)assets[i].asset)->Unload();
				return;
			case ASSET_MESHBLEND_FILE:
				((graphics::MeshBlend*)assets[i].asset)->Unload();
				return;
			case ASSET_MESHDEFORM_FILE:
				((graphics::MeshDeform*)assets[i].asset)->Unload();
				return;
			};
			assets[i].loaded = false;
			asset_loaded_size -= assets[i].file_size;
			delete assets[i].asset;
		}
	};

	bool IsLoaded(btui32 i) {
		return assets[i].loaded;
	}

	btui32 AssetCount() {
		return assetCount;
	}

	//#endif // DEF_ARCHIVER

	void ClearMemory() {
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

	//#ifndef DEF_ARCHIVER

	forceinline void AssetAccessCheck(btui32 index) {
		// Load the asset if needed
		if (!acv::assets[index].loaded)
			acv::LoadAsset(index);
		acv::assets[index].tickLastAccessed = tickCount;
	}

	graphics::Texture& GetT(btui32 index) {
		if (index < acv::assetCount && acv::assets[index].type == ASSET_TEXTURE_FILE) {
			AssetAccessCheck(index);
			return *(graphics::Texture*)acv::assets[index].asset;
		}
		AssetAccessCheck(DEFAULT_TEXTURE);
		return *(graphics::Texture*)acv::assets[DEFAULT_TEXTURE].asset;
	}

	graphics::Mesh& GetM(btui32 index) {
		if (index < acv::assetCount && acv::assets[index].type == ASSET_MESH_FILE) {
			AssetAccessCheck(index);
			return *(graphics::Mesh*)acv::assets[index].asset;
		}
		AssetAccessCheck(DEFAULT_MESH);
		return *(graphics::Mesh*)acv::assets[DEFAULT_MESH].asset;
	}

	graphics::MeshBlend& GetMB(btui32 index) {
		if (index < acv::assetCount && acv::assets[index].type == ASSET_MESHBLEND_FILE) {
			AssetAccessCheck(index);
			return *(graphics::MeshBlend*)acv::assets[index].asset;
		}
		AssetAccessCheck(DEFAULT_MESHBLEND);
		return *(graphics::MeshBlend*)acv::assets[DEFAULT_MESHBLEND].asset;
	}

	graphics::MeshDeform& GetMD(btui32 index) {
		if (index < acv::assetCount && acv::assets[index].type == ASSET_MESHDEFORM_FILE) {
			AssetAccessCheck(index);
			return *(graphics::MeshDeform*)acv::assets[index].asset;
		}
		AssetAccessCheck(DEFAULT_MESHDEFORM);
		return *(graphics::MeshDeform*)acv::assets[DEFAULT_MESHDEFORM].asset;
	}

	bool IsTexture(btui32 index) {
		return acv::assets[index].type == ASSET_TEXTURE_FILE;
	}

	bool IsMesh(btui32 index) {
		return acv::assets[index].type == ASSET_MESH_FILE;
	}

	bool IsMeshBlend(btui32 index) {
		return acv::assets[index].type == ASSET_MESHBLEND_FILE;
	}

	// put in network code
	void GetSum(char* fn, btui8* out_sum)
	{
		btui8 read = 0ui8;
		FILE* file = fopen(fn, "rb"); // Open file
		if (file != NULL) {
			fseek(file, 0, SEEK_END); // Seek file end
			btui64 bytecount = ftell(file); // Get file size
			fseek(file, 0, SEEK_SET); // Seek file beginning
			// Add bytes
			for (btui64 i = 0; i < bytecount; ++i) {
				fread(&read, 1, 1, file);
				*out_sum += read;
			}
			fclose(file);
		}
	}
	//#endif
}
