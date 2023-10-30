// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Abilities/GA_PutTrap.h"
#include "AbilitySystemComponent.h"
#include "Spider/Slime_A.h"
#include "Actors/VFX/SLSpiderWebBall.h"
#include "AbilityTask_PutTrap.h"


UGA_PutTrap::UGA_PutTrap()
{
	// Check if cost is set. Might want to delete
}

void UGA_PutTrap::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{	
	ASlime_A* Spider = Cast<ASlime_A>(CurrentActorInfo->OwnerActor);
	AActor* MyChildActor = Spider->SpiderWebBallF->GetChildActor();
	ASLSpiderWebBall* SpiderWebBall = Cast<ASLSpiderWebBall>(MyChildActor);
	
	GetAbilitySystemComponentFromActorInfo()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.PutTrap.Channeling")));

	UAbilityTask_PutTrap* PutTrapTask = UAbilityTask_PutTrap::PutTrapChannelingTask(this, FGameplayTag::RequestGameplayTag(TEXT("Ability.PutTrap.Channeling")), TimeToChannel);

	if (MyChildActor)
	{
		SpiderWebBall->ResetTimeLine();
		Spider->SpiderWebBallF->SetVisibility(true);
	}
	Spider->bIsPuttingTrap = true;

	PutTrapTask->ChannelingComplete.AddDynamic(this, &UGA_PutTrap::AbilityChannelComplete);
	PutTrapTask->ChannelingCanceled.AddDynamic(this, &UGA_PutTrap::AbilityChannelCanceled);
	PutTrapTask->ReadyForActivation();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UGA_PutTrap::ActionPutTrap()
{
	ASlime_A* Spider = Cast<ASlime_A>(CurrentActorInfo->OwnerActor);

	FVector spiderPoint = Spider->GetMesh()->GetSocketLocation("Mouth");

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
		FTransform cableTransform;
		cableTransform.SetLocation(cablePosition);
		ASpiderWeb* spiderWebTrap = GetWorld()->SpawnActorDeferred<ASpiderWeb>(ASpiderWeb::StaticClass(), cableTransform);
		spiderWebTrap->CableComponent->bAttachEnd = true; // Attach the end of the cable to the spider web
		spiderWebTrap->SpiderWebType = EWebType::TrapWeb;
		spiderWebTrap->FinishSpawning(cableTransform);
		if (bHit2)
			spiderWebTrap->CableComponent->EndLocation = -(spiderWebTrap->CableComponent->GetComponentLocation() - Hit2.Location);
		else
			spiderWebTrap->CableComponent->EndLocation = spiderWebTrap->CableComponent->GetComponentLocation() - (spiderPoint + FVector::UpVector * 3000.0f);
		spiderWebTrap->SetTrap();
		
		Spider->AddNewTrap(spiderWebTrap);
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
	ASlime_A* Spider = Cast<ASlime_A>(CurrentActorInfo->OwnerActor);
	AActor* MyChildActor = Spider->SpiderWebBallF->GetChildActor();
	ASLSpiderWebBall* SpiderWebBall = Cast<ASLSpiderWebBall>(MyChildActor);
	//GetAbilitySystemComponentFromActorInfo()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.DoorBlock.Status.Channeling")));
	if (MyChildActor)
	{
		Spider->SpiderWebBallF->SetVisibility(false);
	}
	Spider->bIsPuttingTrap = false;

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