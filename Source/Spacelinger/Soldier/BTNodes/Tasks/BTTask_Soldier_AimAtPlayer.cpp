
#include "BTTask_Soldier_AimAtPlayer.h"
#include "AIController.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Spider/Slime_A.h"
#include "Soldier/SLSoldier.h"

UBTTask_Soldier_AimAtPlayer::UBTTask_Soldier_AimAtPlayer()
{
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_Soldier_AimAtPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	if (AAIController* MyController = OwnerComp.GetAIOwner()) {
		if(ASLSoldier *SoldierActor = Cast<ASLSoldier>(MyController->GetInstigator())) {
			SoldierActor->RotatorToFaceWhileAiming = SoldierActor->GetActorRotation();
		}
	}

	RemainingTime = AimTime;
	return EBTNodeResult::InProgress;
}

void UBTTask_Soldier_AimAtPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (!ensure(MyController)) return;

	ASLSoldier *SoldierActor = Cast<ASLSoldier>(MyController->GetInstigator());
	if (!ensure(SoldierActor)) return;

	ASlime_A *PlayerActor = Cast<ASlime_A>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	if (!ensure(PlayerActor)) return;

	FVector MyLocation     = SoldierActor->GetActorLocation();
	FVector PlayerLocation = PlayerActor->GetActorLocation();

	FVector MyPlayerDir = PlayerLocation - MyLocation;
	FVector MyForward   = SoldierActor->GetActorForwardVector();
	MyPlayerDir.Z = 0; MyPlayerDir.Normalize();
	MyForward  .Z = 0; MyForward  .Normalize();
	float Angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(MyPlayerDir, MyForward)));

	if (Angle > 35.0f) {
		FRotator LookAtRotator = FRotationMatrix::MakeFromX(PlayerLocation - MyLocation).Rotator(); // Code from FindLookAtRotation()
		SoldierActor->RotatorToFaceWhileAiming = LookAtRotator;
		//FRotator NewRotation = SoldierActor->GetActorRotation();
		//NewRotation.Yaw = LookAtRotator.Yaw;
		//SoldierActor->SetActorRotation(NewRotation);
	}

	RemainingTime -= DeltaSeconds;
	SoldierActor->AnimationState = (RemainingTime <= 0) ? SoldierAIState::ATTACK : SoldierAIState::AIMING;
	if (RemainingTime <= 0) {
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTTask_Soldier_AimAtPlayer::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) 
{
	if (AAIController* MyController = OwnerComp.GetAIOwner()) {
		if (ASLSoldier *SoldierActor = Cast<ASLSoldier>(MyController->GetInstigator())) {
			SoldierActor->AnimationState = SoldierAIState::ALERTED;
		}
	}

	return EBTNodeResult::Aborted;
}
