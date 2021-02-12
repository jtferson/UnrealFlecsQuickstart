// Copyright 2021 Red J

#pragma once
#include <unordered_map>

#include "CoreMinimal.h"
#include "Framework/FlecsModuleBase.h"
#include "Scene/ISMController.h"

#include "MainGameplay_Components.generated.h"

struct UWorldRef
{
	UWorld* Value;
};

struct Transform
{
	FTransform Value;
};

struct GameSettings
{
	FVector2D SpawnRange;
	float ShootingCellSize;
};

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


struct ISM_Map
{
	TMap<uint32, AISMController*> ISMs;
};

struct ISM_ControllerRef
{
	AISMController* Value;
};

struct ISM_Hash
{
	int32 Value;
};

struct ISM_Index
{
	int Value;
};

struct ISM_RemovedInstance
{
};






struct BatchInstanceAdding
{
	int Num;
	int32 Hash;
	flecs::entity Prefab;
};

struct ISM_AddInstance
{
	int32 Hash;
	flecs::entity Prefab;
	FTransform Transform;
};


//
struct SpaceshipWeaponCooldownTime
{
	float MaxValue;
	float CurrentValue;
	bool Initialized;
};

struct SpaceshipTarget
{
	flecs::entity Entity;
	FVector Position;
};

struct BoidInstance
{
};
struct ProjectileInstance
{
};

struct SpaceshipWeaponData
{
	flecs::entity ProjectilePrefab;
	int32 ProjectileHash;
	bool IsBeam;
	float ProjectileScale;
	float BeamMeshLength;
};

struct ProjectileLifetime
{
	float CurrentTime;
};

struct Speed
{
	float Value;
};

struct BattleTeam
{
	flecs::entity Value;
};

struct SystemQuery
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

struct TargetHashMap
{
	TMap<FIntVector, TArray<Data_TargetInstance>> Value;
};


UCLASS()
class MAINGAMEPLAY_API UMainGameplay_Components : public UFlecsModuleBase
{
	GENERATED_BODY()

	virtual void Initialize(flecs::world& ecs) override;
};
