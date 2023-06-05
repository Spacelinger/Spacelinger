// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "Actors/Weapons/CorrosiveSpit/SLCorrosiveSpit.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/DamageEvents.h"

ASLCorrosiveSpit::ASLCorrosiveSpit() { }

void ASLCorrosiveSpit::BeginPlay()
{
	Super::BeginPlay();

	CapsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &ASLCorrosiveSpit::OnBeginOverlap_SphereCollision);
	CapsuleCollision->OnComponentHit         .AddDynamic(this, &ASLCorrosiveSpit::OnHit_SphereCollision);
}

void ASLCorrosiveSpit::OnBeginOverlap_SphereCollision(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult & SweepResult)
{
	/* TODO(Sergi): Properly handle collisions instead of checking them in code */
	// These are things we don't want to collide with
	if (ASLCorrosiveSpit *OtherProjectile = Cast<ASLCorrosiveSpit>(OtherActor)) { return; }
	if (!GetWorld()) { return; }
	if (OtherActor == UGameplayStatics::GetPlayerPawn(GetWorld(), 0)) { return; }
	/* END OF TODO */

	// Following the example from UGameplayStatics::ApplyDamage()
	FDamageEvent DamageEvent(UDamageType::StaticClass());
	OtherActor->TakeDamage(INFINITY, DamageEvent, nullptr, nullptr);
	Destroy();
}

void ASLCorrosiveSpit::OnHit_SphereCollision(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	Destroy();
}
