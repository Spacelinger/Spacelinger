// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Abilities/GA_StunWeb.h"
#include <Spider/Slime_A.h>
#include <Actors/SpiderProjectile.h>
#include "Components/CapsuleComponent.h"

UGA_StunWeb::UGA_StunWeb()
{
	// Might want to delete
}

void UGA_StunWeb::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	ActionThrowStunWeb();
}

void UGA_StunWeb::ActionThrowStunWeb()
{
	ASlime_A* Spider = Cast<ASlime_A>(CurrentActorInfo->OwnerActor);

	//Cut spider web - really needed?
	if (Spider->spiderWebReference)
		Spider->CutSpiderWeb();

	//Spawn projectile
	FVector2D ScreenLocation = Spider->GetViewportCenter();
	FVector LookDirection = Spider->GetLookDirection(ScreenLocation);
	FVector StartPosition = Spider->GetMesh()->GetSocketLocation("Mouth");

	// Spawn the projectile
	if (ASpiderProjectile* Projectile = GetWorld()->SpawnActor<ASpiderProjectile>(ASpiderProjectile::StaticClass(), StartPosition, FRotator::ZeroRotator))
	{
		// Ignore ASpider
		Projectile->Spider = Spider;
		Projectile->SphereCollider->IgnoreActorWhenMoving(Spider, true);
		// Make ASpider ignore the Projectile
		Spider->GetCapsuleComponent()->IgnoreActorWhenMoving(Projectile, true);
		Spider->GetMesh()->IgnoreActorWhenMoving(Projectile, true);

		// Set projectile velocity
		if (Projectile->ProjectileMovementComponent != nullptr) {
			Projectile->ProjectileMovementComponent->InitialSpeed = 2000.0f;
			Projectile->ProjectileMovementComponent->MaxSpeed = 2000.0f;
			Projectile->ProjectileMovementComponent->Velocity = LookDirection * 1000.0f;
		}
	}

	//Play sound 
	Spider->AudioManager->Spider_StunningWeb();

	//Start cooldown timer
	GetWorld()->GetTimerManager().SetTimer(Spider->StunWebTimerHandle, FTimerDelegate::CreateLambda([&] { bCanThrowStunWeb = true; }), Spider->SecondsBetweenStuns, false);
	bCanThrowStunWeb = false;

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_StunWeb::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UGA_StunWeb::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	const bool bCanActivate = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	
	// Any additional checks performed here
	if (bCanThrowStunWeb)
	{
		if (CheckCost(Handle, ActorInfo) == false)
			Cast<ASlime_A>(CurrentActorInfo->OwnerActor)->WarningNotEnoughStamina();
	}

	return bCanActivate && bCanThrowStunWeb;
}