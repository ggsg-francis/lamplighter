#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "graphics.hpp"
#include "maths.hpp"

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
#define DEFAULT_MESHSET 3u
#define DEFAULT_MESHDEFORM 4u

typedef lui16 assetID;

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
		ms_default,
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
		lui64 file_pos = 0u;
		lui64 file_size = 0u;
		AssetType type = ASSET_NONE;
		bool loaded = false;
		lui64 tickLastAccessed = 0u;
		LtrID asset = ID2_NULL;
	};

	graphics::Texture& GetT(lui32 index);
	graphics::Mesh& GetM(lui32 index);
	graphics::MeshBlend& GetMB(lui32 index);
	graphics::MeshSet& GetMS(lui32 index);
	graphics::MeshDeform& GetMD(lui32 index);

	bool IsTexture(lui32 index);
	bool IsMesh(lui32 index);
	bool IsMeshBlend(lui32 index);

	void Init();
	void End();

	#ifndef DEF_ARCHIVER

	void LoadAsset(lui32 INDEX);
	void UnloadAsset(lui32 INDEX);
	bool IsLoaded(lui32 INDEX);
	lui32 AssetCount();

	#endif // DEF_ARCHIVER

	void ClearMemory();

	//#endif

	struct PropRecord {
		enum EnvPropFloorMat : lui8 {
			FLOOR_STANDARD,
			FLOOR_WATER,
			FLOOR_TAR,
			FLOOR_QUICKSAND,
			FLOOR_LAVA,
			FLOOR_ICE,
			FLOOR_ACID,
		};
		// well.... maybe this should be directly in the tiles
		enum EnvPropPhysShape : lui8 {
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
		enum EnvPropFlags : lui8 {
			eBLOCK_LIGHT_SKY = 0b00000001,
			eNONE1 = 0b00000010,
			eNONE2 = 0b00000100,
			eNONE3 = 0b00001000,
			eNONE4 = 0b00010000,
			eNONE5 = 0b00100000,
			eNONE6 = 0b01000000,
			eNONE7 = 0b10000000,
		};
		ID16 idMesh = ID_NULL;
		ID16 idTxtr = ID_NULL;
		EnvPropFloorMat floorType;
		EnvPropPhysShape physShape;
		//mem::bv<lui8, EnvPropFlags> flags;
		EnvPropFlags flags;
		lui8 placeholder;
	};

	enum SpellCastType : lui8 {
		ON_CASTER,
		ON_TARGET,
		ON_CASTER_AND_TARGET,
	};
	typedef struct SpellRecord {
		char handle[8];
		li8 name[32];
		SpellCastType cast_type = ON_TARGET;
		lui8 filler;
		lui16 target_effect_type;
		lf32 target_effect_duration;
		lui32 target_effect_magnitude;
		ID16 icon;
		lui16 filler2;
	} Spell;

	typedef struct ProjectileRecord {
		char handle[8];
		lui32 damage;
		bool saveOnHit;
		lui8 ammunition_type;
		ID16 mesh;
		ID16 texture;
	} ProjectileTemplate;

	#define ENTITY_MAX_LIMB_NUM 4
	typedef struct ActorRecord {
		char handle[8];
		ID16 m_head, m_body, m_arm, m_leg;
		ID16 t_head, t_body, t_arm, t_leg;
		lf32 jpos_arm_fw, jpos_arm_rt, jpos_arm_up, leng_arm;
		lf32 jpos_leg_fw, jpos_leg_rt, jpos_leg_up, leng_leg;
		lf32 leng_body;
	} ActorRecord;

	struct ItemRecord {
		enum BVBase { // 16 of these
			eDETONATEABLE = 1u,
			eUNUSED1 = 1u << 1u,
			eUNUSED2 = 1u << 2u,
			eUNUSED3 = 1u << 3u,
		};
		char handle[8];
		li8 name[64];
		ID16 id_icon = 0u;
		lui16 bv_base = 0u;
		lf32 f_weight = 0.f;
		lui32 f_value_base = 0u;
		lf32 f_radius = 0.f;
		lf32 f_model_height = 0.f;
		ID16 id_mesh = 0u;
		ID16 id_mesh_lod = 0u;
		ID16 id_tex = 0u;
		li16 FILLER2 = 0u;
	};
	struct ItemRecordEqp : public ItemRecord {
		ID16 id_mesh_head; ID16 id_texture_head;
		ID16 id_mesh_arms; ID16 id_texture_arms;
		ID16 id_mesh_legs; ID16 id_texture_legs;
		float block_pierce; float block_slice; float block_slam;
	};
	struct ItemRecordMel : public ItemRecord {
		float f_dam_pierce; float f_dam_slash; float f_dam_slam;
	};
	struct ItemRecordGun : public ItemRecord {
		lui8 b_automatic;
		lui8 ammunition_type;
	};
	struct ItemRecordMgc : public ItemRecord {
		float f_temp;
	};
	struct ItemRecordCon : public ItemRecord {
		lui32 use_count;
		ID16 id_effect;
		ID16 id_projectile;
	};

	struct ActivatorRecord {
		lui32 type;
	};

	struct AnimationRecord {
		m::Vector3 p[MESHSET_MAX_COUNT];
		m::Quaternion r[MESHSET_MAX_COUNT];
		lui32 frameCount;
	};

	//items (also make inaccessable)
	extern ItemRecord* items[ITEM_RECORD_COUNT];
	extern ItemType item_types[ITEM_RECORD_COUNT];
	extern lui32 item_index; // number of items, I think

	extern PropRecord props[PROP_RECORD_COUNT];
	extern lui32 prop_index;

	extern SpellRecord spells[SPELL_RECORD_COUNT];
	extern lui32 spell_index;

	extern ProjectileRecord projectiles[PROJECTILE_RECORD_COUNT];
	extern lui32 projectiles_index;

	extern ActorRecord actor_templates[ACTOR_RECORD_COUNT];
	extern lui32 actor_template_index;

	extern ActivatorRecord activators[ACTIVATOR_RECORD_COUNT];
	extern lui32 activator_index;

	extern AnimationRecord animation;
}

#endif
