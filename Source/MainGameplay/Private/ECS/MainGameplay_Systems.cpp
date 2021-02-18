// Copyright 2021 Red J

#include "ECS/MainGameplay_Systems.h"
#include "ECS/MainGameplay_Components.h"

void SystemSpawnInstancesInRadius(flecs::iter& It)
{
	auto ecs = It.world();
	auto cBatch = It.column<BatchInstanceAdding>(1);
	auto cMap = It.column<ISM_Map>(2);
	auto cGameSettings = It.column<GameSettings>(3);

	for (auto i : It)
	{
		auto batch = cBatch[i];
		auto controller = *(cMap->ISMs.Find(batch.Hash));
		if (controller != nullptr)
		{
			for (auto j = 0; j < batch.Num; j++)
			{
				auto instanceIndex = controller->AddInstance();
				float spawnRadius = FMath::RandRange(cGameSettings->SpawnRange.X, cGameSettings->SpawnRange.Y);
				FVector direction = FMath::VRand();
				auto pos = direction * spawnRadius;
				FTransform transformValue = FTransform{FVector{pos.X, pos.Y, pos.Z}};
				ecs.entity()
				   .add_instanceof(batch.Prefab)
				   .set<ISM_ControllerRef>({controller})
				   .set<ISM_Index>({instanceIndex})
				   .set<ISM_Hash>({batch.Hash})
				   .set<Transform>({transformValue});
			}
			controller->CreateOrExpandTransformArray();
		}
		It.entity(i).destruct();
	}
}

void SystemAddInstance(flecs::iter& It)
{
	auto ecs = It.world();
	auto cAdd = It.column<ISM_AddInstance>(1);
	auto cMap = It.column<ISM_Map>(2);

	for (auto i : It)
	{
		auto controller = *(cMap->ISMs.Find(cAdd[i].Hash));
		if (controller != nullptr)
		{
			auto instanceIndex = controller->AddInstance();
			ecs.entity()
			   .add_instanceof(cAdd[i].Prefab)
			   .set<ISM_ControllerRef>({controller})
			   .set<ISM_Index>({instanceIndex})
			   .set<ISM_Hash>({cAdd[i].Hash})
			   .set<Transform>({cAdd[i].Transform});
		}
		It.entity(i).destruct();
	}

	for (auto& data : cMap->ISMs)
	{
		data.Value->CreateOrExpandTransformArray();
	}
}

void SystemRemoveInstance(flecs::iter& It)
{
	auto cHash = It.column<ISM_Hash>(1);
	auto cIndex = It.column<ISM_Index>(2);
	auto cMap = It.column<ISM_Map>(3);

	for (auto i : It)
	{
		auto hash = cHash[i];
		auto controller = *(cMap->ISMs.Find(hash.Value));
		if (controller != nullptr)
		{
			controller->RemoveInstance(cIndex[i].Value);
		}
		It.entity(i).destruct();
	}
}

void SystemCopyInstanceTransforms(flecs::iter& It)
{
	auto cTransform = It.column<Transform>(1);
	auto cISMIndex = It.column<ISM_Index>(2);
	auto cISMController = It.column<ISM_ControllerRef>(3);

	for (auto i : It)
	{
		auto index = cISMIndex[i].Value;
		cISMController[i].Value->SetTransform(index, cTransform[i].Value);
	}
}

void SystemUpdateTransformsInBatch(flecs::iter& It)
{
	auto cMap = It.column<ISM_Map>(1);
	for (auto& data : cMap->ISMs)
	{
		data.Value->BatchUpdateTransform();
	}
}


