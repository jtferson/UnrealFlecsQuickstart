// Copyright 2021 Red J

#include "ECS/MainGameplay_Systems.h"
#include "ECS/MainGameplay_Components.h"

void SystemSpawnInstancesInRadius(flecs::iter& It)
{
	auto ecs = It.world();
	auto cBatch = It.field<BatchInstanceAdding>(1);
	auto map = It.field<const ISM_Map>(2); //flecs::IN
	auto gameSettings = It.field<const GameSettings>(3); //flecs::IN

	for (auto i : It)
	{
		auto batch = cBatch[i];
		auto controller = *map->ISMs.Find(batch.Hash);
		if (controller != nullptr)
		{
			for (auto j = 0; j < batch.Num; j++)
			{
				auto instanceIndex = controller->AddInstance();
				float spawnRadius = FMath::RandRange(gameSettings->SpawnRange.X, gameSettings->SpawnRange.Y);
				FVector direction = FMath::VRand();
				auto pos = direction * spawnRadius;
				FTransform transformValue = FTransform{FVector{pos.X, pos.Y, pos.Z}};
				ecs.entity()
				   .is_a(batch.Prefab)
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
	auto cAdd = It.field<ISM_AddInstance>(1);
	auto map = It.field<const ISM_Map>(2); //flecs::IN

	for (auto i : It)
	{
		AISMController* controller = *map->ISMs.Find(cAdd[i].Hash);
		if (controller != nullptr)
		{
			auto instanceIndex = controller->AddInstance();
			ecs.entity()
			   .is_a(cAdd[i].Prefab)
			   .set<ISM_ControllerRef>({controller})
			   .set<ISM_Index>({instanceIndex})
			   .set<ISM_Hash>({cAdd[i].Hash})
			   .set<Transform>({cAdd[i].Transform});
		}
		It.entity(i).destruct();
		for (auto& data : map->ISMs)
		{
			data.Value->CreateOrExpandTransformArray();
		}
	}
}

void SystemRemoveInstance(flecs::iter& It)
{
	auto cHash = It.field<ISM_Hash>(1);
	auto cIndex = It.field<ISM_Index>(2);
	auto map = It.field<const ISM_Map>(3); //flecs::IN

	for (auto i : It)
	{
		auto controller = *map->ISMs.Find(cHash[i].Value);
		if (controller != nullptr)
		{
			controller->RemoveInstance(cIndex[i].Value);
		}
		It.entity(i).destruct();
	}
}

void SystemCopyInstanceTransforms(flecs::iter& It)
{
	auto transform = It.field<Transform>(1);
	auto ISMIndex = It.field<ISM_Index>(2);
	auto ISMController = It.field<ISM_ControllerRef>(3);

	for (auto i : It)
	{
		auto index = ISMIndex[i].Value;
		ISMController[i].Value->SetTransform(index, transform[i].Value);
	}
}

void SystemUpdateTransformsInBatch(flecs::iter& It)
{
	auto map = It.field<const ISM_Map>(1); //flecs::IN
	
	for (auto& data : map->ISMs)
	{
		data.Value->BatchUpdateTransform();
	}
}


void SystemUpdateBoids(flecs::iter& It)
{
	auto cTransform = It.field<Transform>(1);
	auto boidSettings = It.field<const BoidSettings>(2); //flecs::IN
	auto speed = It.field<const Speed>(3); //flecs::IN

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
		FIntVector hashedVector = FIntVector(location / boidSettings->CellSize);

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
			float maxDistToAveragePositionSq = boidSettings->CellSize * boidSettings->CellSize;

			float distanceNormalized = distToAveragePositionSq / maxDistToAveragePositionSq;
			float needToLeave = FMath::Max(1 - distanceNormalized, 0.f);

			FVector toAveragePosition = (averagePosition - boidPosition).GetSafeNormal();
			auto averageHeading = alignmentSum / nearbyBoidCount;

			force += -toAveragePosition * boidSettings->SeparationWeight * needToLeave;
			force += toAveragePosition * boidSettings->CohesionWeight;
			force += averageHeading * boidSettings->AlignmentWeight;
		}

		if (FMath::Min(FMath::Min(
			               (boidSettings->CageSize / 2.f) - FMath::Abs(boidPosition.X),
			               (boidSettings->CageSize / 2.f) - FMath::Abs(boidPosition.Y)),
		               (boidSettings->CageSize / 2.f) - FMath::Abs(boidPosition.Z))
			< boidSettings->CageAvoidDistance)
		{
			force += -boidPosition.GetSafeNormal() * boidSettings->CageAvoidWeight;
		}

		FVector velocity = transform.GetRotation().GetForwardVector();
		velocity += force * It.delta_time();
		velocity = velocity.GetSafeNormal() * speed->Value;
		transform.SetLocation(transform.GetLocation() + velocity * It.delta_time());

		auto rotator = FRotationMatrix::MakeFromX(velocity.GetSafeNormal()).Rotator();
		transform.SetRotation(rotator.Quaternion());

		cTransform[boidIndex].Value = transform;
	}
}


void SystemUpdateTargetHashMap(flecs::iter& It)
{
	auto targets = It.field<TargetHashMap>(1);
	auto gameSrttings = It.field<const GameSettings>(2); //flecs::IN
	auto query = It.field<const SystemQuery>(3); //flecs::IN

	targets->Value.Empty();

	query->Value.iter([&](flecs::iter& qIt)
	{
		auto cTransform = qIt.field<Transform>(1);
		auto cCooldown = qIt.field<SpaceshipWeaponCooldownTime>(2);
		auto cTeam = qIt.field<BattleTeam>(3);

		for (auto i : qIt)
		{
			auto location = cTransform[i].Value.GetLocation();
			FIntVector hashedVector = FIntVector(location / gameSrttings->ShootingCellSize);

			Data_TargetInstance data{qIt.entity(i), location, cTeam[i].Value, cCooldown[i].CurrentValue <= 0};

			auto entityData = targets->Value.Find(hashedVector);
			if (!entityData)
			{
				TArray<Data_TargetInstance> newEntityData;

				newEntityData.Add(data);
				targets->Value.Emplace(hashedVector, std::move(newEntityData));
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
	auto targets = It.field<TargetHashMap>(1);

	for (auto& hashedData : targets->Value)
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
	auto cTarget = It.field<SpaceshipTarget>(1);
	auto cCooldown = It.field<SpaceshipWeaponCooldownTime>(2);
	auto weaponData = It.field<const SpaceshipWeaponData>(3); //flecs::IN
	auto cTransform = It.field<Transform>(4);

	for (auto i : It)
	{
		if (cTarget[i].Entity.id() != flecs::entity::null().id())
		{
			FTransform projectileTransform = cTransform[i].Value;
			FVector direction = (cTarget[i].Position - projectileTransform.GetLocation()).GetSafeNormal();
			FRotator rotation = direction.ToOrientationRotator();
			FVector scale = FVector(1);

			if (weaponData->IsBeam)
			{
				auto distance = FVector::Distance(cTarget[i].Position, projectileTransform.GetLocation());
				auto xScale = distance / weaponData->BeamMeshLength;
				scale = FVector(xScale, weaponData->ProjectileScale, weaponData->ProjectileScale);
			}
			else
			{
				scale = FVector(weaponData->ProjectileScale);
			}
			projectileTransform.SetScale3D(scale);
			projectileTransform.SetRotation(rotation.Quaternion());

			ecs.entity().set<ISM_AddInstance>(
				{weaponData->ProjectileHash, weaponData->ProjectilePrefab, projectileTransform});

			cCooldown[i].CurrentValue = cCooldown[i].MaxValue * FMath::RandRange(0.5f, 1.5f);
			cTarget[i].Entity = flecs::entity::null();
		}
	}
}

void SystemMoveProjectiles(flecs::iter& It)
{
	auto cTransform = It.field<Transform>(1);
	auto speed = It.field<const Speed>(2);

	for (auto i : It)
	{
		auto velocity = cTransform[i].Value.GetRotation().GetForwardVector() * speed->Value * It.delta_time();
		cTransform[i].Value.SetLocation(cTransform[i].Value.GetLocation() + velocity);
	}
}


void SystemComputeCooldownTime(flecs::iter& It)
{
	auto cCooldown = It.field<SpaceshipWeaponCooldownTime>(1);
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
	auto cLifetime = It.field<ProjectileLifetime>(1);
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
	ecs.system<BatchInstanceAdding, ISM_Map, GameSettings>("SystemSpawnInstancesInRadius")
	   .arg(2).src("Game")
	   .arg(3).src("Game")
	   .kind(flecs::OnUpdate)
	   .instanced()
	   .iter(SystemSpawnInstancesInRadius);

	ecs.system<ISM_AddInstance, ISM_Map>("SystemAddInstance")
	   .arg(2).src("Game")
	   .kind(flecs::OnUpdate)
	   .instanced()
	   .iter(SystemAddInstance);

	ecs.system<ISM_Hash, ISM_Index, ISM_Map, ISM_RemovedInstance>("SystemRemoveInstance")
	   .arg(3).src("Game")
	   .kind(flecs::OnUpdate)
	   .instanced()
	   .iter(SystemRemoveInstance);

	ecs.system<Transform, ISM_Index, ISM_ControllerRef>("SystemCopyInstanceTransforms")
	   .kind(flecs::OnUpdate)
	   .instanced()
	   .iter(SystemCopyInstanceTransforms);

	ecs.system<ISM_Map>("SystemUpdateTransformsInBatch")
	   .arg(1).src("Game")
	   .kind(flecs::OnUpdate)
	   .iter(SystemUpdateTransformsInBatch);

	ecs.system<Transform, BoidSettings, Speed, BoidInstance>("SystemUpdateBoids")
	   .arg(2).src("Game")
	   .arg(3).in()
	   .kind(flecs::OnUpdate)
	   .instanced()
	   .iter(SystemUpdateBoids);

	ecs.system<SpaceshipTarget, SpaceshipWeaponCooldownTime, SpaceshipWeaponData, Transform>("SystemSpawnProjectiles")
	   .arg(3).in()
	   .kind(flecs::OnUpdate)
	   .instanced()
	   .iter(SystemSpawnProjectiles);

	ecs.system<SpaceshipWeaponCooldownTime>("SystemComputeCooldownTime")
	   .kind(flecs::OnUpdate)
	   .instanced()
	   .iter(SystemComputeCooldownTime);

	flecs::query<> targetsQuery = ecs.query_builder<Transform, SpaceshipWeaponCooldownTime, BattleTeam>()
	                                 .build();

	ecs.system<TargetHashMap, GameSettings, SystemQuery>("SystemUpdateTargetHashMap")
	   .arg(1).src("Game").inout()
	   .arg(2).src("Game")
	   .kind(flecs::OnUpdate)
	   .instanced()
	   .iter(SystemUpdateTargetHashMap)
	   .set<SystemQuery>({targetsQuery});

	ecs.system<TargetHashMap>("SystemSearchNewTargets")
	   .arg(1).src("Game").inout()
	   .kind(flecs::OnUpdate)
	   .instanced()
	   .iter(SystemSearchNewTargets);

	ecs.system<Transform, Speed, ProjectileInstance>("SystemMoveProjectiles")
	   .kind(flecs::OnUpdate)
	   .instanced()
	   .iter(SystemMoveProjectiles);

	ecs.system<ProjectileLifetime>("SystemCheckProjectileLifetime")
	   .kind(flecs::OnUpdate)
	   .instanced()
	   .iter(SystemCheckProjectileLifetime);
}
