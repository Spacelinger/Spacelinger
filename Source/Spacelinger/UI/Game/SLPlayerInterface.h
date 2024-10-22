// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SLPlayerInterface.generated.h"

class UProgressBar;
class UImage;
struct FSlateBrush;

/**
 * 
 */
UCLASS()
class SPACELINGER_API USLPlayerInterface : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UProgressBar* HealthBar = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UImage* SelectedAbilityIcon = nullptr;

	UFUNCTION(BlueprintCallable)
	float HealthBar_Percent() const;

protected:
	//void NativeOnInitialized() override;
};
