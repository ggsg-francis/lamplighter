#ifdef __GNUC__
#include <math.h>
#endif

#include "index.h"

#include "objects_entities.h"
#include "objects_items.h"

//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//--------------------------- ENTITY BUFFERS -------------------------------------------------------------------------------------
//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||

//-------------------------------- ENTITIES

//block of IDs in memory, tracks the numbers and IDs of any type of object
mem::objbuf block_entity; // Entity buffer
EntAddr block_entity_data[BUF_SIZE];
// TODO: using fixed size arrays is a big memory hog, and the amount of space allocated here can never be filled
// unless everything shares the same type
//mem::CkBuffer2<Chara> buf_entities;
mem::objbuf buf_resting_item;
RestingItem buf_resting_item_data[BUF_SIZE];
mem::objbuf buf_chara;
Actor       buf_chara_data[BUF_SIZE];

mem::objbuf* BufPtr[ENTITY_TYPE_COUNT] = { &buf_chara, &buf_resting_item, &buf_chara };
void* BufDataPtr[ENTITY_TYPE_COUNT] = { &buf_chara_data, &buf_resting_item_data, &buf_chara_data };
unsigned long long BufDataSize[ENTITY_TYPE_COUNT] = { sizeof(EditorPawn), sizeof(RestingItem), sizeof(Actor) };

// Return a string which will be printed to the screen when this entity is looked at
char*(*fpName[ENTITY_TYPE_COUNT])(void* self) = {
	DisplayNameActor,
	DisplayNameRestingItem,
	DisplayNameActor,
};

// Tick this entity
void(*fpTick[ENTITY_TYPE_COUNT])(void* self, btf32 dt) = {
	TickEditorPawn,
	TickRestingItem,
	TickChara,
};

// Render graphics of this entity
void(*fpDraw[ENTITY_TYPE_COUNT])(void* self) = {
	DrawEditorPawn,
	DrawRestingItem,
	DrawChara,
};

// Get Entity address from ID
void* getEntEditorPawn(btID id) { return &buf_chara_data[id]; }
void* getEntRestingItem(btID id) { return &buf_resting_item_data[id]; }
void* getEntActor(btID id) { return &buf_chara_data[id]; }
// Array lookup for the above functions
void*(*GetEntArray[ENTITY_TYPE_COUNT])(btID) = {
	getEntEditorPawn,
	getEntRestingItem,
	getEntActor,
};
// Array lookup function
void* GetEntityPtr(btID id)
{
	return GetEntArray[block_entity_data[id].type](block_entity_data[id].type_buffer_index);
}

//-------------------------------- ITEMS

mem::objbuf block_item; // Item buffer
EntAddr block_item_data[BUF_SIZE];

mem::objbuf buf_item_misc; // Item buffer
HeldItem buf_item_misc_data[BUF_SIZE];

mem::objbuf buf_item_melee; // Item buffer
HeldMel buf_item_melee_data[BUF_SIZE];

mem::objbuf buf_item_gun; // Item buffer
HeldGun buf_item_gun_data[BUF_SIZE];

mem::objbuf buf_item_mgc; // Item buffer
HeldMgc buf_item_mgc_data[BUF_SIZE];

mem::objbuf buf_item_con; // Item buffer
HeldCons buf_item_con_data[BUF_SIZE];

void* getItemMis(btID id) { return &buf_item_misc_data[id]; }
void* getItemEqp(btID id) { return &buf_item_misc_data[id]; }
void* getItemMel(btID id) { return &buf_item_melee_data[id]; }
void* getItemGun(btID id) { return &buf_item_gun_data[id]; }
void* getItemMgc(btID id) { return &buf_item_mgc_data[id]; }
void* getItemCon(btID id) { return &buf_item_con_data[id]; }
void*(*GetItemArray[])(btID) = { getItemMis, getItemEqp, getItemMel, getItemGun, getItemMgc, getItemCon };
mem::objbuf* ItemBufPtr[] = { &buf_item_misc, &buf_item_misc, &buf_item_melee, &buf_item_gun, &buf_item_mgc, &buf_item_con };
void* GetItemPtr(btID id)
{
	return GetItemArray[block_item_data[id].type](block_item_data[id].type_buffer_index);
}
ItemType GetItemType(btID id)
{
	return block_item_data[id].type;
}

