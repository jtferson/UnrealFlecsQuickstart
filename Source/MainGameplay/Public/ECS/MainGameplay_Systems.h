// Copyright 2021 Red J

#pragma once

#include "CoreMinimal.h"

#include "Framework/FlecsModuleBase.h"
#include "MainGameplay_Systems.generated.h"



/**
 * 
 */
UCLASS()
class MAINGAMEPLAY_API UMainGameplay_Systems : public UFlecsModuleBase
{
	GENERATED_BODY()

	virtual void Initialize(flecs::world& ecs) override;
};
