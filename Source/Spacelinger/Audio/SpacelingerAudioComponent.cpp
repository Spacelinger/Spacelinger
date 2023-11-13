// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Audio/SpacelingerAudioComponent.h"

bool USpacelingerAudioComponent::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void USpacelingerAudioComponent::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USpacelingerAudioComponent::Deinitialize()
{
	Super::Deinitialize();
}


void USpacelingerAudioComponent::BeginPlay(UWorld* world)
{
	if (IsInGameThread())
	{
		PlayBackgroundMusic();
	}
}


void USpacelingerAudioComponent::Soldier_DeathAudioReaction(FVector Location, FRotator Rotation)
{
	StopAllSoundsExceptDeathReaction();
	
	SoldierDeathCounter++;
	
	UGameplayStatics::SpawnSound2D(this, SoldierDeathSFX, SoldierDeathSFXVolumeMultiplier);
	
	if (SoldierDeathCounter == 1)
	{
		UGameplayStatics::SpawnSoundAtLocation(this, FirstKillSFX, Location, Rotation, FirstKillSFXVolumeMultiplier);
	}
}

void USpacelingerAudioComponent::StopAllSoundsExceptDeathReaction()
{
	IsSoldierVoiceCuePlaying = false;
	
	if (CurrentSoldierVoiceCue)
	{
		CurrentSoldierVoiceCue->Stop();
	}
	
	if (CurrentSoldierResumePatrolSFX)
	{
		CurrentSoldierResumePatrolSFX->Stop();
	}
	
	if (CurrentSoldierStunnedSFX)
	{
		CurrentSoldierStunnedSFX->Stop();
		CurrentSoldierStrugglingSFX->Stop();
	}
}

void USpacelingerAudioComponent::Spider_StunningWeb()
{
	UGameplayStatics::SpawnSound2D(this, StunningWebSFX);
}

void USpacelingerAudioComponent::Spider_SpiderWebStart()
{
	UGameplayStatics::SpawnSound2D(this, SpiderWebStartSFX);
}

void USpacelingerAudioComponent::Spider_SpiderWebEnd()
{
	UGameplayStatics::SpawnSound2D(this, SpiderWebEndSFX);
}

void USpacelingerAudioComponent::Spider_PutTrap()
{
	CurrentPutTrapSound = UGameplayStatics::SpawnSound2D(this, SpiderPutTrapSFX);
}

void USpacelingerAudioComponent::Spider_CancelPutTrap()
{
	if (CurrentPutTrapSound != nullptr)
	{
		CurrentPutTrapSound->Stop();
	}
}


void USpacelingerAudioComponent::Soldier_ResumePatrol()
{
	if (this != nullptr)
	{
		if (!CurrentSoldierResumePatrolSFX)
		{
			IsSoldierVoiceCuePlaying = false;
			CurrentSoldierResumePatrolSFX = UGameplayStatics::SpawnSound2D(this, SoldierResumePatrolSFX, SoldierResumePatrolVolumeMultiplier);
		}
	}
}

void USpacelingerAudioComponent::Soldier_VoiceCue(FVector Location, FRotator Rotation)
{
	if (!IsSoldierVoiceCuePlaying)
	{
		IsSoldierVoiceCuePlaying = true;
		CurrentSoldierVoiceCue = UGameplayStatics::SpawnSoundAtLocation(this, SoldierVoiceCue,
						Location, Rotation, SoldierVoiceCueVolumeMultiplier);
	}
}

void USpacelingerAudioComponent::Soldier_Stunned()
{
	if (this != nullptr)
	{
		if (!CurrentSoldierStunnedSFX)
		{
			IsSoldierVoiceCuePlaying = false;
			CurrentSoldierStunnedSFX = UGameplayStatics::SpawnSound2D(this, SoldierStunnedSFX, SoldierStunnedSFXVolumeMultiplier);
			CurrentSoldierStrugglingSFX = UGameplayStatics::SpawnSound2D(this, SoldierStrugglingSFX, SoldierStunnedSFXVolumeMultiplier);
		}
	}
}

void USpacelingerAudioComponent::Soldier_ResetStunnedState()
{
	CurrentSoldierStunnedSFX = nullptr; 
}

void USpacelingerAudioComponent::PlayDetectionSound()
{
	CurrentDetectionSound = UGameplayStatics::SpawnSound2D(this, DetectionSound, DetectionSoundVolumeMultiplier);
}


void USpacelingerAudioComponent::PlayChaseMusic()
{
	if (!IsAnySoldierAlerted)
	{
		StopBackgroundMusic();
		IsAnySoldierAlerted = true;
		PlayDetectionSound();
		CurrentChaseMusic = UGameplayStatics::SpawnSound2D(this, ChaseMusic, ChaseMusicVolumeMultiplier);
	}
}

void USpacelingerAudioComponent::StopChaseMusic()
{
	if (this != nullptr)
	{
		IsAnySoldierAlerted = false;
		if (CurrentChaseMusic)
		{
			CurrentChaseMusic->FadeOut(2.0f, 0.f);
		}
		CurrentChaseMusic = nullptr;
		CurrentDetectionSound = nullptr;
		PlayBackgroundMusic();
	}
}

void USpacelingerAudioComponent::PlayBackgroundMusic()
{
	if (!CurrentBackgroundMusic)
	{
		CurrentBackgroundMusic = UGameplayStatics::SpawnSound2D(this, BackgroundMusic,
			GlobalMasterVolumeMultiplier/100 * GlobalMusicVolumeMultiplier/100 * BackgroundMusicVolumeMultiplier);
	}
}

void USpacelingerAudioComponent::StopBackgroundMusic()
{
	if (CurrentBackgroundMusic)
	{
		CurrentBackgroundMusic->FadeOut(2.0f, 0.f);
		CurrentBackgroundMusic = nullptr;
	}
}

void USpacelingerAudioComponent::PlayLaserPuzzleAnnouncer()
{
	if (!LaserPuzzleAnnouncerHasPlayed)
	{
		LaserPuzzleAnnouncerHasPlayed = true;
		UGameplayStatics::SpawnSound2D(this, DoomedAnnouncerCue, LaserPuzzleAnnouncerVolumeMultiplier);
	}
}

void USpacelingerAudioComponent::UpdateBarFillingSound(float awareness)
{
	if (awareness < 1.0f)
	{
		awareness += 0.1f;
		if (!CurrentBarFillingSound)
		{
			CurrentBarFillingSound = UGameplayStatics::SpawnSound2D(this, BarFillingSound, BarFillingSoundVolumeMultiplier*awareness, BarFillingSoundPitchMultiplier*awareness);
		} else
		{
			CurrentBarFillingSound->SetVolumeMultiplier(BarFillingSoundVolumeMultiplier*awareness);
			CurrentBarFillingSound->SetPitchMultiplier(BarFillingSoundPitchMultiplier*awareness);
		}
	}
}

void USpacelingerAudioComponent::StopBarFillingSound()
{
	if (this != nullptr)
	{
		if (CurrentBarFillingSound)
		{
			CurrentBarFillingSound->FadeOut(1.5f, 0.f);
		}
		CurrentBarFillingSound = nullptr;
	}
}

void USpacelingerAudioComponent::PlayElectricitySFX(FVector Location, FRotator Rotation)
{
	UGameplayStatics::SpawnSoundAtLocation(this, ElectricitySFX, Location, Rotation, ElectricitySFXVolumeMultiplier, ElectricitySFXPitchMultiplier);
}





