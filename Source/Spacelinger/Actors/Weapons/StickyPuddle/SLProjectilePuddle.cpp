// Without Copyright. Project SpaceLinger, Master en Creación de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "Actors/Weapons/StickyPuddle/SLProjectilePuddle.h"
#include "Actors/Weapons/StickyPuddle/SLStickyPuddle.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

ASLProjectilePuddle::ASLProjectilePuddle() { }

void ASLProjectilePuddle::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ASLProjectilePuddle::OnBeginOverlap_SphereCollision);
	SphereCollision->OnComponentHit         .AddDynamic(this, &ASLProjectilePuddle::OnHit_SphereCollision);
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
	if (ASLProjectilePuddle* OtherProjectile = Cast<ASLProjectilePuddle>(OtherActor)) { return; }
	if (!GetWorld()) { return; }
	if (OtherActor == UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) { return; }
	/* END OF TODO */

	// TODO: Get Component Status Effect and apply paralyzed. Same stuff as SLStickyPuddle
	// OtherActor->GetComponentByClass(...)

	Destroy();
}

#pragma optimize("", off)

void ASLProjectilePuddle::OnHit_SphereCollision(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	ensure(!StickyPuddleBlueprint.IsNull());

	// Check if we've hit the floor
	float AmountLookingUp = FVector::DotProduct(Hit.Normal, FVector::UpVector);
	if (AmountLookingUp > UpAmountThreshold) {
		if (!StickyPuddleBlueprint.IsNull() && StickyPuddleBlueprint.IsValid()) {
			FTransform SelfTransform = GetTransform();
			FVector  Location = SelfTransform.GetLocation();
			FRotator Rotator  = SelfTransform.Rotator();
			
			AActor* SpawnedActor = GetWorld()->SpawnActor(StickyPuddleBlueprint.Get(), &Location, &Rotator);
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("SLProjectilePuddle.cpp -> StickyPuddleBlueprint hasn't been set!"));
		}

		Destroy();
	}
}

#pragma optimize("", on)
