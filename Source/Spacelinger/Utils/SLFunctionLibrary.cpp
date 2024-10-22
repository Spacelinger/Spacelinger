// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "SLFunctionLibrary.h"
#include "Kismet/GameplayStaticsTypes.h"

// NOTE(Sergi): This can be deleted, but I'm leaving it up so it
// can be used as a reference on how to get the UWorld
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

	return;
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

const TArray<ULevelStreaming*>& USLFunctionLibrary::GetWorldStreamingLevels(const UObject* WorldContextObject) {
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	return World->GetStreamingLevels();
}
