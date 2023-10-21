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

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SPACELINGER_API UInteractableComponent : public UActorComponent, public IInteractInterface
{
	GENERATED_BODY()
	
public:

	UInteractableComponent();

	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	virtual void SetAsCandidate(AActor* ActorInteracting) override;
	UFUNCTION(BlueprintCallable)
	virtual void RemoveAsCandidate(AActor* ActorInteracting) override;
	UFUNCTION(BlueprintCallable)
	virtual void Interact(AActor* ActorInteracting) override;
	virtual int GetInteractPriority() const override { return Priority; }
	virtual bool CanInteract() const override { return bCanInteract; }
	virtual void OnRegister() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (UIMin = "1", UIMax = "99"))
	int Priority = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanInteract = true;

	UPROPERTY(BlueprintAssignable)
	FOnSetCandidate OnSetCandidateDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnRemoveCandidate OnRemoveCandidateDelegate;

	UPROPERTY(BlueprintAssignable)
	FOnInteract OnInteractDelegate;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UWidgetComponent* InteractPromptWidget = nullptr;

	// Num of interacting triggers currently overlapping with this component
	// This is updated by InteractingComponent only
	int InteractingCount = 0;

protected:

	UPROPERTY()
	AActor* CurrentInteractingActor = nullptr;

	UPROPERTY()
	TArray<UActorComponent*> InteractableWidgetComponents;
};
