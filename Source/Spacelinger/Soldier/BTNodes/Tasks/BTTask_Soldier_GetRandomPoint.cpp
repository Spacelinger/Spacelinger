
#include "BTTask_Soldier_GetRandomPoint.h"
#include "AIController.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Soldier/SoldierAIController.h"
#include "NavigationSystem.h"

UBTTask_Soldier_GetRandomPoint::UBTTask_Soldier_GetRandomPoint()
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Soldier_GetRandomPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	return EBTNodeResult::InProgress;
}

void UBTTask_Soldier_GetRandomPoint::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	ASoldierAIController* MyController = Cast<ASoldierAIController>(OwnerComp.GetAIOwner());
	if(ensure(MyController)) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	AActor *MyActor = Cast<AActor>(MyController->GetInstigator());
	if(ensure(MyActor)) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

	UBlackboardComponent *MyBlackboard = MyController->GetBlackboardComponent();
	if(ensure(MyBlackboard)) FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	
	FVector Origin = MyActor->GetActorLocation();
	FVector ResultLocation;
	bool Success = UNavigationSystemV1::K2_GetRandomReachablePointInRadius(GetWorld(), Origin, ResultLocation, PatrolRadius);
	MyBlackboard->SetValueAsVector(LocationKey.SelectedKeyName, Success ? ResultLocation : Origin);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

EBTNodeResult::Type UBTTask_Soldier_GetRandomPoint::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	return EBTNodeResult::Aborted;
}
