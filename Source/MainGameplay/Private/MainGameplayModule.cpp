// Copyright 2021 Red J

#include "MainGameplay/Public/MainGameplayModule.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

IMPLEMENT_GAME_MODULE(FMainGameplayModule, MainGameplay);

DEFINE_LOG_CATEGORY(MainGameplay);
 
#define LOCTEXT_NAMESPACE "MainGameplay"
 
void FMainGameplayModule::StartupModule()
{
	UE_LOG(MainGameplay, Warning, TEXT("MainGameplay module has started!"));
}
 
void FMainGameplayModule::ShutdownModule()
{
	UE_LOG(MainGameplay, Warning, TEXT("MainGameplay module has shut down"));
}
 
#undef LOCTEXT_NAMESPACE
 

// Add default functionality here for any IMainGameplayModule functions that are not pure virtual.
