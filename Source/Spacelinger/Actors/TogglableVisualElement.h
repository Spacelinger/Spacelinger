#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TogglableVisualElement.generated.h"

// Used as an interface for visual elements in the game that we want to turn on and off from C++
UCLASS()
class ATogglableVisualElement : public AActor {
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger")
	bool bIsActive = false;

	UFUNCTION(BlueprintImplementableEvent)
	void SetState(bool NewState);

	UFUNCTION(BlueprintCallable)
	bool Toggle() {
		bIsActive = !bIsActive;
		SetState(bIsActive);
		return bIsActive;
	}
};
