#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Soldier_GetRandomPoint.generated.h"

class UBehaviorTree;

UCLASS()
class SPACELINGER_API UBTTask_Soldier_GetRandomPoint : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PatrolRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector LocationKey;

	UBTTask_Soldier_GetRandomPoint();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask  (UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};