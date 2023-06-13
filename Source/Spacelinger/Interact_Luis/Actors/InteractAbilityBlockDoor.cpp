// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Interact_Luis/Actors/InteractAbilityBlockDoor.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "SpacelingerCharacter.h"

// Sets default values
AInteractAbilityBlockDoor::AInteractAbilityBlockDoor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(FName(TEXT("Root")));

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("Mesh")));
	StaticMeshComp->SetupAttachment(RootComponent);

	ColliderComp = CreateDefaultSubobject<UBoxComponent>(FName(TEXT("Interaction Bounds Collider")));
	ColliderComp->SetupAttachment(RootComponent);

	InteractPromptFront = CreateDefaultSubobject<UWidgetComponent>(FName(TEXT("Interact UI Front")));
	InteractPromptFront->SetupAttachment(RootComponent);

	InteractPromptBack = CreateDefaultSubobject<UWidgetComponent>(FName(TEXT("Interact UI Back")));
	InteractPromptBack->SetupAttachment(RootComponent);
	
}

void AInteractAbilityBlockDoor::BeginPlay()
{
	Super::BeginPlay();

	// Collision presets had to be set in BeginPlay instead of the constructor, otherwise wont work
	ColliderComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ColliderComp->OnComponentBeginOverlap.AddDynamic(this, &AInteractAbilityBlockDoor::OnBoundsBeginOverlap);
	ColliderComp->OnComponentEndOverlap.AddDynamic(this, &AInteractAbilityBlockDoor::OnBoundsEndOverlap);

	bIsInteracting = false;	// Control bool to keep track if the component can interact or is already blocked

	EnableBlockComponent(false);	// Disable the block mesh and collisions
	
	SetUIVisibility(false);			// Disable the UI Text
}

void AInteractAbilityBlockDoor::Interact(AActor* ActorInteracting) 
{
	if (bIsInteracting)		// If already blocking, do nothing
		return;

	EnableBlockComponent(true);	// Activate visuals and blocking by collision

	SetUIVisibility(false);		// Disable the UI Text
	
	bIsInteracting = true;		// Set the control bool to true
	
	// Timer to automatically disable block
	GetWorld()->GetTimerManager().SetTimer(InteractTimerHandle, this, &AInteractAbilityBlockDoor::OnTimerEnded, BlockingTime, false);
}

void AInteractAbilityBlockDoor::OnTimerEnded()
{
	// Disable visuals and blocking mesh, and set the control bool to false
	EnableBlockComponent(false);
	bIsInteracting = false;

	// Check if player is inside collider to show UI Text again
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);
	for (AActor* Actor : OverlappingActors) {
		if (Cast<ASpacelingerCharacter>(Actor))
			SetUIVisibility(true);
	}
}

void  AInteractAbilityBlockDoor::EnableBlockComponent(bool BlockState)
{
	StaticMeshComp->SetVisibility(BlockState);
	StaticMeshComp->SetComponentTickEnabled(BlockState);

	auto CollisionType = BlockState ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision;
	StaticMeshComp->SetCollisionEnabled(CollisionType);
}

void AInteractAbilityBlockDoor::OnBoundsBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsInteracting) // If block is active, UI shouldn't show up
		return;

	ASpacelingerCharacter* Player = Cast<ASpacelingerCharacter>(OtherActor);
	if(Player)
		SetUIVisibility(true);
}

void AInteractAbilityBlockDoor::OnBoundsEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int OtherBodyIndex)
{
	ASpacelingerCharacter* Player = Cast<ASpacelingerCharacter>(OtherActor);
	if (Player)
		SetUIVisibility(false);
}


void AInteractAbilityBlockDoor::SetUIVisibility(bool IsVisible) 
{
	InteractPromptFront->SetVisibility(IsVisible);
	InteractPromptBack->SetVisibility(IsVisible);
}
