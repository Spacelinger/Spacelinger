// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "Actors/LaserPuzzle/SLLaserPuzzle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Spider/SpiderWeb.h"
#include "Actors/Door/LockedDoor/SLLockedDoor.h"

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
	MeshLeft->SetRelativeLocation(FVector::ZeroVector);
	MeshLeft->SetRelativeRotation(FRotator::ZeroRotator);

	MeshRightLocation.X = 0.0f;
	MeshRightLocation.Z = 0.0f;
	if (MeshRightLocation.Y > 0.0f) { MeshRightLocation.Y = 0.0f; }
	MeshRight->SetRelativeLocation(MeshRightLocation);

	// Set the new state for the visual elements
	UpdateAssociatedVisualElements(bActiveBeamBot && bActiveBeamTop);

	// TODO: Set keys in the editor (as well as abstract the code to be written once)
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

	SetBeamVisuals(BeamBot, bActiveBeamBot, false);
	SetBeamVisuals(BeamTop, bActiveBeamTop, false);
}

void ASLLaserPuzzle::SetBeamVisuals(UParticleSystemComponent *Beam, bool IsActive, bool RefreshParticleSytem) {
	// TODO: Set keys in the editor
	Beam->SetEmitterEnable(FName(TEXT("Beam2")), IsActive);
	Beam->SetEmitterEnable(FName(TEXT("Beam3")), IsActive);
	Beam->SetEmitterEnable(FName(TEXT("EnergyDust")), IsActive);
	Beam->SetEmitterEnable(FName(TEXT("OrbSource")), IsActive);
	Beam->SetEmitterEnable(FName(TEXT("OrbDestination")), IsActive);

	// This resets the particles, but if we reset it at the start then it doesn't properly update
	if (RefreshParticleSytem) Beam->Activate(true);
}

void ASLLaserPuzzle::UpdateAssociatedVisualElements(bool NewState) {
	for (auto Element : AssociatedVisualElements) {
		Element->SetState(NewState);
	}
}

void ASLLaserPuzzle::WebEndConnection(ASpiderWeb *Web) {
	// If both beams are active there is nothing to do here!
	if (bActiveBeamBot && bActiveBeamTop) return;
	
	// If both ends of the web are connected to the Actor, then we check if the connections are successful.
	if (LastAttachedWeb == Web) {
		FVector WebStart = Web->GetStartAttachLocation();
		FVector WebEnd   = Web->GetEndAttachLocation();

		if (IsWebConnectingBeam(BeamTop, WebStart, WebEnd)) {
			bActiveBeamTop = true;
			SetBeamVisuals(BeamTop, true);
		}
		else if (IsWebConnectingBeam(BeamBot, WebStart, WebEnd)) {
			bActiveBeamBot = true;
			SetBeamVisuals(BeamBot, true);
		}

		if (bActiveBeamBot && bActiveBeamTop) {
			UpdateAssociatedVisualElements(true);
			for(ASLLockedDoor *Element : AssociatedDoors) {
				Element->OpenDoor();
			}
		}
	}
}

bool ASLLaserPuzzle::IsWebConnectingBeam(UParticleSystemComponent *Beam, FVector WebStart, FVector WebEnd) {
	FVector BeamStart = Beam->GetComponentLocation();
	FVector BeamEnd   = BeamStart - FVector(.0f, BeamLength, .0f);

	// Calc distances
	float WebStartBeamStart = FVector::Distance(WebStart, BeamStart);
	float WebStartBeamEnd   = FVector::Distance(WebStart, BeamEnd);
	float WebEndBeamStart   = FVector::Distance(WebEnd,   BeamStart);
	float WebEndBeamEnd     = FVector::Distance(WebEnd,   BeamEnd);
	
	bool IsConnecting = ((WebStartBeamStart <= ConnectedRadius && WebEndBeamEnd   <= ConnectedRadius)
	                  || (WebStartBeamEnd   <= ConnectedRadius && WebEndBeamStart <= ConnectedRadius));

	if (IsConnecting) UE_LOG(LogTemp, Display, TEXT("SLLaserPuzzle: Web is connecting beam! -> %s"), *Beam->GetName());

	return IsConnecting;
}
