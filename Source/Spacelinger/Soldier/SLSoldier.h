#pragma once

#include "CoreMinimal.h"
#include "Audio/SpacelingerAudioComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractInterface.h"
#include "SLSoldier.generated.h"

class UWidgetComponent;
class USLDetectionWidget;
class ASLSoldierPath;

UENUM()
enum SoldierAIState {
	IDLE,
	SUSPICIOUS,
	ALERTED,
	AIMING,
	ATTACK,
	STUNNED
};

UENUM(BlueprintType)
enum class SLIdleType : uint8 {
	Standing = 0 UMETA(ToolTip="Stays in place, doesn't move"),
	PatrolRandom UMETA(ToolTip="Moves randomly around the room at a specific radius"),
	PatrolGuided UMETA(ToolTip="Moves from waypoint to waypoint"),
};

UCLASS(config=Game)
class ASLSoldier : public ACharacter, public IInteractInterface
{
	GENERATED_BODY()

public:
	ASLSoldier();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|UI", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* DetectionWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger|UI", meta = (AllowPrivateAccess = "true"))
	UClass *OffscreenDetectionWidgetClass;
	UPROPERTY()
	USLDetectionWidget *OffscreenDetectionWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger|AI|Patrol")
	ASLSoldierPath *PathActor;

	FTimerHandle UnstunTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|Audio", meta = (AllowPrivateAccess = "true"))
	USpacelingerAudioComponent* AudioManager = nullptr;

// Interact stuff
	int InteractPriority = 99;
	//int GetInteractPriority() const;
	//void Interact(AActor* ActorInteracting);	//LUIS: Interact has been refactored
	//void SetAsCandidate(bool IsCandidate);
	void MoveToCeiling();
	void ReceiveDamage(AActor *DamageDealer);
	void Stun(float StunDuration, FVector ThrowLocation);
	void Unstun();
	bool IsStunned();
	float GetRemainingTimeToUnstunAsPercentage();
	void Die(AActor *Killer);
	UFUNCTION(BlueprintImplementableEvent)
	void SoldierHasDied(AActor *Killer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
	bool bMoveToCeiling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	SLIdleType PatrolType = SLIdleType::Standing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	bool bIsStunned = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	TEnumAsByte<SoldierAIState> AnimationState = SoldierAIState::IDLE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger")
	UGameInstance *GameInstance;

	FVector GetNextPatrolPoint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger")
	bool bIsDead = false;

	UFUNCTION(BlueprintImplementableEvent)
	void SoldierDied();

	UFUNCTION()
	USpacelingerAudioComponent * GetAudioManager();

	UFUNCTION()
	bool IsDead();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger")
	bool bHasKey = false;

private:
	// Patrol
	int32 CurrentPatrolPointIndex = 0;
	TArray<FVector> WorldPatrolPoints; // Patrol points in World Space. Generated at BeginPlay()
};
