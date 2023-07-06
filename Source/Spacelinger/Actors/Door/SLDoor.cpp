// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "Actors/Door/SLDoor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/SceneComponent.h"

ASLDoor::ASLDoor() {
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	DoorMesh   = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));

	BoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Trigger"));

	DoorMesh  ->SetupAttachment(RootComponent);
	BoxTrigger->SetupAttachment(RootComponent);
}

void ASLDoor::BeginPlay() {
	Super::BeginPlay();

	InitialDoorLocation = DoorMesh->GetRelativeLocation();

	if (ReactToPlayer) {
		BoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &ASLDoor::BoxTrigger_OnBeginOverlap);
		BoxTrigger->OnComponentEndOverlap  .AddDynamic(this, &ASLDoor::BoxTrigger_OnEndOverlap);
	}
}

void ASLDoor::BoxTrigger_OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult & SweepResult)
{
	if (OtherComp != GetPlayerRoot()) return;

	ActorsOnTrigger += 1;
	if (ActorsOnTrigger == 1) {
		GetWorldTimerManager().SetTimer(DoorTickHandle, this, &ASLDoor::DoorTickOpen, .001f, true);
	}
}

void ASLDoor::BoxTrigger_OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (OtherComp != GetPlayerRoot()) return;

	ActorsOnTrigger -= 1;
	ensure(ActorsOnTrigger >= 0);
	if (ActorsOnTrigger == 0) {
		DoorMesh->SetVisibility(true);
		GetWorldTimerManager().SetTimer(DoorTickHandle, this, &ASLDoor::DoorTickClose, .001f, true);
	}
}

void ASLDoor::DoorTickOpen() {
	CurrentDoorOffset += GetWorld()->DeltaTimeSeconds * DoorSpeed;
	if (CurrentDoorOffset >= DoorOffset) {
		CurrentDoorOffset = DoorOffset;
		DoorMesh->SetVisibility(false);
		GetWorldTimerManager().ClearTimer(DoorTickHandle);
	}
	UpdateDoorLocation();
}

void ASLDoor::DoorTickClose() {
	CurrentDoorOffset -= GetWorld()->DeltaTimeSeconds * DoorSpeed;
	if (CurrentDoorOffset <= 0) {
		CurrentDoorOffset = 0;
		GetWorldTimerManager().ClearTimer(DoorTickHandle);
	}
	UpdateDoorLocation();
}

void ASLDoor::UpdateDoorLocation() {
	FVector NewLocation = InitialDoorLocation;
	NewLocation.Z += CurrentDoorOffset;
	DoorMesh->SetRelativeLocation(NewLocation);
}

USceneComponent* ASLDoor::GetPlayerRoot() {
	UWorld *W = GetWorld();
	if (!W) return nullptr;

	APlayerController* PC = W->GetFirstPlayerController();
	if (!PC) return nullptr;

	APawn *PlayerPawn = PC->GetPawnOrSpectator();
	if (!PlayerPawn) return nullptr;

	return PlayerPawn->GetRootComponent();
}