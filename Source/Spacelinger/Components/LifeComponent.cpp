// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "Components/LifeComponent.h"

ULifeComponent::ULifeComponent() { }
void ULifeComponent::InitializeComponent() { }

void ULifeComponent::ReceiveDamage(int Damage, AActor* DamageDealer) {

    CurrentLife = FMath::Max(CurrentLife - Damage, 0);
    OnDamageReceivedDelegate.Broadcast(Damage, DamageDealer);

    if (CurrentLife <= 0) {
        OnDie(DamageDealer);
    }
}

void ULifeComponent::ReceiveHeal(int Heal, AActor* Healer) {
    CurrentLife = FMath::Min(CurrentLife + Heal, MaxLife);
    OnHealReceivedDelegate.Broadcast(Heal, Healer);
}

void ULifeComponent::OnDie(AActor* Killer) {
    OnDieDelegate.Broadcast(Killer);
    if (TimeToDestroyAfterDeath >= 0) {
        GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ULifeComponent::OnDestroyTimer, TimeToDestroyAfterDeath, false);
    }
}

void ULifeComponent::OnDestroyTimer() {
    GetOwner()->Destroy();
}