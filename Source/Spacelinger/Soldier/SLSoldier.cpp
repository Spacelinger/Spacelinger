#include "SLSoldier.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"

ASLSoldier::ASLSoldier() {
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// TODO(Sergi): Everything that has to do with AI stuff needs to be in the AI controller, otherwise
	// Unreal complains. So I need to create a C++ for the AIController and implement all of this there
	AISensingComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AISensingComponent")); // Senses set in editor
}

void ASLSoldier::BeginPlay() {
	Super::BeginPlay();

	AISensingComponent->OnTargetPerceptionInfoUpdated.AddDynamic(this, &ASLSoldier::OnTargetPerceptionInfoUpdated);
}

// This not only gets you the elapsed seconds, it also updates the values.
// Therefore, this might only be called once per frame.
float ASLSoldier::UpdateAndGetElapsedSeconds() {
	float TimeSecondsElapsed = GetWorld()->GetTimeSeconds() - LastTimeSecondsTimer;
	LastTimeSecondsTimer = GetWorld()->GetTimeSeconds();
	ensure(TimeSecondsElapsed > 0.0f);
	return TimeSecondsElapsed;
}

void ASLSoldier::OnActorDetected() {
	ensure(DetectedActor.IsValid());

	float Distance = DetectedActor.Get()->GetDistanceTo(this);
	if (Distance <= DistanceInstantDetection) {
		// Immediately detect player, arbitrary number to indicate detection
		CurrentAwareness = 999.0f;
	}
	else {
		// Calc new awareness
		float TimeSecondsElapsed = UpdateAndGetElapsedSeconds();
		float DistanceNormalized = (Distance - DistanceInstantDetection)/(SensingComponentMaxDistance - DistanceInstantDetection);
		ensure(DistanceNormalized >= 0.0f && DistanceNormalized <= 1.0f);

		CurrentAwareness += TimeSecondsElapsed * (1-DistanceNormalized) * DetectionSpeed;
	}

	if (CurrentAwareness >= 1.0f) {
		CurrentAwareness = 1.0f;
		bIsAlerted = true;
		GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
	}
	UE_LOG(LogTemp, Display, TEXT("Awareness rising: %f"), CurrentAwareness);
}
void ASLSoldier::OnActorUndetected() {
	float TimeSecondsElapsed = UpdateAndGetElapsedSeconds();

	CurrentAwareness -= TimeSecondsElapsed * UndetectionSpeed;
	if (CurrentAwareness <= 0.0f) {
		CurrentAwareness = 0.0f;
		bIsAlerted = false;
		GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
	}
	UE_LOG(LogTemp, Display, TEXT("Awareness lowering: %f"), CurrentAwareness);
}

// TODO(Sergi): The player can be detected and undetected *in the same* frame if we play with
// AI's peripherial vision, rethink how to write this based on this information.
void ASLSoldier::OnTargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo) {
	ensure(GetWorld());
	const float TimerTickRate = 0.001f; // We want to make our timers tick every frame

	// NOTE(Sergi): This should be elsewhere, but I'll leave it here for now while I think about it
	if (UAISenseConfig *Config = AISensingComponent->GetSenseConfig(UpdateInfo.Stimulus.Type)) {
		if (UAISenseConfig_Sight *SightConfig = Cast<UAISenseConfig_Sight>(Config)) {
			SensingComponentMaxDistance = SightConfig->LoseSightRadius;
		}
	}

	FAIStimulus Stimulus = UpdateInfo.Stimulus;
	if (Stimulus.WasSuccessfullySensed()) {
		ensure(DetectedActor.IsExplicitlyNull());
		UE_LOG(LogTemp, Display, TEXT("Solider seeing player!"));

		if (bIsAlerted) {
			// If it's alerted we just set up the awareness to max
			CurrentAwareness = 1.0f;
		}
		else {
			DetectedActor = TWeakObjectPtr<AActor>(UpdateInfo.Target);
			GetWorldTimerManager().SetTimer(DetectionTimerHandle, this, &ASLSoldier::OnActorDetected, TimerTickRate, true);
		}
	}
	else {
		ensure(!DetectedActor.IsExplicitlyNull());
		UE_LOG(LogTemp, Display, TEXT("Solider no longer seeing player!"));

		// It doesn't matter if it's alerted or not, we're going to reduce the awareness gradually
		DetectedActor.Reset();
		GetWorldTimerManager().SetTimer(DetectionTimerHandle, this, &ASLSoldier::OnActorUndetected, TimerTickRate, true);
	}
	LastTimeSecondsTimer = GetWorld()->GetTimeSeconds();
}

//------------------------//
//     Interact stuff     //
//------------------------//
int ASLSoldier::GetInteractPriority() const {
	// TODO(Sergi): What if there's other enemies nearby who can be killed but this one can't? We should lower priority based on that!
	return InteractPriority;
}
void ASLSoldier::Interact(AActor* ActorInteracting) {
	// TODO(Sergi): Things that probably need to happen
	// - Check if enemy can die (won't be able to if it's seeing the player or is already dying)
	// - Play dying animation and set state to dying
	// - Delete actor at some point
}
void ASLSoldier::SetAsCandidate(bool IsCandidate) {
	// TO-DO

	if (IsCandidate) 
	{
		// Show UI to interact
	}
	else 
	{
		// Hide UI to interact
	}
}
