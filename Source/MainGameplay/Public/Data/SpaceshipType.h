// Copyright 2021 Red J

#pragma once

#include "CoreMinimal.h"
#include "SpaceshipWeapon.h"
#include "SpaceshipType.generated.h"

/**
 * 
 */
UCLASS()
class MAINGAMEPLAY_API USpaceshipType : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	UStaticMesh* Mesh = nullptr;
	UPROPERTY(EditAnywhere)
	UMaterial* Material = nullptr;
	UPROPERTY(EditAnywhere)
	USpaceshipWeapon* Weapons = nullptr;
	UPROPERTY(EditAnywhere)
	float MaxSpeed;
};
