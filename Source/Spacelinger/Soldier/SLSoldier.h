#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractInterface.h"
#include "SLSoldier.generated.h"

class UWidgetComponent;
class USLDetectionWidget;

UENUM()
enum SoldierAIState {
	IDLE,
	SUSPICIOUS,
	ALERTED,
	AIMING,
	ATTACK,
	STUNNED
};

UCLASS(config=Game)
class ASLSoldier : public ACharacter, public IInteractInterface
{
	GENERATED_BODY()

public:
	ASLSoldier();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|UI", meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* DetectionWidget;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger|UI", meta = (AllowPrivateAccess = "true"))
	UClass *OffscreenDetectionWidgetClass;
	UPROPERTY()
	USLDetectionWidget *OffscreenDetectionWidget;

// Interact stuff
public:
	int InteractPriority = 99;
	//int GetInteractPriority() const;
	//void Interact(AActor* ActorInteracting);	//LUIS: Interact has been refactored
	//void SetAsCandidate(bool IsCandidate);
	void MoveToCeiling();
	void StopAdaptToCeiling();
	void AdaptToCeiling();
	void ReceiveDamage();
	void Stun(float StunDuration);
	void Unstun();
	bool IsStunned();
	void Die();

	UFUNCTION()
	void OnEndPointCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
	bool bMoveToCeiling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	bool bCanPatrol = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	bool bIsStunned = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	TEnumAsByte<SoldierAIState> AnimationState = SoldierAIState::IDLE;
};
