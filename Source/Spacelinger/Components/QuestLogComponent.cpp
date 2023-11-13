// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Components/QuestLogComponent.h"
#include <SpacelingerGameMode.h>

UQuestLogComponent::UQuestLogComponent()
{
}

void UQuestLogComponent::SetQuestOnSpawnPoint(ESpawnPoint SpawnPoint)
{
	
	if (SpawnPoint)
	{
		//Update Quest Log (implemented in BP)
		ForceQuestLogUpdate(SpawnPoint);

		//Teleport player
		ASpacelingerGameMode* GameMode = Cast<ASpacelingerGameMode>(GetWorld()->GetAuthGameMode());
		APawn* Owner = Cast<APawn>(GetOwner());
		switch (SpawnPoint) 
		{
			case ESpawnPoint::GarbagePit:
				//Teleport to StartPoint
				if (GameMode)
				{
					GameMode->SetNewPlayerStart(TEXT("GarbagePit"));
					AActor* PlayerStart = GameMode->FindPlayerStart(Owner->GetController(), GameMode->CurrentPlayerStartTag);
					Owner->TeleportTo(PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
				}
				break;

			case ESpawnPoint::Level0:
				//Teleport to StartPoint
				if (GameMode)
				{
					GameMode->SetNewPlayerStart(TEXT("Level0"));
					AActor* PlayerStart = GameMode->FindPlayerStart(Owner->GetController(), GameMode->CurrentPlayerStartTag);
					Owner->TeleportTo(PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
				}
				break;

			case ESpawnPoint::Hangar:
				//Teleport to StartPoint
				if (GameMode)
				{
					GameMode->SetNewPlayerStart(TEXT("Hangar"));
					AActor* PlayerStart = GameMode->FindPlayerStart(Owner->GetController(), GameMode->CurrentPlayerStartTag);
					Owner->TeleportTo(PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation());
				}
				break;

			default:
				UE_LOG(LogTemp, Warning, TEXT("QuestLog: Tried to reset the SpawnPoint using an invalid handle"));
		}
	}
}


// Called when the game starts
void UQuestLogComponent::BeginPlay()
{
	Super::BeginPlay();
	
}