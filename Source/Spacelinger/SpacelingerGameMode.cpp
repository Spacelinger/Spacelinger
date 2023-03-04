// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpacelingerGameMode.h"
#include "SpacelingerCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASpacelingerGameMode::ASpacelingerGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
