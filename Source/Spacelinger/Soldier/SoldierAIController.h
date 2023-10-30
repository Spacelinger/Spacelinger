// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Soldier\SLSoldier.h"
#include "SoldierAIController.generated.h"

#define SL_ECC_SoldierAI ECC_GameTraceChannel1

class ASlime_A;

UCLASS()
class SPACELINGER_API ASoldierAIController : public AAIController
{
	GENERATED_BODY()

	ASoldierAIController();

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	bool IsPlayerInSight();
	bool IsPlayerInSightRaycast(FVector SoldierPosition, FVector PlayerPosition);
	
public:
	// If we see the player and they are at this distance or closer, they will be detected instanly.
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger|AI", meta = (UIMin = "0.0"))
	float DistanceInstantDetection = 250.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger|AI", meta = (UIMin = "0.0", UIMax = "2.0"))
	float DetectionSpeed = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger|AI", meta = (UIMin = "0.0", UIMax = "2.0"))
	float UndetectionSpeed = 0.2f;

	// Awareness goes from 0 to 1
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|AI|Internal")
	float CurrentAwareness = 0.0f;
	// If the AI is alerted, but not necessarily detecting the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|AI|Internal")
	bool bIsAlerted = false;
	// If the AI is seeing the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|AI|Internal")
	bool bPlayerInSight = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI|Internal")
	bool bSearchLastLocation = false; // This will be modified by tasks in the Behavior Tree too
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI|Internal")
	TWeakObjectPtr<AActor> DetectedActor; // NOTE: Not needed, but I don't want to break things rn by deleting this
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|AI|Internal")
	FVector DetectedLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|AI|Internal")
	float MaxSightRadius = 0.0f; // Radius of the biggest cone of vision, retrieved on BeginPlay
	
	// Seconds it stays aiming before shooting
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	float AimTime = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI|Internal")
	float AimTimeRemaining = 2.0f; // Keeps track of time since it was alerted

	// If the AI is stunned by the player (currently not in use, but might be considered useful if the stun implementation is changed -- else delete)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|AI|Internal")
	bool bIsStunned = false;

	// AI Guarding
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI|Internal")
	FTransform InitialTransform;

	// AI Alerted
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger|AI", meta = (UIMin = "0.0", UIMax = "2.0"))
	float WalkingSpeed = 150.0f;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger|AI", meta = (UIMin = "0.0", UIMax = "2.0"))
	float RunningSpeed = 500.0f; 

	UFUNCTION(BlueprintCallable)
	bool CanPatrol() const;

	// Stun
	UFUNCTION(BlueprintCallable)
	void StopLogic();
	UFUNCTION(BlueprintCallable)
	void ResumeLogic();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsStunned();
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsAlerted();

	void SetIsAlerted(bool NewState);
	void AlertAssignedSoldiers();

private:
	// Helper functions
	UPROPERTY()
	ASlime_A *PlayerCharacterRef = nullptr;
	ASlime_A* GetPlayerCharacter();

	UPROPERTY()
	ASLSoldier *SoldierCharacterRef = nullptr;
	ASLSoldier* GetInstigatorSoldier();
};
