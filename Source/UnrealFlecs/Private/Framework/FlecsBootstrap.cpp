// Copyright 2021 Red J


#include "Framework/FlecsBootstrap.h"


#include "Framework/UnrealFlecsSubsystem.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AFlecsBootstrap::AFlecsBootstrap()
{	
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AFlecsBootstrap::BeginPlay()
{
	UGameInstance* gameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	UUnrealFlecsSubsystem* subSystem = gameInstance->GetSubsystem<UUnrealFlecsSubsystem>();

	Bootstrap(*subSystem->GetEcsWorld());
	
	Super::BeginPlay();
}

void AFlecsBootstrap::Bootstrap(flecs::world& ecs)
{
	// Here we initialize flecs systems and components
}

