// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Audio/SpacelingerAudioComponent.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

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
	SoldierDeathCounter++;
	
	UGameplayStatics::SpawnSound2D(this, SoldierDeathSFX, 0.5f);
	
	if (SoldierDeathCounter == 1)
	{
		UGameplayStatics::SpawnSound2D(this, FirstKillSFX, 0.5f);
	}
	
}

void USpacelingerAudioComponent::Soldier_ResumePatrol()
{
	if (!CurrentSoldierResumePatrolSFX)
	{
		CurrentSoldierResumePatrolSFX = UGameplayStatics::SpawnSound2D(this, SoldierResumePatrolSFX, 0.5f);
	} else
	{
		CurrentSoldierResumePatrolSFX = nullptr;
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




