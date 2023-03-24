// Fill out your copyright notice in the Description page of Project Settings.


#include "SLFunctionLibrary.h"
#include "Kismet/GameplayStaticsTypes.h"

void USLFunctionLibrary::DrawDebugPredictionPath(
	const UObject* WorldContextObject,
	const TArray<FPredictProjectilePathPointData> PathPoints,
	const FColor Color,
	const float Thickness,
	const bool IsHit,
	const FVector HitPoint,
	const float SphereRadius)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	for (int i = 0; i < PathPoints.Num() - 1; ++i)
	{
		FVector P0 = PathPoints[i]  .Location;
		FVector P1 = PathPoints[i+1].Location;
		DrawDebugLine(World, P0, P1, Color, false, -1, 0, Thickness);
	}
	if (IsHit) {
		FVector P0 = PathPoints[PathPoints.Num()-1].Location;
		DrawDebugLine(World, P0, HitPoint, Color, false, -1, 0, Thickness);
		DrawDebugSphere(World, HitPoint, SphereRadius, 12, Color, false, -1.0f, 0U, Thickness);
	}
}
