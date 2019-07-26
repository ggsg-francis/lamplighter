// For inclusion in index_decl.h only

// Inherited from index.cpp
#include "network.h"
#include "networking-common.h"
#include "resources.h"
#include "cfg.h"
#include "factions.h"
#include "env.h"
#include "time.h"
#include "collision.hpp"
#include "input.h"
#include "memory.h"
#include "Transform.h"
#include "maths.hpp"
#include "graphics.hpp"

//forward declarations
class Model;
class Shader;
class NPCController;
namespace collision
{
	struct hit_info;
}

//could just send a gl index for the texture value, but i'd rather future-proof it in case I need to store more metadata in the texture class
//void DrawMeshAtTransform(graphics::NewMesh& MODEL, Texture& TEXTURE, Shader& SHADER, Transform3D TRANSFORM);
void DrawMeshAtTransform(res::assetid MODEL, res::assetid TEXTURE, Shader& SHADER, Transform3D TRANSFORM);
//void DrawBlendMeshAtTransform(ModelBlend& MODEL, float BLENDSTATE, Texture& TEXTURE, Shader& SHADER, Transform3D TRANSFORM);
void DrawBlendMeshAtTransform(res::assetid MODEL, btf32 BLENDSTATE, res::assetid TEXTURE, Shader& SHADER, Transform3D TRANSFORM);

//duplicate struct (of what?)
struct CellCoord
{
	btui8 x = 0, y = 0;
	CellCoord(btui8 _x, btui8 _y) {	x = _x; y = _y;	}
};
// Collection of 4 cells relevant to an entity
struct CellGroup
{
	CellCoord c[4u]{ CellCoord(0ui8,0ui8), CellCoord(0ui8,0ui8), CellCoord(0ui8,0ui8), CellCoord(0ui8,0ui8) };
};
// Collection of 4 cells and XY offsets relevant to an entity
struct CellSpaceInfo
{
	CellCoord c[4u]{ CellCoord(0ui8,0ui8), CellCoord(0ui8,0ui8), CellCoord(0ui8,0ui8), CellCoord(0ui8,0ui8) };
	fw::Vector2 offset;
};

//transform
class Transform2D
{
public:
	// world space position
	fw::Vector2 position;
	fw::Vector2 velocity;
	btf32 height = 0.f;
	btui8 cellx = 0ui8;
	btui8 celly = 0ui8;
};

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//------------- ITEM STUFF ---------------------------------------

struct Item
{

};
struct Musket : public Item
{
	// GUN STUFF (will eventually end up elsewhere)
	enum musket_hold_state : btui8
	{
		aim,
		inspect_pan,
		inspect_barrel,
	};
	musket_hold_state eMusketHoldState = inspect_pan;

	enum musket_state : btui16
	{
		latch_pulled = (0x1 << 0x0),
		fpan_hatch_open = (0x1 << 0x1),
		fpan_powder_in = (0x1 << 0x2),
		barrel_armed = (0x1 << 0x3),
		barrel_rod_in = (0x1 << 0x4),
		match_lit = (0x1 << 0x7),
		match_held = (0x1 << 0x8),

		get_can_fire = fpan_hatch_open & fpan_powder_in & barrel_armed,
		unset_fire = fpan_powder_in | barrel_armed,
	};
	mem::bv<btui16, musket_state> bvMusketState;

	btf32 fpan;
	btf32 lever;
	btf32 rod;

	//btf32 musket_pitch;
	fw::Vector3 loc;
	btf32 pitch;
	btf32 yaw;

	void Tick();
	void Draw(fw::Vector2 pos, btf32 height, m::Angle ang);
};

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//------------- ENTITY STRUCTS -----------------------------------

struct ent_state
{
	bool alive = true;
	btf32 hp = 1.f;
};

struct Entity
{
	enum prefabtype : btui8
	{
		prefab_player,
		prefab_npc,
		prefab_inanimate_bb,
	};

	// Variables
	fac::faction faction;
	ent_state state;
	btf32 radius = 0.5f; // Radius of the entity (no larger than .5)
	Transform2D t;
	m::Angle yaw;

	CellGroup group;

	enum etype : btui8
	{
		entity,
		actor,
		chara,
		musket,
	};
	virtual etype Type() { return entity; };
	btID id;
};
struct Actor : public Entity
{
	virtual etype Type() { return actor; };

	enum actor_input : btui8
	{
		in_run = (0x1ui8 << 0x0ui8),
		in_aim = (0x1ui8 << 0x1ui8),
		in_atk = (0x1ui8 << 0x2ui8),
	};

	//******************************** Actor stuff

	fw::Vector2 input; // Input vector, might be temporary
	m::Angle viewYaw;
	m::Angle viewPitch;
	// Movement stuff
	bool moving = false;
	mem::bv<btui8, actor_input> inputbv;
	Texture* skin; // The texture we use for drawing the character
	btf32 speed = 2.3f;
	btf32 agility = 0.f; // 0?? use agility to determine turning speed?

	//******************************** AI stuff

	btui32 target_ent = BUF_NULL;
	bool aiControlled = false;
	btui32 node = 0u;
	bool haspath = false;
	std::vector<env::node_coord> pathnodes;
};