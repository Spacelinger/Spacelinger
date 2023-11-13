// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "QuestLogComponent.generated.h"

UENUM(BlueprintType)
enum ESpawnPoint
{
	GarbagePit	UMETA(DisplayName = "Garbage pit"),
	Level0		UMETA(DisplayName = "Level 0"),
	Hangar		UMETA(DisplayName = "Hangar"),
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPACELINGER_API UQuestLogComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UQuestLogComponent();
	
	FDataTableRowHandle StartingQuest;

	void SetQuestOnSpawnPoint(ESpawnPoint SpawnPoint);

	UFUNCTION(BlueprintImplementableEvent)
	void ForceQuestLogUpdate(ESpawnPoint SpawnPoint);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;	
};
