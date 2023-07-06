#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDec_Soldier_AtShootingDistance.generated.h"

UCLASS()
class SPACELINGER_API UBTDec_Soldier_AtShootingDistance : public UBTDecorator
{
	GENERATED_BODY()

	/** blackboard key selector */
	UPROPERTY(EditAnywhere, Category=Blackboard)
	FBlackboardKeySelector PlayerActorKey;

	UBTDec_Soldier_AtShootingDistance();
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};