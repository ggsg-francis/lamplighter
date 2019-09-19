// For inclusion in index_decl.h only

#include "objects_fn_array.h"

// Inherited from index.cpp
#include "resources.h"
#include "cfg.h"
#include "factions.h"
#include "env.h"
#include "time.hpp"
#include "collision.hpp"
#include "input.h"
#include "memory.h"
#include "Transform.h"
#include "maths.hpp"
#include "graphics.hpp"

namespace inv
{
	struct InvItems
	{
		btui32 invSize = 0u;
		btui32* itemCounts = nullptr;
		btID* items = nullptr;
	};

	class Inventory
	{
	public:
		Inventory() {};
		~Inventory()
		{
			free(invstate.items);
			free(invstate.itemCounts);
		};
	private:
		InvItems invstate;
		inline void AddStack(btID itemid)
		{
			++invstate.invSize;
			if (invstate.invSize > 1u)
			{
				// Reallocate items
				invstate.items = (btID*)realloc(invstate.items, sizeof(btID) * invstate.invSize);
				// Reallocate item counts
				invstate.itemCounts = (btui32*)realloc(invstate.itemCounts, sizeof(btui32) * invstate.invSize);
				invstate.items[invstate.invSize - 1] = itemid;
				invstate.itemCounts[invstate.invSize - 1] = 1u;
			}
			else
			{
				// Allocate items
				invstate.items = (btID*)malloc(sizeof(btID) * invstate.invSize);
				// Allocate item counts
				invstate.itemCounts = (btui32*)malloc(sizeof(btui32) * invstate.invSize);
				invstate.items[0u] = itemid;
				invstate.itemCounts[0u] = 1u;
			}
		}
		inline void RemvStack(int index)
		{
			if (invstate.invSize > 1u)
			{
				--invstate.invSize;
				btID* _items = (btID*)malloc(sizeof(btID) * invstate.invSize);
				btui32* _itemCounts = (btui32*)malloc(sizeof(btui32) * invstate.invSize);
				if (_items && _itemCounts) {
					btui32 i;
					// copy up until index directly
					for (i = 0; i < index; i++)
					{
						_items[i] = invstate.items[i];
						_itemCounts[i] = invstate.itemCounts[i];
					}
					// copy index & after with 1 offset
					for (i = index; i < invstate.invSize; i++)
					{
						_items[i] = invstate.items[i + 1];
						_itemCounts[i] = invstate.itemCounts[i + 1];
					}
					free(invstate.items);
					free(invstate.itemCounts);
					invstate.items = _items;
					invstate.itemCounts = _itemCounts;
				}
			}
			else
			{
				invstate.invSize = 0u;
				free(invstate.items);
				free(invstate.itemCounts);
			}
		}
		inline void IncrStack(int index)
		{
			++invstate.itemCounts[index];
		}
		inline void DecrStack(int index)
		{
			if (invstate.itemCounts[index] > 1u)
				--invstate.itemCounts[index];
			else
				RemvStack(index);
		}
	public:
		void AddItem(btID itemid)
		{
			bool added = false;
			for (int i = 0; i < invstate.invSize; ++i)
			{
				if (invstate.items[i] == itemid) // if we already have a stack of this item
				{
					IncrStack(i); added = true; // Add to existing stack
				}
			}
			if (!added)
			{
				AddStack(itemid);
			}
		}
		void RemvItem(btID itemid)
		{
			for (int i = 0; i < invstate.invSize; ++i)
			{
				if (invstate.items[i] == itemid) // if we already have a stack of this item
				{
					DecrStack(i);
				}
			}
		}
		inline void RemvItemAt(int index)
		{
			DecrStack(index);
		}
		InvItems* Get()
		{
			return &invstate;
		}
	};
}

//could just send a gl index for the texture value, but i'd rather future-proof it in case I need to store more metadata in the texture class
void DrawMeshAtTransform(btID ID, res::assetid MODEL, res::assetid TEXTURE, graphics::Shader& SHADER, Transform3D TRANSFORM);
void DrawMesh(btID ID, res::assetid MODEL, res::assetid TEXTURE, graphics::Shader& SHADER, glm::mat4 MATRIX);
void DrawMesh(btID ID, res::assetid MODEL, res::assetid TEXTURE, graphics::Shader& SHADER, graphics::Matrix4x4 MATRIX);
void DrawBlendMeshAtTransform(btID ID, res::assetid MODEL, btf32 BLENDSTATE, res::assetid TEXTURE, graphics::Shader& SHADER, Transform3D TRANSFORM);
void DrawBlendMesh(btID ID, res::assetid MODEL, btf32 BLENDSTATE, res::assetid TEXTURE, graphics::Shader& SHADER, graphics::Matrix4x4 MATRIX);

