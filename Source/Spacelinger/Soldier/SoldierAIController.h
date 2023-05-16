// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "SoldierAIController.generated.h"

class UAIPerceptionComponent;
struct FActorPerceptionUpdateInfo;
class UAISenseConfig_Sight;

UCLASS()
class SPACELINGER_API ASoldierAIController : public AAIController
{
	GENERATED_BODY()

	ASoldierAIController();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	UAIPerceptionComponent *AIPerceptionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI, meta = (AllowPrivateAccess = "true"))
	UAISenseConfig_Sight *AISenseConfigSight;

protected:
	virtual void BeginPlay();
	
public:
	// If we see the player and they are at this distance or closer, they will be detected instanly.
	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Spacelinger|AI", meta = (UIMin = "0.0"))
	float DistanceInstantDetection = 200.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Spacelinger|AI", meta = (UIMin = "0.0", UIMax = "2.0"))
	float DetectionSpeed = 0.5f;
	UPROPERTY(VisibleAnywhere, BlueprintReadwrite, Category = "Spacelinger|AI", meta = (UIMin = "0.0", UIMax = "2.0"))
	float UndetectionSpeed = 0.5f;

	// Awareness goes from 0 to 1
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|AI|Internal")
	float CurrentAwareness = 0.0f;
	// If the AI is alerted, but not necessarily detecting the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|AI|Internal")
	bool bIsAlerted = false;
	// If the patrol mechanic must be disabled
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	bool bCanPatrol = true;

	// AI Patrol
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	TArray<AActor*> PatrolPoints; // To be used in the future

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	float PatrolAcceptanceRadius = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	bool bFoundPatrolPoint = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	FVector CurrentPatrolPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	TWeakObjectPtr<AActor> DetectedActor; // if null, we're not detecting anyone

private:
	UFUNCTION()
	void OnTargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo);

	// Tick functions we're going to enable and disable as we see fit
	void OnActorDetected();
	void OnActorUndetected();
	
	FTimerHandle DetectionTimerHandle; // Timers are going to be set to loop
	float LastTimeSecondsTimer = 0.0f;

	// Patrol
	int32 CurrentPatrolPointIndex = 0;
	FTimerHandle PatrolTimerHandle;
	void Patrol();
	void ResumePatrol();

	const float TimerTickRate = 0.001f; // We want to make our timers tick every frame
};
