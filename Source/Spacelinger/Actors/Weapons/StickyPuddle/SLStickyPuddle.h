// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLStickyPuddle.generated.h"

class UBoxComponent;
class UDecalComponent;

UCLASS()
class SPACELINGER_API ASLStickyPuddle : public AActor
{
	GENERATED_BODY()
	
public:	
	ASLStickyPuddle();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UDecalComponent* Decal;

	// How long is the sticky paddle going to stay in the floor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spacelinger")
		float LifeSpan = 15.0f;

	void ResetLifeSpan();


protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBeginOverlap_BoxCollision(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult);
};
