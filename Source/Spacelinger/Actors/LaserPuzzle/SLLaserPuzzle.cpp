// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "Actors/LaserPuzzle/SLLaserPuzzle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

ASLLaserPuzzle::ASLLaserPuzzle() {
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ASLLaserPuzzle::BeginPlay() {
	Super::BeginPlay();
}

/*void ASLLaserPuzzle::WebEndConnection(ASpiderWeb *Web) {
	UE_LOG(LogTemp, Display, TEXT("ASLLaserPuzzle::WebEndConnection default implementation."));
}*/
