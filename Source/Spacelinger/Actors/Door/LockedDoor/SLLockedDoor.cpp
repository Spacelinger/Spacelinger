// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "SLLockedDoor.h"
#include "Components/InteractableComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/InventoryComponent.h"

ASLLockedDoor::ASLLockedDoor()
{
	ColliderComponent = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("Interaction Bounds")));
	ColliderComponent->SetupAttachment(RootComponent);

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(FName(TEXT("Interactable Component")));

	InteractPromptWidget = CreateDefaultSubobject<UWidgetComponent>(FName(TEXT("Interact UI")));
	InteractPromptWidget->SetupAttachment(RootComponent);
	InteractPromptWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractPromptWidget->ComponentTags.Add(FName(TEXT("Interact UI")));

	ReactToPlayer = false;	// Sergi <3 // Disables player interaction form the parent door class so it won't open
}

void ASLLockedDoor::OnConstruction(const FTransform& Transform) {
	Reset();
	InteractPromptWidget->SetVisibility(LockType == DoorLockType::KEY);
	UpdateAssociatedVisualElements(false);
}

void ASLLockedDoor::BeginPlay()
{
	Super::BeginPlay();

	Reset();

	BoxTrigger->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InteractableComponent->OnSetCandidateDelegate.AddDynamic(this, &ASLLockedDoor::SetAsCandidate);
	InteractableComponent->OnRemoveCandidateDelegate.AddDynamic(this, &ASLLockedDoor::RemoveAsCandidate);
	InteractableComponent->OnInteractDelegate.AddDynamic(this, &ASLLockedDoor::Interact);
}

void ASLLockedDoor::SetAsCandidate(AActor* InteractingActor)
{
	InteractPromptWidget->SetVisibility(true);
}

void ASLLockedDoor::RemoveAsCandidate(AActor* InteractingActor)
{
	InteractPromptWidget->SetVisibility(false);
}

void ASLLockedDoor::Interact(AActor* InteractingActor)
{
	InteractableComponent->bCanInteract = false;

	//Check if InteractingActor has InventoryComponent
	UInventoryComponent* ActorInventory = InteractingActor->FindComponentByClass<UInventoryComponent>();
	if (!ActorInventory)
	{
		OpenDoorFail();
		return;
	}

	// Check if Actor's inventory has specific Item
	int ItemInventoryIndex = ActorInventory->ItemNames.Find(ItemToUnblock);
	if (ItemInventoryIndex == INDEX_NONE)
	{
		OpenDoorFail();
		return;
	}

	// Remove Item
	ActorInventory->ItemNames.RemoveAt(ItemInventoryIndex);
	
	// OpenDoor
	OpenDoor();
	UpdateAssociatedVisualElements(true);
}

void ASLLockedDoor::OpenDoor()
{
	ColliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// May want to call the DoorTickOpen at SLDoor --> NO! "tick" function works based on collition delegate
	
	ReactToPlayer = true;
	BoxTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//UE_LOG(LogActor, Warning, TEXT("Success!"));

	SetKeyVisibility(); // Remove Key UI
}


void ASLLockedDoor::OpenDoorFail()
{
	//UE_LOG(LogActor, Warning, TEXT("Failed!"));
	UWorld* World = GetWorld();
	FTimerHandle TimerHandle;
	// Use a dummy timer handle as we don't need to store it for later but we don't need to look for something to clear
	// Hard coded a 1s cooldown on reset after a failed attempt to block a door. May want to turn into parameter
	World->GetTimerManager().SetTimer(TimerHandle, this, &ASLLockedDoor::Reset, 1.0f, false);
	Reset();
}

void ASLLockedDoor::Reset() 
{
	InteractableComponent->bCanInteract = (LockType == DoorLockType::KEY);

	InteractPromptWidget->SetVisibility(false);

	ColliderComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ASLLockedDoor::UpdateAssociatedVisualElements(bool State) {
	for (ATogglableVisualElement *Element : VisualElements) {
		if (Element) Element->SetState(State);
	}
}