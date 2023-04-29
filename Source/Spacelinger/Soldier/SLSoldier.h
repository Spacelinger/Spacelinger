#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interact_Luis/Interfaces/InteractInterface.h"
#include "SLSoldier.generated.h"

class UAIPerceptionComponent;
struct FActorPerceptionUpdateInfo;

UCLASS(config=Game)
class ASLSoldier : public ACharacter, public IInteractInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent* AISensingComponent;

public:
	ASLSoldier();
protected:
	virtual void BeginPlay();

private:
	UFUNCTION()
	void OnTargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo);

	// Tick functions we're going to enable and disable as we see fit
	void OnActorDetected();
	void OnActorUndetected();

	TWeakObjectPtr<AActor> DetectedActor; // if null, we're not detecting anyone
	FTimerHandle DetectionTimerHandle; // Timers are going to be set to loop
	float LastTimeSecondsTimer = 0.0f;
	float UpdateAndGetElapsedSeconds();
	float SensingComponentMaxDistance; // NOTE(Sergi): This should not exist, check code to know why

	
public:
	// If we see the player and they are at this distance or closer, they will be detected instanly.
	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Spacelinger|AI")
	float DistanceInstantDetection = 200.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Spacelinger|AI")
	float DetectionSpeed = 0.5f;
	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Spacelinger|AI")
	float UndetectionSpeed = 0.5f;

	// Awareness goes from 0 to 1
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|AI|Internal")
	float CurrentAwareness = 0.0f;
	// If the AI is alerted, but not necessarily detecting the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|AI|Internal")
	bool bIsAlerted = false;

// Interact stuff
public:
	int InteractPriority = 99;
	int GetInteractPriority() const;
	void Interact(AActor* ActorInteracting);
	void SetAsCandidate(bool IsCandidate);
};
