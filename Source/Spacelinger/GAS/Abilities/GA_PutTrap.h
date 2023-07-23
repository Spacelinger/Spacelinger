// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Abilities/MCV_GameplayAbility.h"
#include "GA_PutTrap.generated.h"

/**
 * 
 */
UCLASS()
class SPACELINGER_API UGA_PutTrap : public UMCV_GameplayAbility
{
	GENERATED_BODY()
	
public:
	UGA_PutTrap();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (UIMin = "0.2", UIMax = "99"))
		float TimeToChannel = 3.0f; // TO-DO: Cannot be < 0

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	UFUNCTION()
		void AbilityChannelComplete();
	UFUNCTION()
		void AbilityChannelCanceled();

	void ActionPutTrap();

	const float TraceDistance = 50.0f;
};
