// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Abilities/GA_DoorBlock.h"
#include "Components/InteractingComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilityTask_DoorBlock.h"
//#include "Actors/DoorBlock.h"

UGA_DoorBlock::UGA_DoorBlock()
{

}

void UGA_DoorBlock::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	// Implemented in BP for testing purposes
	K2_CheckAbilityCost(); // Check for cost without consuming it. Cost will be commited once the channeling task is completed
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.DoorBlock.Status.Channeling")));

	// Start AbilityTask_DoorBlock
	UAbilityTask_DoorBlock* DoorBlockTask = UAbilityTask_DoorBlock::DoorBlockChannelingTask(this, FGameplayTag::RequestGameplayTag(TEXT("Ability.DoorBlock.Status.Channeling")), 3.0f);
	
	DoorBlockTask->ChannelingComplete.AddDynamic(this, &UGA_DoorBlock::AbilityChannelComplete);
	DoorBlockTask->ChannelingCanceled.AddDynamic(this, &UGA_DoorBlock::AbilityChannelCanceled);
	DoorBlockTask->ReadyForActivation();
	
	//DoorToBlock = Cast<ADoorBlock>(TriggerEventData->Target);

	//UE_LOG(LogTemp, Warning, TEXT("TARGET: %s"), *InteractableObject.GetName());

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGA_DoorBlock::AbilityChannelComplete()
{
	//DoorToBlock->DoorBlockSuccess();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_DoorBlock::AbilityChannelCanceled()
{
	//DoorToBlock->DoorBlockFail();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_DoorBlock::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.DoorBlock.Status.Channeling")));
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