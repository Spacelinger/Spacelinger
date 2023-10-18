
#include "BTTask_Soldier_AimAtPlayer.h"
#include "AIController.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Spider/Slime_A.h"
#include "Soldier/SLSoldier.h"
#include "Soldier/SoldierAIController.h"


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

	return EBTNodeResult::InProgress;
}

void UBTTask_Soldier_AimAtPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) {
	ASoldierAIController* MyController = Cast<ASoldierAIController>(OwnerComp.GetAIOwner());
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
	}

	SoldierActor->AnimationState = SoldierAIState::AIMING;

	MyController->AimTimeRemaining -= DeltaSeconds;
	if (MyController->AimTimeRemaining <= 0) {
		SoldierActor->AnimationState = SoldierAIState::ATTACK;
		// If it's very close we might stomp instead
		if (FVector::Dist2D(MyLocation, PlayerLocation) < 70.0f) {
			FVector FootLocation = SoldierActor->GetMesh()->GetSocketLocation(FName("Bip001-R-Toe0"));
			FVector PlayerFootVector = FootLocation - PlayerLocation;
			// If it's at the same height than the foot of the soldier, we stomp
			if (FMath::Abs(PlayerFootVector.Z) < 20.0f) {
				SoldierActor->AnimationState = SoldierAIState::STOMP;
			}
		}
		MyController->AimTimeRemaining = MyController->AimTime;

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
