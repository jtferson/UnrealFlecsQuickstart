// Copyright 2021 Red J
#include "Framework/UnrealFlecsSubsystem.h"


void UUnrealFlecsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	OnTickDelegate = FTickerDelegate::CreateUObject(this, &UUnrealFlecsSubsystem::Tick);
	OnTickHandle = FTicker::GetCoreTicker().AddTicker(OnTickDelegate);

	ECSWorld = new flecs::world();
	
	UE_LOG(LogTemp, Warning, TEXT("UUnrealFlecsSubsystem has started!"));
	
	Super::Initialize(Collection);
}

void UUnrealFlecsSubsystem::Deinitialize()
{
	FTicker::GetCoreTicker().RemoveTicker(OnTickHandle);

	if(!ECSWorld) delete(ECSWorld);


	UE_LOG(LogTemp, Warning, TEXT("UUnrealFlecsSubsystem has shut down!"));
	
	Super::Deinitialize();
}

flecs::world* UUnrealFlecsSubsystem::GetEcsWorld() const
{
	return ECSWorld;
}

bool UUnrealFlecsSubsystem::Tick(float DeltaTime)
{
	if(ECSWorld) ECSWorld->progress(DeltaTime);

	return true;
}
