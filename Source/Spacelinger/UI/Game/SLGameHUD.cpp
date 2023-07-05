// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "UI/Game/SLGameHUD.h"
#include "Blueprint/UserWidget.h"

ASLGameHUD::ASLGameHUD() { }

void ASLGameHUD::BeginPlay() {
	Super::BeginPlay();

	PlayerInterface    = InstantiateWidget(PlayerInterfaceClass);
	if (PlayerInterface) PlayerInterface->AddToViewport();
}

UUserWidget* ASLGameHUD::InstantiateWidget(UClass *InterfaceClass) {
	if (!InterfaceClass) {
		UE_LOG(LogTemp, Error, TEXT("UI Interface Widget Class not set in player character!"));
		return nullptr;
	}

	UUserWidget* NewInterface = CreateWidget(GetOwningPlayerController(), InterfaceClass);
	if (!NewInterface) {
		UE_LOG(LogTemp, Error, TEXT("UI Interface Widget couldn't be created!"));
		return nullptr;
	}
	return NewInterface;
}
