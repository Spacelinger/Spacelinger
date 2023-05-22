// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "SpiderWeb.h"
#include "Math/UnrealMathUtility.h"
#include "Slime_A.h"


// Sets default values
ASpiderWeb::ASpiderWeb()
{
	PrimaryActorTick.bCanEverTick = true;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// Create and attach the cable component
	CableComponent = CreateDefaultSubobject<UCableComponent>(TEXT("CableComponent"));
	RootComponent = CableComponent;

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

void ASpiderWeb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bSetPosition) {
		FVector CurrentLocation = GetActorLocation();
		FVector NewLocation = FMath::Lerp(CurrentLocation, initialPosition, 0.5f);
		SetActorLocation(NewLocation);

		// Check if the actor has reached the final position
		if (FVector::DistSquared(CurrentLocation, initialPosition) <= FMath::Square(0.1f)) {
			bSetPosition = false; // Stop updating the position
			if(bAttached)
				spider->JumpToPosition();
			else {
				this->CableComponent->bAttachEnd = false;
				spider->bHasTrownSpiderWeb = false;
			}
		}
	}
}

void ASpiderWeb::ResetConstraint()
{
	ConstraintComp->SetLinearXLimit(ELinearConstraintMotion::LCM_Free, 0.0f);
	ConstraintComp->SetLinearYLimit(ELinearConstraintMotion::LCM_Free, 0.0f);
	ConstraintComp->SetLinearZLimit(ELinearConstraintMotion::LCM_Free, 0.0f);

	ConstraintComp->SetConstrainedComponents(
		nullptr, NAME_None,
		nullptr, NAME_None
	);
	CableComponent->bAttachEnd = false;
	ConstraintComp = nullptr;
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

void ASpiderWeb::setFuturePosition(FVector futurePosition, ASlime_A* spiderRef, bool attached) {
	bSetPosition = true;
	initialPosition = futurePosition;
	spider = spiderRef;
	bAttached = attached;
}
