#pragma once

#include "graphics.hpp"

#ifndef DEF_ARCHIVER

#define DEFAULT_TEXTURE 0u
#define DEFAULT_MESH 1u
#define DEFAULT_MESHBLEND 2u

typedef btui16 assetID;

namespace res
{
	extern graphics::ModifiableTexture skin_t[4];

	// This is designed as a head start towards using an indexed asset system
	// I'd like to say it won't make it into the final game, but I'm probably much too lazy to replace it.
	// ONLY CONTAINS ASSET IDS DIRECTLY REFERENCED IN THE CODE, THIS LIST SHOULD GET SMALLER OVER TIME
	// AND REQUIRE LESS MAINTENANCE AS THE ARCHIVE IS IMPROVED UPON
	enum AssetConstantID : assetID // Maintain with 0000gameassets
	{
		// Debug stuff
		t_default,
		m_default,
		mb_default,
		m_debug_bb,
		t_debug_bb,
		m_debugcell,
		t_noise,
		// Compass
		m_compass,
		t_compass,
		// Shadow
		m_shadow,
		t_shadow,
		// sky
		m_skydome,
		m_skystars,
		t_sky, // role is indeterminate so name is vague
		m_skymoon,
		t_skymoon,
		// terrain
		m_terrain_near,
		m_terrain_oob,
		// Chara textures
		t_skin_template,
		t_skin2,
		t_skin3,
		t_skin4,
		// Chara models
		mb_legs,
		mb_armscast,
		mb_char_head,
		mb_char_leg,
		// Err....
		m_ex1e_air_carrier,
		// Equipment
		m_equip_head_pickers,
		mb_equip_body_pickers_step,
		mb_equip_body_pickers_run,
		// Matchlock gun
		m_item_matchlock_01,
		mb_item_matchlock_01_lever,
		m_item_matchlock_01_rod,
		mb_item_matchlock_01_rod_anim,
		m_item_matchlock_01_pan,
		m_item_matchlock_01_pan_open,
		m_item_matchlock_01_pan_open_full,
		t_item_matchlock_01,
		// Other shit
		m_proj,
		t_proj,
		m_proj_2,
		t_proj_2,
		mb_smoke_trail_segment,
		t_smoke_trail,
		m_kitfloor,
		t_terrain_sanddirt,
		t_terrain_scorch,
		// Gui stuff
		t_gui_crosshair,
		t_gui_font,
		t_gui_bar_red,
		t_gui_bar_yellow,
		t_gui_box,
		t_gui_select_box,
		t_gui_inv_slot,
	};

	graphics::Texture& GetT(btui32 index);
	graphics::Mesh& GetM(btui32 index);
	graphics::MeshBlend& GetMB(btui32 index);

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
		enum ItemType : btui8
		{
			ITEM_ROOT,
			ITEM_EQUIP,
			ITEM_WPN_MELEE,
			ITEM_WPN_MATCHGUN,
			ITEM_WPN_MAGIC,
			ITEM_CONS,
		};
		enum AssetType : btui8
		{
			ASSET_NONE,
			ASSET_TEXTURE_FILE,
			ASSET_MESH_FILE,
			ASSET_MESHBLEND_FILE,
		};
	}

	struct item
	{
		// Root
		btID id = 0u;
		btID id_icon = 0u;
		bti8 name[64];
		btf32 f_weight = 0.f;
		btui32 f_value_base = 0u;
		btf32 f_radius = 0.f;
		btf32 f_model_height = 0.f;
		btID id_mesh = 0u;
		btID id_tex = 0u;
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

	struct item_w_melee : public item
	{
		// Damage values
		float f_dam_pierce; float f_dam_slash; float f_dam_slam;
	};

	struct item_w_gun : public item
	{
		float f_temp;
	};

	struct item_w_magic : public item
	{
		float f_temp;
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
		types::AssetType type = types::ASSET_NONE;
		bool loaded;
		void* asset = nullptr;
	};

	//items (also make inaccessable)
	extern item* items[ITEMS_COUNT];
	extern types::ItemType item_types[ITEMS_COUNT];
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