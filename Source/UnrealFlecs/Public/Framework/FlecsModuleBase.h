#pragma once
#include "FlecsLibrary/Public/flecs.h"
#include "FlecsModuleBase.generated.h"

UCLASS(Abstract)
class UNREALFLECS_API UFlecsModuleBase : public UObject
{
	GENERATED_BODY()
public:
	virtual void Initialize(flecs::world& ecs);
};
