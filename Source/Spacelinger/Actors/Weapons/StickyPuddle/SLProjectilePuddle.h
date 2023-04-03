// Without Copyright. Project SpaceLinger, Master en Creación de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Actors/Weapons/SLProjectile.h"
#include "SLProjectilePuddle.generated.h"

UCLASS()
class SPACELINGER_API ASLProjectilePuddle : public ASLProjectile
{
	GENERATED_BODY()
	
public:	
	ASLProjectilePuddle();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Bounce)
	float UpAmountThreshold;

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
