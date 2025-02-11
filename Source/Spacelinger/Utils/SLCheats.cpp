#include "SLCheats.h"
#include "Engine/Console.h"
#include "Spider/Slime_A.h"
#include "Components/InventoryComponent.h"
#include <Components/QuestLogComponent.h>
#include "Components/LifeComponent.h"

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
		{
			FAutoCompleteCommand Cmd;
			Cmd.Command = FString::Printf(TEXT("sl.spawnpoint"));
			Cmd.Desc = FString::Printf(TEXT("Select a different point to spawn the player"));
			Commands.Add(Cmd);
		}
	});
}

static FAutoConsoleCommandWithWorldAndArgs FCheatDebugRaycast(
	TEXT("sl.toggleraycasts"), TEXT("Toggle the raycasts being thrown"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World) {
		if (ASlime_A *Player= CheatUtils::GetPlayerCharacter(World)) {
			Player->bDrawDebugLines = (Args.Num() > 0) ? Args[0].ToBool() : !Player->bDrawDebugLines;
			UE_LOG(LogTemp, Display, TEXT("FCheatDebugRaycast: Raycasts have been turned %s."), Player->bDrawDebugLines ? TEXT("on") : TEXT("off"));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("FCheatDebugRaycast: PlayerCharacter couldn't be found."));
		}
	}
), ECVF_Cheat);

static FAutoConsoleCommandWithWorldAndArgs FCheatGiveKey(
	TEXT("sl.givekey"),
	TEXT("Gives the player a 'hand' or 'eyes' to unlock doors. It gives both keys if not specified"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World) {
		if (ASlime_A* Player = CheatUtils::GetPlayerCharacter(World)) {
			UInventoryComponent* InventoryComponent = Player->FindComponentByClass<UInventoryComponent>();
			if(InventoryComponent) {
				bool bGiveHand = true;
				bool bGiveEyes = true;
				if (Args.Num() > 0) {
					bGiveHand = Args[0].Contains(FString(TEXT("hand")));
					bGiveEyes = Args[0].Contains(FString(TEXT("eye")));
				}
				if (bGiveHand) InventoryComponent->AddItem(TEXT("KeyHand"));
				if (bGiveEyes) InventoryComponent->AddItem(TEXT("KeyEyes"));
			}
		}
	})
);

static FAutoConsoleCommandWithWorldAndArgs FCheatSpawnPoint(
	TEXT("sl.spawnpoint"),
	TEXT("Select a specific spawn point [hangar/level0/garbagepit] to start the game from. If not specified starts at regular spawn"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World) {
		if (ASlime_A* Player = CheatUtils::GetPlayerCharacter(World)) {
			UQuestLogComponent* QuestLogComponent = Player->FindComponentByClass<UQuestLogComponent>();
			if (QuestLogComponent) {
				if (Args.Num() > 0) {
					if(Args[0].Contains(FString(TEXT("hangar"))))
						QuestLogComponent->SetQuestOnSpawnPoint(ESpawnPoint::Hangar);

					if (Args[0].Contains(FString(TEXT("level0"))))
						QuestLogComponent->SetQuestOnSpawnPoint(ESpawnPoint::Level0);

					if (Args[0].Contains(FString(TEXT("garbagepit"))))
						QuestLogComponent->SetQuestOnSpawnPoint(ESpawnPoint::GarbagePit);
				}
				else {
					QuestLogComponent->SetQuestOnSpawnPoint(ESpawnPoint::GarbagePit);
				}
			}
		}
	})
);

static FAutoConsoleCommandWithWorldAndArgs FCheatHeal(
	TEXT("sl.heal"),
	TEXT("Heals the player the % indicated. If not specified it'll heal to full"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World) {
		if (ASlime_A *Player = CheatUtils::GetPlayerCharacter(World)) {
			int AmountToHeal = (Args.Num() > 0) ? FCString::Atoi(*Args[0]) : 100;
			Player->LifeComponent->ReceiveHeal(AmountToHeal, Player);
			UE_LOG(LogTemp, Display, TEXT("Amount Healed = %d"), AmountToHeal);
		}
	})
);

static FAutoConsoleCommandWithWorldAndArgs FCheatImmortal(
	TEXT("sl.god"),
	TEXT("Toggles god mode so the player can't be hurt. Specify 'true' or 'false' to set a value"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World) {
		if (ASlime_A* Player = CheatUtils::GetPlayerCharacter(World)) {
			Player->bDebugImmortal = (Args.Num() > 0) ? Args[0].ToBool() : !Player->bDebugImmortal;
			if (Player->bDebugImmortal) { UE_LOG(LogTemp, Display, TEXT("bDebugImmortal = true")); }
			else                        { UE_LOG(LogTemp, Display, TEXT("bDebugImmortal = false")); }
		}
	})
);

static FAutoConsoleCommandWithWorldAndArgs FCheatAlwaysKill(
	TEXT("sl.alwayskill"),
	TEXT("Toggles the always kill bool to always kill soldiers using E. Specify 'true' or 'false' to set a value"),
	FConsoleCommandWithWorldAndArgsDelegate::CreateLambda([](const TArray<FString>& Args, UWorld* World) {
		if (ASlime_A* Player = CheatUtils::GetPlayerCharacter(World)) {
			Player->bDebugAlwaysKill = (Args.Num() > 0) ? Args[0].ToBool() : !Player->bDebugAlwaysKill;
			if (Player->bDebugAlwaysKill) { UE_LOG(LogTemp, Display, TEXT("bDebugAlwaysKill = true")); }
			else                          { UE_LOG(LogTemp, Display, TEXT("bDebugAlwaysKill = false")); }
		}
	})
);