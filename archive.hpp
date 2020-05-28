#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "graphics.hpp"

#ifndef DEF_ARCHIVER

#define DEFAULT_TEXTURE 0u
#define DEFAULT_MESH 1u
#define DEFAULT_MESHBLEND 2u
#define DEFAULT_MESHDEFORM 3u

// test definitions (this is still really not necessary for C, enums still work)
/*
#define ITEM_TYPE_ROOT         0u
#define ITEM_TYPE_EQUIP        1u
#define ITEM_TYPE_WPN_MELEE    2u
#define ITEM_TYPE_WPN_MATCHGUN 3u
#define ITEM_TYPE_WPN_MAGIC    4u
#define ITEM_TYPE_CONS         5u

#define ASSET_TYPE_NONE            0u
#define ASSET_TYPE_TEXTURE_FILE    1u
#define ASSET_TYPE_MESH_FILE       2u
#define ASSET_TYPE_MESHBLEND_FILE  3u
#define ASSET_TYPE_MESHDEFORM_FILE 4u
*/

typedef btui16 assetID;

namespace res
{
	//extern graphics::ModifiableTexture skin_t[4];

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
		md_default,
		m_debug_bb,
		t_debug_bb,
		m_debugcell,
		m_debug_sphere,
		m_debug_monkey,
		t_meat_test,
		// Debug Colours
		t_col_black,
		t_col_red,
		// Shadow
		m_shadow,
		t_shadow,
		// sky
		m_skydome,
		m_skystars,
		t_sky, // role is indeterminate so name is vague
		m_skymoon,
		t_skymoon,
		// Chara textures
		t_skin_template,
		t_skin2,
		t_skin3,
		t_skin4,
		// Err....
		m_ex1e_air_carrier,
		// Equipment
		m_equip_head_pickers,
		md_equip_body_robe_01,
		t_equip_body_robe_01,
		t_equip_legs_robe_01,
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
		t_gui_hurt,
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
}
#endif

// Number of characters in a filename
#define FN_SIZE 64
// Number of filenames (number of assets, in other words)
#define FN_COUNT 128
// -
#define ITEMS_COUNT 64
// -
#define PROPS_COUNT 32
// -
#define SPELL_COUNT 8
// -
#define PROJECTILE_TEMPLATE_COUNT 8
// -
#define ACTORBASE_COUNT 8

namespace acv
{	
	struct EnvProp
	{
		enum EnvPropFloorMat : btui8
		{
			FLOOR_STANDARD,
			FLOOR_WATER,
			FLOOR_TAR,
			FLOOR_QUICKSAND,
		};
		// well.... maybe this should be directly in the tiles
		enum EnvPropPhysShape : btui8
		{
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
		enum EnvPropFlags : btui8
		{
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

	enum SpellCastType : btui8
	{
		ON_CASTER,
		ON_TARGET,
		ON_CASTER_AND_TARGET,
	};
	typedef struct Spell
	{
		char handle[8];
		bti8 name[32];
		SpellCastType cast_type = ON_TARGET;
		btui16 target_effect_type;
		btf32 target_effect_duration;
		btui32 target_effect_magnitude;
	} Spell;

	typedef struct ProjectileTemplate
	{
		char handle[8];
		btui32 damage;
		bool saveOnHit;
		btui8 ammunition_type;
		btID mesh;
		btID texture;
	} ProjectileTemplate;

	#define ENTITY_MAX_LIMB_NUM 4
	typedef struct ActorBase
	{
		char handle[8];
		btID m_head;
		btID m_body;
		btID m_arm;
		btID m_leg;
		btID t_head;
		btID t_body;
		btID t_arm;
		btID t_leg;
		btf32 jpos_arm_fw[ENTITY_MAX_LIMB_NUM];
		btf32 jpos_arm_rt[ENTITY_MAX_LIMB_NUM];
		btf32 jpos_arm_up[ENTITY_MAX_LIMB_NUM];
		btf32 leng_arm[ENTITY_MAX_LIMB_NUM];
		btf32 jpos_leg_fw[ENTITY_MAX_LIMB_NUM];
		btf32 jpos_leg_rt[ENTITY_MAX_LIMB_NUM];
		btf32 jpos_leg_up[ENTITY_MAX_LIMB_NUM];
		btf32 leng_leg[ENTITY_MAX_LIMB_NUM];
		btf32 leng_body[ENTITY_MAX_LIMB_NUM];
	} EntityTemplate;

	struct BaseItem
	{
		// Root
		char handle[8];
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

	struct BaseItemEqp : public BaseItem
	{
		// Meshes
		btID id_mesh_head; btID id_texture_head;
		btID id_mesh_arms; btID id_texture_arms;
		btID id_mesh_legs; btID id_texture_legs;
		// Block values
		float block_pierce; float block_slice; float block_slam;
	};

	struct BaseItemMel : public BaseItem
	{
		// Damage values
		float f_dam_pierce; float f_dam_slash; float f_dam_slam;
	};

	struct BaseItemGun : public BaseItem
	{
		float f_temp;
		bool b_automatic;
		btui8 ammunition_type;
	};

	struct BaseItemMgc : public BaseItem
	{
		float f_temp;
	};

	struct BaseItemCon : public BaseItem
	{
		// Spell effects
		btID id_effect = ID_NULL;
		btID id_projectile = ID_NULL;
	};

	// art assets archive
	struct archive_asset
	{
		char handle[8];
		char filename[FN_SIZE];
		AssetType type = ASSET_NONE;
		bool loaded;
		void* asset = nullptr;
	};

	//items (also make inaccessable)
	extern BaseItem* items[ITEMS_COUNT];
	extern ItemType item_types[ITEMS_COUNT];
	extern btui32 item_index; // number of items, I think

	extern EnvProp props[PROPS_COUNT];
	extern btui32 prop_index;

	extern Spell spells[SPELL_COUNT];
	extern btui32 spell_index;

	extern ProjectileTemplate projectiles[PROJECTILE_TEMPLATE_COUNT];
	extern btui32 projectiles_index;

	extern ActorBase actor_templates[ACTORBASE_COUNT];
	extern btui32 actor_template_index;

	// make inaccessable
	extern archive_asset assets[FN_COUNT];
	extern btui32 assetCount;

	#ifndef DEF_ARCHIVER

	void LoadAsset(btui32 INDEX);
	void UnloadAsset(btui32 INDEX);

	#endif // DEF_ARCHIVER

	void ClearMemory();
}

#endif
