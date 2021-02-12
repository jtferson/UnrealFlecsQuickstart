// Copyright 2021 Red J


#include "Scene/ISMController.h"


AISMController::AISMController()
{
	PrimaryActorTick.bCanEverTick = false;

	InstancedStaticMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticMeshComponent"));
	SetRootComponent(InstancedStaticMeshComponent);
}

void AISMController::Initialize(UStaticMesh* InMesh, UMaterialInterface* InMaterial) const
{
	InstancedStaticMeshComponent->SetStaticMesh(InMesh);
	InstancedStaticMeshComponent->SetMaterial(0, InMaterial);
	InstancedStaticMeshComponent->CastShadow = false;
	InstancedStaticMeshComponent->bCastDynamicShadow = false;
	InstancedStaticMeshComponent->bCastStaticShadow = false;
	InstancedStaticMeshComponent->bUseDefaultCollision = false;
	InstancedStaticMeshComponent->SetGenerateOverlapEvents(false);
	InstancedStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InstancedStaticMeshComponent->SetCanEverAffectNavigation(false);

	InstancedStaticMeshComponent->NumCustomDataFloats = 1;
}

int32 AISMController::GetInstanceCount() const
{
	return InstancedStaticMeshComponent->GetInstanceCount();
}



int32 AISMController::AddInstance(FVector location)
{
	int32 instanceIndex;
	if(indexPool.IsEmpty())
	{
		FTransform transform{location};
		instanceIndex = InstancedStaticMeshComponent->AddInstance(transform);
	}
	else
	{
		indexPool.Dequeue(instanceIndex);
		InstancedStaticMeshComponent->SetCustomDataValue(instanceIndex, 0, 0);
	}
	return instanceIndex;
}

int32 AISMController::AddInstance()
{
	return AddInstance(FVector::ZeroVector);
}

void AISMController::RemoveInstance(int32 instanceIndex)
{
	indexPool.Enqueue(instanceIndex);
	
	//Just hide an instance without actual removing
	//Soon newly added instance replace the hidden one
	InstancedStaticMeshComponent->SetCustomDataValue(instanceIndex, 0, 1);
}

void AISMController::CreateOrExpandTransformArray()
{
	if(GetInstanceCount() != transforms.Num())
	{
		transforms.AddUninitialized(GetInstanceCount() - transforms.Num());
		for (auto i = 0; i < transforms.Num(); i++)
		{
			transforms[i] = FTransform{FVector::ZeroVector};
		}
	}
}

void AISMController::SetTransform(int32 instanceIndex, const FTransform& transform)
{
	transforms[instanceIndex] = transform;
}


void AISMController::BatchUpdateTransform() const
{
	if(transforms.Num() > 0)
	{
		InstancedStaticMeshComponent->BatchUpdateInstancesTransforms(0, transforms, true, true);
	}
}

