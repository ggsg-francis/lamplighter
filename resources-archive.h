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
			misc,
			apparel,
			weapon,
			potion,
		};
		enum asset_type : btui8
		{
			btt,
			btm,
			btmb,
		};
	}
	extern char filenames_OBSOLETE[FN_SIZE][FN_COUNT];
	extern types::asset_type fn_types_OBSOLETE[FN_COUNT];
	extern btID fn_index_OBSOLETE;
	//items
	extern void* items[ITEMS_COUNT];
	extern types::item_type item_types[ITEMS_COUNT];
	extern btID item_index;

	struct item
	{
		// Root
		btID id = 0u;
		btID id_icon = 0u;
		char name[64];
		float f_weight = 0.f; float f_value_base = 0.f; float f_radius = 0.f;
		btID id_mesh = 0u; btID id_tex = 0u;
	};

	struct item_misc : public item
	{
		// Mesh
		//id_t mesh; id_t texture;
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

	// NEW STUFF !!!!!!!!!!!!!!!!!!!!!

	// art assets archive

	struct archive_asset
	{
		char filename[FN_SIZE];
		types::asset_type type = types::btt;
		bool loaded;
		void* asset = nullptr;
	};

	// make inaccessable
	extern archive_asset assets[FN_COUNT];
	extern btui32 assetCount;

	#ifndef DEF_ARCHIVER

	void LoadAsset(btui32 INDEX);
	void UnloadAsset(btui32 INDEX);

	#endif // DEF_ARCHIVER

	void clear_memory();
}