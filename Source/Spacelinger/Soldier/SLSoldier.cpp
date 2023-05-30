#include "SLSoldier.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/StaticMeshActor.h"

ASLSoldier::ASLSoldier() {
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	GetMesh()->OnComponentHit.AddDynamic(this, &ASLSoldier::OnEndPointCollision);
}

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
	/* Disable all collision on capsule */


	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	SetActorEnableCollision(true);

	// Ragdoll
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetSimulatePhysics(true);
	MeshComp->WakeAllRigidBodies();
	MeshComp->bBlendPhysics = true;
	// Enable "Simulation Generates Hit Events"
	MeshComp->SetNotifyRigidBodyCollision(true);
	
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


