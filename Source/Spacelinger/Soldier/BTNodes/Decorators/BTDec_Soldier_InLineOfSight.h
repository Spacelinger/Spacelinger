#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDec_Soldier_InLineOfSight.generated.h"

UCLASS()
class SPACELINGER_API UBTDec_Soldier_InLineOfSight : public UBTDecorator
{
	GENERATED_BODY()

	UBTDec_Soldier_InLineOfSight();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};