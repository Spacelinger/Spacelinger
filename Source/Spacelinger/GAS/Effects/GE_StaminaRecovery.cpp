// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Effects/GE_StaminaRecovery.h"
#include "GAS/Attributes/StaminaAttributeSet.h"

UGE_StaminaRecovery::UGE_StaminaRecovery() {

	DurationPolicy = EGameplayEffectDurationType::Infinite;
	Period = .25f;
	bExecutePeriodicEffectOnApplication = false;

	FGameplayModifierInfo modifierInfo;
	modifierInfo.Attribute = UStaminaAttributeSet::GetStaminaAttribute();
	FSetByCallerFloat SetByCallerValue;
	SetByCallerValue.DataTag = FGameplayTag::RequestGameplayTag("Attribute.Stamina.RecoveryValue");
	modifierInfo.ModifierMagnitude = SetByCallerValue;
	Modifiers.Add(modifierInfo);
}