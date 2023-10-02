
#include "BTTask_Soldier_AimAtPlayer.h"
#include "AIController.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Spider/Slime_A.h"

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
	if (!ensure(MyController)) return;

	AActor *MyActor = Cast<AActor>(MyController->GetInstigator());
	if (!ensure(MyActor)) return;

	UBlackboardComponent *MyBlackboard = MyController->GetBlackboardComponent();
	if (!ensure(MyBlackboard)) return;
	ASlime_A *PlayerActor = Cast<ASlime_A>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	if (!ensure(PlayerActor)) return;

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
