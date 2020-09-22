#ifndef INVENTORY
#define INVENTORY

#include "global.h"
#include "memory.hpp"

class Inventory
{
public:
	mem::Buffer64<btID> items;
public:
	void AddNew(btID item_template);
	void DestroyIndex(btui32 index);
	void DestroyID(btID item_id);
	void Destroy(btID item_template);
	btui32 TransferItemRecv(btID item_id);
	void TransferItemSendIndex(btui32 index);
	void TransferItemSend(btID item_id);
	//btID GetItemOfType(ItemType TYPE);
	btID GetItemOfTemplate(btID item_template);
	btui32 CountItemsOfTemplate(btID item_template);
	btID GetItemOfAmmunitionType(btui8 ammo_type);
	btui32 GetFirstEmptySpace();
};

#endif // !INVENTORY
