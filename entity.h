#ifndef ENTITY_H
#define ENTITY_H

#include "global.h"

struct EntityComponent {
	virtual char* Name();
	virtual void Tick();
	virtual void Draw();
	virtual void RFile(void* file, btui32 version);
	virtual void WFile(void* file, btui32 version);
};

struct Entity {
	// Contains the data of this entity
	EntityComponent* component;
	template <typename ComponentType> void Init();
	void End();
};
template <typename ComponentType> inline void Entity::Init() {
	component = new ComponentType;
}
inline void Entity::End() {
	delete component;
}

#endif
