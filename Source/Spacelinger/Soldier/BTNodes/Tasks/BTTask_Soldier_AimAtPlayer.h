#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Abilities/GameplayAbility.h"
#include "BTTask_Soldier_AimAtPlayer.generated.h"

class UBehaviorTree;

UCLASS()
class SPACELINGER_API UBTTask_Soldier_AimAtPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float AimTime = 1.5f;

	UPROPERTY(EditAnywhere)
	FName PlayerSeenKey = FName("PlayerSeen");

	UBTTask_Soldier_AimAtPlayer();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask  (UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	float RemainingTime;
};