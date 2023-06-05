// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Actors/Weapons/Spear/Spear.h"

#include "Actors/Weapons/StickyPuddle/SLProjectilePuddle.h"
#include "AI/NavigationSystemBase.h"

#define DAMAGE_STRENGTH 10.0f
#define TIME_TO_DISAPPEAR 0.5f

// Sets default values
ASpear::ASpear()
{
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

}

// TASCAAAAAAAAAAAAAAAAAAAAAS
// Usar la clase padre SLProjectile
// Estandarizar la forma de proyectar las trayectorias

// Called when the game starts or when spawned
void ASpear::BeginPlay()
{
	Super::BeginPlay();
	
	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ASpear::OnBeginOverlap_BoxCollision);
}

void ASpear::OnBeginOverlap_BoxCollision(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult & SweepResult)
{
	if (!OtherActor)
		return;

		// Check if OtherActor is different from the player pawn
	const bool bHitPlayer = (OtherActor == GetWorld()->GetFirstPlayerController()->GetPawn());
	const bool bHitProjectil = OtherActor != this && !OtherActor->IsA<ASLProjectile>();
	if (!bHitPlayer && !bHitProjectil)
	{

	}
	// Check if OtherActor is not the spear itself and also not from classes SLProjectilePuddle and SLStickyPuddle
	/* &&
	// Check if OtherActor can be cast to BP_InteractLockDoor_C, and if it can, then check if a "Cylinder" subobject is visible
	(OtherActor->IsA(InteractLockDoor_C::StaticClass()) ? Cast<InteractLockDoor_C>(OtherActor)->Cylinder->IsVisible() : true) */
	{
		// Stop movement immediately, receiving projectile movement as an input
		MovementComponent->StopMovementImmediately();
		// Set projectile gravity scale to 0
		MovementComponent->ProjectileGravityScale = 0;
		BoxCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		// set the CollisionedActor variable to the OtherActor variable
		AActor* CollisionedActor = OtherActor;
		// Apply damage to the collisioned actor of DAMAGE_STRENGTH points
		// CollisionedActor->ApplyDamage(DAMAGE_STRENGTH, GetWorld()->GetFirstPlayerController());
		// Set a delay of TIME_TO_DISAPPEAR seconds before destroying the spear
		GetWorld()->GetTimerManager().SetTimer(DestroyTimer, this, &ASpear::DestroySpear, TIME_TO_DISAPPEAR, false);
	}
}

void ASpear::DestroySpear()
{
	// Destroy the spear
	Destroy();
}
