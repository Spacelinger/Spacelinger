// Without Copyright. Project SpaceLinger, Master en Creación de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapons/SLProjectile.h"
#include "SLProjectilePuddle.generated.h"

class ASLStickyPuddle;

UCLASS()
class SPACELINGER_API ASLProjectilePuddle : public ASLProjectile
{
	GENERATED_BODY()
	
public:	
	ASLProjectilePuddle();

	// How flat the surface needs to be to be considered floor. For example, 0.5 means any surface of 45º or less is floor.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|Bounce")
	float UpAmountThreshold = 0.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger")
	TSoftClassPtr<ASLStickyPuddle> StickyPuddleBlueprint;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnBeginOverlap_SphereCollision(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult);

	UFUNCTION()
	void OnHit_SphereCollision(
		UPrimitiveComponent* HitComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit);

};
