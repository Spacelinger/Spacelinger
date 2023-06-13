// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorBlock.generated.h"

class UInteractableComponent;
class UBoxComponent;

UCLASS()
class SPACELINGER_API ADoorBlock : public AActor
{
	GENERATED_BODY()


public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInteractableComponent* InteractableComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* ColliderComp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMeshComp = nullptr;

	// Sets default values for this actor's properties
	ADoorBlock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void SetAsCandidate(AActor* InteractingActor);
	void RemoveAsCandidate(AActor* InteractingActor);
	void Interact(AActor* InteractingActor);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
