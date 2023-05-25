// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Abilities/AbilityTask_SlowTime.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"


UAbilityTask_SlowTime::UAbilityTask_SlowTime(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
}

void UAbilityTask_SlowTime::TickTask(float DeltaTime)
{
	if (bSlowingTime)
		SmoothSlowTime(DeltaTime);
}


UAbilityTask_SlowTime* UAbilityTask_SlowTime::SlowTimeGameplayEvent(UGameplayAbility* OwningAbility, FGameplayTag SuccessTag, FGameplayTag FailedTag, AActor* OptionalExternalTarget, bool OnlyTriggerOnce, bool OnlyMatchExact)
{
	UAbilityTask_SlowTime* MyObj = NewAbilityTask<UAbilityTask_SlowTime>(OwningAbility);
	MyObj->SuccessTag = SuccessTag;
	MyObj->FailedTag = FailedTag;
	MyObj->SetExternalTarget(OptionalExternalTarget);
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
	MyObj->OnlyMatchExact = OnlyMatchExact;

	return MyObj;
}

void UAbilityTask_SlowTime::SetExternalTarget(AActor* Actor)
{
	if (Actor)
	{
		UseExternalTarget = true;
		OptionalExternalTarget = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	}
}

UAbilitySystemComponent* UAbilityTask_SlowTime::GetTargetASC() 
{
	if (UseExternalTarget)
	{
		return OptionalExternalTarget;
	}

	return AbilitySystemComponent.Get();
}

void UAbilityTask_SlowTime::Activate()
{
	UAbilitySystemComponent* ASC = GetTargetASC();
	if (ASC)
	{
		if (OnlyMatchExact)
		{
			SuccessHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(SuccessTag).AddUObject(this, &UAbilityTask_SlowTime::SuccessEventCallback);
			FailedHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(FailedTag).AddUObject(this, &UAbilityTask_SlowTime::FailedEventCallback);
		}
		else
		{
			SuccessHandle = ASC->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(SuccessTag), FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UAbilityTask_SlowTime::SuccessEventContainerCallback));
			FailedHandle = ASC->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(FailedTag), FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UAbilityTask_SlowTime::FailedEventContainerCallback));
		}
	}

	bSlowingTime = true;
	SlowStep = (1- CustomTimeDilation) / SlowTimeFadeInRate;

	Super::Activate();
}

void UAbilityTask_SlowTime::SuccessEventCallback(const FGameplayEventData* Payload)
{
	SuccessEventContainerCallback(SuccessTag, Payload);
}

void UAbilityTask_SlowTime::SuccessEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload)
{
	UWorld* World = GetWorld();
	AWorldSettings* const WorldSettings = World->GetWorldSettings();
	if (!WorldSettings)
		return;

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	WorldSettings->SetTimeDilation(1);
	Player->CustomTimeDilation = 1;

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempPayload = *Payload;
		TempPayload.EventTag = MatchingTag;
		SuccessEventReceived.Broadcast(MoveTemp(TempPayload));
	}
	if (OnlyTriggerOnce)
	{
		EndTask();
	}
}

void UAbilityTask_SlowTime::FailedEventCallback(const FGameplayEventData* Payload)
{
	FailedEventContainerCallback(FailedTag, Payload);
}


void UAbilityTask_SlowTime::FailedEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload)
{
	UWorld* World = GetWorld();
	AWorldSettings* const WorldSettings = World->GetWorldSettings();
	if (!WorldSettings)
		return;

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	WorldSettings->SetTimeDilation(1);
	Player->CustomTimeDilation = 1;

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempPayload = *Payload;
		TempPayload.EventTag = MatchingTag;
		FailedEventReceived.Broadcast(MoveTemp(TempPayload));
	}
	if (OnlyTriggerOnce)
	{
		EndTask();
	}
}

void UAbilityTask_SlowTime::SmoothSlowTime(float DeltaTime)
{
	UWorld* World = GetWorld();
	AWorldSettings* const WorldSettings = World->GetWorldSettings();
	if (!WorldSettings)
		return;

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	CurrentSlowTimeDilation -= SlowStep * DeltaTime;

	if (CurrentSlowTimeDilation <= CustomTimeDilation) {
		WorldSettings->SetTimeDilation(CurrentSlowTimeDilation);
		Player->CustomTimeDilation = 1 / CurrentSlowTimeDilation;

		bSlowingTime = false;
		CurrentSlowTimeDilation = 0;
	}
	else {
		WorldSettings->SetTimeDilation(CurrentSlowTimeDilation);
		Player->CustomTimeDilation = 1 / CurrentSlowTimeDilation;
	}
}

void UAbilityTask_SlowTime::OnDestroy(bool AbilityEnding)
{
	UAbilitySystemComponent* ASC = GetTargetASC();
	if (ASC && SuccessHandle.IsValid())
	{
		if (OnlyMatchExact)
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(SuccessTag).Remove(SuccessHandle);
		}
		else
		{
			ASC->RemoveGameplayEventTagContainerDelegate(FGameplayTagContainer(SuccessTag), SuccessHandle);
		}

	}

	if (ASC && FailedHandle.IsValid())
	{
		if (OnlyMatchExact)
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(FailedTag).Remove(FailedHandle);
		}
		else
		{
			ASC->RemoveGameplayEventTagContainerDelegate(FGameplayTagContainer(FailedTag), FailedHandle);
		}

	}

	Super::OnDestroy(AbilityEnding);
}