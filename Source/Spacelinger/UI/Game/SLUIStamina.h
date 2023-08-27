// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SLUIStamina.generated.h"

class UProgressBar;
class UMCV_AbilitySystemComponent;
struct FOnAttributeChangeData;

UCLASS()
class SPACELINGER_API UUIStamina : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (Bindwidget))
	UProgressBar* StaminaBar = nullptr;

	UFUNCTION(BlueprintImplementableEvent)
	void BP_OnStaminaChanged();

	void NativeOnInitialized() override;

private:

	void OnStaminaChanged(const FOnAttributeChangeData& Data);

	UMCV_AbilitySystemComponent* GetASC();
};
