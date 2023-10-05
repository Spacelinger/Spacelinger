#include "SLCheats.h"
#include "Engine/Console.h"
#include "Spider/Slime_A.h"
#include "Components/InventoryComponent.h"

namespace CheatUtils {
	ASlime_A* GetPlayerCharacter(UWorld* World) {
		if (!World) return nullptr;

		APlayerController *PC = World->GetFirstPlayerController();
		if (!PC) return nullptr;

		ACharacter *Character = PC->GetCharacter();
		if (!Character) return nullptr;

		return Cast<ASlime_A>(Character);
	}

	template< class T >
	T* GetPlayerComponent(UWorld* World)
	{
		ASlime_A *Character = GetPlayerCharacter(World);
		if (!Character) return nullptr;

		return Character->FindComponentByClass<T>();
	}
}

void RegisterConsoleCheatsNames() {
	UConsole::RegisterConsoleAutoCompleteEntries.AddLambda([](TArray<FAutoCompleteCommand>& Commands) {
		// NOTE(Sergi): Probably delete these if we have lots of commands, but I'll keep them as an example on how to do this.
		{
			FAutoCompleteCommand Cmd;
			Cmd.Command = FString::Printf(TEXT("sl.toggleraycasts true"));
			Cmd.Desc    = FString::Printf(TEXT("Show raycasts being thrown"));
			Commands.Add(Cmd);
		}
		{
			FAutoCompleteCommand Cmd;
			Cmd.Command = FString::Printf(TEXT("sl.toggleraycasts false"));
			Cmd.Desc    = FString::Printf(TEXT("Hide raycasts being thrown"));
			Commands.Add(Cmd);
		}
		{
			FAutoCompleteCommand Cmd;
			Cmd.Command = FString::Printf(TEXT("sl.givekey"));
			Cmd.Desc = FString::Printf(TEXT("Gives the player a key to unlock doors"));
			Commands.Add(Cmd);
		}
	});
}

static FAutoConsoleCommandWithWorldAndArgs FCheatDebugRaycast(
	TEXT("sl.toggleraycasts"), TEXT("Toggle the raycasts being thrown"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World) {
		if (ASlime_A *PlayerCharacter = CheatUtils::GetPlayerCharacter(World)) {
			PlayerCharacter->bDrawDebugLines = (Args.Num() > 0) ? Args[0].ToBool() : !PlayerCharacter->bDrawDebugLines;
			UE_LOG(LogTemp, Display, TEXT("FCheatDebugRaycast: Raycasts have been turned %s."), PlayerCharacter->bDrawDebugLines ? TEXT("on") : TEXT("off"));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("FCheatDebugRaycast: PlayerCharacter couldn't be found."));
		}
	}
), ECVF_Cheat);

static FAutoConsoleCommandWithWorld FCheatGiveKey(
	TEXT("sl.givekey"),
	TEXT("Gives the player a key to unlock doors"),
	FConsoleCommandWithWorldDelegate::CreateLambda([](UWorld* World) {
		if (ASlime_A* Player = CheatUtils::GetPlayerCharacter(World)) {
			UInventoryComponent* InventoryComponent = Player->FindComponentByClass<UInventoryComponent>();
			if(InventoryComponent)
				InventoryComponent->AddItem(TEXT("DoorKey"));
		}
	})
);