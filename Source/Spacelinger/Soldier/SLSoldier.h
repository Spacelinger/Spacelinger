#pragma once

#include "CoreMinimal.h"
#include "Audio/SpacelingerAudioComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractInterface.h"
#include "SLSoldier.generated.h"

class UWidgetComponent;
class USLDetectionWidget;
class ASLSoldierPath;
class UInteractableComponent;
class UInteractWidget;

UENUM()
enum SoldierAIState {
	IDLE,
	SUSPICIOUS, // Unused, maybe some others are unusued too idk
	ALERTED,
	AIMING,
	STUNNED,
	ATTACK, // Shoot
	STOMP
};

UENUM(BlueprintType)
enum class SLIdleType : uint8 {
	Standing = 0 UMETA(ToolTip="Stays in place, doesn't move"),
	PatrolRandom UMETA(ToolTip="Moves randomly around the room at a specific radius"),
	PatrolGuided UMETA(ToolTip="Moves from waypoint to waypoint"),
};


UENUM(BlueprintType)
enum class SLTypeKey : uint8 {
	NONE = 0,
	HAND,
	EYES,
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
	void UpdateWidgetSize();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger|UI", meta = (AllowPrivateAccess = "true"))
	UClass *OffscreenDetectionWidgetClass;
	UPROPERTY()
	USLDetectionWidget *OffscreenDetectionWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger|AI|Patrol")
	ASLSoldierPath *PathActor;

	FTimerHandle UnstunTimerHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|Audio", meta = (AllowPrivateAccess = "true"))
	USpacelingerAudioComponent* AudioManager = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|Interact")
	UInteractableComponent* InteractableComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|Interact")
	UWidgetComponent* InteractWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|Interact")
	UInteractWidget* InteractPromptWidget = nullptr;

// Interact stuff
	int InteractPriority = 99;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spacelinger|Interact")
	float SecondsBetweenAttacks = 3.0f;

	void MoveToCeiling();
	UFUNCTION()
	void ReceiveDamage(AActor *DamageDealer);
	void Stun(float StunDuration, FVector ThrowLocation);
	void Unstun();
	bool IsStunned() const { return bIsStunned; }
	UFUNCTION(BlueprintImplementableEvent)
	void SoldierBeginStun(float StunDuration);
	float GetRemainingTimeToUnstunAsPercentage();
	void Die(AActor *Killer);
	UFUNCTION(BlueprintImplementableEvent)
	void SoldierHasDied(AActor *Killer);
	UFUNCTION()
	void OnPlayerDead(AActor *Killer);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
	bool bMoveToCeiling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI|Patrol")
	SLIdleType PatrolType = SLIdleType::Standing;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	bool bIsStunned = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	int ShootDamage = 40;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	int StompDamage = 40;
	
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
	SLTypeKey OwnedKey = SLTypeKey::NONE;

// AI stuff
	UPROPERTY(EditAnywhere, Category = "Spacelinger|AI|Perception")
	TArray<FUSLAICone> ConesOfVision;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI|Perception")
	bool bDrawDebugAI = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	float ShootingAcceptanceRadius = 1600.0f;

	void DrawDebugCones();

	FTimerHandle DrawDebugTimerHandle;

private:
	// Patrol
	int32 CurrentPatrolPointIndex = 0;
	TArray<FVector> WorldPatrolPoints; // Patrol points in World Space. Generated at BeginPlay()

public:
	// When the soldier is aiming at the player it might rotate in place to not lose the line of sight
	// All of this is related to that. Variable names are self-explanatory
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI|Internal")
	bool bHasRotatedLastFrameWhileAiming = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI|Internal")
	FRotator RotatorToFaceWhileAiming = FRotator::ZeroRotator;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI|Internal")
	float RotationSpeedWhileAiming = 1.0f;
};
