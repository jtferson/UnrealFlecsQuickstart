// Copyright 2021 Red J
#include "Framework/UnrealFlecsSubsystem.h"

void UUnrealFlecsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	OnTickHandle = FWorldDelegates::OnWorldPostActorTick.AddUObject(this, &UUnrealFlecsSubsystem::Tick);

	ECSWorld = new flecs::world();

	//flecs explorer and monitor
	//comment this out if you not using it, it has some performance overhead
	//go to https://www.flecs.dev/explorer/ when the project is running to inspect active entities and values
	GetEcsWorld()->import<flecs::monitor>();
	GetEcsWorld()->set<flecs::Rest>({});
	
	UE_LOG(LogTemp, Warning, TEXT("UUnrealFlecsSubsystem has started!"));
	
	Super::Initialize(Collection);
}

void UUnrealFlecsSubsystem::Deinitialize()
{
	FWorldDelegates::OnWorldPostActorTick.Remove(OnTickHandle);

	if(ECSWorld)
	{
		delete(ECSWorld);
	}

	UE_LOG(LogTemp, Warning, TEXT("UUnrealFlecsSubsystem has shut down!"));
	
	Super::Deinitialize();
}

flecs::world* UUnrealFlecsSubsystem::GetEcsWorld() const
{
	return ECSWorld;
}

void UUnrealFlecsSubsystem::Tick(UWorld* World, ELevelTick LevelTick, float DeltaTime)
{
	if(ECSWorld) ECSWorld->progress(DeltaTime);
}
