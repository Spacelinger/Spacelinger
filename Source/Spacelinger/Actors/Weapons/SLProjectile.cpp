// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "Actors/Weapons/SLProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"

ASLProjectile::ASLProjectile()
{
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Sphere Collision"));
	CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // NOTE(Sergi): Maybe this should be a new one?
	CapsuleCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		CapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
		CapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,         ECollisionResponse::ECR_Overlap);
		CapsuleCollision->SetCollisionProfileName("Projectile");
	RootComponent = CapsuleCollision;

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement Component"));

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Main Mesh"));
	MainMesh->SetupAttachment(RootComponent);
}

void ASLProjectile::BeginPlay()
{
	Super::BeginPlay();
}