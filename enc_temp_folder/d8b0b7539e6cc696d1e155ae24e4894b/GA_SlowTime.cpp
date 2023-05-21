// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Abilities/GA_SlowTime.h"

UGA_SlowTime::UGA_SlowTime() {

	//AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Fullbody")));

}

void UGA_SlowTime::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CommitAbility(Handle, ActorInfo, ActivationInfo)) {

	}
	else {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}
void UGA_SlowTime::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}