// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "UI/Game/SLUIStamina.h"
#include "Components/ProgressBar.h"
#include "Components/MCV_AbilitySystemComponent.h"
#include "GAS/Attributes/StaminaAttributeSet.h"


void UUIStamina::NativeOnInitialized() {

	Super::NativeOnInitialized();

    if (StaminaBar) {

        StaminaBar->SetPercent(1.0f);
    }
    if (UMCV_AbilitySystemComponent* ASC = GetASC()) {

        // Bind to Stamina GAS Attribute value change delegate
        ASC->GetGameplayAttributeValueChangeDelegate(UStaminaAttributeSet::GetStaminaAttribute()).AddUObject(this, &UUIStamina::OnStaminaChanged);
    }

}

void UUIStamina::OnStaminaChanged(const FOnAttributeChangeData& Data)
{

    //Get ASC stamina attribute, check if valid, get MaxStaminaAttribute
    const float NewPercent = (1.0 / 100.0f * Data.NewValue);
    StaminaBar->SetPercent(NewPercent);
    BP_OnStaminaChanged();
}

UMCV_AbilitySystemComponent* UUIStamina::GetASC()
{
    const AActor* PlayerPawn = GetOwningPlayerPawn();
    return PlayerPawn ? PlayerPawn->FindComponentByClass<UMCV_AbilitySystemComponent>() : nullptr;
}

