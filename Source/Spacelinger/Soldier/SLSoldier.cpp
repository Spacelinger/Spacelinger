#include "SLSoldier.h"
#include "Components/CapsuleComponent.h"

ASLSoldier::ASLSoldier() {
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
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
