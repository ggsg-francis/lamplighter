#include "objects_inventory.h"
#include "core.h"
#include "index.h"
#include "objects_items.h"

void Inventory::AddNew(ID16 item_template)
{
	LtrID id = core::SpawnItem(item_template);
	if (IDCHECK(id)) {
		items.Add(id);
	}
}
void Inventory::DestroyIndex(lui32 index)
{
	items.Remove(index);
	core::DestroyItem(items[index]);
}
void Inventory::DestroyID(LtrID id)
{
	for (lui32 i = 0; i < items.Size(); ++i)
	{
		if (items[i].GUID() == id.GUID()) // if we already have a stack of this item
		{
			DestroyIndex(i);
			return;
		}
	}
}
void Inventory::Destroy(ID16 item_template)
{
	for (lui32 i = 0; i < items.Size(); ++i)
	{
		if (GETITEMINST(items[i])->id_item_template == item_template) // if we already have a stack of this item
		{
			DestroyIndex(i);
			return;
		}
	}
}
lui32 Inventory::TransferItemRecv(LtrID item_ID)
{
	return items.Add(item_ID); // Add the item without creating a new instance
}
void Inventory::TransferItemSendIndex(lui32 index)
{
	items.Remove(index); // Remove the item from inventory without deleting the instance
}
void Inventory::TransferItemSend(LtrID item_ID)
{
	for (lui32 i = 0; i < items.Size(); ++i)
	{
		if (items.Used(i))
		{
			if (items[i].GUID() == item_ID.GUID()) // if we have this item instance
			{
				items.Remove(i); // Remove the item from inventory without deleting the instance
				return;
			}
		}
	}
}
LtrID Inventory::GetItemOfTemplate(ID16 item_template)
{
	for (lui32 i = 0; i < items.Size(); ++i)
		if (items.Used(i))
			if (((HeldItem*)GetItemInstance(items[i].Index()))->id_item_template == item_template) // if we have this item instance
				return items[i];
	return ID2_NULL;
}
lui32 Inventory::CountItemsOfTemplate(ID16 item_template)
{
	lui32 count = 0u;
	for (lui32 i = 0; i < items.Size(); ++i)
		if (items.Used(i))
			if (((HeldItem*)GetItemInstance(items[i].Index()))->id_item_template == item_template) // if we have this item instance
				++count;
	return count;
}
LtrID Inventory::GetItemOfAmmunitionType(lui8 ammo_type)
{
	for (lui32 i = 0; i < items.Size(); ++i) {
		if (items.Used(i)) {
			if (GetItemInstanceType(items[i].Index()) == ITEM_TYPE_CONS) {
				if (((acv::ItemRecordCon*)acv::items[((HeldItem*)GetItemInstance(items[i].Index()))->id_item_template])->id_projectile != ID_NULL) {
					// TODO: again, this is the fucking worst, i mean jus look at it...
					if (ammo_type == acv::projectiles[((acv::ItemRecordCon*)acv::items[((HeldItem*)GetItemInstance(items[i].Index()))->id_item_template])->id_projectile].ammunition_type) {
						return items[i];
					}
				}
			}
		}
	}
	return ID2_NULL;
}
lui32 Inventory::GetFirstEmptySpace()
{
	// For all spaces in the inventory, even ones past the last used ID
	for (lui32 i = 0; i < 64; ++i) {
		if (!items.Used(i)) return i;
	}
	return ID_NULL;
}
