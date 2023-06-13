// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/InteractInterface.h"
#include "InteractableComponent.generated.h"


UCLASS()
class SPACELINGER_API UInteractableComponent : public UActorComponent, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (UIMin = "0", UIMax = "99"))
	int Priority = 50;

	UInteractableComponent();

	virtual void BeginPlay() override;

	virtual void StartCanInteract(AActor* ActorInteracting) override;
	virtual void EndCanInteract(AActor* ActorInteracting) override;
	virtual void Interact(AActor* ActorInteracting) override;
	virtual int GetInteractPriority() const override { return Priority; }
	virtual void SetAsCandidate(bool IsCandidate) override;

protected:

	UPROPERTY()
	AActor* CurrentInteractingActor = nullptr;

	bool bIsCandidate = false;
};
