#include "objects_inventory.h"
#include "core.h"
#include "index.h"
#include "objects_items.h"

graphics::GUIText text_inventory_temp;
graphics::GUIBox guibox_selection;

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
	for (btui32 i = 0; i < items.Size(); ++i)
	{
		if (items.Used(i))
		{
			if (GetItemInstanceType(items[i]) == ITEM_TYPE_CONS)
			{
				if (((acv::ItemRecordCon*)acv::items[((HeldItem*)GetItemInstance(items[i]))->id_item_template])->id_projectile != ID_NULL)
				{
					// TODO: again, this is the fucking worst, i mean jus look at it...
					if (ammo_type == acv::projectiles[((acv::ItemRecordCon*)acv::items[((HeldItem*)GetItemInstance(items[i]))->id_item_template])->id_projectile].ammunition_type)
					{
						return items[i];
					}
				}
			}
		}
	}
	return ID_NULL;
}
void Inventory::Draw(btui16 active_slot)
{
	int p1_x_start = -(int)graphics::FrameSizeX() / 2;
	int p1_y_start = -(int)graphics::FrameSizeY() / 2;

	const bti32 invspace = 38;

	graphics::GUIText text;

	bti32 offset = p1_x_start + 96 - 16;
	for (btui16 i = 0; i < items.Size(); i++)
	{
		if (items.Used(i))
		{
			if (i == active_slot)
				graphics::DrawGUITexture(&acv::GetT(acv::items[GETITEMINST(items[i])->id_item_template]->id_icon), offset + i * invspace, p1_y_start + 30, 64, 64);
			else
				graphics::DrawGUITexture(&acv::GetT(acv::items[GETITEMINST(items[i])->id_item_template]->id_icon), offset + i * invspace, p1_y_start + 24, 64, 64);
		}
	}
	// Draw Count GUI on top
	for (btui16 i = 0; i < items.Size(); i++)
	{
		if (items.Used(i))
		{
			// get item type
			if (GetItemInstanceType(items[i]) == ITEM_TYPE_CONS)
			{
				char textbuffer[8];
				_itoa(GETITEMINST(items[i])->uses, textbuffer, 10);
				text.ReGen(textbuffer, offset + i * invspace - 16, offset + i * invspace + 32, p1_y_start + 20);
				text.Draw(&acv::GetT(acv::t_gui_font));
			}
		}
	}
	guibox_selection.ReGen((offset + active_slot * invspace) - 12, (offset + active_slot * invspace) + 12, p1_y_start + 12, p1_y_start + 36, 8, 8);
	guibox_selection.Draw(&acv::GetT(acv::t_gui_select_box));
}
