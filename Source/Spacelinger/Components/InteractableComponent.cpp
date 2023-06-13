// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Components/InteractableComponent.h"

// Sets default values
UInteractableComponent::UInteractableComponent()
{
	SetIsReplicatedByDefault(true);
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractableComponent::StartCanInteract(AActor* ActorInteracting)
{

}

void UInteractableComponent::EndCanInteract(AActor* ActorInteracting)
{

}

void UInteractableComponent::Interact(AActor* ActorInteracting)
{
	UE_LOG(LogTemp, Warning, TEXT("Request interaction from %s"), *ActorInteracting->GetName());
}

void UInteractableComponent::SetAsCandidate(bool IsCandidate)
{

}