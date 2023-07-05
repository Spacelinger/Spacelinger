// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Abilities/AbilityTask_SlowTime.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Slime_A.h"
#include "Components/PostProcessComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "GAS/Attributes/StaminaAttributeSet.h"

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
	MyObj->bOnlyTriggerOnce = OnlyTriggerOnce;
	MyObj->bOnlyMatchExact = OnlyMatchExact;

	return MyObj;
}

void UAbilityTask_SlowTime::SetExternalTarget(AActor* Actor)
{
	if (Actor)
	{
		bUseExternalTarget = true;
		OptionalExternalTarget = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	}
}

UAbilitySystemComponent* UAbilityTask_SlowTime::GetTargetASC() 
{
	if (bUseExternalTarget)
	{
		return OptionalExternalTarget;
	}

	return AbilitySystemComponent.Get();
}

void UAbilityTask_SlowTime::Activate()
{
	Super::Activate();

	UAbilitySystemComponent* ASC = GetTargetASC();
	if (ASC)
	{
		if (bOnlyMatchExact)
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
	LineStep = SlowTimeEffectLineDensity / SlowTimeFadeInRate;

	if (PostProcessSpeedLinesMaterial) {
		DynamicSpeedLinesMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, PostProcessSpeedLinesMaterial);
	}

	ASlime_A* Slime = Cast<ASlime_A>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	PPComp = Slime->GetPostProcessComponent();
	if (PPComp) {
		//FPostProcessSettings& PostProcessSettings = PPComp->Settings;

		WeightedBlendable.Object = DynamicSpeedLinesMaterial;
		WeightedBlendable.Weight = 1.0f;
		
		PPComp->Settings.WeightedBlendables.Array.Add(WeightedBlendable);
	}
	
	Slime->SetStaminaRecoveryValue(StaminaCostOverTime);

	/*
	USpringArmComponent* CameraBoom;
	CameraBoom = Slime->GetCameraBoom();
	CameraBoom->TargetArmLength = 420.0f;
	*/

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

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0.0f);

	WorldSettings->SetTimeDilation(1);
	Player->CustomTimeDilation = 1;

	if (PPComp) {
		FPostProcessSettings* PostProcessSettings = new FPostProcessSettings();
		PPComp->Settings = *PostProcessSettings; // This removes all applied materials, so might be overkill if other PPFX were applied
	}

	Cast<ASlime_A>(Player)->ResetStaminaRecoveryValue();

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempPayload = *Payload;
		TempPayload.EventTag = MatchingTag;
		SuccessEventReceived.Broadcast(MoveTemp(TempPayload));
	}
	if (bOnlyTriggerOnce)
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

	WorldSettings->SetTimeDilation(1.0f);
	Player->CustomTimeDilation = 1.0f;

	if (PPComp) {
		FPostProcessSettings* PostProcessSettings = new FPostProcessSettings();
		PPComp->Settings = *PostProcessSettings; // This removes all applied materials, so might be overkill if other PPFX were applied
	}

	Cast<ASlime_A>(Player)->ResetStaminaRecoveryValue();

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempPayload = *Payload;
		TempPayload.EventTag = MatchingTag;
		FailedEventReceived.Broadcast(MoveTemp(TempPayload));
	}
	if (bOnlyTriggerOnce)
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
	CurrentLineStep += LineStep * DeltaTime;

	USpringArmComponent* CameraBoom;
	CameraBoom = Cast<ASlime_A>(Player)->GetCameraBoom();
	CameraBoom->TargetArmLength = 420.0f;

	if (CurrentSlowTimeDilation <= CustomTimeDilation) {
		WorldSettings->SetTimeDilation(CustomTimeDilation);
		Player->CustomTimeDilation = 1.0f / CustomTimeDilation;
		if (DynamicSpeedLinesMaterial)
			DynamicSpeedLinesMaterial->SetScalarParameterValue("Line Density", SlowTimeEffectLineDensity);
		bSlowingTime = false;
		CurrentSlowTimeDilation = 1.0f;
	}
	else {
		WorldSettings->SetTimeDilation(CurrentSlowTimeDilation);
		Player->CustomTimeDilation = 1.0f / CurrentSlowTimeDilation;
		if (DynamicSpeedLinesMaterial)
			DynamicSpeedLinesMaterial->SetScalarParameterValue("Line Density", CurrentLineStep);
	}
}

void UAbilityTask_SlowTime::OnDestroy(bool AbilityEnding)
{
	UAbilitySystemComponent* ASC = GetTargetASC();
	if (ASC && SuccessHandle.IsValid())
	{
		if (bOnlyMatchExact)
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
		if (bOnlyMatchExact)
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

void UAbilityTask_SlowTime::SetSpeedLinesMaterial(UMaterialInstance* SpeedLinesMaterial)
{
	PostProcessSpeedLinesMaterial = SpeedLinesMaterial;
}