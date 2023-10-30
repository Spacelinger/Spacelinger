// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SpacelingerAudioComponent.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class SPACELINGER_API USpacelingerAudioComponent : public UGameInstanceSubsystem 
{
	GENERATED_BODY()

public:
	USpacelingerAudioComponent(){FWorldDelegates::OnPostWorldCreation.AddUObject(this, &USpacelingerAudioComponent::BeginPlay);};
	
	// Begin USubsystem
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	// End USubsystem

	UFUNCTION()
	void BeginPlay(UWorld* world);
	UFUNCTION()
	void Soldier_DeathAudioReaction(FVector Location, FRotator Rotation);
	UFUNCTION()
	void Soldier_ResumePatrol();
	UFUNCTION()
	void PlayBackgroundMusic();
	UFUNCTION()
	void StopBackgroundMusic();
	UFUNCTION()
	void Soldier_Stunned();
	UFUNCTION()
	void Soldier_ResetStunnedState();
	UFUNCTION()
	void Spider_StunningWeb();
	UFUNCTION()
	void PlayChaseMusic();
	UFUNCTION()
	void StopChaseMusic();
	UFUNCTION()
	void Soldier_VoiceCue(FVector Location, FRotator Rotation);
	UFUNCTION()
	void StopAllSoundsExceptDeathReaction();
	UFUNCTION()
	void PlayLaserPuzzleAnnouncer();
	UFUNCTION()
	void UpdateBarFillingSound(float awareness);
	UFUNCTION()
	void StopBarFillingSound();
	UFUNCTION()
	void PlayDetectionSound();
	UFUNCTION()
	void PlayElectricitySFX(FVector Location, FRotator Rotation);
	UFUNCTION()
	void Spider_SpiderWebStart();
	UFUNCTION()
	void Spider_SpiderWebEnd();
	UFUNCTION()
	void Spider_PutTrap();
	UFUNCTION()
	void Spider_CancelPutTrap();

	// Individual multipliers
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	float FirstKillSFXVolumeMultiplier = 0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	float SoldierVoiceCueVolumeMultiplier = 0.4f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	float SoldierStunnedSFXVolumeMultiplier = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	float SoldierDeathSFXVolumeMultiplier = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	float SoldierResumePatrolVolumeMultiplier = 0.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	float BarFillingSoundVolumeMultiplier = 0.75f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	float BarFillingSoundPitchMultiplier = 3.0f; // To be multiplied by awareness
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	float DetectionSoundVolumeMultiplier = 0.3f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	float ChaseMusicVolumeMultiplier = 0.05f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	float BackgroundMusicVolumeMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	float LaserPuzzleAnnouncerVolumeMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	float ElectricitySFXVolumeMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	float ElectricitySFXPitchMultiplier = 1.0f;

	// Global Multipliers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	int GlobalMasterVolumeMultiplier = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	int GlobalSFXVolumeMultiplier = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	int GlobalVoicesVolumeMultiplier = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	int GlobalMusicVolumeMultiplier = 100;
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * BackgroundMusic;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * FirstKillSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * SoldierDeathSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * SoldierStunnedSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * SoldierStrugglingSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * StunningWebSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * ChaseMusic;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * SoldierVoiceCue; // First encounter w/ spider and also random instances
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * SoldierResumePatrolSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * DoomedAnnouncerCue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * BarFillingSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * DetectionSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * ElectricitySFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * SpiderWebStartSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * SpiderWebEndSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	USoundCue * SpiderPutTrapSFX;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentBackgroundMusic;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentSoldierResumePatrolSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentSoldierStunnedSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentSoldierStrugglingSFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentChaseMusic;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentSoldierVoiceCue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentBarFillingSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentDetectionSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	UAudioComponent * CurrentPutTrapSound;

private:
	friend class FSubsystemCollectionBase;
	FSubsystemCollectionBase* InternalOwningSubsystem;
	
	int SoldierDeathCounter = 0;
	bool LaserPuzzleAnnouncerHasPlayed = false;
	bool IsResumingPatrol = false;
	bool IsAnySoldierAlerted = false;
	bool IsSoldierVoiceCuePlaying = false;
	
};
