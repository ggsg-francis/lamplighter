#pragma once

#define FN_SIZE 64
#define FN_COUNT 128
#define ITEMS_COUNT 128

namespace archive
{
	namespace types
	{
		enum item_type : btui8
		{
			root,
			apparel,
			weapon,
			potion,
		};
		enum asset_type : btui8
		{
			texturefile,
			meshfile,
			meshblendfile,
		};
	}

	struct item
	{
		// Root
		btID id = 0u;
		btID id_icon = 0u;
		bti8 name[64];
		btf32 f_weight = 0.f; btui32 f_value_base = 0u; btf32 f_radius = 0.f;
		btID id_mesh = 0u; btID id_tex = 0u;
	};

	struct item_aprl : public item
	{
		// Meshes
		btID id_mesh_head; btID id_texture_head;
		btID id_mesh_arms; btID id_texture_arms;
		btID id_mesh_legs; btID id_texture_legs;
		// Block values
		float block_pierce; float block_slice; float block_slam;
	};

	struct item_wpon : public item
	{
		// Damage values
		float f_dam_pierce; float f_dam_slash; float f_dam_slam;
	};

	struct item_pton : public item
	{
		// Spell effects
		btID effect;
		float effect_value;
		float effect_time;
	};

	// art assets archive
	struct archive_asset
	{
		char handle[8];
		char filename[FN_SIZE];
		types::asset_type type = types::texturefile;
		bool loaded;
		void* asset = nullptr;
	};

	//items (also make inaccessable)
	extern item* items[ITEMS_COUNT];
	extern types::item_type item_types[ITEMS_COUNT];
	extern btID item_index; // number of items, I think

	// make inaccessable
	extern archive_asset assets[FN_COUNT];
	extern btui32 assetCount;

	#ifndef DEF_ARCHIVER

	void LoadAsset(btui32 INDEX);
	void UnloadAsset(btui32 INDEX);

	#endif // DEF_ARCHIVER

	void ClearMemory();
}