// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Actors/DoorBlock.h"
#include "Components/InteractableComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
ADoorBlock::ADoorBlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	//Consider remove

	ColliderComp = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("Interaction Bounds")));
	ColliderComp->SetupAttachment(RootComponent);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Mesh")));
	StaticMeshComp->SetupAttachment(ColliderComp);
	
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(FName(TEXT("Interactable Component")));
}

// Called when the game starts or when spawned
void ADoorBlock::BeginPlay()
{
	Super::BeginPlay();
	
	StaticMeshComp->SetVisibility(false);

	InteractableComponent->OnSetCandidateDelegate.AddDynamic(this, &ADoorBlock::SetAsCandidate);
	InteractableComponent->OnRemoveCandidateDelegate.AddDynamic(this, &ADoorBlock::RemoveAsCandidate);
	InteractableComponent->OnInteractDelegate.AddDynamic(this, &ADoorBlock::Interact);

	if (!InteractPreviewMaterial)
		UE_LOG(LogTemp, Warning, TEXT("Missing Interact Preview Material in actor %s"), *GetName());

	OriginalMaterial = StaticMeshComp->GetMaterial(0);
}

void ADoorBlock::SetAsCandidate(AActor* InteractingActor)
{
	if(InteractPreviewMaterial)
		StaticMeshComp->SetMaterial(0, InteractPreviewMaterial);

	StaticMeshComp->SetVisibility(true);
}

void ADoorBlock::RemoveAsCandidate(AActor* InteractingActor)
{
	StaticMeshComp->SetMaterial(0, OriginalMaterial);
	StaticMeshComp->SetVisibility(false);
}

void ADoorBlock::Interact(AActor* InteractingActor)
{
	ColliderComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComp->SetMaterial(0, OriginalMaterial);
	StaticMeshComp->SetVisibility(true);
	UE_LOG(LogActor, Warning, TEXT("Actor %s was interacted."), *GetName());
}

// Called every frame
void ADoorBlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	//Consider delete
}

