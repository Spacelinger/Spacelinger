// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Abilities/AbilityTask_PutTrap.h"
#include "AbilitySystemComponent.h"	// Might not be needed
#include "TimerManager.h"
#include "AbilitySystemGlobals.h"
#include <Actors/DoorBlock.h>

UAbilityTask_PutTrap::UAbilityTask_PutTrap(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	Time = 0.0f;
	TimeStarted = 0.0f;
}

UAbilityTask_PutTrap* UAbilityTask_PutTrap::PutTrapChannelingTask(UGameplayAbility* OwningAbility, FGameplayTag ChannelingTag, float Time, bool OnlyTriggerOnce)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Duration(Time);

	UAbilityTask_PutTrap* MyObj = NewAbilityTask<UAbilityTask_PutTrap>(OwningAbility);
	MyObj->ChannelingTag = ChannelingTag;
	MyObj->Time = Time;
	MyObj->bOnlyTriggerOnce = OnlyTriggerOnce;
	return MyObj;
}

void UAbilityTask_PutTrap::Activate()
{
	UAbilitySystemComponent* ASC = GetTargetASC();
	if (ASC && ASC->HasMatchingGameplayTag(ChannelingTag) == false)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			ChannelingCanceled.Broadcast();
		}
		if (bOnlyTriggerOnce)
		{
			EndTask();
			return;
		}
	}

	UWorld* World = GetWorld();
	TimeStarted = World->GetTimeSeconds();

	// May be better to use a dummy timer handle as we don't need to store it for later but we don't need to look for something to clear
	World->GetTimerManager().SetTimer(TimerHandle, this, &UAbilityTask_PutTrap::OnTimeFinish, Time, false);

	Super::Activate();
}

void UAbilityTask_PutTrap::TickTask(float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("Normalized time elapsed: %.2f"), GetNormalizedTimeElapsed());

	// If the actor interacting moves, cancel the task
	bool bIsInteractingActorMoving = GetTargetASC()->GetAvatarActor()->GetVelocity().Length() > 0;
	if (bIsInteractingActorMoving)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			ChannelingCanceled.Broadcast();	// Cancel channeling
		}
		if (bOnlyTriggerOnce)
		{
			EndTask();	// And end the task
		}
	}
}

void UAbilityTask_PutTrap::OnTimeFinish()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ChannelingComplete.Broadcast();
	}
	EndTask();
}

void UAbilityTask_PutTrap::GameplayTagCallback(const FGameplayTag InTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			ChannelingCanceled.Broadcast();
		}
		if (bOnlyTriggerOnce)
		{
			EndTask();
		}
	}
}

UAbilitySystemComponent* UAbilityTask_PutTrap::GetTargetASC()
{
	return AbilitySystemComponent.Get();
}

float UAbilityTask_PutTrap::GetNormalizedTimeElapsed() const
{
	if (UWorld* World = GetWorld())
	{
		return World->TimeSince(TimeStarted) / Time;
	}
	else
	{
		return 0.f;
	}
}


FString UAbilityTask_PutTrap::GetDebugString() const	// Consider Delete
{
	if (UWorld* World = GetWorld())
	{
		const float TimeLeft = Time - World->TimeSince(TimeStarted);
		return FString::Printf(TEXT("WaitDelay. Time: %.2f. TimeLeft: %.2f"), Time, TimeLeft);
	}
	else
	{
		return FString::Printf(TEXT("WaitDelay. Time: %.2f. Time Started: %.2f"), Time, TimeStarted);
	}
}