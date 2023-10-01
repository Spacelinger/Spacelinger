// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Audio/SpacelingerAudioComponent.h"
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





