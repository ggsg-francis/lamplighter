#pragma once

#include "graphics.hpp"

#ifndef DEF_ARCHIVER

#define DEFAULT_TEXTURE 0u
#define DEFAULT_MESH 1u
#define DEFAULT_MESHBLEND 2u

typedef btui16 assetID;

namespace res
{
	// This is designed as a head start towards using an indexed asset system
	// I'd like to say it won't make it into the final game, but I'm probably much too lazy to replace it.
	// ONLY CONTAINS ASSET IDS DIRECTLY REFERENCED IN THE CODE, THIS LIST SHOULD GET SMALLER OVER TIME
	// AND REQUIRE LESS MAINTENANCE AS THE ARCHIVE IS IMPROVED UPON
	enum AssetConstantID : assetID // Maintain with 0000gameassets
	{
		// Debug stuff
		t_default, // good name
		m_default, // good name
		mb_default, // good name
		m_debug_bb, // good name
		t_debug_bb, // good name
		m_debugcell,
		t_noise, // good name
		// Compass
		m_compass, // good name
		t_compass, // good name
		// Shadow
		m_shadow, // good name
		t_shadow, // good name
		// sky
		m_skydome, // good name
		m_skystars, // good name
		t_sky, // role is indeterminate so name is vague
		m_skymoon, // good name
		t_skymoon, // good name
		// terrain
		m_terrain_near, // good name
		m_terrain_oob, // good name
		// Chara textures
		t_skin1,
		t_skin2,
		t_skin3,
		t_skin4,
		t_equip_atlas,
		// Chara models
		mb_legs,
		mb_armscast,
		mb_char_head,
		mb_char_leg, // good name
		// Equipment
		m_equip_head_pickers, // good name
		mb_equip_body_pickers_step, // good name
		mb_equip_body_pickers_run, // good name
		// Matchlock gun
		m_item_matchlock_01, // good name
		mb_item_matchlock_01_lever, // good name
		mb_item_matchlock_01_rod, // good name
		mb_item_matchlock_01_pan, // good name
		mb_item_matchlock_01_pan_full, // good name
		t_item_matchlock_01, // good name
		// Other shit
		m_proj,
		t_proj,
		mb_smoke_trail_segment, // good name
		t_smoke_trail, // good name
		m_kitfloor,
		t_terrain_sanddirt, // good name
		t_terrain_scorch, // good name
		// Gui stuff
		t_gui_crosshair, // good name
		t_gui_font, // good name
		t_gui_bar_red, // good name
		t_gui_bar_yellow, // good name
		t_gui_box, // good name
		t_gui_select_box, // good name
		t_gui_inv_slot, // good name
	};

	graphics::Texture& GetTexture(btui32 index);
	graphics::Mesh& GetMesh(btui32 index);
	graphics::MeshBlend& GetMeshBlend(btui32 index);

	bool IsTexture(btui32 index);
	bool IsMesh(btui32 index);
	bool IsMeshBlend(btui32 index);

	void Init();
	void End();
}
#endif

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
			none,
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
		types::asset_type type = types::none;
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