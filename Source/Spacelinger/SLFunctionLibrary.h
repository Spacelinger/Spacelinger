// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SLFunctionLibrary.generated.h"

struct FPredictProjectilePathPointData;

UCLASS()
class SPACELINGER_API USLFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "SpaceLinger Debug", meta = (WorldContext = "WorldContextObject"))
	static void DrawDebugPredictionPath(
			const UObject* WorldContextObject,
			const TArray<FPredictProjectilePathPointData> PathPoints,
			const FColor Color,
			const float Thickness,
			const bool IsHit,
			const FVector HitPoint,
			const float SphereRadius);

};
