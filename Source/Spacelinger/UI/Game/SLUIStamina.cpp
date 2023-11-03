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
    if (StaminaSecondBar) {
        StaminaSecondBar->SetPercent(1.0f);
    }
    if (UMCV_AbilitySystemComponent* ASC = GetASC()) {

        // Bind to Stamina GAS Attribute value change delegate
        ASC->GetGameplayAttributeValueChangeDelegate(UStaminaAttributeSet::GetStaminaAttribute()).AddUObject(this, &UUIStamina::OnStaminaChanged);
    }

}

void UUIStamina::OnStaminaChanged(const FOnAttributeChangeData& Data)
{
    //Get ASC stamina attribute, check if valid, get MaxStaminaAttribute
    if (Data.NewValue != Data.OldValue)
    {
        const float NewPercent = (1.0 / 100.0f * Data.NewValue);
        StaminaBar->SetPercent(NewPercent);
        BP_OnStaminaChanged();
        SetVisibility(ESlateVisibility::Visible);
        GetWorld()->GetTimerManager().SetTimer(ResetVisibilityTimerHandle, FTimerDelegate::CreateLambda([&] { SetVisibility(ESlateVisibility::Hidden); }), 1.5f, false);

        if (Data.NewValue < Data.OldValue)
        {
            SetRenderOpacity(1.0f);
            GetWorld()->GetTimerManager().SetTimer(UpdateStaminaSecondBarHandle, this, &UUIStamina::TickUpdateStaminaSecondBar,.001f, true, 1.0f);
        }
    }
}

UMCV_AbilitySystemComponent* UUIStamina::GetASC()
{
    const AActor* PlayerPawn = GetOwningPlayerPawn();
    return PlayerPawn ? PlayerPawn->FindComponentByClass<UMCV_AbilitySystemComponent>() : nullptr;
}

void UUIStamina::TickUpdateStaminaSecondBar()
{
    float SecondBarCurrentPercent = StaminaSecondBar->GetPercent();
    float SecondBarNewPercentDelta = GetWorld()->DeltaTimeSeconds * 0.05f;
    if (StaminaBar->GetPercent() < SecondBarCurrentPercent)
    {
        StaminaSecondBar->SetPercent(SecondBarCurrentPercent - SecondBarNewPercentDelta);
    }
    else
    {
        SetRenderOpacity(0.5f);
        StaminaSecondBar->SetPercent(StaminaBar->GetPercent());
    }
}
