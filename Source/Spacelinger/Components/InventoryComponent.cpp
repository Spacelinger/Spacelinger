// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "InventoryComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
}

void UInventoryComponent::AddItem(FName NewItem)
{
	ItemNames.Add(NewItem);
	UE_LOG(LogTemp, Warning, TEXT("Inventory Added Item %s"), *NewItem.ToString());
}

void UInventoryComponent::FindItem(FName Item)
{

}

void UInventoryComponent::TryToRemoveItem(FName ItemToRemove)
{

}


/*
// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
*/