// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Abilities/AbilityTask_DoorBlock.h"
#include "AbilitySystemComponent.h"	// Might not be needed
#include "TimerManager.h"
#include "AbilitySystemGlobals.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include <Actors/DoorBlock.h>

UAbilityTask_DoorBlock::UAbilityTask_DoorBlock(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	Time = 0.0f;
	TimeStarted = 0.0f;
}

UAbilityTask_DoorBlock* UAbilityTask_DoorBlock::DoorBlockChannelingTask(UGameplayAbility* OwningAbility, FGameplayTag ChannelingTag, float Time, ADoorBlock* DoorToBlock, /*AActor* OptionalExternalTarget,*/ bool OnlyTriggerOnce)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Duration(Time);

	UAbilityTask_DoorBlock* MyObj = NewAbilityTask<UAbilityTask_DoorBlock>(OwningAbility);
	MyObj->ChannelingTag = ChannelingTag;
	MyObj->Time = Time;
	MyObj->DoorToBlock = DoorToBlock;
	//MyObj->SetExternalTarget(OptionalExternalTarget);	// Not used
	MyObj->bOnlyTriggerOnce = OnlyTriggerOnce;
	return MyObj;
}

void UAbilityTask_DoorBlock::Activate()
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
	World->GetTimerManager().SetTimer(TimerHandle, this, &UAbilityTask_DoorBlock::OnTimeFinish, Time, false);

	Super::Activate();
}

void UAbilityTask_DoorBlock::TickTask(float DeltaTime)
{
	//UE_LOG(LogTemp, Warning, TEXT("Normalized time elapsed: %.2f"), GetNormalizedTimeElapsed());

	UpdateProgressBar();

	// If the actor interacting moves, cancel the task
	bool bIsInteractingActorMoving = GetTargetASC()->GetAvatarActor()->GetVelocity().Length() > 0;
	if (bIsInteractingActorMoving)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			ChannelingCanceled.Broadcast();	// Cancel channeling
			
			UWorld* World = GetWorld();
			World->GetTimerManager().ClearTimer(TimerHandle);
		}
		if (bOnlyTriggerOnce)
		{
			EndTask();	// And end the task
		}
	}
}

void UAbilityTask_DoorBlock::OnTimeFinish()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ChannelingComplete.Broadcast();
	}
	EndTask();
}

void UAbilityTask_DoorBlock::GameplayTagCallback(const FGameplayTag InTag, int32 NewCount)
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

UAbilitySystemComponent* UAbilityTask_DoorBlock::GetTargetASC()
{
	/* External Target is not used
	if (bUseExternalTarget)
	{
		return OptionalExternalTarget;
	}
	*/
	return AbilitySystemComponent.Get();
}

/*void UAbilityTask_DoorBlock::SetExternalTarget(AActor* Actor)		External Target is not used
{
	if (Actor)
	{
		bUseExternalTarget = true;
		OptionalExternalTarget = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	}
}*/

void UAbilityTask_DoorBlock::UpdateProgressBar()
{
	if (DoorToBlock)
		DoorToBlock->UpdateDoorBlockProgress(GetNormalizedTimeElapsed());
}

float UAbilityTask_DoorBlock::GetNormalizedTimeElapsed() const
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


FString UAbilityTask_DoorBlock::GetDebugString() const	// Consider Delete
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