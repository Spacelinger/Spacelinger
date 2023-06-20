// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Actors/DoorBlock.h"
#include "Components/InteractableComponent.h"
#include "Components/BoxComponent.h"
//#include <AbilitySystemComponent.h>
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/WidgetComponent.h"

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
	
	InteractPromptWidget = CreateDefaultSubobject<UWidgetComponent>(FName(TEXT("Interact UI")));
	InteractPromptWidget->SetupAttachment(ColliderComponent);
	InteractPromptWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractPromptWidget->ComponentTags.Add(FName(TEXT("Interact UI")));
	
	InteractingProgressBarWidget = CreateDefaultSubobject<UWidgetComponent>(FName(TEXT("Interact Progress Bar")));
	InteractingProgressBarWidget->SetupAttachment(ColliderComponent);
	InteractingProgressBarWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractingProgressBarWidget->ComponentTags.Add(FName(TEXT("Interact Progress Bar")));

}

// Called when the game starts or when spawned
void ADoorBlock::BeginPlay()
{
	Super::BeginPlay();
	
	Reset();

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
	FGameplayEventData Payload;
	Payload.Instigator = InteractingActor;
	Payload.Target = this;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InteractingActor, FGameplayTag::RequestGameplayTag(TEXT("Ability.DoorBlock.Started")), Payload);

	//FinalStaticMeshComponent->SetVisibility(true);
	UE_LOG(LogActor, Warning, TEXT("Actor %s was interacted."), *GetName());
	
	ColliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewStaticMeshComponent->SetVisibility(false);
}

void ADoorBlock::DoorBlockSuccess()
{
	UE_LOG(LogActor, Warning, TEXT("Success! To handle."));
	FinalStaticMeshComponent->SetVisibility(true);
	FinalStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ADoorBlock::DoorBlockFail()
{
	UE_LOG(LogActor, Warning, TEXT("Failed! To handle."));
	Reset();	//Remove?
}

void ADoorBlock::Reset()
{
	PreviewStaticMeshComponent->SetVisibility(false);
	PreviewStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FinalStaticMeshComponent->SetVisibility(false);
	FinalStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractPromptWidget->SetVisibility(false);
	InteractingProgressBarWidget->SetVisibility(false);

	ColliderComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}