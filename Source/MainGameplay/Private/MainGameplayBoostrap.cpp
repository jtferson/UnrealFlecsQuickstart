// Copyright 2021 Red J

#include "MainGameplay/Public/MainGameplayBootstrap.h"


void IGameplayConfigSet::SetConfig(UGameplayConfig* InConfig)
{
	Config = InConfig;
}

void IWorldSet::SetWorld(UWorld* InWorld)
{
	World = InWorld;
}

void AMainGameplayBootstrap::Bootstrap(flecs::world& ecs)
{
	for(auto moduleType : FlecsModules)
	{
		auto module = NewObject<UFlecsModuleBase>(this, moduleType);
		
		auto bImplementWorldSet = module->Implements<UWorldSet>();
		if(bImplementWorldSet)
		{
			auto configSet = Cast<IWorldSet>(module);
			configSet->SetWorld(GetWorld());
		}

		auto bImplementConfigSet = module->Implements<UGameplayConfigSet>();
		if(bImplementConfigSet)
		{
			auto configSet = Cast<IGameplayConfigSet>(module);
			configSet->SetConfig(Config);
		}
	
		module->Initialize(ecs);
	}
}
