// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Components/InteractableComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
UInteractableComponent::UInteractableComponent()
{
	SetIsReplicatedByDefault(true);
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UInteractableComponent::SetAsCandidate(AActor* ActorInteracting)
{
	//UE_LOG(LogActor, Warning, TEXT("Actor %s can engage an interaction with %s"), *ActorInteracting->GetName(), *GetOwner()->GetName());
	OnSetCandidateDelegate.Broadcast(ActorInteracting);
}

void UInteractableComponent::RemoveAsCandidate(AActor* ActorInteracting)
{
	//UE_LOG(LogActor, Warning, TEXT("Actor %s can no longer engage an interaction with %s"), *ActorInteracting->GetName(), *GetOwner()->GetName());
	OnRemoveCandidateDelegate.Broadcast(ActorInteracting);
}

void UInteractableComponent::Interact(AActor* ActorInteracting)
{
	//UE_LOG(LogActor, Warning, TEXT("%s reveived request interaction from %s"), *GetOwner()->GetName(), *ActorInteracting->GetName());
	OnInteractDelegate.Broadcast(ActorInteracting);
}