// Copyright 2021 Red J

#pragma once
#include <unordered_map>

#include "CoreMinimal.h"
#include "Framework/FlecsModuleBase.h"
#include "Framework/FlecsRegistration.h"
#include "Scene/ISMController.h"

#include "MainGameplay_Components.generated.h"

FLECS_COMPONENT(UWorldRef)
{
	UWorld* Value;
};

FLECS_COMPONENT(Transform)
{
	FTransform Value;
};

FLECS_COMPONENT(GameSettings)
{
	FVector2D SpawnRange;
	float ShootingCellSize;
};

FLECS_COMPONENT(BoidSettings)
{
	float SeparationWeight;
	float CohesionWeight;
	float AlignmentWeight;
	float CageAvoidWeight;
	float CellSize;
	float CageSize;
	float CageAvoidDistance;
};

FLECS_COMPONENT(ISM_Map)
{
	TMap<uint32, AISMController*> ISMs;
};

FLECS_COMPONENT(ISM_ControllerRef)
{
	AISMController* Value;
};

FLECS_COMPONENT(ISM_Hash)
{
	int32 Value;
};

FLECS_COMPONENT(ISM_Index)
{
	int Value;
};

FLECS_COMPONENT(ISM_RemovedInstance)
{
};






FLECS_COMPONENT(BatchInstanceAdding)
{
	int Num;
	int32 Hash;
	flecs::entity Prefab;
};

FLECS_COMPONENT(ISM_AddInstance)
{
	int32 Hash;
	flecs::entity Prefab;
	FTransform Transform;
};


//
FLECS_COMPONENT(SpaceshipWeaponCooldownTime)
{
	float MaxValue;
	float CurrentValue;
	bool Initialized;
};

FLECS_COMPONENT(SpaceshipTarget)
{
	flecs::entity Entity;
	FVector Position;
};

FLECS_COMPONENT(BoidInstance)
{
};

FLECS_COMPONENT(ProjectileInstance)
{
};

FLECS_COMPONENT(SpaceshipWeaponData)
{
	flecs::entity ProjectilePrefab;
	int32 ProjectileHash;
	bool IsBeam;
	float ProjectileScale;
	float BeamMeshLength;
};

FLECS_COMPONENT(ProjectileLifetime)
{
	float CurrentTime;
};

FLECS_COMPONENT(Speed)
{
	float Value;
};

FLECS_COMPONENT(BattleTeam)
{
	flecs::entity Value;
};

FLECS_COMPONENT(SystemQuery)
{
	flecs::query<> Value;
};


struct Data_TargetInstance
{
	flecs::entity Entity;
	FVector Position;
	flecs::entity Team;
	bool CanAttack;
};

FLECS_COMPONENT(TargetHashMap)
{
	TMap<FIntVector, TArray<Data_TargetInstance>> Value;
};

UCLASS()
class MAINGAMEPLAY_API UMainGameplay_Components : public UFlecsModuleBase
{
	GENERATED_BODY()

	virtual void Initialize(flecs::world& ecs) override;
};
