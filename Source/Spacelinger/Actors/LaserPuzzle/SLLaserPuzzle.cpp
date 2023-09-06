// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "Actors/LaserPuzzle/SLLaserPuzzle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Spider/SpiderWeb.h"
#include "Actors/Door/LockedDoor/SLLockedDoor.h"
#include "Components/BoxComponent.h"
#include "Engine/TriggerSphere.h"

ASLLaserPuzzle::ASLLaserPuzzle() {
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	MeshLeft  = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Left"));
	MeshRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh Right"));
	BeamTop = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Beam Top"));
	BeamBot = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Beam Bot"));
	
	TriggerLeftTop  = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Left Top"));
	TriggerRightTop = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Right Top"));
	TriggerLeftBot  = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Left Bot"));
	TriggerRightBot = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Right Bot"));

	MeshLeft ->SetupAttachment(RootComponent);
	MeshRight->SetupAttachment(RootComponent);
	BeamTop->SetupAttachment(MeshLeft);
	BeamBot->SetupAttachment(MeshLeft);
	TriggerLeftTop ->SetupAttachment(MeshLeft);
	TriggerRightTop->SetupAttachment(MeshRight);
	TriggerLeftBot ->SetupAttachment(MeshLeft);
	TriggerRightBot->SetupAttachment(MeshRight);
}

void ASLLaserPuzzle::OnConstruction(const FTransform& Transform) {
	MeshLeft->SetRelativeLocation(FVector::ZeroVector);
	MeshLeft->SetRelativeRotation(FRotator::ZeroRotator);

	MeshRightLocation.X = 0.0f;
	MeshRightLocation.Z = 0.0f;
	if (MeshRightLocation.Y > 0.0f) { MeshRightLocation.Y = 0.0f; }
	MeshRight->SetRelativeLocation(MeshRightLocation);

	// Set the new state for the visual elements
	UpdateAssociatedVisualElements();

	BeamLength = MeshRightLocation.Length() + OffsetMeshes;
	SetBeamVisuals(BeamTop, LengthParticleParameters, BeamLength, false);
	SetBeamVisuals(BeamBot, LengthParticleParameters, BeamLength, false);

}

void ASLLaserPuzzle::BeginPlay() {
	Super::BeginPlay();

	SetBeamVisuals(BeamTop, bActiveBeamTop ? SolvedParticleParameters : UnsolvedParticleParameters, 1.0f, false);
	SetBeamVisuals(BeamBot, bActiveBeamBot ? SolvedParticleParameters : UnsolvedParticleParameters, 1.0f, false);
}

void ASLLaserPuzzle::SetBeamVisuals(UParticleSystemComponent *Beam, TArray<FSLParticleParameter> Parameters, float Magnitude, bool RefreshParticleSytem) {
	for (FSLParticleParameter PP : Parameters) {
		switch(PP.Type) {
			case SLParticleParameterType::Bool:   Beam->SetEmitterEnable  (PP.Key, PP.bEnabled);              break;
			case SLParticleParameterType::Float:  Beam->SetFloatParameter (PP.Key, PP.FloatValue *Magnitude); break;
			case SLParticleParameterType::Vector: Beam->SetVectorParameter(PP.Key, PP.VectorValue*Magnitude); break;
			default: UE_LOG(LogTemp, Error, TEXT("ASLLaserPuzzle::SetBeamVisuals enum not supported!"));
		}
	}

	// This resets the particles, but if we reset it at the start then it doesn't properly update
	if (RefreshParticleSytem) Beam->Activate(true);
}

void ASLLaserPuzzle::UpdateAssociatedVisualElements() {
	for (ATogglableVisualElement *Element : TopVisualElements) {
		if (Element) Element->SetState(bActiveBeamTop);
	}
	for (ATogglableVisualElement *Element : BotVisualElements) {
		if (Element) Element->SetState(bActiveBeamBot);
	}
}

#include "DrawDebugHelpers.h"
void ASLLaserPuzzle::WebEndConnection(ASpiderWeb *Web) {
	// If both beams are active there is nothing to do here!
	if (bActiveBeamBot && bActiveBeamTop) return;
	
	// If both ends of the web are connected to the Actor, then we check if the connections are successful.
	if (LastAttachedWeb == Web) {
		FVector WebStart = Web->GetStartAttachLocation();
		FVector WebEnd   = Web->GetEndAttachLocation();
		ATriggerSphere *SphereStart = GetWorld()->SpawnActor<ATriggerSphere>(WebStart, FRotator::ZeroRotator);
		ATriggerSphere *SphereEnd   = GetWorld()->SpawnActor<ATriggerSphere>(WebEnd,   FRotator::ZeroRotator);

		if (
		    (TriggerLeftBot->IsOverlappingActor(SphereStart) && TriggerRightBot->IsOverlappingActor(SphereEnd))
		 || (TriggerLeftBot->IsOverlappingActor(SphereEnd)   && TriggerRightBot->IsOverlappingActor(SphereStart))
		) {
			bActiveBeamBot = true;
			SetBeamVisuals(BeamBot, SolvedParticleParameters);
			UpdateAssociatedVisualElements();
		}
		else if (
		    (TriggerLeftTop->IsOverlappingActor(SphereStart) && TriggerRightTop->IsOverlappingActor(SphereEnd))
		 || (TriggerLeftTop->IsOverlappingActor(SphereEnd)   && TriggerRightTop->IsOverlappingActor(SphereStart))
		) {
			bActiveBeamTop = true;
			SetBeamVisuals(BeamTop, SolvedParticleParameters);
			UpdateAssociatedVisualElements();
		}
		
		SphereStart->Destroy();
		SphereEnd->Destroy();

		if (bActiveBeamTop && bActiveBeamBot) {
			for(ASLLockedDoor *Door : AssociatedDoors) {
				if (Door) Door->OpenDoor();
			}
		}
	}
}
