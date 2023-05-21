// Copyright 2022 David (Quod) Soler


#include "Misc/MCV_GASFunctionLibrary.h"
#include "AbilitySystemGlobals.h"

UAbilitySystemComponent* UMCV_GASFunctionLibrary::MCVGetAbilitySystemComponent(AActor* TargetActor)
{
	return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor, true);
}
