// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Abilities/GA_PutSpiderWeb.h"
#include "Actors/LaserPuzzle/SLLaserPuzzle.h"
#include <Slime_A.h>

UGA_PutSpiderWeb::UGA_PutSpiderWeb()
{
	// Check if cost is set. Might want to delete
}

void UGA_PutSpiderWeb::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		ActionPutSpiderWeb();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_PutSpiderWeb::ActionPutSpiderWeb()
{
	ASlime_A* Spider = Cast<ASlime_A>(CurrentActorInfo->OwnerActor);

	//if (Spider->bJumpToLocation)	This is checked before Ability activation in CanActivateAbility
	//	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

	FVector spiderPoint = Spider->GetMesh()->GetSocketLocation("SpiderWebPoint");

	if (Spider->spiderWebReference != nullptr)
	{
		Spider->CutSpiderWeb();
	}
	else
	{
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Spider);
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, spiderPoint, spiderPoint + Spider->GetActorUpVector() * -TraceDistance, ECC_Visibility, Params);

		if (bHit)
		{
			FVector cablePosition = Hit.Location;
			Spider->spiderWebReference = GetWorld()->SpawnActor<ASpiderWeb>(ASpiderWeb::StaticClass(), cablePosition, FRotator::ZeroRotator);
			Spider->spiderWebReference->CableComponent->EndLocation = spiderPoint - Spider->spiderWebReference->CableComponent->GetComponentLocation();
			Spider->spiderWebReference->CableComponent->bAttachEnd = true; // Attach the end of the cable to the spider web
			Spider->attached = true;
			Spider->attachedAtCeiling = IsCeiling(Spider->previousNormal);

			if (ASLLaserPuzzle *LaserPuzzle = Cast<ASLLaserPuzzle>(Hit.GetActor())) {
				UE_LOG(LogTemp, Display, TEXT("SERGI: Hey, we hit a laser puzzle for the first time!"), *Hit.GetActor()->GetName());
				LaserPuzzle->LastAttachedWeb = Spider->spiderWebReference;
			}
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_PutSpiderWeb::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGA_PutSpiderWeb::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	const bool bCanActivate = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	if (bCanActivate == false)
	{
		// Here is returned false if the current stamina cost is less than the ability's cost
		return false;
	}

	ASlime_A* Spider = Cast<ASlime_A>(CurrentActorInfo->OwnerActor);
	if (Spider->bJumpToLocation)
		return false;

	return true;		// Go on if so
}