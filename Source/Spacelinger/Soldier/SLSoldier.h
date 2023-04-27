#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interact_Luis/Interfaces/InteractInterface.h"
#include "SLSoldier.generated.h"

// Add Interact interface?

UCLASS(config=Game)
class ASLSoldier : public ACharacter, public IInteractInterface
{
	GENERATED_BODY()

public:
	ASLSoldier();

protected:
	virtual void BeginPlay();


// Interact stuff
public:
	int InteractPriority = 99;
	int GetInteractPriority() const;
	void Interact(AActor* ActorInteracting);
	void SetAsCandidate(bool IsCandidate);
};
