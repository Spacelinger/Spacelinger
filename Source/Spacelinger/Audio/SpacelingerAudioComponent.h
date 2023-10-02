// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sound/SoundCue.h"
#include "SpacelingerAudioComponent.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class SPACELINGER_API USpacelingerAudioComponent : public UGameInstanceSubsystem 
{
	GENERATED_BODY()

public:
	USpacelingerAudioComponent(){};
	// Begin USubsystem
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// End USubsystem
	
	void SoldierDeathAudioReaction();
	void Soldier_ResumePatrol();
	void PlayChaseMusic();
	void StopChaseMusic();
	void Soldier_VoiceCue(FVector Location, FRotator Rotation);
	void StopAllSoundsExceptDeathReaction();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * FirstKillSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * SoldierDeathSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * ChaseMusic;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * SoldierVoiceCue; // First encounter w/ spider and also random instances
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * SoldierResumePatrolSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentSoldierResumePatrolSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentChaseMusic;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentSoldierVoiceCue;

	bool IsResumingPatrol = false;
	bool IsAnySoldierAlerted = false;
	bool IsSoldierVoiceCuePlaying = false;

private:
	friend class FSubsystemCollectionBase;
	FSubsystemCollectionBase* InternalOwningSubsystem;

	int SoldierDeathCounter = 0;
	
};
