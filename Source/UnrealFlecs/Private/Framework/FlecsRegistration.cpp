#include "Framework/FlecsRegistration.h"

TArray<void(*)(flecs::world&)>& FlecsRegContainer::GetFlecsRegs()
{
	static TArray<void(*)(flecs::world&)> instance;
	return instance;
}