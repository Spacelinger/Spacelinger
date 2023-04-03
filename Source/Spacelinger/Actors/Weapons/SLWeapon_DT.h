#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "SLWeapons_DT.generated.h"

USTRUCT(BlueprintType)
struct FSLWeapons_DT : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector2D AimingOffsetHorizontal = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Damage = 0.f;

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
