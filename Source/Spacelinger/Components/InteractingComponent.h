// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractingComponent.generated.h"

class UInteractableComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACELINGER_API UInteractingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractingComponent();

	UFUNCTION(Server, Reliable, WithValidation)
	void TryToInteract();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoundsBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoundsEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex);
	
	void Refresh();
	UPROPERTY()
	AActor* CurrentInteractableActor = nullptr;				//Might not need. Consider delete

	UPROPERTY()
	UInteractableComponent* CurrentInteractable = nullptr;	//Might not need. Consider delete

	UPROPERTY()
	TArray<AActor*> CurrentInteractables2;

	UPROPERTY()
	TArray<UInteractableComponent*> CurrentInteractables;
		
};
