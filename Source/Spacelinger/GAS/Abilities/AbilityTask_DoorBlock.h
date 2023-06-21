// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_DoorBlock.generated.h"

/*
 * Wait for a time delay. A mashup between Wait Delay and Wait Gameplay Tag Removed ability tasks
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitDoorBlockEventDelegate);

class UChannelingProgressBar;

UCLASS()
class SPACELINGER_API UAbilityTask_DoorBlock : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(BlueprintAssignable)
	FWaitDoorBlockEventDelegate	ChannelingComplete;

	UPROPERTY(BlueprintAssignable)
	FWaitDoorBlockEventDelegate	ChannelingCanceled;

	//UPROPERTY()
	//TObjectPtr<UAbilitySystemComponent> OptionalExternalTarget;

	virtual void Activate() override;

	virtual void TickTask(float DeltaTime) override;

	UAbilitySystemComponent* GetTargetASC();

	/** Return debug string describing task */
	virtual FString GetDebugString() const override;	// Might want to delete

	virtual void GameplayTagCallback(const FGameplayTag Tag, int32 NewCount);

	//void SetExternalTarget(AActor* Actor);

	/** Wait specified time OR until the Channeling Tag is removed from the Owning Ability owner. */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_DoorBlock* DoorBlockChannelingTask(UGameplayAbility* OwningAbility, FGameplayTag ChannelingTag, float Time, UWidgetComponent* ChannelingProgressBar = nullptr, /*AActor* OptionalExternalTarget = nullptr,*/ bool OnlyTriggerOnce = false);

protected:
	
	void OnTimeFinish();
	float GetNormalizedTimeElapsed() const;
	void UpdateProgressBar();

	//bool bUseExternalTarget;
	bool bOnlyTriggerOnce = false;

	float Time = 3.0f;	// TO-DO: Time cannot be <= 0
	float TimeStarted = 0.0f;

	FTimerHandle TimerHandle;

	FGameplayTag ChannelingTag;

	UChannelingProgressBar* WidgetChannelingProgressBar = nullptr;
};
