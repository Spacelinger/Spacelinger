// Without Copyright. Project SpaceLinger, Master en Creación de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "Actors/Weapons/CorrosiveSpit/SLCorrosiveSpit.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/DamageEvents.h"

ASLCorrosiveSpit::ASLCorrosiveSpit() { }

void ASLCorrosiveSpit::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ASLCorrosiveSpit::OnBeginOverlap_SphereCollision);
	SphereCollision->OnComponentHit         .AddDynamic(this, &ASLCorrosiveSpit::OnHit_SphereCollision);
}

void ASLCorrosiveSpit::OnBeginOverlap_SphereCollision(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult & SweepResult)
{
	// Following the example from UGameplayStatics::ApplyDamage()
	TSubclassOf<UDamageType> const ValidDamageTypeClass = TSubclassOf<UDamageType>(UDamageType::StaticClass());
	FDamageEvent DamageEvent(ValidDamageTypeClass);
	OtherActor->TakeDamage(INFINITY, DamageEvent, nullptr, nullptr);
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
