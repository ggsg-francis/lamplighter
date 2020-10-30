#ifndef INVENTORY
#define INVENTORY

#include "global.h"
#include "memory.hpp"

class Inventory
{
public:
	mem::Buffer64<lid> items;
public:
	void AddNew(lid item_template);
	void DestroyIndex(lui32 index);
	void DestroyID(lid item_id);
	void Destroy(lid item_template);
	lui32 TransferItemRecv(lid item_id);
	void TransferItemSendIndex(lui32 index);
	void TransferItemSend(lid item_id);
	//lid GetItemOfType(ItemType TYPE);
	lid GetItemOfTemplate(lid item_template);
	lui32 CountItemsOfTemplate(lid item_template);
	lid GetItemOfAmmunitionType(lui8 ammo_type);
	lui32 GetFirstEmptySpace();
};

#endif // !INVENTORY
