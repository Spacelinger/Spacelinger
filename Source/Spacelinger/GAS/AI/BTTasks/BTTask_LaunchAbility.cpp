
#include "BTTask_LaunchAbility.h"
#include "AIController.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

UBTTask_LaunchAbility::UBTTask_LaunchAbility()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_LaunchAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{

	if (!AbilityToLaunch) 
	{
		return EBTNodeResult::Failed;
	}
	
	AAIController* MyController = OwnerComp.GetAIOwner();
	APawn* MyPawn = MyController ? MyController->GetPawn() : nullptr;

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MyPawn);

	if (ASC)
	{
		const bool bLaunchedAbility = ASC->TryActivateAbilityByClass(AbilityToLaunch);
		if (bLaunchedAbility) 
		{
			FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromClass(AbilityToLaunch);
			UGameplayAbility* AbilityInstance = AbilitySpec ? AbilitySpec->GetPrimaryInstance() : nullptr;
			if (AbilityInstance && AbilityInstance->IsActive())
			{

				AbilityInstance->OnGameplayAbilityEnded.AddUObject(this, &UBTTask_LaunchAbility::OnAbilityEnded);
				MyOwnerComp = &OwnerComp;

				return EBTNodeResult::InProgress;
			}

			return EBTNodeResult::Succeeded;
		}
		else 
		{
			EBTNodeResult::Failed;
		}
	}


	return EBTNodeResult::Failed;
}

void UBTTask_LaunchAbility::OnAbilityEnded(UGameplayAbility* Ability)
{
	if (MyOwnerComp.IsValid()) 
	{
		FinishLatentTask(*MyOwnerComp.Get(), EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTTask_LaunchAbility::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	return EBTNodeResult::Aborted;
}
