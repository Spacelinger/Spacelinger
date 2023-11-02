// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_SlowTime.generated.h"

/**
* Class mostly based off AbilityTask_WaitGameplayEvent
*/

class UAbilitySystemComponent;
class UPostProcessComponent;

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

	virtual void FailedEventCallback(const FGameplayEventData* Payload);
	virtual void FailedEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload);

	virtual void TickTask(float DeltaTime) override;

	void OnDestroy(bool AbilityEnding) override;

	FGameplayTag SuccessTag;
	FGameplayTag FailedTag;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> OptionalExternalTarget;


	bool bUseExternalTarget;
	bool bOnlyTriggerOnce;
	bool bOnlyMatchExact;

	FDelegateHandle SuccessHandle;
	FDelegateHandle FailedHandle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float StaminaCostOverTime = -20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CustomTimeDilation = 0.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SlowTimeFadeInRate = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float SlowTimeEffectLineDensity = 0.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float CameraBoomArmLengthExtension = 200.0f;

	void SetSpeedLinesMaterial(UMaterialInstance* SpeedLinesMaterial);
	
private:

	void SmoothSlowTime(float DeltaTime);
	bool bSlowingTime = false;

	float CurrentSlowTimeDilation = 1.0f;
	float SlowStep = 0.0f;
	float CurrentLineStep = 0.0f;
	float LineStep = 0.0f;

	UMaterialInstance* PostProcessSpeedLinesMaterial = nullptr;
	UMaterialInstanceDynamic* DynamicSpeedLinesMaterial = nullptr;
	UPostProcessComponent* PPComp = nullptr;
	FWeightedBlendable WeightedBlendable;	// May be unnecessary
	float CameraBoomDefaultArmLength = 200.0f;
};
