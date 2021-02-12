// Copyright 2021 Red J

#include "UnrealFlecs/Public/UnrealFlecsModule.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"

IMPLEMENT_GAME_MODULE(FUnrealFlecsModule, UnrealFlecs);

DEFINE_LOG_CATEGORY(UnrealFlecs);
 
#define LOCTEXT_NAMESPACE "UnrealFlecs"
 
void FUnrealFlecsModule::StartupModule()
{
	UE_LOG(UnrealFlecs, Warning, TEXT("UnrealFlecs module has started!"));
}
 
void FUnrealFlecsModule::ShutdownModule()
{
	UE_LOG(UnrealFlecs, Warning, TEXT("UnrealFlecs module has shut down"));
}
 
#undef LOCTEXT_NAMESPACE
 

// Add default functionality here for any IUnrealFlecsModule functions that are not pure virtual.
