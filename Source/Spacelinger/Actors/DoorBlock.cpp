// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Actors/DoorBlock.h"
#include "Components/InteractableComponent.h"
#include "Components/BoxComponent.h"
//#include <AbilitySystemComponent.h>
#include "AbilitySystemBlueprintLibrary.h"

// Sets default values
ADoorBlock::ADoorBlock()
{
	ColliderComponent = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("Interaction Bounds")));
	ColliderComponent->SetupAttachment(RootComponent);

	PreviewStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Preview Interact Mesh")));
	PreviewStaticMeshComponent->SetupAttachment(ColliderComponent);

	FinalStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Final Interact Mesh")));
	FinalStaticMeshComponent->SetupAttachment(ColliderComponent);
	
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(FName(TEXT("Interactable Component")));
}

// Called when the game starts or when spawned
void ADoorBlock::BeginPlay()
{
	Super::BeginPlay();
	
	PreviewStaticMeshComponent->SetVisibility(false);
	PreviewStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FinalStaticMeshComponent->SetVisibility(false);
	FinalStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InteractableComponent->OnSetCandidateDelegate.AddDynamic(this, &ADoorBlock::SetAsCandidate);
	InteractableComponent->OnRemoveCandidateDelegate.AddDynamic(this, &ADoorBlock::RemoveAsCandidate);
	InteractableComponent->OnInteractDelegate.AddDynamic(this, &ADoorBlock::Interact);

	if (!InteractPreviewMaterial)
		UE_LOG(LogTemp, Warning, TEXT("Missing Interact Preview Material in actor %s"), *GetName());

}

void ADoorBlock::SetAsCandidate(AActor* InteractingActor)
{
	PreviewStaticMeshComponent->SetVisibility(true);
}

void ADoorBlock::RemoveAsCandidate(AActor* InteractingActor)
{
	PreviewStaticMeshComponent->SetVisibility(false);
}

void ADoorBlock::Interact(AActor* InteractingActor)
{
	ColliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewStaticMeshComponent->SetVisibility(false);

	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InteractingActor, FGameplayTag::RequestGameplayTag(TEXT("Ability.DoorBlock.Started")), Payload);

	//FinalStaticMeshComponent->SetVisibility(true);
	//UE_LOG(LogActor, Warning, TEXT("Actor %s was interacted."), *GetName());

}