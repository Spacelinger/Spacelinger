// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Abilities/MCV_GameplayAbility.h"
#include "Slime_A.h"
#include "GA_SlowTime.generated.h"

/**
 * 
 */
UCLASS()
class SPACELINGER_API UGA_SlowTime : public UMCV_GameplayAbility
{
	GENERATED_BODY()

public:

	UGA_SlowTime();
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffectCost;

	UPROPERTY(EditDefaultsOnly)
	UMaterialInstance* PostProcessSpeedLinesMaterial;


protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

private:

	UFUNCTION()
	void OnAbilityTaskCompleted(FGameplayEventData Payload);

	UAbilitySystemComponent* ASC = nullptr;
};
