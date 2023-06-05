// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "AttributeSets/MCV_AttributeSet.h"
#include "StaminaAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class SPACELINGER_API UStaminaAttributeSet : public UMCV_AttributeSet
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData Stamina = 100.0f;
	ATTRIBUTE_ACCESSORS(UStaminaAttributeSet, Stamina)

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayAttributeData MaxStamina = 100.0f;
	ATTRIBUTE_ACCESSORS(UStaminaAttributeSet, MaxStamina)

public:

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

};
