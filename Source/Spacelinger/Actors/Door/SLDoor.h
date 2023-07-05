// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLDoor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USpotLightComponent;

UCLASS()
class SPACELINGER_API ASLDoor : public AActor
{
	GENERATED_BODY()
	
public:
	ASLDoor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent *FrameMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent *DoorMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent *LightMesh1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpotLightComponent *Light1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent *LightMesh2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USpotLightComponent *Light2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* BoxTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger|Movement")
	bool ReactToPlayer = true;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger|Movement", meta = (UIMin = "0.0"))
	float DoorSpeed = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger|Movement")
	float DoorOffset = 200.0f;

protected:
	virtual void BeginPlay() override;

private:
	int ActorsOnTrigger = 0;
	float CurrentDoorOffset = 0.0f;
	FVector InitialDoorLocation;

	UFUNCTION()
	void BoxTrigger_OnBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult & SweepResult);

	UFUNCTION()
	void BoxTrigger_OnEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	// Tick functions
	void DoorTickOpen();
	void DoorTickClose();
	FTimerHandle DoorTickHandle; // Timers are going to be set to loop

	AActor* GetPlayerActor();
	void UpdateDoorLocation();

};
