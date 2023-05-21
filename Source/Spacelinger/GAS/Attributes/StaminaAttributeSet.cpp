// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Attributes/StaminaAttributeSet.h"
#include "GameplayEffectExtension.h"

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
	
	if (true) 
	{
		if (Data.EvaluatedData.Attribute == GetStaminaAttribute()) 
		{
			const float CurrentStamina = GetStamina();
			if (CurrentStamina <= 0.0f) 
			{
				//bIsExhausted = true;

			}

		}
	}
}
