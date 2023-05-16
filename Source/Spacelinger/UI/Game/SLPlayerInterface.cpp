// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "UI/Game/SLPlayerInterface.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"

float USLPlayerInterface::HealthBar_Percent() const {
	AActor *PlayerPawn = GetOwningPlayerPawn();
	if (!PlayerPawn) { return 0.0f; }

	// SLLifeComponent LifeComponent = PlayerPawn->FindComponentByClass<SLLifeComponent>();
	// if (!LifeComponent) { return 0.0f; }
	// return LifeComponent.CurrentHealth / LifeComponent.MaxHealth;
	return 0.8f; // TODO
}
