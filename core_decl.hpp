// Included in core_func

//#define CUTE_C2_IMPLEMENTATION
//#include "3rdparty/cute_c2.h"

#include "entity.h"
#include "objects_items.h"
#include "objects_activators.h"

#include "index.h"
#include "network.h"

namespace core
{
	//--------------------------- GLOBAL VARIABLES

	lui32 activePlayer = 0u;
	lid players[NUM_PLAYERS];
	m::Vector3 camViewTarget[NUM_PLAYERS];
	m::Vector3 camViewPosition[NUM_PLAYERS];

	struct EditorStuff {
		// general editor properties
		m::Vector2 cursor = m::Vector2(1024.f, 1024.f);
		CellSpace cursorCS;
		lf32 cursor_height = 0.f;
		lui32 zoom = 5u;
		m::Angle cam_pitch = m::Angle(60.f);
		m::Angle cam_yaw = m::Angle(0.f);
		// Environment edit properties
		lui32 flags_copy;
		lid prop_copy;
		#if DEF_GRID
		env::NodePropDirection prop_dir_copy;
		#endif
		lui8 height_copy_ne;
		lui8 height_copy_nw;
		lui8 height_copy_se;
		lui8 height_copy_sw;
		lui8 material_copy;
	};
	EditorStuff editor;
	
	struct CoreGUI {
		// TODO: shalt optimize later, we dont need all these duplicates
		// Screen message
		graphics::GUIText text_message[NUM_PLAYERS];
		lui64 message_time[NUM_PLAYERS];
		// for measuring HP changes for gui display
		lui16 guiPlayerHP[NUM_PLAYERS];
		lf32 guiInvTimer[NUM_PLAYERS];
		// inventory stuff
		graphics::GUIBox guibox;
		graphics::GUIText text_temp;
		graphics::GUIText text_hp;
		graphics::GUIText text_version;
		graphics::GUIText text_fps;
		// BC stuff
		#if DEF_PROJECT == PROJECT_BC
		bool bShowGuide = false;
		li32 guideW = 0;
		graphics::GUIText text_guidehelp;
		#endif
	};
	CoreGUI gui;

	struct ReferenceCell
	{
		mem::idbuf ref_ents; // Entity references
	};
	ReferenceCell refCells[WORLD_SIZE][WORLD_SIZE];

	int CellEntityCount(int x, int y)
	{
		return core::refCells[x][y].ref_ents.Size();
	}
	lid CellEntity(int x, int y, int e)
	{
		return core::refCells[x][y].ref_ents[e];
	}

	//--------------------------- FUNCTION DECLARATIONS

	lid GetClosestPlayer(lid id);
	lid GetClosestEntity(lid id, lf32 dist);
	lid GetClosestEntityAlleg(lid index, lf32 dist, fac::facalleg allegiance);

	void ProjectileTick(lf32 dt);
	void ProjectileDraw();
	void ProjectileHitCheck();
	void RemoveAllReferences(lid id);
}






























































































































































char* TemplatePrefixes[] { // start at line 250
	"Fearless Terror",
	"Hungry",
	"Lovely",
	"Nurse",
	"Naked",
	"Melting Away",
	"Scared",
	"Ladyboy",
	"Radiant",
	"Kill Lord",
	"Huge Thrombo",
	"Llame",
	"Horny",
	"Treasured",
	"Speedy",
	"Slow",
	"Cyber Hacker",
	"Godsent",
	"Tarnished",
	"Debased",
	"Crazy",
	"Squirrel Bladder",
	"Juvenile",
	"Drooling",
	"Cute",
};
// count the above
#define TEMP_NAME_PREFIX_COUNT (269u - 250u)





















char* TemplateFamilyNames[]{ // start at line 300
	"Loomin",
	"Bludsong",
	"Bludworm",
	"Creedmoor",
	"Bloodfire",
	"Brightfury",
	"Broadraven",
	"Snowskin",
	"Loomin",
	"Lenore",
	"Jouby",
	"Radang",
	"Bole",
	"Bremar",
	"Ashcoarm",
	"Two-Eyes",
	"Scumbottom",
};
// count the above
#define TEMP_NAME_FAMILY_COUNT (316u - 300u)





























