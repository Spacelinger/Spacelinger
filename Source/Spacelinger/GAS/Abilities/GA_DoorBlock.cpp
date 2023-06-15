// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Abilities/GA_DoorBlock.h"
#include "Components/InteractingComponent.h"
#include "AbilitySystemComponent.h"

UGA_DoorBlock::UGA_DoorBlock()
{

}

void UGA_DoorBlock::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	//Get the interactable component and perform sth
}

void UGA_DoorBlock::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGA_DoorBlock::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	const bool bCanActivate = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	if (bCanActivate == false)
		return false;
	
	// Check if owning player has the InteractingComponent
	UInteractingComponent* PlayerInteractComp = GetAvatarActorFromActorInfo()->FindComponentByClass<UInteractingComponent>();
	// Check from the InteractingComponent if has a CurrentInteractableComponent
	if (!PlayerInteractComp->GetCurrentInteractable())
		return false;	// Cancel if not
	
	
	return true;		// Go on if so
}