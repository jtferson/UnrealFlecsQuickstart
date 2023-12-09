// Copyright 2021 Red J

#pragma once

#include "CoreMinimal.h"

#include "flecs.h"
#include "UnrealFlecsSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UNREALFLECS_API UUnrealFlecsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	flecs::world* GetEcsWorld() const;
protected:
	FDelegateHandle OnTickHandle;

	flecs::world* ECSWorld = nullptr;
	
private:
    void Tick(UWorld* World, ELevelTick LevelTick, float DeltaTime);
};
