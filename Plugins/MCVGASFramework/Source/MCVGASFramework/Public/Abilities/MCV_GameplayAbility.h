// Copyright 2022 David (Quod) Soler

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MCV_GameplayAbility.generated.h"

/**
 * Base class for all abilities.
 * Adds the ability to auto activate abilities when granted for passive behaviors.
 */
UCLASS()
class MCVGASFRAMEWORK_API UMCV_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
    UMCV_GameplayAbility();
    // If an ability is marked as 'ActivateAbilityOnGranted', activate them immediately when given here
    // Epic's comment: Projects may want to initiate passives or do other "BeginPlay" type of logic here.
    virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
public:
    // Tells an ability to activate immediately when its granted. Used for passive abilities and abilities forced on others.
    UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Ability")
    bool bActivateAbilityOnGranted = false;
	
};
