// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Actors/Door/SLDoor.h"
#include "SLLockedDoor.generated.h"

class UInteractableComponent;
class UBoxComponent;
class UWidgetComponent;

UENUM(BlueprintType)
enum class DoorLockType : uint8 {
	KEY = 0 UMETA(DisplayName = "Key"),
	LASER = 1  UMETA(DisplayName = "Laser Puzzle"),
};

UCLASS()
class SPACELINGER_API ASLLockedDoor : public ASLDoor
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UInteractableComponent* InteractableComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UBoxComponent* ColliderComponent = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UWidgetComponent* InteractPromptWidget = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName ItemToUnblock = "Item Name";

	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger")
	DoorLockType LockType = DoorLockType::KEY;

	// Hide the key when it doesn't need to be shown
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
		void SetKeyVisibility();

	ASLLockedDoor();

	UFUNCTION(BlueprintCallable)
		void OpenDoor();
	UFUNCTION(BlueprintCallable)
		void OpenDoorFail();

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION()
		void SetAsCandidate(AActor* InteractingActor);
	UFUNCTION()
		void RemoveAsCandidate(AActor* InteractingActor);
	UFUNCTION()
		void Interact(AActor* InteractingActor);

private:

	void Reset();
};
