#include "Framework/FlecsRegistration.h"

namespace FlecsGlobals
{
	TArray<void (*)(flecs::world&)> FlecsRegs;
}