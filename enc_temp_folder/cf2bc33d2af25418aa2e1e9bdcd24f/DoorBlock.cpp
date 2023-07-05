// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Actors/DoorBlock.h"
#include "Components/InteractableComponent.h"
#include "Components/BoxComponent.h"
//#include <AbilitySystemComponent.h>
#include "AbilitySystemBlueprintLibrary.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetMaterialLibrary.h"

// Sets default values
ADoorBlock::ADoorBlock()
{
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName(TEXT("Root"))));

	PreviewStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Preview Interact Mesh")));
	PreviewStaticMeshComponent->SetupAttachment(RootComponent);

	FinalStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Final Interact Mesh")));
	FinalStaticMeshComponent->SetupAttachment(RootComponent);
	FinalStaticMeshComponent->SetVisibility(false);
	
	ColliderComponent = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("Interaction Bounds")));
	ColliderComponent->SetupAttachment(RootComponent);
	
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(FName(TEXT("Interactable Component")));
	
	InteractPromptWidget = CreateDefaultSubobject<UWidgetComponent>(FName(TEXT("Interact UI")));
	InteractPromptWidget->SetupAttachment(RootComponent);
	InteractPromptWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractPromptWidget->ComponentTags.Add(FName(TEXT("Interact UI")));
}

// Called when the game starts or when spawned
void ADoorBlock::BeginPlay()
{
	Super::BeginPlay();
	
	FinalStaticMeshComponent->SetRelativeTransform(PreviewStaticMeshComponent->GetRelativeTransform());
	// Set the Final Static mesh to the same transform as the preview one, which can be set and modified in the editor.

	Reset();

	InteractableComponent->OnSetCandidateDelegate.AddDynamic(this, &ADoorBlock::SetAsCandidate);
	InteractableComponent->OnRemoveCandidateDelegate.AddDynamic(this, &ADoorBlock::RemoveAsCandidate);
	InteractableComponent->OnInteractDelegate.AddDynamic(this, &ADoorBlock::Interact);

	UMaterialInterface* ProgressMaterial = FinalStaticMeshComponent->GetMaterial(0);
	if (ProgressMaterial)
	{
		DynamicChannelingProgressMaterial = UKismetMaterialLibrary::CreateDynamicMaterialInstance(this, ProgressMaterial);
		FinalStaticMeshComponent->SetMaterial(0, DynamicChannelingProgressMaterial);
		DynamicChannelingProgressMaterial->SetScalarParameterValue("DissolveAmount", 0.0f);
	} 
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Missing Progress Material for the final mesh in actor %s"), *GetName());
	}
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
	InteractableComponent->bCanInteract = false;

	FGameplayEventData Payload;
	Payload.Instigator = InteractingActor;
	Payload.Target = this;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(InteractingActor, FGameplayTag::RequestGameplayTag(TEXT("Ability.DoorBlock.Started")), Payload);
	//UE_LOG(LogActor, Warning, TEXT("Actor %s was interacted."), *GetName());
}

void ADoorBlock::BeginDoorBlock()
{
	ColliderComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PreviewStaticMeshComponent->SetVisibility(false);
	FinalStaticMeshComponent->SetVisibility(true);
}

void ADoorBlock::DoorBlockSuccess()
{
	//UE_LOG(LogActor, Warning, TEXT("Success!"));

	FinalStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);	// BUG: This collision affects the query to try to interact, so prompts to interact again
																						// Currently using InteractableComponent->bCanInteract flag to disable from the actor the ability to be interactable

	// Dummy timer to handle reset
	UWorld* World = GetWorld();
	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimer(TimerHandle, this, &ADoorBlock::Reset, TimeToUnblock, false);
}

void ADoorBlock::DoorBlockFail()
{
	//UE_LOG(LogActor, Warning, TEXT("Failed!"));

	Reset();
}

void ADoorBlock::Reset()
{
	InteractableComponent->bCanInteract = true;

	PreviewStaticMeshComponent->SetVisibility(false);
	PreviewStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FinalStaticMeshComponent->SetVisibility(false);
	FinalStaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractPromptWidget->SetVisibility(false);

	ColliderComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ADoorBlock::UpdateDoorBlockProgress(float NormalizedProgress)
{
	DynamicChannelingProgressMaterial->SetScalarParameterValue("DissolveAmount", NormalizedProgress);
}
