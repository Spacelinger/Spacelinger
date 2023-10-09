// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Abilities/MCV_GameplayAbility.h"
#include "Sound/SoundCue.h"
#include "GA_Hook.generated.h"

/**
 * 
 */
UCLASS()
class SPACELINGER_API UGA_Hook : public UMCV_GameplayAbility
{
	GENERATED_BODY()

public:

	UGA_Hook();

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;	

	void ActionThrowHook();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Audio")
		TArray<USoundCue*> HookShotSoundFXArray;

	const float TraceDistance = 50.0f;

	double FloorThreshold = 0.9;
	
	FORCEINLINE bool IsCeiling(FVector Normal) {
		return FVector::DotProduct(Normal, FVector::UpVector) <= -FloorThreshold;
	}
};
