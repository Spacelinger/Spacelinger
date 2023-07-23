
#include "BTDec_Soldier_InLineOfSight.h"
#include "Soldier/SoldierAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDec_Soldier_InLineOfSight::UBTDec_Soldier_InLineOfSight() {
	NodeName = "Is In Line of Sight?";
}

bool UBTDec_Soldier_InLineOfSight::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const {
	ASoldierAIController *AIController = Cast<ASoldierAIController>(OwnerComp.GetAIOwner());
	return (AIController && AIController->DetectedActor.IsValid());
}
