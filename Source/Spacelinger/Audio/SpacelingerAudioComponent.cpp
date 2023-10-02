// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Audio/SpacelingerAudioComponent.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Soldier/SLSoldier.h"

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

void USpacelingerAudioComponent::SoldierDeathAudioReaction()
{
	StopAllSoundsExceptDeathReaction();
	
	SoldierDeathCounter++;
	
	UGameplayStatics::SpawnSound2D(this, SoldierDeathSFX, 0.5f);
	
	if (SoldierDeathCounter == 1)
	{
		UGameplayStatics::SpawnSound2D(this, FirstKillSFX, 0.5f);
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
}

void USpacelingerAudioComponent::Soldier_ResumePatrol()
{
	if (!CurrentSoldierResumePatrolSFX)
	{
		IsSoldierVoiceCuePlaying = false;
		CurrentSoldierResumePatrolSFX = UGameplayStatics::SpawnSound2D(this, SoldierResumePatrolSFX, 0.5f);
	}
}

void USpacelingerAudioComponent::Soldier_VoiceCue(FVector Location, FRotator Rotation)
{
	if (!IsSoldierVoiceCuePlaying)
	{
		IsSoldierVoiceCuePlaying = true;
		CurrentSoldierVoiceCue = UGameplayStatics::SpawnSoundAtLocation(this, SoldierVoiceCue,
						Location, Rotation, 0.25f);
	}
}

void USpacelingerAudioComponent::PlayChaseMusic()
{
	if (!IsAnySoldierAlerted)
	{
		IsAnySoldierAlerted = true;
		CurrentChaseMusic = UGameplayStatics::SpawnSound2D(this, ChaseMusic, 0.10f);
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
	}
}




