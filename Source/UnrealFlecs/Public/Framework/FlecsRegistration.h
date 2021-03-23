#pragma once
#include "flecs.h"

class FlecsRegContainer
{
public:
	UNREALFLECS_API
    static TArray<void (*)(flecs::world&)>& GetFlecsRegs();
};

template<class T>
class FlecsComponentRegistration
{
	static const FString Name;
	static bool IsReg;
	static bool Init() 
	{
		void (*func)(flecs::world&);
		func = &FuncReg;
		TArray<void (*)(flecs::world&)>& regs = FlecsRegContainer::GetFlecsRegs();
		regs.Add(func);
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
struct Type; \
const FString FlecsComponentRegistration<Type>::Name = FString(#Type); \
bool FlecsComponentRegistration<Type>::IsReg = FlecsComponentRegistration<Type>::Init(); \
struct Type

#define REG_COMPONENT(Type) \
const FString FlecsComponentRegistration<Type>::Name = FString(#Type); \
bool FlecsComponentRegistration<Type>::IsReg = FlecsComponentRegistration<Type>::Init();