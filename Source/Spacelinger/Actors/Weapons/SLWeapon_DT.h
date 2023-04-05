#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SLWeapon_DT.generated.h"

class ASLProjectile;

USTRUCT(BlueprintType)
struct FSLWeapon_DT : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int Slot = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D AimingOffset = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Speed = 700.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Damage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftClassPtr<ASLProjectile> ProjectileToSpawn;

	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLinearColor Color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* TextureHard = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UStaticMesh> StaticMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftClassPtr<AActor> ClassToSpawn;
		*/
};
