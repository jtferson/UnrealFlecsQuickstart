// Copyright 2021 Red J

#pragma once

#include "CoreMinimal.h"

#include "SpaceshipType.h"
#include "UObject/Object.h"
#include "BattleTeam.generated.h"

/**
 * 
 */
UCLASS()
class MAINGAMEPLAY_API UBattleTeam : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	USpaceshipType* SpaceshipType = nullptr;
	UPROPERTY(EditAnywhere)
	int32 NumShips;
};
