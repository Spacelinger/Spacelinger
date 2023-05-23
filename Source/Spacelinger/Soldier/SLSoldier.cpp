#include "SLSoldier.h"
#include "Components/CapsuleComponent.h"

ASLSoldier::ASLSoldier() {
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ASLSoldier::OnEndPointCollision);
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

void ASLSoldier::OnEndPointCollision(class UPrimitiveComponent* HitComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if(bMoveToCeiling)
		bMoveToCeiling = false;

}


void ASLSoldier::Tick(float DeltaTime) {
	if (bMoveToCeiling) {
		// Apply a constant force to make the ragdoll stick to the ceiling
		FVector ForceDirection = FVector::UpVector; // Adjust the force direction as desired
		float ForceMagnitude = 5000.0f; // Adjust the force magnitude as desired
		GetMesh()->AddForce(ForceDirection * ForceMagnitude, NAME_None, true);
	}
}

void ASLSoldier::MoveToCeiling() {
	/* Disable all collision on capsule */


	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	SetActorEnableCollision(true);

	// Ragdoll
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetAllBodiesSimulatePhysics(true);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->WakeAllRigidBodies();
	MeshComp->bBlendPhysics = true;
	GetMesh()->SetEnableGravity(false);

	bMoveToCeiling = true;
}


