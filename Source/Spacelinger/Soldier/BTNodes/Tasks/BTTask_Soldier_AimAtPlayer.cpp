
#include "BTTask_Soldier_AimAtPlayer.h"
#include "AIController.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Soldier_AimAtPlayer::UBTTask_Soldier_AimAtPlayer()
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Soldier_AimAtPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	RemainingTime = AimTime;
	return EBTNodeResult::InProgress;
}

void UBTTask_Soldier_AimAtPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	AAIController* MyController = OwnerComp.GetAIOwner();
	ensure(MyController);

	AActor *MyActor = Cast<AActor>(MyController->GetInstigator());
	ensure(MyActor);

	UBlackboardComponent *MyBlackboard = MyController->GetBlackboardComponent();
	ensure(MyBlackboard);
	AActor *PlayerActor = Cast<AActor>(MyBlackboard->GetValueAsObject(PlayerSeenKey));
	ensure(PlayerActor);

	FVector MyLocation     = MyActor->GetActorLocation();
	FVector PlayerLocation = PlayerActor->GetActorLocation();
	FRotator LookAtRotator = FRotationMatrix::MakeFromX(PlayerLocation - MyLocation).Rotator(); // Code from FindLookAtRotation()

	FRotator NewRotation = MyActor->GetActorRotation();
	NewRotation.Yaw = LookAtRotator.Yaw;

	MyActor->SetActorRotation(NewRotation);

	RemainingTime -= DeltaSeconds;
	if (RemainingTime <= 0) {
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTTask_Soldier_AimAtPlayer::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	return EBTNodeResult::Aborted;
}
