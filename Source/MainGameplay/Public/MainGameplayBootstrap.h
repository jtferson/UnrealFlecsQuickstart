// Copyright 2021 Red J

#pragma once

#include "CoreMinimal.h"

#include "Framework/FlecsBootstrap.h"
#include "Framework/FlecsModuleBase.h"
#include "Data/GameplayConfig.h"

#include "MainGameplayBootstrap.generated.h"

UINTERFACE(MinimalAPI)
class UGameplayConfigSet : public UInterface
{    
	GENERATED_BODY()
};
class MAINGAMEPLAY_API IGameplayConfigSet
{
	GENERATED_BODY()
public:
	virtual void SetConfig(UGameplayConfig* InConfig);

	UGameplayConfig* Config = nullptr;
};


UINTERFACE(MinimalAPI)
class UWorldSet : public UInterface
{    
	GENERATED_BODY()
};
class MAINGAMEPLAY_API IWorldSet
{
	GENERATED_BODY()
	public:
	virtual void SetWorld(UWorld* InWorld);

	UWorld* World = nullptr;
};


UCLASS()
class MAINGAMEPLAY_API AMainGameplayBootstrap : public AFlecsBootstrap
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UGameplayConfig* Config;
	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<UFlecsModuleBase>> FlecsModules;
protected:
	virtual void Bootstrap(flecs::world& ecs) override;
};
