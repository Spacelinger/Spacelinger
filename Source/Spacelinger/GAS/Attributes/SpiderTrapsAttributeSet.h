// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "AttributeSets/MCV_AttributeSet.h"
#include "SpiderTrapsAttributeSet.generated.h"

/**
 * 
 */

UCLASS()
class SPACELINGER_API USpiderTrapsAttributeSet : public UMCV_AttributeSet
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData AvailableTraps = 100.0f;
	ATTRIBUTE_ACCESSORS(USpiderTrapsAttributeSet, AvailableTraps)

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxTraps = 100.0f;
	ATTRIBUTE_ACCESSORS(USpiderTrapsAttributeSet, MaxTraps)

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FActiveGameplayEffectHandle SpiderTrapsRecoveryEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SpiderTrapsRecoveryBaseRate = 10.0f;

public:

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	void SetSpiderTrapsRecoveryValue(float Value);

protected:

	void RestoreAvailableTrap();
};