void SystemUpdateBoids(flecs::iter& It)
{
	auto cTransform = It.column<Transform>(1);
	auto cBoidSettings = It.column<BoidSettings>(2);
	auto cSpeed = It.column<Speed>(3);

	TMap<FIntVector, TArray<int>> hashMap;
	TArray<FVector> cellPositions;
	cellPositions.SetNumUninitialized(It.count());
	TArray<FVector> cellAlignment;
	cellAlignment.SetNumUninitialized(It.count());
	TArray<int> cellBoidCount;
	cellBoidCount.SetNumZeroed(It.count());
	TArray<int> cellIndices;
	cellIndices.SetNumUninitialized(It.count());


	for (auto i : It)
	{
		auto location = cTransform[i].Value.GetLocation();
		FIntVector hashedVector = FIntVector(location / cBoidSettings->CellSize);

		auto entityIndices = hashMap.Find(hashedVector);
		if (!entityIndices)
		{
			TArray<int> newEntityIndices;

			newEntityIndices.Add(i);
			hashMap.Emplace(hashedVector, std::move(newEntityIndices));
		}
		else
		{
			entityIndices->Add(i);
		}


		cellPositions[i] = location;
		cellAlignment[i] = cTransform[i].Value.GetRotation().GetForwardVector();
	}

	// Merge Cells
	for (auto& hashedData : hashMap)
	{
		if (hashedData.Value.Num() > 0)
		{
			auto cellIndex = hashedData.Value[0];
			cellIndices[cellIndex] = cellIndex;
			cellBoidCount[cellIndex] = 1;

			for (auto i = 1; i < hashedData.Value.Num(); i++)
			{
				auto index = hashedData.Value[i];
				cellIndices[index] = cellIndex;
				cellBoidCount[cellIndex] += 1;
				cellPositions[cellIndex] += cellPositions[index];
				cellAlignment[cellIndex] += cellAlignment[index];
			}
		}
	}


	for (auto boidIndex : It)
	{
		auto transform = cTransform[boidIndex].Value;
		auto boidPosition = transform.GetLocation();
		auto boidForward = transform.GetRotation().GetForwardVector();
		int cellIndex = cellIndices[boidIndex];

		int nearbyBoidCount = cellBoidCount[cellIndex] - 1;

		FVector force = FVector::ZeroVector;

		if (nearbyBoidCount > 0)
		{
			auto positionSum = cellPositions[cellIndex] - boidPosition;
			auto alignmentSum = cellAlignment[cellIndex] - boidForward;

			auto averagePosition = positionSum / nearbyBoidCount;

			float distToAveragePositionSq = FVector::DistSquared(averagePosition, boidPosition);
			float maxDistToAveragePositionSq = cBoidSettings->CellSize * cBoidSettings->CellSize;

			float distanceNormalized = distToAveragePositionSq / maxDistToAveragePositionSq;
			float needToLeave = FMath::Max(1 - distanceNormalized, 0.f);

			FVector toAveragePosition = (averagePosition - boidPosition).GetSafeNormal();
			auto averageHeading = alignmentSum / nearbyBoidCount;

			force += -toAveragePosition * cBoidSettings->SeparationWeight * needToLeave;
			force += toAveragePosition * cBoidSettings->CohesionWeight;
			force += averageHeading * cBoidSettings->AlignmentWeight;
		}

		if (FMath::Min(FMath::Min(
			               (cBoidSettings->CageSize / 2.f) - FMath::Abs(boidPosition.X),
			               (cBoidSettings->CageSize / 2.f) - FMath::Abs(boidPosition.Y)),
		               (cBoidSettings->CageSize / 2.f) - FMath::Abs(boidPosition.Z))
			< cBoidSettings->CageAvoidDistance)
		{
			force += -boidPosition.GetSafeNormal() * cBoidSettings->CageAvoidWeight;
		}

		FVector velocity = transform.GetRotation().GetForwardVector();
		velocity += force * It.delta_time();
		velocity = velocity.GetSafeNormal() * cSpeed->Value;
		transform.SetLocation(transform.GetLocation() + velocity * It.delta_time());

		auto rotator = FRotationMatrix::MakeFromX(velocity.GetSafeNormal()).Rotator();
		transform.SetRotation(rotator.Quaternion());

		cTransform[boidIndex].Value = transform;
	}
}


