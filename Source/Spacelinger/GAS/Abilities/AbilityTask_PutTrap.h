// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_PutTrap.generated.h"

/*
 * Wait for a time delay or tag removal. A mashup between Wait Delay and Wait Gameplay Tag Removed ability tasks
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitPutTrapEventDelegate);

UCLASS()
class SPACELINGER_API UAbilityTask_PutTrap : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
		FWaitPutTrapEventDelegate	ChannelingComplete;

	UPROPERTY(BlueprintAssignable)
		FWaitPutTrapEventDelegate	ChannelingCanceled;

	//UPROPERTY()
	//TObjectPtr<UAbilitySystemComponent> OptionalExternalTarget;

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;

	UAbilitySystemComponent* GetTargetASC();

	/** Return debug string describing task */
	virtual FString GetDebugString() const override;	// Might want to delete

	virtual void GameplayTagCallback(const FGameplayTag Tag, int32 NewCount);

	/** Wait specified time OR until the Channeling Tag is removed from the Owning Ability owner. */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_PutTrap* PutTrapChannelingTask(UGameplayAbility* OwningAbility, FGameplayTag ChannelingTag, float Time, bool OnlyTriggerOnce = false);

protected:
	
	void OnTimeFinish();
	float GetNormalizedTimeElapsed() const;

	bool bOnlyTriggerOnce = false;

	float Time = 3.0f;	// Placeholder. Time is set by the constructor
	float TimeStarted = 0.0f;

	FTimerHandle TimerHandle;

	FGameplayTag ChannelingTag;
};
