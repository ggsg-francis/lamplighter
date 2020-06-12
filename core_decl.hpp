// Included in core_func

//#define CUTE_C2_IMPLEMENTATION
//#include "3rdparty/cute_c2.h"

#include "objects_entities.h"
#include "objects_items.h"
#include "objects_statics.h"

#include "index.h"
#include "network.h"

struct Index
{
	struct cell
	{
		mem::idbuf ents;
	};
	cell cells[WORLD_SIZE][WORLD_SIZE];
};

namespace core
{
	// Environment lightmap texture
	// R: Unused | G: Lightsources | B: Unused | A: Unused
	graphics::ModifiableTexture t_EnvLightmap;
	// R: Height | G: Unused | B: Unused | A: Unused
	graphics::ModifiableTexture t_EnvHeightmap;

	//--------------------------- GLOBAL VARIABLES

	btf64 networkTimerTemp;

	unsigned int activePlayer = 0u;
	btID players[2];
	m::Vector3 viewTarget[2];
	m::Vector3 viewPosition[2];
	btID viewtarget[2]{ ID_NULL, ID_NULL };
	btID viewtarget_last_tick[2]{ ID_NULL, ID_NULL };
	// for measuring HP changes for gui display
	btui16 player_hp[2]{ 1000u, 1000u };

	//env::EnvNode editor_node_copy;
	btID editor_prop_copy;
	env::NodePropDirection editor_prop_dir_copy;
	btui8 editor_height_copy_ne;
	btui8 editor_height_copy_nw;
	btui8 editor_height_copy_se;
	btui8 editor_height_copy_sw;
	btui8 editor_material_copy;

	struct ReferenceCell
	{
		mem::idbuf ref_ents; // Entity references
	};
	ReferenceCell refCells[WORLD_SIZE][WORLD_SIZE];

	int CellEntityCount(int x, int y)
	{
		return core::refCells[x][y].ref_ents.end() + 1;
	}
	btID CellEntity(int x, int y, int e)
	{
		return core::refCells[x][y].ref_ents[e];
	}

	// inventory stuff
	graphics::GUIBox guibox;
	graphics::GUIText text_temp;
	graphics::GUIText text_hp;
	graphics::GUIText text_version;
	graphics::GUIText text_fps;

	graphics::GUIText text_message[2];
	btui64 message_time[2];

	//--------------------------- FUNCTION DECLARATIONS

	btID GetClosestPlayer(btID id);
	btID GetClosestEntity(btID id, btf32 dist);
	btID GetClosestEntityAlleg(btID index, btf32 dist, fac::facalleg allegiance);

	void ProjectileTick(btf32 dt);
	void ProjectileDraw();
	void ProjectileHitCheck();
	void RemoveAllReferences(btID id);
}
