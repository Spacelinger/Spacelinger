// Copyright 2022 David (Quod) Soler

#include "Abilities/MCV_GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Components/MCV_AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

UMCV_GameplayAbility::UMCV_GameplayAbility()
{
    // Default to Instance Per Actor
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UMCV_GameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
    Super::OnGiveAbility(ActorInfo, Spec);

    // Auto activate ability if needed
    if (bActivateAbilityOnGranted)
    {
        bool bAbilityActivated = ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
    }
}