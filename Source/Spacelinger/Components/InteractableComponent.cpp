// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Components/InteractableComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
UInteractableComponent::UInteractableComponent()
{
	SetIsReplicatedByDefault(true);
	
	//InteractPromptWidget = CreateDefaultSubobject<UWidgetComponent>(FName(TEXT("Interactable UI")));
	//InteractPromptWidget = NewObject<UWidgetComponent>(GetOwner(), UWidgetComponent::StaticClass(), FName(TEXT("Interactable UI")), RF_Transient);
}

void UInteractableComponent::OnRegister()
{
	Super::OnRegister();

	//InteractPromptWidget->SetupAttachment(GetOwner()->GetRootComponent());
	//InteractPromptWidget->RegisterComponent();
}

void UInteractableComponent::BeginPlay()
{
	Super::BeginPlay();

	InteractableWidgetComponents = GetOwner()->GetComponentsByTag(UPrimitiveComponent::StaticClass(), FName("Interact UI"));
	UE_LOG(LogActor, Warning, TEXT("Linked to %i components"), InteractableWidgetComponents.Num());
}

void UInteractableComponent::SetAsCandidate(AActor* ActorInteracting)
{
	UE_LOG(LogActor, Warning, TEXT("Actor %s can engage an interaction with %s"), *ActorInteracting->GetName(), *GetOwner()->GetName());
	for (UActorComponent* WidgetComponent : InteractableWidgetComponents)
	{
		Cast<UWidgetComponent>(WidgetComponent)->SetVisibility(true);
	}

	OnSetCandidateDelegate.Broadcast(ActorInteracting);
}

void UInteractableComponent::RemoveAsCandidate(AActor* ActorInteracting)
{
	UE_LOG(LogActor, Warning, TEXT("Actor %s can no longer engage an interaction with %s"), *ActorInteracting->GetName(), *GetOwner()->GetName());
	for (UActorComponent* WidgetComponent : InteractableWidgetComponents)
	{
		Cast<UWidgetComponent>(WidgetComponent)->SetVisibility(false);
	}
	OnRemoveCandidateDelegate.Broadcast(ActorInteracting);
}

void UInteractableComponent::Interact(AActor* ActorInteracting)
{
	//UE_LOG(LogActor, Warning, TEXT("%s reveived request interaction from %s"), *GetOwner()->GetName(), *ActorInteracting->GetName());
	OnInteractDelegate.Broadcast(ActorInteracting);
}