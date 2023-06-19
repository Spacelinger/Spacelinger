#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractInterface.h"
#include "SLSoldier.generated.h"

UENUM()
enum SoldierAIState {
	IDLE,
	SUSPICIOUS,
	ALERTED,
	AIMING,
	ATTACK
};

UCLASS(config=Game)
class ASLSoldier : public ACharacter, public IInteractInterface
{
	GENERATED_BODY()

public:
	ASLSoldier();
	virtual void Tick(float DeltaTime) override;

// Interact stuff
public:
	int InteractPriority = 99;
	int GetInteractPriority() const;
	void Interact(AActor* ActorInteracting);
	void SetAsCandidate(bool IsCandidate);
	void MoveToCeiling();
	void StopAdaptToCeiling();
	void AdaptToCeiling();
	void ReceiveDamage();
	void Die();

	UFUNCTION()
	void OnEndPointCollision(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
	bool bMoveToCeiling = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	bool bCanPatrol = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger|AI")
	TEnumAsByte<SoldierAIState> AnimationState = SoldierAIState::IDLE;
};