// TODO: doesnt really belong here
char Capitalize(char c)
{
	if (c >= 'a' && c <= 'z')
		return c + ('A' - 'a');
	return c;
}

void IndexInitEntity(btID id, EntityType type)
{
	block_entity.used[id] = true;
	block_entity_data[id].type = type;
	switch (type)
	{
	case ENTITY_TYPE_EDITOR_PAWN:
		memset(ENT_VOID(id), 0, sizeof(EditorPawn));
		break;
	case ENTITY_TYPE_RESTING_ITEM:
		block_entity_data[id].type_buffer_index = buf_resting_item.add();
		memset(ENT_VOID(id), 0, sizeof(RestingItem));
		break;
	case ENTITY_TYPE_CHARA:
		block_entity_data[id].type_buffer_index = buf_chara.add();
		memset(ENT_VOID(id), 0, sizeof(Actor));
		((Actor)*(ACTOR(id))) = Actor();
		{
			// generate name
			FILE* file = fopen("n.txt", "rb"); // Open file
			if (file != NULL)
			{
				fseek(file, 0L, SEEK_END);
				long sz = ftell(file);
				long random = (long)m::Random(0, sz);
				// TODO: quick fix, this system has no answer for reaching the end of the file
				random = 0;
				fseek(file, random, SEEK_SET); // Seek file beginning

				int name_index = 0;
				char c;
				bool has_advanced_word = false;
			getchar:
				fread(&c, 1, 1, file);
				if (c == CHARCODE_ASCII::space || c == CHARCODE_ASCII::CR || c == CHARCODE_ASCII::LF)
				{
					// if we reach the first 'empty character' we've hit the end of the current word
					has_advanced_word = true;
					goto getchar;
				}
				else
				{
					if (!has_advanced_word)
						goto getchar;
					ACTOR(id)->name[0] = Capitalize(c);
					// else we can read from here assuming this is the start of a new word
					fgets((char*)(&ACTOR(id)->name[1]), 31, file);
				}
			}
			fclose(file);
		}
		break;
	default:
		std::cout << "Tried to initialize entity of no valid type" << std::endl;
		break;
	}

	ENTITY(id)->id = id;
	ENTITY(id)->type = type;
}
void IndexFreeEntity(btID id)
{
	if (block_entity.used[id])
	{
		switch (ENTITY(id)->type)
		{
		case ENTITY_TYPE_EDITOR_PAWN:
			//delete _entities[id];
			break;
		case ENTITY_TYPE_RESTING_ITEM:
			buf_resting_item.remove(block_entity_data[id].type_buffer_index);
			break;
		case ENTITY_TYPE_CHARA:
			buf_chara.remove(block_entity_data[id].type_buffer_index);
			break;
		}
	}
	block_entity.remove(id);
}
void IndexInitItem(btID id, ItemType type)
{
	block_item.used[id] = true;
	block_item_data[id].type = type;
	block_item_data[id].type_buffer_index = ItemBufPtr[type]->add();
	HeldItem* held_item = GETITEM_MISC(id);
	switch (type)
	{
	case ITEM_TYPE_MISC:
		*held_item = HeldItem();
		held_item->fpTick = HeldItemTick;
		held_item->fpDraw = HeldItemDraw;
		held_item->fpOnEquip = HeldItemOnEquip;
		held_item->fpGetLeftHandPos = HeldItemGetLeftHandPos;
		held_item->fpGetRightHandPos = HeldItemGetRightHandPos;
		held_item->fpBlockTurn = HeldItemBlockTurn;
		held_item->fpBlockMove = HeldItemBlockMove;
		break;
	case ITEM_TYPE_EQUIP:
		*held_item = HeldItem();
		held_item->fpTick = HeldItemTick;
		held_item->fpDraw = HeldItemDraw;
		held_item->fpOnEquip = HeldItemOnEquip;
		held_item->fpGetLeftHandPos = HeldItemGetLeftHandPos;
		held_item->fpGetRightHandPos = HeldItemGetRightHandPos;
		held_item->fpBlockTurn = HeldItemBlockTurn;
		held_item->fpBlockMove = HeldItemBlockMove;
		break;
	case ITEM_TYPE_WPN_MELEE:
		*held_item = HeldMel();
		held_item->fpTick = HeldMelTick;
		held_item->fpDraw = HeldMelDraw;
		held_item->fpOnEquip = HeldMelOnEquip;
		held_item->fpGetLeftHandPos = HeldMelGetLeftHandPos;
		held_item->fpGetRightHandPos = HeldMelGetRightHandPos;
		held_item->fpBlockTurn = HeldMelBlockTurn;
		held_item->fpBlockMove = HeldMelBlockMove;
		break;
	case ITEM_TYPE_WPN_MATCHGUN:
		*held_item = HeldGun();
		//memset(held_item, 0, sizeof(HeldGun));
		//*held_item = HeldGunMatchLock();
		held_item->fpTick = HeldGunTick;
		held_item->fpDraw = HeldGunDraw;
		held_item->fpOnEquip = HeldGunOnEquip;
		held_item->fpGetLeftHandPos = HeldGunGetLeftHandPos;
		held_item->fpGetRightHandPos = HeldGunGetRightHandPos;
		held_item->fpBlockTurn = HeldGunBlockTurn;
		held_item->fpBlockMove = HeldGunBlockMove;
		break;
	case ITEM_TYPE_WPN_MAGIC:
		*held_item = HeldMgc();
		held_item->fpTick = HeldMgcTick;
		held_item->fpDraw = HeldMgcDraw;
		held_item->fpOnEquip = HeldMgcOnEquip;
		held_item->fpGetLeftHandPos = HeldMgcGetLeftHandPos;
		held_item->fpGetRightHandPos = HeldMgcGetRightHandPos;
		held_item->fpBlockTurn = HeldMgcBlockTurn;
		held_item->fpBlockMove = HeldMgcBlockMove;
		break;
	case ITEM_TYPE_CONS:
		*held_item = HeldCons();
		held_item->fpTick = HeldConTick;
		held_item->fpDraw = HeldConDraw;
		held_item->fpOnEquip = HeldConOnEquip;
		held_item->fpGetLeftHandPos = HeldItemGetLeftHandPos;
		held_item->fpGetRightHandPos = HeldItemGetRightHandPos;
		held_item->fpBlockTurn = HeldItemBlockTurn;
		held_item->fpBlockMove = HeldItemBlockMove;
		break;
	}
}
void IndexFreeItem(btID id)
{
	if (block_item.used[id])
	{
		switch (block_item_data[id].type)
		{
		case ITEM_TYPE_EQUIP:
			buf_item_misc.remove(block_item_data[id].type_buffer_index);
			break;
		case ITEM_TYPE_WPN_MELEE:
			buf_item_melee.remove(block_item_data[id].type_buffer_index);
			break;
		case ITEM_TYPE_WPN_MATCHGUN:
			buf_item_gun.remove(block_item_data[id].type_buffer_index);
			break;
		case ITEM_TYPE_WPN_MAGIC:
			buf_item_mgc.remove(block_item_data[id].type_buffer_index);
			break;
		case ITEM_TYPE_CONS:
			buf_item_con.remove(block_item_data[id].type_buffer_index);
			break;
		}
	}
	block_item.remove(id);
}

PrjID MakePrjID(int i)
{
	PrjID h;
	h.id = (btID)i;
	return h;
}

//________________________________________________________________________________________________________________________________
// TEMP MATHS --------------------------------------------------------------------------------------------------------------------

// temp
#define CONV_RAD 0.01745329251994329576923690768489
#define CONV_DEG 57.295779513082320876798154814105

btf32 Random(btf32 min, btf32 max)
{
	//return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
	return min + (btf32)(rand()) / (((btf32)RAND_MAX) / (max - min));
};
inline btf32 Radians(btf32 value)
{
	return value * CONV_RAD;
}
inline btf32 Degrees(btf32 value)
{
	return value * CONV_DEG;
}

//________________________________________________________________________________________________________________________________
// INDEX -------------------------------------------------------------------------------------------------------------------------

mem::objbuf_caterpillar block_proj; // Projectile buffer
Projectile proj[BUF_SIZE];

Projectile* GetProj(PrjID id)
{
	return &(proj[id.id]);
}

void IndexInitialize()
{
	//
}

PrjID IndexSpawnProjectile()
{
	return MakePrjID(block_proj.add());
}

void IndexDestroyProjectileC(PrjID id)
{
	block_proj.remove(id.id);
}
