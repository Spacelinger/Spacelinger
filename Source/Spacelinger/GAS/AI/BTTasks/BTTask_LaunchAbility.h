#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "BehaviorTree/BTTaskNode.h"
#include "Abilities/GameplayAbility.h"
#include "BTTask_LaunchAbility.generated.h"

class UBehaviorTree;

UCLASS()
class SPACELINGER_API UBTTask_LaunchAbility : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGameplayAbility> AbilityToLaunch;

	UBTTask_LaunchAbility();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	
	void OnAbilityEnded(UGameplayAbility* Ability);

	TWeakObjectPtr<UBehaviorTreeComponent> MyOwnerComp;
};