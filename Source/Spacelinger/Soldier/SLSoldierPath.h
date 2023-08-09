#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLSoldierPath.generated.h"

class ASLSoldier;

UCLASS(config=Game)
class ASLSoldierPath : public AActor
{
	GENERATED_BODY()

public:
	ASLSoldierPath();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger|Internal")
	ASLSoldier *OwnerSoldier;

	// Last patrol point will always be the initial location, so it loops
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger", Meta = (MakeEditWidget = true))
	TArray<FVector> PatrolPoints;
};