//transform
class Transform2D
{
public:
	// world space position
	m::Vector2 position;
	m::Vector2 velocity;
	btf32 height = 0.f;
	btf32 height_velocity = 0.f;
	btui8 cellx = 0ui8;
	btui8 celly = 0ui8;
};

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//------------- ITEM STUFF ---------------------------------------

struct Item_DEPRECATED
{

};
struct Musket : public Item_DEPRECATED
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
	m::Vector3 loc;
	btf32 pitch;
	btf32 yaw;

	void Tick();
	void Draw(m::Vector2 pos, btf32 height, m::Angle ang);
};

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//------------- ENTITY STRUCTS -----------------------------------

struct ActiveState
{
	// Global properties, ultimately to be used by every object in the game, incl. environment tiles
	enum globalProperty : btui8 // WIP
	{
		eALIVE = 1ui8,
		eFLAMMABLE = 1ui8 << 6ui8,
		eON_FIRE = 1ui8 << 7ui8,
	};
	mem::bv<btui8, globalProperty> properties;
	btf32 hp = 1.f;
};

struct Entity
{
	enum eprop : btui8
	{
		// Basic properties
		eCOLLIDE_ENV = 1ui8, // Handle collision between this entity and the environment
		eCOLLIDE_ENT = 1ui8 << 1ui8, // Handle collision between this entity and other entities
		eCOLLIDE_PRJ = 1ui8 << 2ui8, // Handle collision between this entity and physical projectiles
		eCOLLIDE_MAG = 1ui8 << 3ui8, // Handle collision between this entity and magic effects
		eREPORT_TOUCH = 1ui8 << 4ui8, // Use callback function when another entity touches this one
		eNO_TICK = 1ui8 << 5ui8, // Do not tick every frame
		//eALIGN_MESH_TO_GROUND = 1ui8 << 6ui8, // Align this object's mesh to the ground normal (useless because each class has its own draw fn anyway)
		// Can go up to 1 << 7

		ePREFAB_FULLSOLID = eCOLLIDE_ENV | eCOLLIDE_ENT | eCOLLIDE_PRJ | eCOLLIDE_MAG,
		//ePREFAB_ITEM = eCOLLIDE_ENV | eNO_TICK | eALIGN_MESH_TO_GROUND,
		ePREFAB_ITEM = eCOLLIDE_ENV | eNO_TICK,
	};
	// Entity base properties
	mem::bv<btui8, eprop> properties;

	fac::faction faction;
	ActiveState state;
	btui8 statebuffer[32 - sizeof(ActiveState)]; // reserved space for save / load data

	bool freeTurn;
	btf32 radius = 0.5f; // Radius of the entity (no larger than .5)
	btf32 height = 1.f; // Height of the entity cylinder
	Transform2D t;
	m::Angle yaw;

	CellSpaceInfo csi; // Where we are in cell space

	virtual etype::etype Type() { return etype::entity; };
};
// Entity type representing placed items
struct EItem : public Entity
{
	virtual etype::etype Type() { return etype::eitem; };
	btID itemid;
	Transform3D t_item;
};
struct Actor : public Entity
{
	virtual etype::etype Type() { return etype::actor; };

	enum actor_input : btui8
	{
		in_run = (0x1ui8 << 0x0ui8),
		in_aim = (0x1ui8 << 0x1ui8),
		in_atk = (0x1ui8 << 0x2ui8),
	};

	//******************************** Actor stuff

	m::Vector2 input; // Input vector, might be temporary
	m::Angle viewYaw;
	m::Angle viewPitch;
	// Movement stuff
	bool moving = false;
	mem::bv<btui8, actor_input> inputbv;

	res::assetid t_skin; // The texture we use for drawing the character
	res::AnimMeshSet* ams_apparel;

	btf32 speed = 2.3f;
	btf32 agility = 0.f; // 0?? use agility to determine turning speed?

	inv::Inventory inventory;

	//******************************** AI stuff

	btui32 target_ent = BUF_NULL;
	bool aiControlled = false;
	btui32 node = 0u;
	bool haspath = false;
	std::vector<env::node_coord> pathnodes;

	void PickUpItem(btID ID);
	void DropItem(btID SLOT);
};