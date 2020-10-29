#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "graphics.hpp"

//#ifndef DEF_ARCHIVER

// Number of characters in a filename
#define FN_SIZE 64
// Number of filenames (number of assets, in other words)
#define FN_COUNT 128
// -
#define ITEM_RECORD_COUNT 64
// -
#define PROP_RECORD_COUNT 32
// -
#define SPELL_RECORD_COUNT 8
// -
#define PROJECTILE_RECORD_COUNT 8
// -
#define ACTOR_RECORD_COUNT 8
// -
#define ACTIVATOR_RECORD_COUNT 8

#define DEFAULT_TEXTURE 0u
#define DEFAULT_MESH 1u
#define DEFAULT_MESHBLEND 2u
#define DEFAULT_MESHDEFORM 3u

typedef btui16 assetID;

namespace acv
{
	// Indices of all the constant assets
	// That means anything that needs to be referenced by the engine directly
	// ONLY CONTAINS ASSET IDS DIRECTLY REFERENCED IN THE CODE, THIS LIST SHOULD GET SMALLER OVER TIME
	// AND REQUIRE LESS MAINTENANCE AS THE ARCHIVE IS IMPROVED UPON
	enum AssetConstantID : assetID { // Maintain with 0000gameassets
		// Debug stuff
		t_default,
		m_default,
		mb_default,
		md_default,
		m_debug_bb,
		t_debug_bb,
		m_debugcell,
		m_debug_sphere,
		m_debug_monkey,
		t_guide,
		// Debug Colours
		t_col_black,
		t_col_red,
		// Shadow
		t_cursor,
		// sky
		m_world_phys,
		m_world,
		t_sky, // role is indeterminate so name is vague
		t_debug_loaded_y,
		t_debug_loaded_n,
		// Chara textures
		t_skin_template,
		t_skin2,
		t_skin3,
		t_skin4,
		// Other shit
		mb_smoke_trail_segment,
		t_smoke_trail,
		t_terrain_01,
		t_terrain_02,
		t_terrain_03,
		t_terrain_04,
		t_terrain_05,
		t_terrain_06,
		t_terrain_07,
		t_terrain_08,
		// Gui stuff
		t_gui_crosshair,
		t_gui_font,
		t_gui_bar_red,
		t_gui_bar_yellow,
		t_gui_box,
		t_gui_select_box,
		t_gui_icon_pick_up,
		t_gui_icon_hold_hand,
		t_gui_hurt,
		t_gui_guide,
	};

	// art assets archive
	struct Resource {
		char handle[8];
		btui64 file_pos = 0u;
		btui64 file_size = 0u;
		AssetType type = ASSET_NONE;
		bool loaded = false;
		btui64 tickLastAccessed = 0u;
		btui32 asset = ID_NULL;
	};

	graphics::Texture& GetT(btui32 index);
	graphics::Mesh& GetM(btui32 index);
	graphics::MeshBlend& GetMB(btui32 index);
	graphics::MeshDeform& GetMD(btui32 index);

	bool IsTexture(btui32 index);
	bool IsMesh(btui32 index);
	bool IsMeshBlend(btui32 index);

	void Init();
	void End();

	#ifndef DEF_ARCHIVER

	void LoadAsset(btui32 INDEX);
	void UnloadAsset(btui32 INDEX);
	bool IsLoaded(btui32 INDEX);
	btui32 AssetCount();

	#endif // DEF_ARCHIVER

	void ClearMemory();

	//#endif

	struct PropRecord {
		enum EnvPropFloorMat : btui8 {
			FLOOR_STANDARD,
			FLOOR_WATER,
			FLOOR_TAR,
			FLOOR_QUICKSAND,
			FLOOR_LAVA,
			FLOOR_ICE,
			FLOOR_ACID,
		};
		// well.... maybe this should be directly in the tiles
		enum EnvPropPhysShape : btui8 {
			eSHAPE_NONE, // No shape, just floor
			eSHAPE_BOX, // full square impassable tile
			eSHAPE_DIAG_NE, // Diagonal face
			eSHAPE_DIAG_NW, // Diagonal face
			eSHAPE_DIAG_SE, // Diagonal face
			eSHAPE_DIAG_SW, // Diagonal face
			eSHAPE_CIRCLE, // Full 0.5 radius circle
			eSHAPE_QUARTER_CIRCLE_NE, // Quarter-circle with 1 radius
			eSHAPE_QUARTER_CIRCLE_NW, // Quarter-circle with 1 radius
			eSHAPE_QUARTER_CIRCLE_SE, // Quarter-circle with 1 radius
			eSHAPE_QUARTER_CIRCLE_SW, // Quarter-circle with 1 radius
		};
		enum EnvPropFlags : btui8 {
			eBLOCK_LIGHT_SKY = 0b00000001,
			eNONE1 = 0b00000010,
			eNONE2 = 0b00000100,
			eNONE3 = 0b00001000,
			eNONE4 = 0b00010000,
			eNONE5 = 0b00100000,
			eNONE6 = 0b01000000,
			eNONE7 = 0b10000000,
		};
		btID idMesh = ID_NULL;
		btID idTxtr = ID_NULL;
		EnvPropFloorMat floorType;
		EnvPropPhysShape physShape;
		//mem::bv<btui8, EnvPropFlags> flags;
		EnvPropFlags flags;
		btui8 placeholder;
	};

