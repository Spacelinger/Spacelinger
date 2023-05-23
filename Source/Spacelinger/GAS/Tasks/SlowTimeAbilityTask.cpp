// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Tasks/SlowTimeAbilityTask.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

/*
USlowTimeAbilityTask::USlowTimeAbilityTask(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}
*/

USlowTimeAbilityTask* USlowTimeAbilityTask::WaitGameplayEvent(UGameplayAbility* OwningAbility, FGameplayTag Tag, AActor* OptionalExternalTarget, bool OnlyTriggerOnce, bool OnlyMatchExact)
{
	USlowTimeAbilityTask* MyObj = NewAbilityTask<USlowTimeAbilityTask>(OwningAbility);
	MyObj->Tag = Tag;
	MyObj->SetExternalTarget(OptionalExternalTarget);
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
	MyObj->OnlyMatchExact = OnlyMatchExact;

	return MyObj;
}

void USlowTimeAbilityTask::Activate()
{
	UAbilitySystemComponent* ASC = GetTargetASC();
	if (ASC)
	{
		if (OnlyMatchExact)
		{
			MyHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(Tag).AddUObject(this, &USlowTimeAbilityTask::GameplayEventCallback);
		}
		else
		{
			MyHandle = ASC->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(Tag), FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &USlowTimeAbilityTask::GameplayEventContainerCallback));
		}
	}

	Super::Activate();
}

void USlowTimeAbilityTask::GameplayEventCallback(const FGameplayEventData* Payload)
{
	GameplayEventContainerCallback(Tag, Payload);
}

void USlowTimeAbilityTask::GameplayEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempPayload = *Payload;
		TempPayload.EventTag = MatchingTag;
		EventReceived.Broadcast(MoveTemp(TempPayload));
	}
	if (OnlyTriggerOnce)
	{
		EndTask();
	}
}

void USlowTimeAbilityTask::SetExternalTarget(AActor* Actor)
{
	if (Actor)
	{
		UseExternalTarget = true;
		OptionalExternalTarget = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	}
}

UAbilitySystemComponent* USlowTimeAbilityTask::GetTargetASC()
{
	if (UseExternalTarget)
	{
		return OptionalExternalTarget;
	}

	return AbilitySystemComponent.Get();
}

void USlowTimeAbilityTask::OnDestroy(bool AbilityEnding)
{
	UAbilitySystemComponent* ASC = GetTargetASC();
	if (ASC && MyHandle.IsValid())
	{
		if (OnlyMatchExact)
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(Tag).Remove(MyHandle);
		}
		else
		{
			ASC->RemoveGameplayEventTagContainerDelegate(FGameplayTagContainer(Tag), MyHandle);
		}

	}

	Super::OnDestroy(AbilityEnding);
}