// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "Actors/Weapons/StickyPuddle/SLProjectilePuddle.h"
#include "Actors/Weapons/StickyPuddle/SLStickyPuddle.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

ASLProjectilePuddle::ASLProjectilePuddle() { }

void ASLProjectilePuddle::BeginPlay()
{
	Super::BeginPlay();

	CapsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &ASLProjectilePuddle::OnBeginOverlap_SphereCollision);
	CapsuleCollision->OnComponentHit         .AddDynamic(this, &ASLProjectilePuddle::OnHit_SphereCollision);
}

void ASLProjectilePuddle::OnBeginOverlap_SphereCollision(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult & SweepResult)
{
	/* TODO(Sergi): Properly handle collisions instead of checking them in code */
	// These are things we don't want to collide with
	if (ASLProjectilePuddle *OtherProjectile = Cast<ASLProjectilePuddle>(OtherActor)) { return; }
	if (OtherActor == UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) { return; }
	/* END OF TODO */

	// TODO: Get Component Status Effect and apply paralyzed. Same stuff as SLStickyPuddle
	// OtherActor->GetComponentByClass(...)

	if (ASLStickyPuddle *OtherPuddle = Cast<ASLStickyPuddle>(OtherActor)) {
		OtherPuddle->ResetLifeSpan();
		Destroy();
	}
	/*else if (UProjectileMovementComponent *PMC = Cast<UProjectileMovementComponent>(OtherActor->GetComponentByClass(UProjectileMovementComponent::StaticClass()))) {
		Destroy();
	}*/

}

void ASLProjectilePuddle::OnHit_SphereCollision(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	ensureMsgf(!StickyPuddleBlueprint.IsNull() && StickyPuddleBlueprint.IsValid(), TEXT("ERROR: SLProjectilePuddle doesn't have a StickyPuddleBlueprint to spawn!"));

	// Check if we've hit the floor
	float AmountLookingUp = FVector::DotProduct(Hit.Normal, FVector::UpVector);
	if (AmountLookingUp > UpAmountThreshold) {

		FTransform SelfTransform = GetTransform();
		FVector  Location = SelfTransform.GetLocation();
		FRotator Rotator  = SelfTransform.Rotator();
			
		AActor* SpawnedActor = GetWorld()->SpawnActor(StickyPuddleBlueprint.Get(), &Location, &Rotator);

		Destroy();
	}
}
