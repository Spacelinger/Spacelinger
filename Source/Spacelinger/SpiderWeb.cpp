// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "SpiderWeb.h"

// Sets default values
ASpiderWeb::ASpiderWeb()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// Create and attach the cable component
	CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("CableComponent"));
	RootComponent = CableComponent;

	CableComponent->SetSimulatePhysics(true);

	CableComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CableComponent->SetCollisionObjectType(ECC_PhysicsBody); // Changed from ECC_WorldDynamic
	CableComponent->SetCollisionResponseToAllChannels(ECR_Overlap); // Changed from ECR_Block

	// Create and attach the sphere component
	StartLocationCable = CreateDefaultSubobject<USphereComponent>(TEXT("InvisibleComponent"));
	StartLocationCable->SetVisibility(true);
	StartLocationCable->SetSphereRadius(10.f); // Adjust the radius as desired
	StartLocationCable->AttachToComponent(CableComponent, FAttachmentTransformRules::KeepRelativeTransform);

	ConstraintComp = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("ConstraintComp"));
	ConstraintComp->AttachToComponent(StartLocationCable, FAttachmentTransformRules::KeepRelativeTransform);

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("/Game/Characters/Spider/SpiderWeb/M_SpiderWeb"));
	if (FoundMaterial.Succeeded())
	{
		StoredMaterial = FoundMaterial.Object;
		CableComponent->SetMaterial(0, StoredMaterial);
	}
}

void ASpiderWeb::BeginPlay()
{
	Super::BeginPlay();

	if (CableComponent && StartLocationCable)
	{
		// Set the StartLocationCable's position to match the CableComponent's position
		StartLocationCable->SetWorldLocation(CableComponent->GetComponentLocation());
	}
}
