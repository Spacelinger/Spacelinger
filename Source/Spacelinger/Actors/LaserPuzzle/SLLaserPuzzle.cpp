// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "Actors/LaserPuzzle/SLLaserPuzzle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "SpiderWeb.h"

ASLLaserPuzzle::ASLLaserPuzzle() {
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MeshLeft  = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Left"));
	MeshRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Right"));
	BeamTop = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Beam Top"));
	BeamBot = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Beam Bot"));

	MeshLeft ->SetupAttachment(RootComponent);
	MeshRight->SetupAttachment(RootComponent);
	BeamTop->SetupAttachment(MeshLeft);
	BeamBot->SetupAttachment(MeshLeft);
}

void ASLLaserPuzzle::OnConstruction(const FTransform& Transform) { 
	MeshLeft ->SetRelativeLocation(FVector::ZeroVector);
	MeshLeft->SetRelativeRotation(FRotator::ZeroRotator);

	MeshRightLocation.X = 0.0f;
	MeshRightLocation.Z = 0.0f;
	if (MeshRightLocation.Y > 0.0f) { MeshRightLocation.Y = 0.0f; }
	MeshRight->SetRelativeLocation(MeshRightLocation);

	BeamLength = MeshRightLocation.Length() + OffsetMeshes;
	BeamTop->SetVectorParameter(FName(TEXT("BallLocation")), FVector(BeamLength, 0.0f, 0.0f));
	BeamTop->SetVectorParameter(FName(TEXT("Line1")),        FVector(BeamLength, 0.0f, 0.0f));
	BeamTop->SetVectorParameter(FName(TEXT("Line2")),        FVector(BeamLength, 0.0f, 0.0f));
	BeamTop->SetVectorParameter(FName(TEXT("Line3")),        FVector(BeamLength, 0.0f, 0.0f));
	BeamTop->SetFloatParameter(FName(TEXT("LittleParticles")), BeamLength/100.0f);

	BeamBot->SetVectorParameter(FName(TEXT("BallLocation")), FVector(BeamLength, 0.0f, 0.0f));
	BeamBot->SetVectorParameter(FName(TEXT("Line1")),        FVector(BeamLength, 0.0f, 0.0f));
	BeamBot->SetVectorParameter(FName(TEXT("Line2")),        FVector(BeamLength, 0.0f, 0.0f));
	BeamBot->SetVectorParameter(FName(TEXT("Line3")),        FVector(BeamLength, 0.0f, 0.0f));
	BeamBot->SetFloatParameter(FName(TEXT("LittleParticles")), BeamLength/100.0f);
}

void ASLLaserPuzzle::BeginPlay() {
	Super::BeginPlay();

	BeamBot->SetEmitterEnable(FName(TEXT("OrbDestination")), false);
}

void ASLLaserPuzzle::WebEndConnection(ASpiderWeb *Web) {
	// If both ends of the web are connected to the Actor, then we check if the connections are successful.
	if (LastAttachedWeb == Web) {
		FVector WebStart = Web->GetStartAttachLocation();
		FVector WebEnd   = Web->GetEndAttachLocation();
		FVector TopStart = BeamTop->GetComponentLocation();
		FVector TopEnd   = TopStart - FVector(.0f, BeamLength, .0f);
		FVector BotStart = BeamBot->GetComponentLocation();
		FVector BotEnd   = BotStart - FVector(.0f, BeamLength, .0f);

		// Calc distances
		float WebStartTopStart = FVector::Distance(WebStart, TopStart);
		float WebStartTopEnd   = FVector::Distance(WebStart, TopEnd);
		float WebEndTopStart   = FVector::Distance(WebEnd,   TopStart);
		float WebEndTopEnd     = FVector::Distance(WebEnd,   TopEnd);

		float WebStartBotStart = FVector::Distance(WebStart, BotStart);
		float WebStartBotEnd   = FVector::Distance(WebStart, BotEnd);
		float WebEndBotStart   = FVector::Distance(WebEnd,   BotStart);
		float WebEndBotEnd     = FVector::Distance(WebEnd,   BotEnd);

		// Check if there's a successful connection
		float ConnectedThreshold = 50.0f;
		if ((WebStartTopStart <= ConnectedThreshold && WebEndTopEnd   <= ConnectedThreshold)
		 || (WebStartTopEnd   <= ConnectedThreshold && WebEndTopStart <= ConnectedThreshold)
		) {
			UE_LOG(LogTemp, Display, TEXT("SERGI: Top beam activated!"));
			BeamTop->SetEmitterEnable(FName(TEXT("OrbDestination")), true);
			BeamTop->Activate(true); // Reset particles
		}
		else
		if ((WebStartBotStart <= ConnectedThreshold && WebEndBotEnd   <= ConnectedThreshold)
		 || (WebStartBotEnd   <= ConnectedThreshold && WebEndBotStart <= ConnectedThreshold)
		) {
			UE_LOG(LogTemp, Display, TEXT("SERGI: Bottom beam activated!"));
			BeamBot->SetEmitterEnable(FName(TEXT("OrbDestination")), true);
			BeamBot->Activate(true); // Reset particles
		}
	}
}
