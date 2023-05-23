// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "GAS/Abilities/GA_SlowTime.h"
#include "Kismet/GameplayStatics.h"

UGA_SlowTime::UGA_SlowTime() {

	//AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Fullbody")));

}

void UGA_SlowTime::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CommitAbility(Handle, ActorInfo, ActivationInfo)) {

		UWorld* World = GetWorld();
		AWorldSettings* const WorldSettings = World->GetWorldSettings();
		if (!WorldSettings)
			return;

		float TimeDilation = 0.2f;	// TODO: Parameter

		WorldSettings->SetTimeDilation(TimeDilation);

		//GetWorld()->GetFirstPlayerController()->CustomTimeDilation = 1 / TimeDilation;

		ACharacter* Char = UGameplayStatics::GetPlayerCharacter(GetWorld(),0);
		Char->CustomTimeDilation = 1 / TimeDilation;

	}
	else {
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}
void UGA_SlowTime::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

bool UGA_SlowTime::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const {

	const bool bSuperCanActivate = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	
	if (bSuperCanActivate == false) {
		return false;
	}

	bool bResult = true;	// TO-DO: Further assessments

	return bResult;
}
