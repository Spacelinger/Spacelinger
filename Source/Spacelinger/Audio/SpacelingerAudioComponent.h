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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * FirstKillSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * SoldierDeathSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * ChaseMusic;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * SoldierResumePatrolSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentSoldierResumePatrolSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentChaseMusic;

	bool IsResumingPatrol = false;
	bool IsAnySoldierAlerted = false;

private:
	friend class FSubsystemCollectionBase;
	FSubsystemCollectionBase* InternalOwningSubsystem;

	int SoldierDeathCounter = 0;
	
};
