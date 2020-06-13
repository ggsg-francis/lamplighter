#pragma once

#include "global.h"
#include "objects_inventory.h"

/*
static activators are static items in the world that can be interacted with
they should bring up a menu which allows a variety of different options
*/

enum ActivatorType
{
	eATR_CONTAINER,
};

enum ActivatorBits
{
	ATR_USE_INVENTORY = 1u << 8u,
	ATR_2 = 1u << 1u,
};

typedef struct StaticActivator
{
	mem::bv<btui64, ActivatorBits> bits;
	Inventory inv;
	WCoord homePosition;
} StaticActivator;

//struct ActContainer
//{
//	int test;
//	Inventory inv;
//};
//
//struct StaticActivator
//{
//	void* data;
//	StaticActivator(ActivatorType type)
//	{
//		switch (type)
//		{
//		case eATR_CONTAINER:
//			data = new ActContainer;
//			break;
//		}
//	}
//	~StaticActivator()
//	{
//		delete data;
//	}
//	WCoord homePosition;
//};
