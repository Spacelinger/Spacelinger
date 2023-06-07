// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Abilities/GA_SlowTime.h"
#include "Kismet/GameplayStatics.h"
#include <AbilitySystemGlobals.h>
#include "AbilitySystemComponent.h"
#include "AbilityTask_SlowTime.h"

UGA_SlowTime::UGA_SlowTime() {

	// TODO?: Check that a cost GE is set
	if (!GetCostGameplayEffect()) {
		UE_LOG(LogTemp, Warning, TEXT("Cost Gameplay Effect not set for %s!"), *this->GetName());
	}
}

void UGA_SlowTime::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	GetAvatarActorFromActorInfo();

	if (CommitAbility(Handle, ActorInfo, ActivationInfo)) {

		GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("ActiveAbility.SlowTime")));

		// Start AbilityTask_SlowTime
		UAbilityTask_SlowTime* SlowTimeTask = UAbilityTask_SlowTime::SlowTimeGameplayEvent(this, FGameplayTag::RequestGameplayTag(TEXT("Input.SlowTime.Completed")), 
																								FGameplayTag::RequestGameplayTag(TEXT("Attribute.Stamina.Empty")));
		
		SlowTimeTask->SuccessEventReceived.AddDynamic(this, &UGA_SlowTime::OnAbilityTaskCompleted);
		SlowTimeTask->SetSpeedLinesMaterial(PostProcessSpeedLinesMaterial);
		SlowTimeTask->ReadyForActivation();
	}
	else {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_SlowTime::OnAbilityTaskCompleted(FGameplayEventData Payload)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_SlowTime::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{	
	
	// Removing Cost GameplayEffect
	FGameplayTagContainer TagContainer;
	TagContainer.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Cost.SlowTime")));	// Hardcoded, couldn't find how to link with GetCostGameEffect()
	GetAbilitySystemComponentFromActorInfo()->RemoveActiveEffectsWithSourceTags(TagContainer);

	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("ActiveAbility.SlowTime")));

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGA_SlowTime::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const {

	const bool bSuperCanActivate = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	
	if (bSuperCanActivate == false) {
		return false;
	}

	bool bResult = true;

	return bResult;
}
