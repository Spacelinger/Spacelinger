// Without Copyright. Project SpaceLinger, Master en Creación de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "Actors/Weapons/SLProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

ASLProjectile::ASLProjectile()
{
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collision"));
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SphereCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic); // NOTE(Sergi): Maybe this should be a new one?
	SphereCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
		SphereCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
		SphereCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,         ECollisionResponse::ECR_Overlap);
	RootComponent = SphereCollision;

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement Component"));

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Main Mesh"));
	MainMesh->SetupAttachment(RootComponent);
}

void ASLProjectile::BeginPlay()
{
	Super::BeginPlay();
}