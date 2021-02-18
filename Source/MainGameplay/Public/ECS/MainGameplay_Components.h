// Copyright 2021 Red J

#pragma once
#include <unordered_map>

#include "CoreMinimal.h"
#include "Framework/FlecsModuleBase.h"
#include "Framework/FlecsRegistration.h"
#include "Scene/ISMController.h"

#include "MainGameplay_Components.generated.h"

struct UWorldRef
{
	UWorld* Value;
};
FLECS_COMPONENT(UWorldRef)

struct Transform
{
	FTransform Value;
};
FLECS_COMPONENT(Transform)

struct GameSettings
{
	FVector2D SpawnRange;
	float ShootingCellSize;
};
FLECS_COMPONENT(GameSettings)

struct BoidSettings
{
	float SeparationWeight;
	float CohesionWeight;
	float AlignmentWeight;
	float CageAvoidWeight;
	float CellSize;
	float CageSize;
	float CageAvoidDistance;
};
FLECS_COMPONENT(BoidSettings)


struct ISM_Map
{
	TMap<uint32, AISMController*> ISMs;
};
FLECS_COMPONENT(ISM_Map)

struct ISM_ControllerRef
{
	AISMController* Value;
};
FLECS_COMPONENT(ISM_ControllerRef)

struct ISM_Hash
{
	int32 Value;
};
FLECS_COMPONENT(ISM_Hash)

struct ISM_Index
{
	int Value;
};
FLECS_COMPONENT(ISM_Index)

struct ISM_RemovedInstance
{
};
FLECS_COMPONENT(ISM_RemovedInstance)






struct BatchInstanceAdding
{
	int Num;
	int32 Hash;
	flecs::entity Prefab;
};
FLECS_COMPONENT(BatchInstanceAdding)

struct ISM_AddInstance
{
	int32 Hash;
	flecs::entity Prefab;
	FTransform Transform;
};
FLECS_COMPONENT(ISM_AddInstance)


//
struct SpaceshipWeaponCooldownTime
{
	float MaxValue;
	float CurrentValue;
	bool Initialized;
};
FLECS_COMPONENT(SpaceshipWeaponCooldownTime)

struct SpaceshipTarget
{
	flecs::entity Entity;
	FVector Position;
};
FLECS_COMPONENT(SpaceshipTarget)

struct BoidInstance
{
};
FLECS_COMPONENT(BoidInstance)

struct ProjectileInstance
{
};
FLECS_COMPONENT(ProjectileInstance)

struct SpaceshipWeaponData
{
	flecs::entity ProjectilePrefab;
	int32 ProjectileHash;
	bool IsBeam;
	float ProjectileScale;
	float BeamMeshLength;
};
FLECS_COMPONENT(SpaceshipWeaponData)

struct ProjectileLifetime
{
	float CurrentTime;
};
FLECS_COMPONENT(ProjectileLifetime)

struct Speed
{
	float Value;
};
FLECS_COMPONENT(Speed)

struct BattleTeam
{
	flecs::entity Value;
};
FLECS_COMPONENT(BattleTeam)

struct SystemQuery
{
	flecs::query<> Value;
};
FLECS_COMPONENT(SystemQuery)


struct Data_TargetInstance
{
	flecs::entity Entity;
	FVector Position;
	flecs::entity Team;
	bool CanAttack;
};

struct TargetHashMap
{
	TMap<FIntVector, TArray<Data_TargetInstance>> Value;
};
FLECS_COMPONENT(TargetHashMap)

UCLASS()
class MAINGAMEPLAY_API UMainGameplay_Components : public UFlecsModuleBase
{
	GENERATED_BODY()

	virtual void Initialize(flecs::world& ecs) override;
};
