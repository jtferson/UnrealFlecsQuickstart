#pragma once
#include "flecs.h"

namespace FlecsGlobals
{
	UNREALFLECS_API
	extern TArray<void (*)(flecs::world&)> FlecsRegs;
}

template<class T>
class FlecsComponentRegistration
{
	static const FString Name;
	static bool IsReg;
	static bool Init() 
	{
		void (*func)(flecs::world&);
		func = &FuncReg;
		FlecsGlobals::FlecsRegs.Add(func);
		return true;
	}
	static void FuncReg(flecs::world& InWorld)
	{
		InWorld.component<T>();
		UE_LOG(LogTemp, Warning, TEXT("FlecsComponent %s is registered"), *Name);
	}
};

template<class T>
const FString FlecsComponentRegistration<T>::Name = FString("ComponentName");
template<class T>
bool FlecsComponentRegistration<T>::IsReg = FlecsComponentRegistration<T>::Init();

#define FLECS_COMPONENT(Type) \
	const FString FlecsComponentRegistration<Type>::Name = FString(#Type); \
	bool FlecsComponentRegistration<Type>::IsReg = FlecsComponentRegistration<Type>::Init();