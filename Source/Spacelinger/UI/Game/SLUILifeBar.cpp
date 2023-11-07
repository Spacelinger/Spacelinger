// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "UI/Game/SLUILifeBar.h"
#include "Components/LifeComponent.h"
#include "Components/Image.h"

void UUILifeBar::NativeOnInitialized() {

	Super::NativeOnInitialized();

    if (LifeBar) {

        LifeBar->GetDynamicMaterial()->SetScalarParameterValue(FName ("Percent"), 1.0f);
    }
    if (ULifeComponent* LifeComponent = GetLifeComponent()) {
        LifeComponent->OnDamageReceivedDelegate.AddUniqueDynamic(this, &UUILifeBar::OnReceiveDamage);
        LifeComponent->OnHealReceivedDelegate  .AddUniqueDynamic(this, &UUILifeBar::OnReceiveDamage);
    }
}

void UUILifeBar::OnReceiveDamage(int Damage, AActor* DamageDealer) {

    ULifeComponent* LifeComponent = GetLifeComponent();
    if (!LifeComponent)
        return;
    const float NewPercent = (1.0 / LifeComponent->GetMaxLife()) * LifeComponent->GetCurrentLife();
    LifeBar->GetDynamicMaterial()->SetScalarParameterValue(FName("Percent"), NewPercent);

    // Defines orange color
    FLinearColor Orange = FLinearColor(1.0f, 0.5f, 0.0f);

    // Changes the color of the material based on the value of NewPercent
    if (NewPercent <= 1 && NewPercent >= 0.6)
    {
        LifeBar->GetDynamicMaterial()->SetVectorParameterValue(FName("Base Color"), FLinearColor::Green);
    }
    else if (NewPercent < 0.6 && NewPercent >= 0.3)
    {
        LifeBar->GetDynamicMaterial()->SetVectorParameterValue(FName("Base Color"), Orange);
    }
    else
    {
        LifeBar->GetDynamicMaterial()->SetVectorParameterValue(FName("Base Color"), FLinearColor::Red);
    }

    BP_OnReceiveDamage();
}

ULifeComponent* UUILifeBar::GetLifeComponent() {
    
    const AActor* PlayerPawn = GetOwningPlayerPawn();
	return PlayerPawn ? PlayerPawn->FindComponentByClass<ULifeComponent>() : nullptr;

}