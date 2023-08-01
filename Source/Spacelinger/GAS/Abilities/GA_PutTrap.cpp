// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Abilities/GA_PutTrap.h"
#include "AbilitySystemComponent.h"
#include "Spider/Slime_A.h"
#include "AbilityTask_PutTrap.h"


UGA_PutTrap::UGA_PutTrap()
{
	// Check if cost is set. Might want to delete
}

void UGA_PutTrap::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	/*if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		ActionPutTrap();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}*/

	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.PutTrap.Channeling")));

	UAbilityTask_PutTrap* PutTrapTask = UAbilityTask_PutTrap::PutTrapChannelingTask(this, FGameplayTag::RequestGameplayTag(TEXT("Ability.PutTrap.Channeling")), TimeToChannel);

	PutTrapTask->ChannelingComplete.AddDynamic(this, &UGA_PutTrap::AbilityChannelComplete);
	PutTrapTask->ChannelingCanceled.AddDynamic(this, &UGA_PutTrap::AbilityChannelCanceled);
	PutTrapTask->ReadyForActivation();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGA_PutTrap::ActionPutTrap()
{
	ASlime_A* Spider = Cast<ASlime_A>(CurrentActorInfo->OwnerActor);

	FVector spiderPoint = Spider->GetMesh()->GetSocketLocation("SpiderWebPoint");

	FHitResult Hit;
	FHitResult Hit2;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Spider);
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, spiderPoint, spiderPoint + Spider->GetActorUpVector() * -TraceDistance, ECC_Visibility, Params);
	bool bHit2 = GetWorld()->LineTraceSingleByChannel(Hit2, spiderPoint, spiderPoint + FVector::UpVector * -3000.0f, ECC_Visibility, Params);
	//DrawDebugLine(GetWorld(), spiderPoint + Spider->GetActorUpVector(), spiderPoint + FVector::UpVector * -3000.0f, FColor::Red, false, 0.2f, 0, 1.0f);

	if (bHit)
	{
		FVector cablePosition = Hit.Location;
		ASpiderWeb* spiderWebTrap = GetWorld()->SpawnActor<ASpiderWeb>(ASpiderWeb::StaticClass(), cablePosition, FRotator::ZeroRotator);
		spiderWebTrap->CableComponent->bAttachEnd = true; // Attach the end of the cable to the spider web
		if (bHit2)
			spiderWebTrap->CableComponent->EndLocation = -(spiderWebTrap->CableComponent->GetComponentLocation() - Hit2.Location);
		else
			spiderWebTrap->CableComponent->EndLocation = spiderWebTrap->CableComponent->GetComponentLocation() - (spiderPoint + FVector::UpVector * 3000.0f);
		spiderWebTrap->SetTrap();
	}

	//EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}


void UGA_PutTrap::AbilityChannelComplete()
{
	// TODO: Channeling task
	ActionPutTrap();
	CommitAbilityCost(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_PutTrap::AbilityChannelCanceled()
{
	// TODO: Channeling task
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_PutTrap::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	//GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.DoorBlock.Status.Channeling")));
	GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.PutTrap.Channeling")));
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGA_PutTrap::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	const bool bCanActivate = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	if (bCanActivate == false)
	{
		// Here we return false, because the current stamina or available spider traps from the cost is less than the ability's cost
		return false;
	}

	// Check if owning player has enough Spider traps. Return false if not

	return true;		// Go on if so
}