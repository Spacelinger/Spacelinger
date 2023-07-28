// Actors/SpiderProjectile.cpp

#include "Actors/SpiderProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include <Soldier/SLSoldier.h>

ASpiderProjectile::ASpiderProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    // Set up the sphere collider
    SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
    RootComponent = SphereCollider;
    SphereCollider->InitSphereRadius(10.0f * 0.6f); // Reduced by 40%

    // Set up the projectile movement component
    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
    ProjectileMovementComponent->InitialSpeed = InitialSpeed;
    ProjectileMovementComponent->MaxSpeed = InitialSpeed;
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->bShouldBounce = false;
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f; // Ignore gravity
    ProjectileMovementComponent->SetUpdatedComponent(SphereCollider);

    // Set up the sphere mesh
    SphereMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereMesh"));
    SphereMesh->SetupAttachment(RootComponent);

    // Load the sphere mesh from the Starter Content
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMeshAsset(TEXT("/Game/StarterContent/Props/MaterialSphere"));
    if (SphereMeshAsset.Succeeded())
    {
        SphereMesh->SetStaticMesh(SphereMeshAsset.Object);
    }

    // Load the material for the sphere mesh
    static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("/Game/Characters/Spider/SpiderWeb/M_SpiderWeb"));
    if (FoundMaterial.Succeeded())
    {
        SphereMesh->SetMaterial(0, FoundMaterial.Object);
    }

    // Set the scale of the SphereMesh based on the SphereCollider radius
    float ColliderRadius = SphereCollider->GetUnscaledSphereRadius();
    SphereMesh->SetWorldScale3D(FVector(ColliderRadius / 50.0f * 0.6f)); // Reduced by 40%

    SphereCollider->SetCollisionProfileName(TEXT("BlockAll"));

    SphereCollider->OnComponentHit.AddDynamic(this, &ASpiderProjectile::OnSphereColliderHit);
}


void ASpiderProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ASpiderProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void ASpiderProjectile::OnSphereColliderHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Destroy the actor when the SphereCollider collides with another actor
	if (ASLSoldier* Soldier = Cast<ASLSoldier>(OtherActor)) {
		Soldier->Stun(10.0f, this->GetActorLocation());
	}
	Destroy();

}
