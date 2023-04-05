// Without Copyright. Project SpaceLinger, Master en Creación de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "Actors/Weapons/StickyPuddle/SLStickyPuddle.h"
#include "Components/DecalComponent.h"
#include "Components/BoxComponent.h"

ASLStickyPuddle::ASLStickyPuddle() {
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Collision"));
	// These options are set in the editor
	/*
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BoxCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	*/
	RootComponent = BoxCollision;

	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Decal->SetupAttachment(RootComponent);
}

void ASLStickyPuddle::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ASLStickyPuddle::OnBeginOverlap_BoxCollision);
	SetLifeSpan(LifeSpan);
}

void ASLStickyPuddle::OnBeginOverlap_BoxCollision(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult & SweepResult)
{
	// Check if enemy, otherwise ignore

	// TODO: Get Component Status Effect and apply paralyzed. Same stuff as SLProjectilePuddle
	// OtherActor->GetComponentByClass(...)

	Destroy();

}