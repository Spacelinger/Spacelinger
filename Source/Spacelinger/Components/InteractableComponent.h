// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/InteractInterface.h"
#include "InteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetCandidate, AActor*, InteractingActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRemoveCandidate, AActor*, InteractingActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteract, AActor*, InteractingActor);

class UWidgetComponent;

UCLASS()
class SPACELINGER_API UInteractableComponent : public UActorComponent, public IInteractInterface
{
	GENERATED_BODY()
	
public:

	UInteractableComponent();

	virtual void BeginPlay() override;

	virtual void SetAsCandidate(AActor* ActorInteracting) override;
	virtual void RemoveAsCandidate(AActor* ActorInteracting) override;
	virtual void Interact(AActor* ActorInteracting) override;
	virtual int GetInteractPriority() const override { return Priority; }
	virtual void OnRegister() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (UIMin = "1", UIMax = "99"))
	int Priority = 50;

	UPROPERTY(BlueprintAssignable)
	FOnSetCandidate OnSetCandidateDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnRemoveCandidate OnRemoveCandidateDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnInteract OnInteractDelegate;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UWidgetComponent* InteractPromptWidget = nullptr;

protected:

	UPROPERTY()
	AActor* CurrentInteractingActor = nullptr;

	UPROPERTY()
	TArray<UActorComponent*> InteractableWidgetComponents;
};
