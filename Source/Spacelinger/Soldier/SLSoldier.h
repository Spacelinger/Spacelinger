#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interact_Luis/Interfaces/InteractInterface.h"
#include "SLSoldier.generated.h"

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

	UFUNCTION()
		void OnEndPointCollision(class UPrimitiveComponent* HitComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
		bool bMoveToCeiling = false;
};
