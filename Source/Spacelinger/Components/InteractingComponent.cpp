// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Components/InteractingComponent.h"
#include "Components/InteractableComponent.h"

// Sets default values for this component's properties
UInteractingComponent::UInteractingComponent()
{
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void UInteractingComponent::BeginPlay()
{
	Super::BeginPlay();

	// Retrieve all components from my owner with the Tag "Interact Volume" of class PrimitiveComponent
	// This tag has to be set from the actor BP
	TArray<UActorComponent*> Comps = GetOwner()->GetComponentsByTag(UPrimitiveComponent::StaticClass(), FName("Interact Volume"));

	// In case there are multiple, for each one, bind myself to the Begin/End Overlap
	for (UActorComponent* Comp : Comps)
	{
		if (UPrimitiveComponent* PrimComp = Cast< UPrimitiveComponent>(Comp))
		{
			PrimComp->OnComponentBeginOverlap.AddDynamic(this, &UInteractingComponent::OnBoundsBeginOverlap);
			PrimComp->OnComponentEndOverlap.AddDynamic(this, &UInteractingComponent::OnBoundsEndOverlap);
		}
	}
}

bool UInteractingComponent::TryToInteract_Validate()
{
	return true;
}

void UInteractingComponent::TryToInteract_Implementation()
{
	if (CurrentInteractable)
	{
		UE_LOG(LogTemp, Warning, TEXT("Interacting with %s from %s"), *CurrentInteractable->GetOwner()->GetName(), *GetOwner()->GetName());
		CurrentInteractable->Interact(GetOwner());
	}
}

void UInteractingComponent::OnBoundsBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	if (UInteractableComponent* Candidate = OtherActor->FindComponentByClass<UInteractableComponent>())
	{

		CurrentInteractables.AddUnique(Candidate);
		CurrentInteractables.StableSort([](const UInteractableComponent& A, const UInteractableComponent& B) {
			return A.GetInteractPriority() < B.GetInteractPriority();
			});
		Refresh();
	}
}

void UInteractingComponent::OnBoundsEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	if (OtherActor->FindComponentByClass<UInteractableComponent>())
	{
		CurrentInteractables.Remove(OtherActor->FindComponentByClass<UInteractableComponent>());
		Refresh();
	}
}

void UInteractingComponent::Refresh()
{
	// Get the new current
	UInteractableComponent* NewCurrent = CurrentInteractables.Num() ? CurrentInteractables[0] : nullptr;

	if (NewCurrent == CurrentInteractable)
	{
		return;
	}

	// Tell the old one that he is no longer in the CanInteract state
	if (CurrentInteractable)
	{
		CurrentInteractable->RemoveAsCandidate(GetOwner());
		CurrentInteractable = nullptr;
	}

	// Tell the new actor that we are starting to interact with it
	if (NewCurrent)
	{
		CurrentInteractable = NewCurrent;
		CurrentInteractable->SetAsCandidate(GetOwner());
	}
}