// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SLUILifeBar.generated.h"

class UImage;
class ULifeComponent;

UCLASS()
class SPACELINGER_API UUILifeBar : public UUserWidget
{
	GENERATED_BODY()


protected:
	UPROPERTY(BlueprintReadWrite, meta = (Bindwidget))
		UImage* LifeBar = nullptr;

	UFUNCTION(BlueprintImplementableEvent)
		void BP_OnReceiveDamage();

	void NativeOnInitialized() override;

private:

	UFUNCTION()
	void OnReceiveDamage(int Damage, AActor* DamageDealer);
	ULifeComponent* GetLifeComponent();
};
