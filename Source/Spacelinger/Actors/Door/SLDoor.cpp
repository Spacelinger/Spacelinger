// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "Actors/Door/SLDoor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/SceneComponent.h"
#include "Soldier/SLSoldier.h"
#include "Actors/DoorBlock.h"

ASLDoor::ASLDoor() {
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	DoorMesh   = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Mesh"));

	BoxTrigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Trigger"));

	DoorBlock = CreateDefaultSubobject<UChildActorComponent>(TEXT("Door Block"));

	DoorMesh  ->SetupAttachment(RootComponent);
	BoxTrigger->SetupAttachment(RootComponent);
	DoorBlock ->SetupAttachment(RootComponent);
}

void ASLDoor::BeginPlay() {
	Super::BeginPlay();

	InitialDoorLocation = DoorMesh->GetRelativeLocation();

	BoxTrigger->OnComponentBeginOverlap.AddDynamic(this, &ASLDoor::BoxTrigger_OnBeginOverlap);
	BoxTrigger->OnComponentEndOverlap  .AddDynamic(this, &ASLDoor::BoxTrigger_OnEndOverlap);

	ADoorBlock* DoorBlockActor = Cast<ADoorBlock>(DoorBlock->GetChildActor());
	if (DoorBlockActor)
		DoorBlockActor->BlockStatusChangeDelegate.AddDynamic(this, &ASLDoor::HandleDoorBlock);
}

void ASLDoor::BoxTrigger_OnBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult & SweepResult)
{
	if (!ShouldDoorOpen(OtherActor, OtherComp)) return;

	ActorsOnTrigger += 1;

	TryDoorOpen();

	/*Luis was here: this code was useful to call from elsewhere, so I moved it and called through TryDoorOpen - new method
	if (ActorsOnTrigger == 1) {
		GetWorldTimerManager().SetTimer(DoorTickHandle, this, &ASLDoor::DoorTickOpen, .001f, true);
		DoorOpenStarted();
	}*/
}

void ASLDoor::BoxTrigger_OnEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!ShouldDoorOpen(OtherActor, OtherComp)) return;

	ActorsOnTrigger <= 0 ? ActorsOnTrigger = 0 : ActorsOnTrigger -= 1;

	ensure(ActorsOnTrigger >= 0);

	TryDoorClose();

	/* Luis was here: this code was useful to call from elsewhere, so I moved it and called through TryDoorClose - new method
	if (ActorsOnTrigger == 0) {
		DoorMesh->SetVisibility(true);
		GetWorldTimerManager().SetTimer(DoorTickHandle, this, &ASLDoor::DoorTickClose, .001f, true);
		DoorCloseStarted();
	}*/
}

void ASLDoor::DoorTickOpen() {
	if (bIsDoorBlocked)
		return;

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

bool ASLDoor::ShouldDoorOpen(AActor* OtherActor, UPrimitiveComponent* OtherComponent) {
	bool Result = false;
	if (ReactToPlayer)  Result |= OtherComponent == GetPlayerRoot();
	if (ReactToSoldier) Result |= Cast<ASLSoldier>(OtherActor) != nullptr;

	return Result;
}

void ASLDoor::TryDoorOpen() {
	if (bIsDoorBlocked) return;

	if (ActorsOnTrigger >= 1) {
		GetWorldTimerManager().SetTimer(DoorTickHandle, this, &ASLDoor::DoorTickOpen, .001f, true);
		DoorOpenStarted();
	}
}

void ASLDoor::TryDoorClose() {
	if (bIsDoorBlocked) return;

	if (ActorsOnTrigger == 0) {
		DoorMesh->SetVisibility(true);
		GetWorldTimerManager().SetTimer(DoorTickHandle, this, &ASLDoor::DoorTickClose, .001f, true);
		DoorCloseStarted();
	}
}

void ASLDoor::HandleDoorBlock(bool bBlockStatus){

	bIsDoorBlocked = bBlockStatus;
	if (bIsDoorBlocked) {
		DoorBlocked();
	}
	else if (!bIsDoorBlocked) {
		TryDoorOpen();
		TryDoorClose();
		DoorUnblocked();
	}
}