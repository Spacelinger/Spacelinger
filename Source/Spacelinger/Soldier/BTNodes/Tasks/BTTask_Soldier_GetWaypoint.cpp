
#include "BTTask_Soldier_GetWaypoint.h"
#include "AIController.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Soldier/SoldierAIController.h"
#include "Soldier/SLSoldier.h"
#include "NavigationSystem.h"

UBTTask_Soldier_GetWaypoint::UBTTask_Soldier_GetWaypoint()
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Soldier_GetWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	return EBTNodeResult::InProgress;
}

void UBTTask_Soldier_GetWaypoint::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	ASoldierAIController* MyController = Cast<ASoldierAIController>(OwnerComp.GetAIOwner());
	if(ensure(MyController)) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	ASLSoldier *MyActor = Cast<ASLSoldier>(MyController->GetInstigator());
	if(ensure(MyActor)) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	UBlackboardComponent *MyBlackboard = MyController->GetBlackboardComponent();
	if(ensure(MyBlackboard)) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	
	FVector NextWaypoint = MyActor->GetNextPatrolPoint();
	MyBlackboard->SetValueAsVector(LocationKey.SelectedKeyName, NextWaypoint);
	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

EBTNodeResult::Type UBTTask_Soldier_GetWaypoint::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	return EBTNodeResult::Aborted;
}
