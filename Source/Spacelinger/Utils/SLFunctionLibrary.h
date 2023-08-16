// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

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

	UFUNCTION(BlueprintCallable, Category = "SL Streaming Levels", meta = (WorldContext = "WorldContextObject"))
	static const TArray<ULevelStreaming*>& GetWorldStreamingLevels(const UObject* WorldContextObject);
};