void SystemUpdateTargetHashMap(flecs::iter& It)
{
	auto cTargets = It.column<TargetHashMap>(1);
	auto cGameSettings = It.column<GameSettings>(2);
	auto sQuery = It.column<SystemQuery>(3);

	cTargets->Value.Empty();

	sQuery->Value.action([&](flecs::iter& qIt)
	{
		auto cTransform = qIt.column<Transform>(1);
		auto cCooldown = qIt.column<SpaceshipWeaponCooldownTime>(2);
		auto cTeam = qIt.column<BattleTeam>(3);

		for (auto i : qIt)
		{
			auto location = cTransform[i].Value.GetLocation();
			FIntVector hashedVector = FIntVector(location / (cGameSettings->ShootingCellSize));

			Data_TargetInstance data{qIt.entity(i), location, cTeam->Value, cCooldown[i].CurrentValue <= 0};

			auto entityData = cTargets->Value.Find(hashedVector);
			if (!entityData)
			{
				TArray<Data_TargetInstance> newEntityData;

				newEntityData.Add(data);
				cTargets->Value.Emplace(hashedVector, std::move(newEntityData));
			}
			else
			{
				entityData->Add(data);
			}
		}
	});
}


void SystemSearchNewTargets(flecs::iter& It)
{
	auto ecs = It.world();
	auto cTargets = It.column<TargetHashMap>(1);

	for (auto& hashedData : cTargets->Value)
	{
		//This naive approach is not so bad because ships are searching their targets only when they can attack
		for (auto i = 0; i < hashedData.Value.Num(); i++)
		{
			if (!hashedData.Value[i].CanAttack) continue;

			FVector attackerPosition = hashedData.Value[i].Position;
			flecs::entity nearestTarget = flecs::entity::null();
			float minDistance = std::numeric_limits<float>::max();
			FVector positionOfTarget = FVector::ZeroVector;

			for (auto j = 0; j < hashedData.Value.Num(); j++)
			{
				if (i == j) continue;
				if (hashedData.Value[i].Team.id() == hashedData.Value[j].Team.id()) continue;

				FVector targetPosition = hashedData.Value[j].Position;
				float distance = FVector::DistSquared(attackerPosition, targetPosition);
				if (distance < minDistance)
				{
					minDistance = distance;
					nearestTarget = hashedData.Value[j].Entity;
					positionOfTarget = targetPosition;
				}
			}
			hashedData.Value[i].Entity.set<SpaceshipTarget>({nearestTarget, positionOfTarget});
		}
	}
}


void SystemSpawnProjectiles(flecs::iter& It)
{
	auto ecs = It.world();

	auto cTarget = It.column<SpaceshipTarget>(1);
	auto cCooldown = It.column<SpaceshipWeaponCooldownTime>(2);
	auto cWeaponData = It.column<SpaceshipWeaponData>(3);
	auto cTransform = It.column<Transform>(4);

	for (auto i : It)
	{
		if (cTarget[i].Entity.id() != flecs::entity::null().id())
		{
			FTransform projectileTransform = cTransform[i].Value;
			FVector direction = (cTarget[i].Position - projectileTransform.GetLocation()).GetSafeNormal();
			FRotator rotation = direction.ToOrientationRotator();
			FVector scale = FVector(1);

			if (cWeaponData->IsBeam)
			{
				auto distance = FVector::Distance(cTarget[i].Position, projectileTransform.GetLocation());
				auto xScale = distance / cWeaponData->BeamMeshLength;
				scale = FVector(xScale, cWeaponData->ProjectileScale, cWeaponData->ProjectileScale);
			}
			else
			{
				scale = FVector(cWeaponData->ProjectileScale);
			}
			projectileTransform.SetScale3D(scale);
			projectileTransform.SetRotation(rotation.Quaternion());

			ecs.entity().set<ISM_AddInstance>(
				{cWeaponData->ProjectileHash, cWeaponData->ProjectilePrefab, projectileTransform});

			cCooldown[i].CurrentValue = cCooldown[i].MaxValue * FMath::RandRange(0.5f, 1.5f);
			cTarget[i].Entity = flecs::entity::null();
		}
	}
}

