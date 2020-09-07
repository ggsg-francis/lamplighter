#include "entity.h"

char* EntityComponent::Name() { return "BaseEnt"; };
void EntityComponent::Tick() {};
void EntityComponent::Draw() {};
void EntityComponent::RFile(void* file, btui32 version) {};
void EntityComponent::WFile(void* file, btui32 version) {};
