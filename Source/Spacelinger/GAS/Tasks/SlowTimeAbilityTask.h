// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "SlowTimeAbilityTask.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSlowTimeAbilityTask, FGameplayEventData, Payload);

UCLASS()
class SPACELINGER_API USlowTimeAbilityTask : public UAbilityTask
{
	GENERATED_BODY()
	

	UPROPERTY(BlueprintAssignable)
	FSlowTimeAbilityTask EventReceived;

	/**
	 * Wait until the specified gameplay tag event is triggered. By default this will look at the owner of this ability. OptionalExternalTarget can be set to make this look at another actor's tags for changes
	 * It will keep listening as long as OnlyTriggerOnce = false
	 * If OnlyMatchExact = false it will trigger for nested tags
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static USlowTimeAbilityTask* WaitGameplayEvent(UGameplayAbility* OwningAbility, FGameplayTag EventTag, AActor* OptionalExternalTarget = nullptr, bool OnlyTriggerOnce = false, bool OnlyMatchExact = true);

	void SetExternalTarget(AActor* Actor);

	UAbilitySystemComponent* GetTargetASC();

	virtual void Activate() override;

	virtual void GameplayEventCallback(const FGameplayEventData* Payload);
	virtual void GameplayEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload);

	void OnDestroy(bool AbilityEnding) override;

	FGameplayTag Tag;

	UPROPERTY()
		TObjectPtr<UAbilitySystemComponent> OptionalExternalTarget;

	bool UseExternalTarget;
	bool OnlyTriggerOnce;
	bool OnlyMatchExact;

	FDelegateHandle MyHandle;
};
