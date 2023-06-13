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

	// Retrieve all components from my owner with the Tag "InteractVolume" of class PrimitiveComponent
	// Which is the one supporting the Begin/End overlap methods
	TArray<UActorComponent*> Comps = GetOwner()->GetComponentsByTag(UPrimitiveComponent::StaticClass(), FName("Interact Volume"));

	UE_LOG(LogTemp, Warning, TEXT(" ############## Found %d interact componenets in actor"), Comps.Num());

	// Foreach one, bind myself to the Begin/EndOverlap
	for (UActorComponent* Comp : Comps)
	{
		// Confirm the component is of the type PrimComponent
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

	if (CurrentInteractableActor)
	{
		
		UInteractableComponent* ActorInteractComp = CurrentInteractableActor->FindComponentByClass<UInteractableComponent>();
		if (ActorInteractComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("Interacting with %s"), *CurrentInteractableActor->GetName());
			ActorInteractComp->Interact(GetOwner());
		}
	}
}

void UInteractingComponent::OnBoundsBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	//if (IInteractInterface* Candidate = Cast<IInteractInterface>(OtherActor))
	if (AActor* Candidate = OtherActor->FindComponentByClass<UInteractableComponent>()->GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("Begin overlap"));
		CurrentInteractableActor = OtherActor;
	}
}

void UInteractingComponent::OnBoundsEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	if (IInteractInterface* Candidate = Cast<IInteractInterface>(OtherActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("End overlap"));
		CurrentInteractableActor = nullptr;
	}

}
