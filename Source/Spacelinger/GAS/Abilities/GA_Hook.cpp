// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Abilities/GA_Hook.h"
#include "Spider/Slime_A.h"
#include <Kismet/GameplayStatics.h>
#include "Engine/StaticMeshActor.h"

UGA_Hook::UGA_Hook()
{
	// Check if cost is set. Might want to delete
}

void UGA_Hook::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		ActionThrowHook();
	}
	else
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

// Ability helpers

FVector2D GetViewportCenter(AActor* Actor)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(Actor, 0);
	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
	return FVector2D(ViewportSizeX / 2, ViewportSizeY / 2);
}

FVector GetLookDirection(FVector2D ScreenLocation, AActor* Actor)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(Actor, 0);
	FVector LookDirection;
	FVector WorldLocation;
	PlayerController->DeprojectScreenPositionToWorld(ScreenLocation.X, ScreenLocation.Y, WorldLocation, LookDirection);
	return LookDirection;
}

void UGA_Hook::ActionThrowHook()
{
	ASlime_A* Spider = Cast<ASlime_A>(CurrentActorInfo->OwnerActor);

	//bisHook is True on entering this function

	if (!Spider->bHasTrownSpiderWeb)
	{
		if (Spider->spiderWebReference != nullptr) {
			Spider->CutSpiderWeb();
		}
		
		Spider->bHasTrownSpiderWeb = true;
		FVector2D ScreenLocation = GetViewportCenter(Spider);
		FVector LookDirection = GetLookDirection(ScreenLocation, Spider);
		FVector StartPosition = Spider->GetMesh()->GetSocketLocation("Mouth");
		//float LineTraceDistance = 1000.0f;
		FVector EndPosition = StartPosition + (LookDirection * Spider->MaxHookLineTraceDistance);
		
		FHitResult HitResult;
		GetWorld()->LineTraceSingleByChannel(HitResult, StartPosition, EndPosition, ECC_Visibility);
		
		float DistanceHook = FVector::Distance(HitResult.ImpactPoint,StartPosition);

		FTransform CableTransform;
		CableTransform.SetLocation(StartPosition);

		if (DistanceHook <= Spider->MinHookLineTraceDistance) {
			Spider->CutSpiderWeb();
		}
		else {
			if (HitResult.bBlockingHit)
			{
				//SpawnAndAttachSpiderWeb(StartPosition, HitResult.Location, true, true);

				//Spider->spiderWebReference = GetWorld()->SpawnActor<ASpiderWeb>(ASpiderWeb::StaticClass(), StartPosition, FRotator::ZeroRotator);
				Spider->spiderWebReference = GetWorld()->SpawnActorDeferred<ASpiderWeb>(ASpiderWeb::StaticClass(), CableTransform);
				Spider->spiderWebReference->CableComponent->bAttachEnd = true;
				Spider->spiderWebReference->SpiderWebType = EWebType::HookWeb;
				Spider->spiderWebReference->CableComponent->EndLocation = FVector(0, 0, 0);
				Spider->spiderWebReference->CableComponent->SetAttachEndToComponent(Spider->GetMesh(), "Mouth");
				Spider->spiderWebReference->setFuturePosition(HitResult.Location, Spider, true, true);
				Spider->spiderWebReference->FinishSpawning(CableTransform);
			}
			else
			{
				//SpawnAndAttachSpiderWeb(StartPosition, EndPosition, false, true);

				//Spider->spiderWebReference = GetWorld()->SpawnActor<ASpiderWeb>(ASpiderWeb::StaticClass(), StartPosition, FRotator::ZeroRotator);
				Spider->spiderWebReference = GetWorld()->SpawnActorDeferred<ASpiderWeb>(ASpiderWeb::StaticClass(), CableTransform);
				Spider->spiderWebReference->CableComponent->bAttachEnd = true;
				Spider->spiderWebReference->CableComponent->EndLocation = FVector(0, 0, 0);
				Spider->spiderWebReference->CableComponent->SetAttachEndToComponent(Spider->GetMesh(), "Mouth");
				Spider->spiderWebReference->setFuturePosition(EndPosition, Spider, false, true);
				Spider->spiderWebReference->FinishSpawning(CableTransform);

			}
			// At this point the web has been shot, may it hit or not
			if (!HookShotSoundFXArray.IsEmpty())
			{
				USoundCue* HookShotSoundFX = HookShotSoundFXArray[FMath::RandRange(int32(0), HookShotSoundFXArray.Num() - 1)];
				float PitchModulation = FMath::RandRange(0.75f, 1.25f);
				UGameplayStatics::SpawnSound2D(this, HookShotSoundFX, 1.0f, PitchModulation);
			}
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_Hook::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGA_Hook::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	const bool bCanActivate = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	if (bCanActivate == false)
	{
		// Here is returned false if the current stamina cost is less than the ability's cost
		return false;
	}

	return true;		// Go on if so
}