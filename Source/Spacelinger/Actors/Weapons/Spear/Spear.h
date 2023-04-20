// Without Copyright. Project SpaceLinger, Master en Creación de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapons/SLProjectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Spear.generated.h"

UCLASS()
class SPACELINGER_API ASpear : public ASLProjectile
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpear();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxCollision;

	
	FTimerHandle DestroyTimer;

private:
	UFUNCTION()
	void OnBeginOverlap_BoxCollision(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult);

	void DestroySpear();
};
