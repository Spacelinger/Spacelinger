// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_SlowTime.generated.h"

/**
* Class mostly based off AbilityTask_WaitGameplayEvent
*/

class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitGameplayEventDelegate, FGameplayEventData, Payload);

UCLASS()
class SPACELINGER_API UAbilityTask_SlowTime : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FWaitGameplayEventDelegate	SuccessEventReceived;

	UPROPERTY(BlueprintAssignable)
	FWaitGameplayEventDelegate	FailedEventReceived;

	/**
	 * Wait until the specified gameplay tag event is triggered. By default this will look at the owner of this ability. OptionalExternalTarget can be set to make this look at another actor's tags for changes
	 * It will keep listening as long as OnlyTriggerOnce = false
	 * If OnlyMatchExact = false it will trigger for nested tags
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAbilityTask_SlowTime* SlowTimeGameplayEvent(UGameplayAbility* OwningAbility, FGameplayTag SuccessTag, FGameplayTag FailedTag,
			AActor* OptionalExternalTarget = nullptr, bool OnlyTriggerOnce = false, bool OnlyMatchExact = true);

	void SetExternalTarget(AActor* Actor);

	UAbilitySystemComponent* GetTargetASC();

	virtual void Activate() override;

	virtual void SuccessEventCallback(const FGameplayEventData* Payload);
	virtual void SuccessEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload);

	virtual void CanceledEventCallback(const FGameplayEventData* Payload);
	virtual void CanceledEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload);

	virtual void FailedEventCallback(const FGameplayEventData* Payload);
	virtual void FailedEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload);

	virtual void TickTask(float DeltaTime) override;

	void OnDestroy(bool AbilityEnding) override;

	FGameplayTag SuccessTag;
	FGameplayTag FailedTag;

	UPROPERTY()
		TObjectPtr<UAbilitySystemComponent> OptionalExternalTarget;

	bool UseExternalTarget;
	bool OnlyTriggerOnce;
	bool OnlyMatchExact;

	FDelegateHandle SuccessHandle;
	FDelegateHandle FailedHandle;
	
};
