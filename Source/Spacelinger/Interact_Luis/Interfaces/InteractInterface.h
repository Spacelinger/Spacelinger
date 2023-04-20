#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

UINTERFACE(Blueprintable)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

class IInteractInterface
{
	GENERATED_BODY()

public:

	virtual void Interact(AActor* ActorInteracting) {}
	virtual int GetInteractPriority() const { return 99; }
	virtual void SetAsCandidate(bool IsCandidate) {}	// TO-DO
};