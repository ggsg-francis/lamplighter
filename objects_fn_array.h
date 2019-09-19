#pragma once

namespace etype
{
	enum etype : btui8
	{
		entity,
		actor,
		chara,
		eitem,
	};
}

namespace index // Move all this stuff out of index namespace plz
{
	// Tick forward declarations
	void TickEntity(btf32, btID);
	void TickChara(btf32, btID);
	void TickItem(btf32, btID);

	// Function pointers for different entity types
	void(*const tick[])(btf32, btID) = {
		TickEntity,
		TickEntity, // Actor
		TickChara,
		TickItem,
	};

	// Draw forward declarations
	void DrawChara(btID);
	void DrawItem(btID);

	// Function pointers for different entity types
	void(*const draw[])(btID) = {
		DrawChara, // Entity
		DrawChara, // Actor
		DrawChara, // Chara
		DrawItem,
	};
}
