#include "objects_inventory.h"
#include "core.h"
#include "index.h"
#include "objects_items.h"

void Inventory::AddNew(btID item_template)
{
	btID id = core::SpawnItem(item_template);
	if (id != BUF_NULL)
	{
		items.Add(id);
	}
}
void Inventory::DestroyIndex(btui32 index)
{
	items.Remove(index);
	core::DestroyItem(items[index]);
}
void Inventory::DestroyID(btID id)
{
	for (btui32 i = 0; i < items.Size(); ++i)
	{
		if (items[i] == id) // if we already have a stack of this item
		{
			DestroyIndex(i);
			return;
		}
	}
}
void Inventory::Destroy(btID item_template)
{
	for (btui32 i = 0; i < items.Size(); ++i)
	{
		if (GETITEMINST(items[i])->id_item_template == item_template) // if we already have a stack of this item
		{
			DestroyIndex(i);
			return;
		}
	}
}
btui32 Inventory::TransferItemRecv(btID item_ID)
{
	return items.Add(item_ID); // Add the item without creating a new instance
}
void Inventory::TransferItemSendIndex(btui32 index)
{
	items.Remove(index); // Remove the item from inventory without deleting the instance
}
void Inventory::TransferItemSend(btID item_ID)
{
	for (btui32 i = 0; i < items.Size(); ++i)
	{
		if (items.Used(i))
		{
			if (items[i] == item_ID) // if we have this item instance
			{
				items.Remove(i); // Remove the item from inventory without deleting the instance
				return;
			}
		}
	}
}
btID Inventory::GetItemOfTemplate(btID item_template)
{
	for (btui32 i = 0; i < items.Size(); ++i)
		if (items.Used(i))
			if (((HeldItem*)GetItemInstance(items[i]))->id_item_template == item_template) // if we have this item instance
				return items[i];
	return ID_NULL;
}
btui32 Inventory::CountItemsOfTemplate(btID item_template)
{
	btui32 count = 0u;
	for (btui32 i = 0; i < items.Size(); ++i)
		if (items.Used(i))
			if (((HeldItem*)GetItemInstance(items[i]))->id_item_template == item_template) // if we have this item instance
				++count;
	return count;
}
btID Inventory::GetItemOfAmmunitionType(btui8 ammo_type)
{
	for (btui32 i = 0; i < items.Size(); ++i) {
		if (items.Used(i)) {
			if (GetItemInstanceType(items[i]) == ITEM_TYPE_CONS) {
				if (((acv::ItemRecordCon*)acv::items[((HeldItem*)GetItemInstance(items[i]))->id_item_template])->id_projectile != ID_NULL) {
					// TODO: again, this is the fucking worst, i mean jus look at it...
					if (ammo_type == acv::projectiles[((acv::ItemRecordCon*)acv::items[((HeldItem*)GetItemInstance(items[i]))->id_item_template])->id_projectile].ammunition_type) {
						return items[i];
					}
				}
			}
		}
	}
	return ID_NULL;
}
btui32 Inventory::GetFirstEmptySpace()
{
	// For all spaces in the inventory, even ones past the last used ID
	for (btui32 i = 0; i < 64; ++i) {
		if (!items.Used(i)) return i;
	}
	return ID_NULL;
}
