// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Abilities/GA_DoorBlock.h"
#include "Components/InteractingComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilityTask_DoorBlock.h"
#include "Actors/DoorBlock.h"
#include <Spider/Slime_A.h>

UGA_DoorBlock::UGA_DoorBlock()
{
	// Check if cost is set.
}

void UGA_DoorBlock::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	DoorToBlock = Cast<ADoorBlock>(TriggerEventData->Target);
	
	if (!K2_CheckAbilityCost()) // Check for cost without consuming it. Cost will be commited once the channeling task is completed
	{
		// If can't spend the ability cost, notifying it as a fail == BUG! NOT WORKING
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}

	DoorToBlock->BeginDoorBlock();
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.DoorBlock.Status.Channeling")));


	UAbilityTask_DoorBlock* DoorBlockTask = UAbilityTask_DoorBlock::DoorBlockChannelingTask(this, FGameplayTag::RequestGameplayTag(TEXT("Ability.DoorBlock.Status.Channeling")), TimeToChannel, DoorToBlock);
	
	DoorBlockTask->ChannelingComplete.AddDynamic(this, &UGA_DoorBlock::AbilityChannelComplete);
	DoorBlockTask->ChannelingCanceled.AddDynamic(this, &UGA_DoorBlock::AbilityChannelCanceled);
	DoorBlockTask->ReadyForActivation();
	

	//UE_LOG(LogTemp, Warning, TEXT("TARGET: %s"), *InteractableObject.GetName());

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGA_DoorBlock::AbilityChannelComplete()
{
	CommitAbilityCost(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	DoorToBlock->DoorBlockSuccess();
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_DoorBlock::AbilityChannelCanceled()
{
	DoorToBlock->DoorBlockFail();
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
	{
		Cast<ASlime_A>(CurrentActorInfo->OwnerActor)->WarningNotEnoughStamina();
		DoorToBlock->DoorBlockFail();	// Return as the current stamina cost is less than the ability's cost
		return false;
	}
	
	// Check if owning player has the InteractingComponent
	UInteractingComponent* PlayerInteractComp = GetAvatarActorFromActorInfo()->FindComponentByClass<UInteractingComponent>();
	// Check from the InteractingComponent if has a CurrentInteractableComponent
	if (!PlayerInteractComp->GetCurrentInteractable())
		return false;	// Cancel if not
	
	return true;		// Go on if so
}