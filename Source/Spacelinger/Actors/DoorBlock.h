// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DoorBlock.generated.h"

class UInteractableComponent;
class UBoxComponent;
class UWidgetComponent;

UCLASS()
class SPACELINGER_API ADoorBlock : public AActor
{
	GENERATED_BODY()


public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInteractableComponent* InteractableComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* ColliderComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* PreviewStaticMeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* FinalStaticMeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInterface* InteractPreviewMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* InteractPromptWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWidgetComponent* InteractingProgressBarWidget = nullptr;

	ADoorBlock();

	UFUNCTION(BlueprintCallable)
	void DoorBlockSuccess();
	UFUNCTION(BlueprintCallable)
	void DoorBlockFail();

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void SetAsCandidate(AActor* InteractingActor);
	UFUNCTION()
	void RemoveAsCandidate(AActor* InteractingActor);
	UFUNCTION()
	void Interact(AActor* InteractingActor);

private:

	void Reset();
};
