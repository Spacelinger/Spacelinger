// Copyright 2022 David (Quod) Soler

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MCV_GASFunctionLibrary.generated.h"

/**
 * Useful functions for GAS interactions
 */
UCLASS()
class MCVGASFRAMEWORK_API UMCV_GASFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	// Gets the ability system component from an actor even if it does not implement its interface
	UFUNCTION(BlueprintPure)
	static UAbilitySystemComponent* MCVGetAbilitySystemComponent(AActor* TargetActor);

};
