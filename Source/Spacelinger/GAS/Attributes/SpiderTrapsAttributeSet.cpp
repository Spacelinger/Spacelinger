// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Attributes/SpiderTrapsAttributeSet.h"
#include "GameplayEffectExtension.h"
#include <AbilitySystemBlueprintLibrary.h>
#include "GAS/Effects/GE_StaminaRecovery.h"

void USpiderTrapsAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) {
	Super::PreAttributeChange(Attribute, NewValue);
	
	if (Attribute == GetAvailableTrapsAttribute()) {
		NewValue = FMath::Clamp<float>(NewValue, 0.0f, GetMaxTraps());
	}
	

}

void USpiderTrapsAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const {
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetAvailableTrapsAttribute()) {
		NewValue = FMath::Clamp<float>(NewValue, 0.0f, GetMaxTraps());
	}
}

void USpiderTrapsAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) {
	Super::PostGameplayEffectExecute(Data);
	
		if (Data.EvaluatedData.Attribute == GetAvailableTrapsAttribute())
		{
			AActor* PlayerActor = GetWorld()->GetFirstPlayerController()->GetPawn();
			FGameplayEventData Payload;

			const float CurrentAvailableTrapsValue = GetAvailableTraps();
			if (CurrentAvailableTrapsValue <= 0.0f)
			{
				//UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerActor, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Stamina.Empty")), Payload);

				// Here or at abilities end might want to add a cooldown effect so abilities depending on Stamina (ie. slowtime) can't be spammed on 0.x values of it
			}

			if (CurrentAvailableTrapsValue < GetMaxTraps())
			{
				// Timer to recover a trap after SpiderTrapsRecoveryBaseRate
				UWorld* World = GetWorld();
				FTimerHandle TimerHandle;
				World->GetTimerManager().SetTimer(TimerHandle, this, &USpiderTrapsAttributeSet::RestoreAvailableTrap, SpiderTrapsRecoveryBaseRate, false);
			}
		}
}

void USpiderTrapsAttributeSet::RestoreAvailableTrap()
{
	SetAvailableTraps(GetAvailableTraps() + 1.0f);
}

void USpiderTrapsAttributeSet::SetSpiderTrapsRecoveryValue(float Value)
{
	GetOwningAbilitySystemComponent()->
		UpdateActiveGameplayEffectSetByCallerMagnitude(SpiderTrapsRecoveryEffect, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Stamina.RecoveryValue")), Value);
}

