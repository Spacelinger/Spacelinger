#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SpacelingerCharacter.h"
#include "SLWeapon_DT.generated.h"

class ASLProjectile;

USTRUCT(BlueprintType)
struct FSLWeapon_DT : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TEnumAsByte<SLHumanoidAbility> HumanoidAbility = SLHumanoidAbility::StickyPuddle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D AimingOffset = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Speed = 700.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftClassPtr<ASLProjectile> ProjectileToSpawn;
};
