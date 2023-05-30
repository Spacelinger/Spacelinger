// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "SpiderWeb.h"
#include "Math/UnrealMathUtility.h"
#include "Components/CapsuleComponent.h"
#include "Slime_A.h"
#include <Soldier/SLSoldier.h>



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
	ConstraintComp->AttachToComponent(CableComponent, FAttachmentTransformRules::KeepRelativeTransform);

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("/Game/Characters/Spider/SpiderWeb/M_SpiderWeb"));
	if (FoundMaterial.Succeeded())
	{
		StoredMaterial = FoundMaterial.Object;
		CableComponent->SetMaterial(0, StoredMaterial);
	}

	EndPoint = CreateDefaultSubobject<USphereComponent>(TEXT("EndPoint"));
	EndPoint->SetVisibility(true);
	EndPoint->SetSphereRadius(50.f); // Adjust the radius as desired
	EndPoint->AttachToComponent(CableComponent, FAttachmentTransformRules::KeepRelativeTransform);
	EndPoint->OnComponentBeginOverlap.AddDynamic(this, &ASpiderWeb::OnEndPointCollision);
	EndPoint->Deactivate();

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
	else if (bTrapActivated && Soldier) {


		if (!bInitialRelativePositionSet) {
			initialRelativePosition = getRelativePositionPhysicsConstraint();
			bInitialRelativePositionSet = true;
		}

		if (initialRelativePosition.Length() < 20 && !bTrapFinished) {
			bTrapFinished = true;
			
		}

		if (!bTrapFinished) {
			//ConstraintComp->SetConstraintReferencePosition(EConstraintFrame::Frame2, newRelativeLocation);
			FVector vectorDirection = Soldier->GetMesh()->GetBoneLocation("foot_r") - ConstraintComp->GetComponentLocation();
			FVector newLocation = getVectorInConstraintCoordinates(vectorDirection, 300.0f, DeltaTime);
			initialRelativePosition = initialRelativePosition + newLocation;

			ConstraintComp->SetConstraintReferencePosition(EConstraintFrame::Frame2, initialRelativePosition);


			ConstraintComp->UpdateConstraintFrames();
		}
	}
}


FVector ASpiderWeb::getVectorInConstraintCoordinates(FVector input, float Speed, float DeltaTime) {

	// Get the world space location of the physics constraint and the bone
	FVector WorldBoneLocation = Soldier->GetMesh()->GetBoneLocation("foot_r");

	// Convert the constraint location to the local space of the bone
	FTransform boneTransform = Soldier->GetMesh()->GetBoneTransform(Soldier->GetMesh()->GetBoneIndex("foot_r"));

	// Normalize the input vector
	FVector NormalizedInput = input.GetSafeNormal();

	// Apply speed and DeltaTime to the normalized input vector
	FVector Velocity = NormalizedInput * Speed * DeltaTime;

	// Update input with velocity
	FVector inputAdapted = boneTransform.InverseTransformPosition(WorldBoneLocation + Velocity);

	return inputAdapted;
}



FVector ASpiderWeb::getRelativePositionPhysicsConstraint() {

	// Get the world space location of the physics constraint and the bone
	FVector WorldConstraintLocation = ConstraintComp->GetComponentLocation();
	FVector WorldBoneLocation = Soldier->GetMesh()->GetBoneLocation("foot_r");

	// Convert the constraint location to the local space of the bone
	FTransform boneTransform = Soldier->GetMesh()->GetBoneTransform(Soldier->GetMesh()->GetBoneIndex("foot_r"));

	FVector LocalConstraintLocation = boneTransform.InverseTransformPosition(WorldConstraintLocation);

	return LocalConstraintLocation;
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

void ASpiderWeb::SetTrap()
{
	EndPoint->Activate();
	EndPoint->SetRelativeLocation(CableComponent->EndLocation);

	

	
}

void ASpiderWeb::OnEndPointCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Handle the collision event for the EndPoint component here
	// You can add your desired logic or function calls

	// Example: Print the name of the other actor that collided with the EndPoint
	if (OtherActor && OtherActor->IsA<ASLSoldier>() && EndPoint->IsActive())
	{
		Soldier = Cast<ASLSoldier>(OtherActor);
		if (Soldier)
		{
			ConstraintComp->SetWorldLocation(GetActorLocation());

			ConstraintComp->SetConstrainedComponents(
				StartLocationCable, NAME_None,
				Soldier->GetMesh(), "foot_r"
			);

			// Set the linear motion types to 'limited'
			ConstraintComp->ConstraintInstance.SetLinearLimits(ELinearConstraintMotion::LCM_Locked, ELinearConstraintMotion::LCM_Locked, ELinearConstraintMotion::LCM_Locked, 0.0f);
			bTrapActivated = true;
			distanceTrap = Soldier->GetMesh()->GetBoneLocation("foot_r");
			// Set angular limits to restrict swinging
			ConstraintComp->SetAngularSwing1Limit(EAngularConstraintMotion::ACM_Locked,0.0f);
			ConstraintComp->SetAngularSwing2Limit(EAngularConstraintMotion::ACM_Locked,0.0f);
			ConstraintComp->SetAngularTwistLimit(EAngularConstraintMotion::ACM_Locked, 0.0f);


			CableComponent->EndLocation = FVector(0.0f, 0.0f, 0.0f);
			CableComponent->AttachEndTo.OverrideComponent = Soldier->GetMesh();
			CableComponent->AttachEndToSocketName = "foot_r";

			Soldier->MoveToCeiling();
		}
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

void ASpiderWeb::setFuturePosition(FVector futurePosition, ASlime_A* spiderRef, bool attached) {
	bSetPosition = true;
	initialPosition = futurePosition;
	spider = spiderRef;
	bAttached = attached;
}
