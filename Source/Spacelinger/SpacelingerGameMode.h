// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SpacelingerGameMode.generated.h"

UCLASS(minimalapi)
class ASpacelingerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASpacelingerGameMode();

	AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual void RestartPlayer(AController* NewPlayer) override;

	void SetNewPlayerStart(FString PlayerStartTag);

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString CurrentPlayerStartTag = TEXT("GarbagePit");
};



