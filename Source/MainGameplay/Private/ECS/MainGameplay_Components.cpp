// Copyright 2021 Red J


#include "ECS/MainGameplay_Components.h"


void UMainGameplay_Components::Initialize(flecs::world& ecs)
{
	ecs.component<UWorldRef>();
	ecs.component<ISM_Map>();
	ecs.component<ISM_Hash>();
	ecs.component<ISM_Index>();
	ecs.component<ISM_ControllerRef>();
	ecs.component<ISM_RemovedInstance>();

	ecs.component<GameSettings>();
	ecs.component<BoidSettings>();
	ecs.component<BattleTeam>();

	
	ecs.component<SpaceshipWeaponData>();
	ecs.component<SpaceshipWeaponCooldownTime>();
	ecs.component<SpaceshipTarget>();
	ecs.component<BoidInstance>();
	ecs.component<ProjectileInstance>();
	ecs.component<Speed>();
	
	ecs.component<ProjectileLifetime>();
	
	
	ecs.component<BatchInstanceAdding>();
	ecs.component<ISM_AddInstance>();

	ecs.component<Transform>();
	
	ecs.component<SystemQuery>();
	ecs.component<TargetHashMap>();
}
