// Copyright 2021 Red J

#pragma once

#include "CoreMinimal.h"

#include "MainGameplayBootstrap.h"
#include "Framework/FlecsModuleBase.h"
#include "MainGameplay_Inits.generated.h"

/**
 * 
 */
UCLASS()
class MAINGAMEPLAY_API UMainGameplay_Inits : public UFlecsModuleBase, public IGameplayConfigSet, public IWorldSet
{
	GENERATED_BODY()

	virtual void Initialize(flecs::world& ecs) override;
};
