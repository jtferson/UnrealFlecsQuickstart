// Copyright 2021 Red J


#include "ECS/MainGameplay_Inits.h"
#include "ECS/MainGameplay_Components.h"


int32 CreateISMController(UWorld* InWorld, UStaticMesh* InMesh, UMaterialInterface* InMaterial, ISM_Map& InMap)
{
	auto hash = HashCombine(GetTypeHash(InMaterial), GetTypeHash(InMesh));

	auto find = InMap.ISMs.Find(hash);
	if (find == nullptr)
	{
		FVector Location = FVector::ZeroVector;
		FRotator Rotation = FRotator::ZeroRotator;
		FActorSpawnParameters SpawnInfo;

		auto controller = Cast<AISMController>(
			InWorld->SpawnActor(AISMController::StaticClass(), &Location, &Rotation, SpawnInfo));
		controller->Initialize(InMesh, InMaterial);

		InMap.ISMs.Add(hash, controller);
	}
	return hash;
}


void UMainGameplay_Inits::Initialize(flecs::world& ecs)
{
	ISM_Map ismMap{TMap<uint32, AISMController*>()};


	for (auto team : Config->Teams)
	{
		auto teamEntity = ecs.entity();

		auto spaceshipHash = CreateISMController(World, team->SpaceshipType->Mesh, team->SpaceshipType->Material,
		                                         ismMap);
		auto weaponHash = CreateISMController(World, team->SpaceshipType->Weapons->Mesh,
		                                      team->SpaceshipType->Weapons->Material, ismMap);

		auto projectilePrefab = ecs.prefab()
		                           .set_override<ProjectileLifetime>({team->SpaceshipType->Weapons->Lifetime})
		                           .add<ProjectileInstance>();

		if (team->SpaceshipType->Weapons->WeaponType != EWeaponType::Beam)
		{
			projectilePrefab.set<Speed>({team->SpaceshipType->Weapons->Speed});
		}


		auto spaceshipPrefab = ecs.prefab()
		                          .set<SpaceshipWeaponData>({
			                          projectilePrefab, weaponHash,
			                          team->SpaceshipType->Weapons->WeaponType == EWeaponType::Beam,
			                          team->SpaceshipType->Weapons->ProjectileScale,
			                          team->SpaceshipType->Weapons->BeamMeshLength
		                          })
		                          .set_override<SpaceshipWeaponCooldownTime>({team->SpaceshipType->Weapons->Cooldown, 0.f})
		                          .set_override<SpaceshipTarget>({flecs::entity::null()})
		                          .set<Speed>({team->SpaceshipType->MaxSpeed})
		                          .set_override<Transform>({FTransform(FVector::ZeroVector)})
		                          .set<BattleTeam>({teamEntity})
		                          .add<BoidInstance>();

		ecs.entity().set<BatchInstanceAdding>(
			{team->NumShips, spaceshipHash, spaceshipPrefab});
	}

	ecs.entity("Game")
	   .set<UWorldRef>({World})
	   .set<BoidSettings>({
		   Config->SeparationWeight, Config->CohesionWeight, Config->AlignmentWeight, Config->CageAvoidWeight,
		   Config->CellSize, Config->CageSize, Config->CageAvoidDistance
	   })
	   .set<GameSettings>({Config->SpawnRange, Config->ShootingCellSize})
	   .set<ISM_Map>(ismMap)
	   .set<TargetHashMap>({TMap<FIntVector, TArray<Data_TargetInstance>>{}});
}
