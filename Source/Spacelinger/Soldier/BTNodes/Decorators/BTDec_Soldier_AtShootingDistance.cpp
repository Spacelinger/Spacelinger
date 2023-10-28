// NOTE: No longer used, but leaving it here so we can use it as a template
#include "BTDec_Soldier_AtShootingDistance.h"
#include "Soldier/SoldierAIController.h"
#include "Soldier/SLSoldier.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDec_Soldier_AtShootingDistance::UBTDec_Soldier_AtShootingDistance() {
	NodeName = "Is At Shooting Distance?";

	// Accept only actors and vectors
	PlayerActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDec_Soldier_AtShootingDistance, PlayerActorKey), AActor::StaticClass());
	PlayerActorKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTDec_Soldier_AtShootingDistance, PlayerActorKey));
}

void UBTDec_Soldier_AtShootingDistance::InitializeFromAsset(UBehaviorTree& Asset) {
	Super::InitializeFromAsset(Asset);

	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset)) {
		PlayerActorKey.ResolveSelectedKey(*BBAsset);
	}
}

bool UBTDec_Soldier_AtShootingDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const {
	if(!PlayerActorKey.IsSet()) return false;
	UBlackboardComponent *BB = OwnerComp.GetBlackboardComponent();
	AActor *BBPlayer = Cast<AActor>(BB->GetValueAsObject(PlayerActorKey.SelectedKeyName));
	if (!BBPlayer) return false;

	ASoldierAIController *AIController = Cast<ASoldierAIController>(OwnerComp.GetAIOwner());
	if (!AIController) return false;
	ASLSoldier *AIActor = Cast<ASLSoldier>(AIController->GetInstigator());
	if (!AIActor) return false;

	double Distance = FVector::Dist(BBPlayer->GetActorLocation(), AIActor->GetActorLocation());
	return Distance <= AIActor->ShootingAcceptanceRadius;
}
