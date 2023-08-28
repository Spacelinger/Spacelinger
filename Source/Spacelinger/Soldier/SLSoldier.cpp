#include "SLSoldier.h"

#include "SoldierAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Components/WidgetComponent.h"
#include "UI/Soldier/SLDetectionWidget.h"
#include "Soldier/SLSoldierPath.h"

ASLSoldier::ASLSoldier() {
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	GetMesh()->OnComponentHit.AddDynamic(this, &ASLSoldier::OnEndPointCollision);

	DetectionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DetectionWidget"));
	DetectionWidget->SetupAttachment(RootComponent);
	DetectionWidget->SetWidgetSpace(EWidgetSpace::Screen);
}

void ASLSoldier::OnConstruction(const FTransform& Transform) {
	if (PatrolType == SLIdleType::PatrolGuided) {
		if (!PathActor) {
			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = this;
			PathActor = GetWorld()->SpawnActor<ASLSoldierPath>(ASLSoldierPath::StaticClass(), GetTransform(), SpawnParameters);
		}
	}
}


void ASLSoldier::BeginPlay() {
	Super::BeginPlay();

	if (USLDetectionWidget *DetectionInterface = Cast<USLDetectionWidget>(DetectionWidget->GetWidget())) {
		DetectionInterface->OwningActor = this;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ERROR! Soldier's DetectionWidget is not USLDetectionInterface"));
	}

	if (OffscreenDetectionWidgetClass) {
		OffscreenDetectionWidget = Cast<USLDetectionWidget>(CreateWidget(GetGameInstance()->GetPrimaryPlayerController(), OffscreenDetectionWidgetClass));
		if (OffscreenDetectionWidget) {
			OffscreenDetectionWidget->AddToViewport();
			OffscreenDetectionWidget->OwningActor = this;
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ERROR! Soldier's OffscreenDetectionWidgetClass is not set"));
	}

	if (PatrolType == SLIdleType::PatrolGuided) {
		if (PathActor->bInitialPositionAsPath) {
			FVector CurrentLocation = GetActorLocation();
			WorldPatrolPoints.Add(CurrentLocation);
		}
		for (FVector PP : PathActor->PatrolPoints) {
			WorldPatrolPoints.Add(PathActor->GetActorLocation() + PP);
		}
	}
}

/** LUIS WAS HERE: Interacting has been heavily refactored. We can review together
//------------------------//
//     Interact stuff     //
//------------------------//
int ASLSoldier::GetInteractPriority() const {
	// TODO(Sergi): What if there's other enemies nearby who can be killed but this one can't? We should lower priority based on that!
	return InteractPriority;
}
void ASLSoldier::Interact(AActor* ActorInteracting) {
	// TODO(Sergi): Things that probably need to happen
	// - Check if enemy can die (won't be able to if it's seeing the player or is already dying)
	// - Play dying animation and set state to dying
	// - Delete actor at some point
}
void ASLSoldier::SetAsCandidate(bool IsCandidate) {
	// TO-DO

	if (IsCandidate) 
	{
		// Show UI to interact
	}
	else 
	{
		// Hide UI to interact
	}
}
*/

void ASLSoldier::OnEndPointCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	// Check if OtherActor is a StaticMesh
	/*AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(OtherActor);
	if (StaticMeshActor) {
		if (bMoveToCeiling) {

			FTimerHandle TimerHandle;

			// Start the timer
			float DelaySeconds = 2.0f; // Delay in seconds before calling the method
			GetWorldTimerManager().SetTimer(TimerHandle, this, &ASLSoldier::StopMoveToCeiling, DelaySeconds, false);
		}
		
	}
	*/
}





void ASLSoldier::Tick(float DeltaTime) {
	/*if (bMoveToCeiling) {
		// Apply a constant force to make the ragdoll stick to the ceiling
		FVector ForceDirection = FVector::UpVector; // Adjust the force direction as desired
		float ForceMagnitude = 5000.0f; // Adjust the force magnitude as desired
		//GetMesh()->AddForce(ForceDirection * ForceMagnitude, NAME_None, true);
		GetMesh()->AddForceToAllBodiesBelow(ForceDirection * ForceMagnitude, FName("pelvis"), false, true);
	}
	*/
}

void ASLSoldier::MoveToCeiling() {
	Die(nullptr);

	/* Disable all collision on capsule */
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	SetActorEnableCollision(true);

	// Ragdoll
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetSimulatePhysics(true);
	MeshComp->WakeAllRigidBodies();
	MeshComp->bBlendPhysics = true;
	// Enable "Simulation Generates Hit Events"
	MeshComp->SetNotifyRigidBodyCollision(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetEnableGravity(false);
	
}

void ASLSoldier::AdaptToCeiling() {
	/*
	// Disable physics-based movement for the OtherComp
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetEnableGravity(false);

	// Optionally, you can also set the component's velocity to zero to stop any residual movement
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);

	GetCharacterMovement()->Deactivate();
	*/
}


void ASLSoldier::StopAdaptToCeiling() {
	/* Disable all collision on capsule 
	bMoveToCeiling = false;

	// Disable physics-based movement for the OtherComp
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetEnableGravity(false);

	// Optionally, you can also set the component's velocity to zero to stop any residual movement
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);

	GetCharacterMovement()->Deactivate();
	*/
}

void ASLSoldier::ReceiveDamage(AActor *DamageDealer)
{
	Die(DamageDealer);
}

void ASLSoldier::Stun(float StunDuration, FVector ThrowLocation)
{
	bIsStunned = true;
	GetCharacterMovement()->DisableMovement();
	// Get the controller of the character (SoldierAIController) --- CHANGE THIS!!!
	ASoldierAIController* ControllerReference = Cast<ASoldierAIController>(GetController());
	ControllerReference->StopLogic();
	ControllerReference->SetIsAlerted(true);
	ControllerReference->DetectedLocation = ThrowLocation;
	
	GetWorldTimerManager().SetTimer(UnstunTimerHandle, this, &ASLSoldier::Unstun, StunDuration, false);
}

void ASLSoldier::Unstun() 
{
	if (bIsDead) return;

	bIsStunned = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	ASoldierAIController* ControllerReference = Cast<ASoldierAIController>(GetController());
	ControllerReference->ResumeLogic();
	ControllerReference->RefreshDetectionTimers();
}

float ASLSoldier::GetRemainingTimeToUnstunAsPercentage() {
	float Elapsed   = GetWorldTimerManager().GetTimerElapsed(UnstunTimerHandle);
	float Remaining = GetWorldTimerManager().GetTimerRemaining(UnstunTimerHandle);
	return Remaining / (Elapsed+Remaining);
}

bool ASLSoldier::IsStunned()
{
	return bIsStunned;
}


void ASLSoldier::Die(AActor *Killer)
{
	bIsDead = true;
	DetectionWidget->Deactivate();
	OffscreenDetectionWidget->RemoveFromParent();

	GetController()->UnPossess();

	// Disable collision and physics-based movement for the soldier's components
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	SoldierHasDied(Killer);
}

FVector ASLSoldier::GetNextPatrolPoint() {
	int i = CurrentPatrolPointIndex;
	CurrentPatrolPointIndex = (CurrentPatrolPointIndex+1) % WorldPatrolPoints.Num();
	return WorldPatrolPoints[i];
}