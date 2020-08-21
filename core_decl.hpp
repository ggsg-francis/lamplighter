// Included in core_func

//#define CUTE_C2_IMPLEMENTATION
//#include "3rdparty/cute_c2.h"

#include "objects_entities.h"
#include "objects_items.h"
#include "objects_activators.h"

#include "index.h"
#include "network.h"

char* TemplateNames[]
{
	"Tammy",
	"Nutmeg",
	"Beeawoo",
	"Bock Squill",
	"corpse sample",
	"Kill Lord",
	"Dick Chungus",
	"trowzers",
	"Netbean",
	"Llame",
	"Armpir",
	"Shunt",
	"Thrumbus",
	"Huge Thrombo",
	"Thromb",
	"Throbbo",
	"Gi",
	"exciti nag nag",
	"diobola",
	"Atmhacker",
	"MEYER ATM HACKER",
	"Vanessa Williams",
	"Amara Carr",
	"Robinson hacker",
	"robinsonblankatmhacker",
	"Henry",
	"Mr Mark",
	"Atmcardmachine6",
	"cyberworldatmhacks",
	"CYBER HACKER",
	"Godsent",
	"Elizabeth",
	"elizabethcole232",
	"Annie",
	"RICH CRUZ",
	"PAUL",
	"anniemellor233",
	"patricia clifford",
	"Allison babara",
	"Mabel",
	"Morris mabel",
	"dasinator02",
	"securehacker01",
	"ATM CARD",
	"ultimateblankatmhacker",
	"Donald Jude",
	"Mr ken",
	"betting303",
	"RICHARD",
	"vedat",
	"zombies plants",
	"fireboy",
	"watergir",
	"JAMES LAWRENCE",
	"red ball 6",
	"cedric davis",
	"macedr",
	"jane",
	"John Brown",
	"chiyoungtechworld",
	"kenny allen",
	"atmengineer160",
	"Mr kenny",
	"kenny allen",
	"Brenna Helen",
	"Amanda Hugo",
	"MRS SANDRA",
	"giuseppe zanotti outlet",
	"loraic2",
	"Bedroom girls posts",
	"Gold Gizzard Juice",
	"pinoy",
	"Turntable",
	"corajm69",
	"Disencumber ladyboys",
	"robert alexander",
	"robert alex",
	"Ehi hacker",
	"Tampa",
	"RYAN",
	"Thomas Colson",
	"bruno luis",
	"timyj2",
	"cool math games blox",
	"crysle",
	"markleonard",
	"Dr Smith",
	"TARA KLINE",
	"WALKER",
	"Nicky Derek",
	"tanki online",
	"T.Williams",
	"Sandra Pethtel",
	"Kate Johnson",
	"Smith Seth",
	"Amara Carr",
	"David",
	"ASTER NELSON",
	"Rizzo Brain",
	"abigail winfrey",
	"Morrismabel",
	"Scared",
	"anita",
	"Cindy",
	"mikee",
	"paul",
	"joannehn16",
	"ADOLF PRESTON",
	"elizabeth lizzy",
	"Eldred Sephus",
	"EDWARD BEE",
	"Ryan",
	"Mellisa Brandon",
	"Atm machine",
	"Kate Johnson",
	"ANTHONY",
	"lucy camile",
	"Harrison Bouchard",
	"Lily Walker",
	"lucy camile",
	"Fred",
	"mita",
	"Mariam Joneas",
	"jose mario",
	"Georg Bednorz",
	"morris ferdinad",
	"Virgin",
	"THOMAS",
	"ROSSEY",
	"TRICIA",
	"benjamin collie",
	"Harrison",
	"simon",
	"GRACE",
	"Cards",
	"micheal",
	"Connel",
	"Williams",
	"richardpenny",
	"Winstontheli",
	"Mariam Joneas",
	"smith",
	"Steve Ashley",
	"Ryan Gregory",
	"Teresa moore",
	"david wirth",
	"tanki online",
	"rite",
	"kenny allen",
	"Mr Roland martin",
	"Jurgen",
	"gayft1",
	"ednacg3",
	"Simon Philip",
	"REBECCA MORRISON",
	"Stewartgab",
	"mikee",
	"roberto",
	"Ingedegotfish87",
	"Obat Radang Usus",
	"Kate Johnson",
	"Mr simon",
	"Jasonmoito",
	"Masterhacker",
	"Brett Wood",
	"Jasonmoito",
	"GENNADY FEDYA",
	"Sonia buker",
	"christ ben",
	"Natalie",
	"Cari Sue Harris",
	"Obat Toxoplasma",
	"Levi",
	"sarah",
	"Winstontheli",
	"Wilson Scott",
	"geldanlage",
	"Steve Orlando",
	"Kennethdot",
	"AaronTew",
	"Harrison Bouchard",
	"OnlinepTup",
	"Kennethdot",
	"ViahsTup",
	"TracyCep",
	"obat ginjal bocor",
};
// count: 185
#define TEMP_NAME_COUNT 185

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
	btui32 editor_flags_copy;
	btID editor_prop_copy;
	env::NodePropDirection editor_prop_dir_copy;
	btui8 editor_height_copy_ne;
	btui8 editor_height_copy_nw;
	btui8 editor_height_copy_se;
	btui8 editor_height_copy_sw;
	btui8 editor_material_copy;

	m::Vector2 editor_cursor = m::Vector2(1024.f, 1024.f);
	CellSpace editor_cursorCS;
	btf32 editor_cursor_height = 0.f;
	btui32 editor_zoom = 5u;
	m::Angle editor_cam_pitch = m::Angle(60.f);
	m::Angle editor_cam_yaw = m::Angle(0.f);

	struct ReferenceCell
	{
		mem::idbuf ref_ents; // Entity references
		btID ref_activator = ID_NULL;
	};
	ReferenceCell refCells[WORLD_SIZE][WORLD_SIZE];

	int CellEntityCount(int x, int y)
	{
		return core::refCells[x][y].ref_ents.Size();
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

	bti32 cursor_x = 0u;
	bti32 cursor_y = 0u;

	//--------------------------- FUNCTION DECLARATIONS

	btID GetClosestPlayer(btID id);
	btID GetClosestEntity(btID id, btf32 dist);
	btID GetClosestEntityAlleg(btID index, btf32 dist, fac::facalleg allegiance);

	void ProjectileTick(btf32 dt);
	void ProjectileDraw();
	void ProjectileHitCheck();
	void RemoveAllReferences(btID id);
}