	enum SpellCastType : btui8 {
		ON_CASTER,
		ON_TARGET,
		ON_CASTER_AND_TARGET,
	};
	typedef struct SpellRecord {
		char handle[8];
		bti8 name[32];
		SpellCastType cast_type = ON_TARGET;
		btui8 filler;
		btui16 target_effect_type;
		btf32 target_effect_duration;
		btui32 target_effect_magnitude;
		btID icon;
		btui16 filler2;
	} Spell;

	typedef struct ProjectileRecord {
		char handle[8];
		btui32 damage;
		bool saveOnHit;
		btui8 ammunition_type;
		btID mesh;
		btID texture;
	} ProjectileTemplate;

	#define ENTITY_MAX_LIMB_NUM 4
	typedef struct ActorRecord {
		char handle[8];
		btID m_head, m_body, m_arm, m_leg;
		btID t_head, t_body, t_arm, t_leg;
		btf32 jpos_arm_fw, jpos_arm_rt, jpos_arm_up, leng_arm;
		btf32 jpos_leg_fw, jpos_leg_rt, jpos_leg_up, leng_leg;
		btf32 leng_body;
	} ActorRecord;

	struct ItemRecord {
		enum BVBase { // 16 of these
			eDETONATEABLE = 1u,
			eUNUSED1 = 1u << 1u,
			eUNUSED2 = 1u << 2u,
			eUNUSED3 = 1u << 3u,
		};
		char handle[8];
		bti8 name[64];
		btID id_icon = 0u;
		btui16 bv_base = 0u;
		btf32 f_weight = 0.f;
		btui32 f_value_base = 0u;
		btf32 f_radius = 0.f;
		btf32 f_model_height = 0.f;
		btID id_mesh = 0u;
		btID id_mesh_lod = 0u;
		btID id_tex = 0u;
		bti16 FILLER2 = 0u;
	};
	struct ItemRecordEqp : public ItemRecord {
		btID id_mesh_head; btID id_texture_head;
		btID id_mesh_arms; btID id_texture_arms;
		btID id_mesh_legs; btID id_texture_legs;
		float block_pierce; float block_slice; float block_slam;
	};
	struct ItemRecordMel : public ItemRecord {
		float f_dam_pierce; float f_dam_slash; float f_dam_slam;
	};
	struct ItemRecordGun : public ItemRecord {
		btui8 b_automatic;
		btui8 ammunition_type;
	};
	struct ItemRecordMgc : public ItemRecord {
		float f_temp;
	};
	struct ItemRecordCon : public ItemRecord {
		btui32 use_count;
		btID id_effect;
		btID id_projectile;
	};

	struct ActivatorRecord {
		btui32 type;
	};

	//items (also make inaccessable)
	extern ItemRecord* items[ITEM_RECORD_COUNT];
	extern ItemType item_types[ITEM_RECORD_COUNT];
	extern btui32 item_index; // number of items, I think

	extern PropRecord props[PROP_RECORD_COUNT];
	extern btui32 prop_index;

	extern SpellRecord spells[SPELL_RECORD_COUNT];
	extern btui32 spell_index;

	extern ProjectileRecord projectiles[PROJECTILE_RECORD_COUNT];
	extern btui32 projectiles_index;

	extern ActorRecord actor_templates[ACTOR_RECORD_COUNT];
	extern btui32 actor_template_index;

	extern ActivatorRecord activators[ACTIVATOR_RECORD_COUNT];
	extern btui32 activator_index;
}

#endif
