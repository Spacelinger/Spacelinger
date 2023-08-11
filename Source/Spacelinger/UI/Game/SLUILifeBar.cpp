// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "UI/Game/SLUILifeBar.h"
#include "Components/LifeComponent.h"
#include "Components/ProgressBar.h"

void UUILifeBar::NativeOnInitialized() {

	Super::NativeOnInitialized();

    if (LifeBar) {

        LifeBar->SetPercent(1.0f);
    }
    if (ULifeComponent* LifeComponent = GetLifeComponent()) {

        LifeComponent->OnDamageReceivedDelegate.AddUniqueDynamic(this, &UUILifeBar::OnReceiveDamage);
    }
}

void UUILifeBar::OnReceiveDamage(int Damage, AActor* DamageDelaer) {

    ULifeComponent* LifeComponent = GetLifeComponent();
    if (!LifeComponent)
        return;
    const float NewPercent = (1.0 / LifeComponent->GetMaxLife()) * LifeComponent->GetCurrentLife();
    LifeBar->SetPercent(NewPercent);
    BP_OnReceiveDamage();
}

ULifeComponent* UUILifeBar::GetLifeComponent() {
    
    const AActor* PlayerPawn = GetOwningPlayerPawn();
	return PlayerPawn ? PlayerPawn->FindComponentByClass<ULifeComponent>() : nullptr;

}