void SystemMoveProjectiles(flecs::iter& It)
{
	auto cTransform = It.column<Transform>(1);
	auto cSpeed = It.column<Speed>(2);

	for (auto i : It)
	{
		auto velocity = cTransform[i].Value.GetRotation().GetForwardVector() * cSpeed->Value * It.delta_time();
		cTransform[i].Value.SetLocation(cTransform[i].Value.GetLocation() + velocity);
	}
}


void SystemComputeCooldownTime(flecs::iter& It)
{
	auto cCooldown = It.column<SpaceshipWeaponCooldownTime>(1);
	for (auto i : It)
	{
		if (!cCooldown[i].Initialized)
		{
			//We want to have a more unevenly distributed initial time for shooting
			cCooldown[i].CurrentValue = FMath::RandRange(0.f, cCooldown[i].MaxValue);
			cCooldown[i].Initialized = true;
		}

		if (cCooldown[i].CurrentValue > 0)
		{
			cCooldown[i].CurrentValue -= It.delta_time();
		}
		else
		{
			cCooldown[i].CurrentValue = 0;
		}
	}
}

void SystemCheckProjectileLifetime(flecs::iter& It)
{
	auto cLifetime = It.column<ProjectileLifetime>(1);
	for (auto i : It)
	{
		if (cLifetime[i].CurrentTime > 0)
		{
			cLifetime[i].CurrentTime -= It.delta_time();

			if (cLifetime[i].CurrentTime <= 0)
			{
				It.entity(i).add<ISM_RemovedInstance>();
			}
		}
	}
}

void UMainGameplay_Systems::Initialize(flecs::world& ecs)
{
	flecs::system<>(ecs, "SystemSpawnInstancesInRadius")
		.signature("BatchInstanceAdding, Game:ISM_Map, Game:GameSettings")
		.iter(SystemSpawnInstancesInRadius);
	
	flecs::system<>(ecs, "SystemAddInstance")
		.signature("ISM_AddInstance, Game:ISM_Map")
		.iter(SystemAddInstance);
	
	flecs::system<>(ecs, "SystemRemoveInstance")
		.signature("ISM_Hash, ISM_Index, Game:ISM_Map, ISM_RemovedInstance")
		.iter(SystemRemoveInstance);
	
	flecs::system<>(ecs, "SystemCopyInstanceTransforms")
		.signature("Transform, ISM_Index, ISM_ControllerRef")
		.iter(SystemCopyInstanceTransforms);
	
	flecs::system<>(ecs, "SystemUpdateTransformsInBatch")
		.signature("Game:ISM_Map")
		.iter(SystemUpdateTransformsInBatch);
	
	flecs::system<>(ecs, "SystemUpdateBoids")
		.signature("Transform, Game:BoidSettings, SHARED:Speed, BoidInstance")
		.iter(SystemUpdateBoids);
	
	flecs::system<>(ecs, "SystemSpawnProjectiles")
		.signature("SpaceshipTarget, SpaceshipWeaponCooldownTime, SHARED:SpaceshipWeaponData, Transform")
		.iter(SystemSpawnProjectiles);
	
	flecs::system<>(ecs, "SystemComputeCooldownTime")
		.signature("SpaceshipWeaponCooldownTime")
		.iter(SystemComputeCooldownTime);
	
	auto targetsQuery = flecs::query<>(ecs, "Transform, SpaceshipWeaponCooldownTime, SHARED:BattleTeam");
	flecs::system<>(ecs, "SystemUpdateTargetHashMap", "Game:TargetHashMap, Game:GameSettings, SYSTEM:SystemQuery")
		.iter(SystemUpdateTargetHashMap)
		.set<SystemQuery>({targetsQuery});
	
	flecs::system<>(ecs, "SystemSearchNewTargets")
		.signature("Game:TargetHashMap")
		.iter(SystemSearchNewTargets);
	
	flecs::system<>(ecs, "SystemMoveProjectiles")
		.signature("Transform, SHARED:Speed, ProjectileInstance")
		.iter(SystemMoveProjectiles);
	
	flecs::system<>(ecs, "SystemCheckProjectileLifetime")
		.signature("ProjectileLifetime")
		.iter(SystemCheckProjectileLifetime);
}
