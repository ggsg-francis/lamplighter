#ifndef INVENTORY
#define INVENTORY

#include "global.h"
#include "memory.hpp"

class Inventory
{
public:
	mem::Buffer64<LtrID> items;
public:
	void AddNew(ID16 item_template);
	void DestroyIndex(lui32 index);
	void DestroyID(LtrID item_id);
	void Destroy(ID16 item_template);
	lui32 TransferItemRecv(LtrID item_id);
	void TransferItemSendIndex(lui32 index);
	void TransferItemSend(LtrID item_id);
	//lid GetItemOfType(ItemType TYPE);
	LtrID GetItemOfTemplate(ID16 item_template);
	lui32 CountItemsOfTemplate(ID16 item_template);
	LtrID GetItemOfAmmunitionType(lui8 ammo_type);
	lui32 GetFirstEmptySpace();
};

#endif // !INVENTORY
