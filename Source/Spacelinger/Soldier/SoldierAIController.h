// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Soldier\SLSoldier.h"
#include "SoldierAIController.generated.h"

class UAIPerceptionComponent;
struct FActorPerceptionUpdateInfo;
class UAISenseConfig_Sight;
class ASlime_A;

UCLASS()
class SPACELINGER_API ASoldierAIController : public AAIController
{
	GENERATED_BODY()

	ASoldierAIController();

	UAISenseConfig_Sight *AISenseConfigSight;

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	bool IsPlayerInSight();
	
public:
	// If we see the player and they are at this distance or closer, they will be detected instanly.
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger|AI", meta = (UIMin = "0.0"))
	float DistanceInstantDetection = 200.0f;
	
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
	TWeakObjectPtr<AActor> DetectedActor; // NOTE: Probably not needed anymore
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|AI|Internal")
	FVector DetectedLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI|Internal")
	bool bLastLocationKnown = false; // This will be modified by tasks in the Behavior Tree too

	// If the AI is stunned by the player (currently not in use, but might be considered useful if the stun implementation is changed -- else delete)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|AI|Internal")
	bool bIsStunned = false;

	// AI Guarding
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI|Internal")
	FTransform InitialTransform;

	// AI Alerted
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	float ShootingAcceptanceRadius = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadwrite, Category = "Spacelinger|AI", meta = (UIMin = "0.0", UIMax = "2.0"))
	float WalkingSpeed = 180.0f;
	float RunningSpeed = 400.0f; // Obtained in BeginPlay()

	UFUNCTION(BlueprintCallable)
	bool CanPatrol() const;

	// Stun
	UFUNCTION(BlueprintCallable)
	void StopLogic();
	UFUNCTION(BlueprintCallable)
	void ResumeLogic();
	UFUNCTION(BlueprintCallable)
	bool IsStunned();

	void SetIsAlerted(bool NewState);

private:
	// Helper functions
	ASLSoldier* GetInstigatorSoldier() const { return Cast<ASLSoldier>(GetInstigator()); }

	UPROPERTY()
	ASlime_A *PlayerCharacterRef = nullptr;
	ASlime_A* GetPlayerCharacter();
};
