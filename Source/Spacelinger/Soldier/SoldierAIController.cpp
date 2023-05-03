// Without Copyright. Project SpaceLinger, Master en Creación de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Soldier/SoldierAIController.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Slime_A.h"

ASoldierAIController::ASoldierAIController() {
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

	AISenseConfigSight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AISenseConfigSight"));
	AISenseConfigSight->DetectionByAffiliation.bDetectEnemies    = false;
	AISenseConfigSight->DetectionByAffiliation.bDetectNeutrals   = true;
	AISenseConfigSight->DetectionByAffiliation.bDetectFriendlies = false;
	AIPerceptionComponent->ConfigureSense(*AISenseConfigSight);
}

void ASoldierAIController::BeginPlay() {
	Super::BeginPlay();

	AIPerceptionComponent->OnTargetPerceptionInfoUpdated.AddDynamic(this, &ASoldierAIController::OnTargetPerceptionInfoUpdated);
}

void ASoldierAIController::OnActorDetected() {
	ensure(DetectedActor.IsValid());

	float Distance = DetectedActor.Get()->GetDistanceTo(this);
	if (Distance <= DistanceInstantDetection) {
		// Immediately detect player, arbitrary number to indicate detection
		CurrentAwareness = 999.0f;
	}
	else {
		// Calc new awareness
		float TimeSecondsElapsed = GetWorld()->GetTimeSeconds() - LastTimeSecondsTimer;
		float DistanceNormalized = (Distance - DistanceInstantDetection)/(AISenseConfigSight->LoseSightRadius - DistanceInstantDetection);
		if (DistanceNormalized >= 1.0f) { DistanceNormalized = 1.0f; } // Just in case we got away but it didn't quite undetected us yet.

		CurrentAwareness += TimeSecondsElapsed * (1-DistanceNormalized) * DetectionSpeed;
	}

	if (CurrentAwareness >= 1.0f) {
		CurrentAwareness = 1.0f;
		bIsAlerted = true;
		GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
	}

	LastTimeSecondsTimer = GetWorld()->GetTimeSeconds();
	//UE_LOG(LogTemp, Display, TEXT("Awareness rising: %f"), CurrentAwareness);
}
void ASoldierAIController::OnActorUndetected() {
	float TimeSecondsElapsed = GetWorld()->GetTimeSeconds() - LastTimeSecondsTimer;

	CurrentAwareness -= TimeSecondsElapsed * UndetectionSpeed;
	if (CurrentAwareness <= 0.0f) {
		CurrentAwareness = 0.0f;
		bIsAlerted = false;
		GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
	}

	LastTimeSecondsTimer = GetWorld()->GetTimeSeconds();
	//UE_LOG(LogTemp, Display, TEXT("Awareness lowering: %f"), CurrentAwareness);
}

void ASoldierAIController::OnTargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo) {
	ensure(GetWorld());

	// NOTE(Sergi): We could filter the actors detected by using the IGenericTeamAgentInterface. Although I'm not sure it's worth the work
	// https://sologamedevblog.com/tutorials/unreal-perception-c-friend-or-enemy/
	ASlime_A *PlayerCharacter = Cast<ASlime_A>(UpdateInfo.Target);
	if (!PlayerCharacter) { return; }

	const float TimerTickRate = 0.001f; // We want to make our timers tick every frame

	if (UpdateInfo.Stimulus.WasSuccessfullySensed()) {
		UE_LOG(LogTemp, Display, TEXT("Soldier seeing player!"));

		if (bIsAlerted) {
			// If it's alerted we just set up the awareness to max
			CurrentAwareness = 1.0f;
		}
		else {
			DetectedActor = TWeakObjectPtr<AActor>(UpdateInfo.Target);
			GetWorldTimerManager().SetTimer(DetectionTimerHandle, this, &ASoldierAIController::OnActorDetected, TimerTickRate, true);
		}
	}
	else {
		UE_LOG(LogTemp, Display, TEXT("Soldier no longer seeing player!"));

		// It doesn't matter if it's alerted or not, we're going to reduce the awareness gradually
		DetectedActor.Reset();
		GetWorldTimerManager().SetTimer(DetectionTimerHandle, this, &ASoldierAIController::OnActorUndetected, TimerTickRate, true);
	}
	LastTimeSecondsTimer = GetWorld()->GetTimeSeconds();
}
