// Copyright 2021 Red J

#pragma once

#include "CoreMinimal.h"
#include "SpaceshipWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Bolt = 0,
    Beam = 1,
};

/**
 * 
 */
UCLASS()
class MAINGAMEPLAY_API USpaceshipWeapon : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	UStaticMesh* Mesh = nullptr;
	UPROPERTY(EditAnywhere)
	UMaterialInterface* Material = nullptr;
	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType;
	UPROPERTY(EditAnywhere)
	float Cooldown;
	UPROPERTY(EditAnywhere, Category=Projectile)
	float Lifetime;
	UPROPERTY(EditAnywhere, Category=Projectile)
	float Speed;
	UPROPERTY(EditAnywhere, Category=Projectile)
	float ProjectileScale;
	UPROPERTY(EditAnywhere, Category=Projectile)
	float BeamMeshLength;
};
