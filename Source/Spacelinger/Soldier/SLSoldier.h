#pragma once

#include "CoreMinimal.h"
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

USTRUCT()
struct FUSLAICone {
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta = (UIMin = "0.0"))
	float MinSightRadius = 0;
	UPROPERTY(EditAnywhere, meta = (UIMin = "0.0"))
	float MaxSightRadius = 600;
	// Height of sight (both up and down)
	UPROPERTY(EditAnywhere, meta = (UIMin = "0.0"))
	float SightHeight = 200;
	// Peripherial Vision in degrees
	UPROPERTY(EditAnywhere, meta = (UIMin = "0.0", UIMax = "360.0"))
	float PeripherialVision = 50;
	UPROPERTY(EditAnywhere)
	FColor DebugColor = FColor::Red;
};

UCLASS(config=Game)
class ASLSoldier : public ACharacter, public IInteractInterface
{
	GENERATED_BODY()

public:
	ASLSoldier();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|UI", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* DetectionWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger|UI", meta = (AllowPrivateAccess = "true"))
	UClass *OffscreenDetectionWidgetClass;
	UPROPERTY()
	USLDetectionWidget *OffscreenDetectionWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger|AI|Patrol")
	ASLSoldierPath *PathActor;

	FTimerHandle UnstunTimerHandle;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI|Patrol")
	SLIdleType PatrolType = SLIdleType::Standing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	bool bIsStunned = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	TEnumAsByte<SoldierAIState> AnimationState = SoldierAIState::IDLE;

	FVector GetNextPatrolPoint();

	UFUNCTION(BlueprintImplementableEvent)
	void SoldierDied();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger")
	bool bHasKey = false;

	bool bIsDead = false;

// AI stuff
	UPROPERTY(EditAnywhere, Category = "Spacelinger|AI|Perception")
	TArray<FUSLAICone> ConesOfVision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI|Perception")
	bool bDrawDebugAI = false;

	void DrawDebugCones();

	FTimerHandle DrawDebugTimerHandle;

private:
	// Patrol
	int32 CurrentPatrolPointIndex = 0;
	TArray<FVector> WorldPatrolPoints; // Patrol points in World Space. Generated at BeginPlay()
};
