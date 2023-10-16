// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Components/InteractableComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
UInteractableComponent::UInteractableComponent()
{
	SetIsReplicatedByDefault(true);
}

void UInteractableComponent::OnRegister()
{
	Super::OnRegister();
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	InteractableWidgetComponents = GetOwner()->GetComponentsByTag(UPrimitiveComponent::StaticClass(), FName("Interact UI"));

	for (UActorComponent* WidgetComponent : InteractableWidgetComponents)
	{
		Cast<UWidgetComponent>(WidgetComponent)->SetVisibility(false);
	}

	//UE_LOG(LogActor, Warning, TEXT("Linked to %i  UI components"), InteractableWidgetComponents.Num());
}

void UInteractableComponent::SetAsCandidate(AActor* ActorInteracting)
{
	//UE_LOG(LogActor, Warning, TEXT("Actor %s can engage an interaction with %s"), *ActorInteracting->GetName(), *GetOwner()->GetName());
	for (UActorComponent* WidgetComponent : InteractableWidgetComponents)
	{
		Cast<UWidgetComponent>(WidgetComponent)->SetVisibility(true);
	}

	OnSetCandidateDelegate.Broadcast(ActorInteracting);
}

void UInteractableComponent::RemoveAsCandidate(AActor* ActorInteracting)
{
	//UE_LOG(LogActor, Warning, TEXT("Actor %s can no longer engage an interaction with %s"), *ActorInteracting->GetName(), *GetOwner()->GetName());
	for (UActorComponent* WidgetComponent : InteractableWidgetComponents)
	{
		Cast<UWidgetComponent>(WidgetComponent)->SetVisibility(false);
	}
	OnRemoveCandidateDelegate.Broadcast(ActorInteracting);
}

void UInteractableComponent::Interact(AActor* ActorInteracting)
{
	//UE_LOG(LogActor, Warning, TEXT("%s reveived request interaction from %s"), *GetOwner()->GetName(), *ActorInteracting->GetName());
	if(bCanInteract)
		OnInteractDelegate.Broadcast(ActorInteracting);
}