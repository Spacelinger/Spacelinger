#include "Soldier/SLSoldierPath.h"
#include "Soldier/SLSoldier.h"

ASLSoldierPath::ASLSoldierPath() {
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ASLSoldierPath::OnConstruction(const FTransform& Transform) {
	SetActorRotation(FRotator::ZeroRotator);
}


void ASLSoldierPath::BeginPlay() {
	Super::BeginPlay();

}
