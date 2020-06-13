#ifndef INVENTORY
#define INVENTORY

#include "global.h"
#include "memory.hpp"

class Inventory
{
public:
	mem::Buffer64<btID> items;
public:
	void AddNew(btID ITEM_TEMPLATE);
	void DestroyIndex(btui32 INDEX);
	void DestroyID(btID ITEM_ID);
	void Destroy(btID ITEM_TEMPLATE);
	btui32 TransferItemRecv(btID ITEM_ID);
	void TransferItemSendIndex(btui32 INDEX);
	void TransferItemSend(btID ITEM_ID);
	//btID GetItemOfType(ItemType TYPE);
	btID GetItemOfTemplate(btID ITEM_TEMPLATE);
	btID GetItemOfAmmunitionType(btui8 AMMO_TYPE);
	void Draw(btui16 ACTIVE_SLOT);
};

#endif // !INVENTORY
