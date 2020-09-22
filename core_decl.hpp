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

	btf64 networkTimerTemp;

	unsigned int activePlayer = 0u;
	btID players[2];
	m::Vector3 viewTarget[2];
	m::Vector3 viewPosition[2];
	// todo: wrap all of the gui globals into a structure
	// for measuring HP changes for gui display
	btui16 guiPlayerHP[2]{ 1000u, 1000u };
	btf32 guiInvTimer[2]{ 0.f, 0.f };

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
	
	#if DEF_PROJECT == PROJECT_BC
	bool bShowGuide = false;
	bti32 guideW = 0;
	graphics::GUIText text_guidehelp;
	#endif

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
	"Treasured",
	"Speedy",
	"Slow",
	"Cyber Hacker",
	"Godsent",
	"Tarnished",
	"Debased",
	"Crazy",
	/*"Obeskure", // DC First names
	"Brie",
	"Strawberry",
	"Gibby",
	"Tebby",
	"Moon",
	"Eremine",
	"Saramy",
	"Rewit",*/
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
};
// count the above
#define TEMP_NAME_FAMILY_COUNT (305u - 300u)








































char* TemplateNames[] { // start at line 350
	"Loomin", // DC Last names
	"Tammy",
	"Squirrel Bladder",
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
	"Ozoh",
	"Excity",
	"Jpeg",
	"Smutt",
	"Clover",
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
	"Radang",
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
	"OnlinepTup",
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
	"Broadraven",
	"Minyash",
	"Anatolka",
	"Kezanok",
	"Mariana",
	"Jeremiah",
	"Zorin",
	"Yuka",
	"Alekseich",
	"Luizka",
	"Bole",
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
	"Maximus",
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
	"Bremar",
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
	"Lenore",
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
	"Jewelson",
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
	"Snowskin",
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
	"Juvenile",
	"Fief",
	"Jellies",
	"Eudush",
	"Rekz",
	"Rootlet",
	"Valera",
	"Kandis",
	"Lavrunya",
	"Ashcoarm",
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
	"DarrenJouby",
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
	"Renate"
};
// count the above
#define TEMP_NAME_COUNT (912u - 350u)

// just names
/*
"Vladislavpah",
"Alexandrina",
"Antoniokes",
"Janniest",
"Louisa",
"Rydersil",
"Trudymcc",
"Esmeraldajw16",
"Winstontheli",
"Amara carr",
"Robinsonblankatmhacker",
"Robinson hacker",
"Henry",
"Vanessa williams",
"Mr mark",
"Elizabeth",
"Elizabethcole232",
"Annie",
"Rich cruz",
"Paul",
"Anniemellor233",
"Patricia clifford",
"Allison babara",
"Mabel",
"Morris mabel",
"Donald jude",
"Richard",
"Mr ken",
"James lawrence",
"Cedric davis",
"Jane",
"John brown",
"Kenny allen",
"Mr kenny",
"Brenna helen",
"Amanda hugo",
"Mrs sandra",
"Robert alexander",
"Robert alex",
"Ryan",
"Thomas colson",
"Bruno luis",
"Tara kline",
"Dr smith",
"Nicky derek",
"T.williams",
"Sandra pethtel",
"Kate johnson",
"Smith seth",
"Amara carr",
"David",
"Aster nelson",
"Abigail winfrey",
"Anita",
"Cindy",
"Paul",
"Joannehn16",
"Adolf preston",
"Elizabeth lizzy",
"Edward bee",
"Ryan",
"Mellisa brandon",
"Kate johnson",
"Anthony",
"Lucy camile",
"Harrison bouchard",
"Lily walker",
"Lucy camile",
"Fred",
"Mariam joneas",
"Jose mario",
"Georg bednorz",
"Morris ferdinad",
"Thomas",
"Rossey",
"Tricia",
"Benjamin collie",
"Harrison",
"Simon",
"Grace",
"Micheal",
"Connel",
"Williams",
"Richardpenny",
"Mark",
"Leonard",
"Mariam joneas",
"Smith",
"Steve ashley",
"Ryan gregory",
"Teresa moore",
"David wirth",
"Kenny allen",
"Mr roland martin",
"Simon philip",
"Rebecca morrison",
"Stewartgab",
"Roberto",
"Mr simon",
"Brett wood",
"Gennady fedya",
"Sonia buker",
"Christ ben",
"Jasonmoito",
"Natalie",
"Cari sue harris",
"Levi",
"Sarah",
"Winston",
"Wilson",
"Scott",
"Steve",
"Orlando",
"Kenneth",
"Aaron",
"Harrison",
"Bouchard",
"Tracy",
"Giuseppe",
"Zanotti",
"Percy",
"Claudette",
"Joseph",
"Jeffrey",
"David",
"Natasha",
"Helen",
"Alphonso",
"Tyler",
"Jason",
"George",
"Kayleigh",
"Dennis",
"Daniel",
"Melvin",
"Clark",
"Stephen",
"Marty",
"Blake",
"Sabrina",
"Wilford",
"Erica",
"Michael",
"Sharon",
"Robert",
"Ronald",
"Theresa",
"Lucinda",
"Rickie",
"Ashely",
"Shirley",
"Carlos",
"James",
"Raymond",
"Billie",
"Antonio",
"Marissa",
"Alexander",
"Brenda",
"Julianne",
"Adriana",
"Cynthia",
"Paula",
"Cathy",
"Willis",
"Philip",
"Connor",
"Eliza",
"Richard",
"Matt",
"Elizabeth",
"Thomas",
"Howard",
"Susan",
"Anthony",
"Stacy",
"Charles",
"Jennifer",
"Selena",
"Brad",
"Harry",
"Darrell",
"Marcus",
"Glenn",
"Clinton",
"Amy",
"Martin",
"Herbert",
"Kenhix",
"Brucep,
"Betty",
"Leonard",
"Calvin",
"Adam",
"Kevin",
"Gabriel",
"Fred",
"Arthur",
"Anton",
"Patrick",
"Johnnie",
"Allen",
"Corey",
"Gilbert",
"William",
"Ralph",
"Matthew",
"Melissa",
"Donald",
"Johnny",
"Joan",
"Frank",
"Shane",
"Alice",
"Henry",
"Regina",
"Albert",
"Bertie",
"Trenton",
"Mary",
"Nick",
"Freddie",
"Ron",
"Cedric",
"Laurence",
*/
