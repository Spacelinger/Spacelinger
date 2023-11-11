// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpacelingerGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

ASpacelingerGameMode::ASpacelingerGameMode() 
{

}

AActor* ASpacelingerGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	AActor* PlayerStart = FindPlayerStart(GetWorld()->GetFirstPlayerController(), CurrentPlayerStartTag);

	return PlayerStart;
}

void ASpacelingerGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
	//Currently not working as intended, probably because the current player pawn delete and creation of a new one
	//is not properly handled before calling restartplayer
}


void ASpacelingerGameMode::SetNewPlayerStart(FString PlayerStartTag)
{
	CurrentPlayerStartTag = PlayerStartTag;
}
