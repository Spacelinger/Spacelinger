// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "UI/Game/SLGameHUD.h"
#include "Blueprint/UserWidget.h"

ASLGameHUD::ASLGameHUD() { }

void ASLGameHUD::BeginPlay() {
	Super::BeginPlay();

	if (PlayerInterfaceClass) {
		PlayerInterface = CreateWidget(GetOwningPlayerController(), PlayerInterfaceClass);
		if (PlayerInterface) {
			PlayerInterface->AddToViewport();
		}
	}
}
