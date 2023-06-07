// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Attributes/StaminaAttributeSet.h"
#include "GameplayEffectExtension.h"
#include <AbilitySystemBlueprintLibrary.h>
#include "GAS/Effects/GE_StaminaRecovery.h"

void UStaminaAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) {
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetStaminaAttribute()) {
		NewValue = FMath::Clamp<float>(NewValue, 0.0f, GetMaxStamina());
	}
}

void UStaminaAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const {
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetStaminaAttribute()) {
		NewValue = FMath::Clamp<float>(NewValue, 0.0f, GetMaxStamina());
	}
}

void UStaminaAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) {
	Super::PostGameplayEffectExecute(Data);
	
		if (Data.EvaluatedData.Attribute == GetStaminaAttribute()) 
		{
			AActor* PlayerActor = GetWorld()->GetFirstPlayerController()->GetPawn();
			FGameplayEventData Payload;

			const float CurrentStamina = GetStamina();
			if (CurrentStamina <= 0.0f) 
			{
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerActor, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Stamina.Empty")), Payload);
			

				// Here or at abilities end might want to add a cooldown effect so abilities depending on Stamina (ie. slowtime) can't be spammed on 0.x values of it
			}
		}
}