char* TemplateNames[] { // start at line 350
	"Tammy",
	"Jojes",
	"Nutmeg",
	"Beeawoo",
	"Bock",
	"Squill",
	"Corpse Sample",
	"Dick Chungus",
	"Trowzers",
	"Netbean",
	"Armpir",
	"Shunt",
	"Thrumbus",
	"Thromb",
	"Throbbo",
	"Thrungus",
	"Giga",
	"Ozoh",
	"Excity",
	"Jpeg",
	"Smutt",
	"Clover",
	"Rumble",
	"Eldred",
	"Floog",
	"Isvos",
	"Umo",
	"Owugiziji",
	"Veteert",
	"Glige",
	"Hix",
	"Sephus",
	"Obat",
	"Usus",
	"Diobola",
	"Tojybowy",
	"Dasinator",
	"Ogou",
	"Loraic",
	"Vedat",
	"Zombies Plants",
	"Fireboy",
	"Watergir",
	"Red Ball 6",
	"Maceder",
	"Gold Gizzard Juice",
	"Pinoy",
	"Turntable",
	"Disencumber",
	"Tampa",
	"Blox",
	"Crysle",
	"Walker",
	"Rizzo",
	"Brain",
	"ATM Machine",
	"Mita",
	"Virgin",
	"Cards",
	"Rite",
	"Jurgen",
	"Ednac",
	"Ingedegot",
	"Fish",
	"Masterhacker",
	"Toxoplasma",
	"Geldanlage",
	"Onlineptup",
	"ViahsTup",
	"Apumat",
	"Ijexe",
	"Uje",
	"Ginjal",
	"Bocor",
	"Nili",
	"Nina",
	"Moi",
	"Karinmig",
	"Milaha",
	"Andoali",
	"Lavrinenko",
	"Vernontal",
	"Viany",
	"Apurajerd",
	"Faxanixary",
	"Faxinary",
	"Egobaubumu",
	"Fubarpazaw",
	"Oredopo",
	"Eguloti",
	"Qimavopez",
	"Oaicozubaye",
	"Tempa",
	"Luthielf",
	"Byzveteert",
	"Leonid",
	"Larissa",
	"Mironich",
	"Mitryukha",
	"Lavren",
	"Annus",
	"Minyash",
	"Anatolka",
	"Kezanok",
	"Mariana",
	"Jeremiah",
	"Zorin",
	"Yuka",
	"Alekseich",
	"Luizka",
	"Milan",
	"Ltyecz",
	"Elinore",
	"Milenka",
	"Decembrist",
	"Alexeica",
	"Ianikit",
	"Ilyusha",
	"Ninel",
	"Artesha",
	"Laruna",
	"Ynyet",
	"Sofa",
	"Bobea",
	"Kizzys",
	"Angana",
	"Matthias",
	"Null",
	"Kleif",
	"Afyz",
	"Pamala",
	"Stepanida",
	"Mitun",
	"Mihailina",
	"Aralar",
	"Taishyushka",
	"Maryushka",
	"Leist",
	"Nalya",
	"Basida",
	"Lily",
	"Evdonya",
	"Ninka",
	"Polins",
	"Masha",
	"Cegrinn",
	"Nunya",
	"Lula",
	"Margo",
	"Kyhner",
	"Mihaila",
	"Natalya",
	"Dashuta",
	"Topmen",
	"Karyn",
	"Obuloyera",
	"Acavelawa",
	"Fletaserse",
	"Aihuvuadozi",
	"Anisoxoumef",
	"Gorlina",
	"Onavouxla",
	"Vinsa",
	"Helendus",
	"Sherri",
	"Sosse",
	"Ojireqi",
	"Maximych",
	"Ilyukha",
	"Esfirka",
	"Hardisty",
	"Beazekus",
	"Levvy",
	"Geletap",
	"Buzalbine",
	"Mikaska",
	"Spacho",
	"Gettinghungup",
	"Agagrinn",
	"Banez",
	"Anusya",
	"Maneuver",
	"Lazy",
	"Mogy",
	"Trench",
	"Wanting",
	"Vitalina",
	"Mavanaz",
	"Ranes",
	"Greece",
	"Genre",
	"Fedulya",
	"Yurich",
	"Vadyusha",
	"Goose",
	"Briton",
	"Biaemi",
	"Ruslana",
	"Yuliya",
	"Ballalune",
	"Kerfif",
	"Kiryakha",
	"Alfandre",
	"Olena",
	"Natunya",
	"Ksyusha",
	"Innushka",
	"Vladislavka",
	"Polyxena",
	"Fhif",
	"Seas",
	"Inura",
	"Olene",
	"Fedus",
	"Lavrentich",
	"Kuzminichna",
	"Manyusha",
	"Maksyuta",
	"Mitrium",
	"Liza",
	"Angel",
	"Everb",
	"Ujikojay",
	"Acumumi",
	"Ibaperi",
	"Upibruuja",
	"Alurauhfi",
	"Eqliele",
	"Heledo",
	"Udonehokatex",
	"Alian",
	"Yauveteert",
	"Fins",
	"Quiana",
	"Androide",
	"Cajrospealt",
	"Zedec",
	"Geows",
	"Phyllis",
	"Vange",
	"Maria",
	"Deria",
	"Awiososuji",
	"Krims",
	"Gooseveteert",
	"Wude",
	"Piorb",
	"Nastena",
	"Woups",
	"Tolya",
	"Dawnfont",
	"Romaha",
	"Steppe",
	"Dellevar",
	"Whale",
	"Enuta",
	"Valyaka",
	"Broadfire",
	"Felhagra",
	"Alsara",
	"Ghana",
	"Hush",
	"Vsevolod",
	"Yasia",
	"Dawning",
	"Valention",
	"Soar",
	"Zainn",
	"Kyarald",
	"Bark",
	"Bureaus",
	"Anayajurus",
	"Vasiliy",
	"Ivana",
	"Micha",
	"Vadya",
	"Pawl",
	"Quiet",
	"Akinohn",
	"Zamoor",
	"Vedene",
	"Raya",
	"Bobermod",
	"Fien",
	"Ladeau",
	"Pashata",
	"Gron",
	"Petruha",
	"Vendwhrl",
	"Kflecz",
	"Diana",
	"Eudos",
	"Seagroves",
	"Andronka",
	"Twaytway",
	"Akilinka",
	"Emilian",
	"Anura",
	"Jerinovir",
	"Jia",
	"Tesha",
	"Vekz",
	"Alsath",
	"Carriage",
	"Aurilas",
	"Kseniya",
	"Nadish",
	"Berkohik",
	"Union",
	"Ujieyez",
	"Vasa",
	"Anayalore",
	"Arcia",
	"Igula",
	"Avdulya",
	"Vladislav",
	"Swordstaff",
	"Panyula",
	"Artyusha",
	"Quemal",
	"Lavrich",
	"Vecheslav",
	"Coron",
	"Zetadda",
	"Werner",
	"Fedyayka",
	"Ybyekz",
	"Julianich",
	"Tasyuta",
	"Mashuka",
	"Arsuit",
	"Rotya",
	"Dagdanadar",
	"Ekowym",
	"Ikxunigoema",
	"Sogahonquyi",
	"Muss",
	"Xensi",
	"Exuxuhulanehu",
	"Obsamityexe",
	"Usizutonuajar",
	"Emizeyily",
	"Unufix",
	"Natadu",
	"Cardiawhara",
	"Vassveteert",
	"Afayiko",
	"Pefumtacuxa",
	"Bernicealata",
	"Ghinn",
	"Eunice",
	"Elozy",
	"Lyndaik",
	"Or",
	"Muns",
	"Dino",
	"Louelle",
	"Misty",
	"Oqaqbio",
	"Alvaro",
	"Venetta",
	"Rex",
	"Arazuruveteert",
	"Unole",
	"Opohawo",
	"Nicorr",
	"Aximou",
	"Teresita",
	"Oferinag",
	"Mollie",
	"Ulrich",
	"Dakota",
	"Reta",
	"Coraprat",
	"Catja",
	"Ermadown",
	"Oijuge",
	"Vekules",
	"Avaraseyexuzu",
	"Hydra",
	"Duell",
	"Lester",
	"Norvezhskiy",
	"Learl",
	"Holdenveteert",
	"Graciela",
	"Winnie",
	"Ovadeyemi",
	"Margarit",
	"Esaelizifou",
	"Acotobivue",
	"Ijuvoopokaa",
	"Duxinpho",
	"Emivezuko",
	"Nana",
	"Sherman",
	"Elsie",
	"Gum",
	"Eugenrunn",
	"Olgacatt",
	"Solomon",
	"Branden",
	"Moshe",
	"Swexy",
	"Cornell",
	"Carma",
	"Launa",
	"Pearl",
	"AAAAAgmailcom",
	"Vigocaadu",
	"Prunuceo",
	"Sondra",
	"Itefilovia",
	"Pukakilowa",
	"Tia",
	"Kristoph",
	"Buddy",
	"Itariday",
	"Jacques",
	"Javie",
	"Erasmomes",
	"Alejandr",
	"Aviscaro",
	"Maddy",
	"Yukiko",
	"Felipe",
	"Irakreit",
	"Lavonda",
	"Isis",
	"Loren",
	"Agamisav",
	"Coral",
	"Demi",
	"Gabb",
	"Charity",
	"Teodora",
	"Ococuwu",
	"Debraqe",
	"Irekeku",
	"Lettie",
	"Alekzenderr",
	"Smir",
	"Arobe",
	"Nikula",
	"Prasia",
	"J7dzt0yc",
	"Miroha",
	"Andya",
	"Hritsya",
	"Ufkif",
	"Adrielar",
	"Jieylau",
	"Veta",
	"Petania",
	"Darim",
	"Veirf",
	"Daudt",
	"Mariasha",
	"Karina",
	"Tygot",
	"Fief",
	"Jellies",
	"Eudush",
	"Rekz",
	"Rootlet",
	"Valera",
	"Kandis",
	"Lavrunya",
	"Mironka",
	"Odorert",
	"Ulyan",
	"Beazerdred",
	"Yurash",
	"Uyehuguita",
	"Verash",
	"Foderaro",
	"Hive",
	"Fenrinos",
	"Pawelka",
	"Floris",
	"Novel",
	"Livley",
	"Irusha",
	"Artemych",
	"Aleka",
	"Borislav",
	"Polinka",
	"Fedian",
	"Veronique",
	"Utyef",
	"Monrealles",
	"Beabor",
	"Qunayice",
	"Victor",
	"Mitrash",
	"Cat",
	"Ninu",
	"Kozma",
	"Mihailinka",
	"Smack",
	"Antonin",
	"Panash",
	"Phashana",
	"Qunorrar",
	"Andbath",
	"Galisha",
	"Iroll",
	"Artemy",
	"Gogh",
	"Lucasus",
	"Lyubasha",
	"Molosola",
	"Brenas",
	"Arlelond",
	"Verka",
	"Him",
	"Kiryunya",
	"Kitzman",
	"Adietoyn",
	"Bolanim",
	"Serguna",
	"Gholbigel",
	"Tosya",
	"Drelandis",
	"Ariginn",
	"Basilisk",
	"Valencia",
	"Anatoly",
	"Negastail",
	"Timon",
	"Jorge",
	"Lexandra",
	"Nadya",
	"Lynich",
	"Svetlana",
	"Lenya",
	"Turrie",
	"Lilyusha",
	"Nikus",
	"Manantrius",
	"Visamibr",
	"Venedimych",
	"Out",
	"Rust",
	"Pear",
	"Igorasha",
	"Agraine",
	"Dosya",
	"Faville",
	"Lina",
	"Genus",
	"Vladislavpah",
	"Renate",
};
// count the above
#define TEMP_NAME_COUNT (902u - 350u)

/*"Obeskure", // DC First names
"Brie",
"Strawberry",
"Gibby",
"Tebby",
"Moon",
"Eremine",
"Saramy",
"Rewit",*/
