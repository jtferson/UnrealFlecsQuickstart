// Copyright 2021 Red J

#pragma once

#include "CoreMinimal.h"

#include <queue>

#include "GameFramework/Actor.h"
#include "ISMController.generated.h"

UCLASS()
class MAINGAMEPLAY_API AISMController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AISMController();

public:
	//Component fields
	UPROPERTY(EditAnywhere)
	UInstancedStaticMeshComponent* InstancedStaticMeshComponent = nullptr;

public:
	//ISM API functions
	void Initialize(UStaticMesh* InMesh, UMaterialInterface* InMaterial) const;
	int32 GetInstanceCount() const;
	void CreateOrExpandTransformArray();
	int32 AddInstance(FVector location);
	int32 AddInstance();
	void SetTransform(int32 instanceIndex, const FTransform& transform);
	void RemoveInstance(int32 instanceIndex);
	void BatchUpdateTransform() const;
private:
	TArray<FTransform> transforms;
	TQueue<int> indexPool;
};
