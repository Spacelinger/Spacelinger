// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Soldier/SoldierAIController.h"

#include "NavigationSystem.h"
#include "BrainComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Slime_A.h"
#include "Soldier\SLSoldier.h"
#include "GameFramework/CharacterMovementComponent.h"


ASoldierAIController::ASoldierAIController() {
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));

	AISenseConfigSight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AISenseConfigSight"));
	AISenseConfigSight->DetectionByAffiliation.bDetectEnemies    = false;
	AISenseConfigSight->DetectionByAffiliation.bDetectNeutrals   = true;
	AISenseConfigSight->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfigSight->SightRadius     = 800;
	AISenseConfigSight->LoseSightRadius = 1000;
	AISenseConfigSight->AutoSuccessRangeFromLastSeenLocation = 50.0f;
	AIPerceptionComponent->ConfigureSense(*AISenseConfigSight);

	// Initialize patrol parameters
	// CurrentPatrolPoint = 0;
}

void ASoldierAIController::BeginPlay() {
	Super::BeginPlay();
	AIPerceptionComponent->OnTargetPerceptionInfoUpdated.AddDynamic(this, &ASoldierAIController::OnTargetPerceptionInfoUpdated);

	if (APawn *InstigatorPawn = GetInstigator()) {
		InitialTransform = InstigatorPawn->GetTransform();
	}

	if (ASLSoldier *InstigatorSoldier = GetInstigatorSoldier()) {
		if (UCharacterMovementComponent *InstigatorComponent = InstigatorSoldier->GetCharacterMovement()) {
			RunningSpeed = InstigatorComponent->MaxWalkSpeed;
		}
	}

	SetIsAlerted(false);
}

bool ASoldierAIController::CanPatrol() const
{
	ASLSoldier *InstigatorSoldier = GetInstigatorSoldier();
	return (InstigatorSoldier && InstigatorSoldier->bCanPatrol);
}

void ASoldierAIController::ResumePatrol()
{
	if (!bIsAlerted && CanPatrol() && PatrolPoints.Num() > 0) {
		GetWorldTimerManager().SetTimer(PatrolTimerHandle, this, &ASoldierAIController::Patrol, TimerTickRate, true);
	}
}

void ASoldierAIController::OnActorDetected() {
	if (IsStunned()) return;
	ensure(DetectedActor.IsValid());

	float Distance = DetectedActor.Get()->GetDistanceTo(GetInstigator());
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
		SetIsAlerted(true);
		GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
	}

	LastTimeSecondsTimer = GetWorld()->GetTimeSeconds();
}

void ASoldierAIController::OnActorUndetected() {
	if (IsStunned()) return;

	float TimeSecondsElapsed = GetWorld()->GetTimeSeconds() - LastTimeSecondsTimer;

	CurrentAwareness -= TimeSecondsElapsed * UndetectionSpeed;
	if (CurrentAwareness <= 0.0f) {
		SetIsAlerted(false);
		GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
	}
	
	LastTimeSecondsTimer = GetWorld()->GetTimeSeconds();
	ResumePatrol();
}

void ASoldierAIController::OnTargetPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo) {
	ensure(GetWorld());
	ensure(AISenseConfigSight);
	
	// NOTE(Sergi): We could filter the actors detected by using the IGenericTeamAgentInterface. Although I'm not sure it's worth the work
	// https://sologamedevblog.com/tutorials/unreal-perception-c-friend-or-enemy/
	ASlime_A *PlayerCharacter = Cast<ASlime_A>(UpdateInfo.Target);
	if (!PlayerCharacter) { return; }

	if (UpdateInfo.Stimulus.WasSuccessfullySensed()) {
		DetectedActor = TWeakObjectPtr<AActor>(UpdateInfo.Target);
		// If it's alerted we just set up the awareness to max without calling our timer
		if (bIsAlerted) {
			CurrentAwareness = 1.0f;
			GetWorldTimerManager().ClearTimer(DetectionTimerHandle);
		}
		else {
			GetWorldTimerManager().SetTimer(DetectionTimerHandle, this, &ASoldierAIController::OnActorDetected, TimerTickRate, true);
		}
	}
	else {
		// It doesn't matter if it's alerted or not, we're going to reduce the awareness gradually
		DetectedActor.Reset();
		GetWorldTimerManager().SetTimer(DetectionTimerHandle, this, &ASoldierAIController::OnActorUndetected, TimerTickRate, true);
	}
	LastTimeSecondsTimer = GetWorld()->GetTimeSeconds();
}


void ASoldierAIController::Patrol() {
	// Get the patrol point
	// AActor* CurrentPatrolPointActor = PatrolPoints[CurrentPatrolPointIndex]; // To be applied when we have patrol points
	if (bFoundPatrolPoint) {
		// Move to the patrol point
		MoveToLocation(CurrentPatrolPoint, PatrolAcceptanceRadius);
		float Distance = FVector::Dist(GetPawn()->GetActorLocation(), CurrentPatrolPoint);
		if (Distance <= 5.0f) {
			// When we're near, we'll find a new patrol point
			bFoundPatrolPoint = false;
		}
	} else
	{
		// Find a new patrol point
		bFoundPatrolPoint = UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, GetPawn()->GetActorLocation(), CurrentPatrolPoint, 500.0f);
	}
}

void ASoldierAIController::SetIsAlerted(bool NewState) {
	bIsAlerted = NewState;
	CurrentAwareness = bIsAlerted ? 1.0f : 0.0f; 

	if (ASLSoldier *InstigatorSoldier = GetInstigatorSoldier()) {
		if (UCharacterMovementComponent *InstigatorComponent = InstigatorSoldier->GetCharacterMovement()) {
			InstigatorComponent->MaxWalkSpeed = bIsAlerted ? RunningSpeed : WalkingSpeed;
		}
	}
}

void ASoldierAIController::RefreshDetectionTimers() {
	auto TimerFunction = DetectedActor.IsValid() ? &ASoldierAIController::OnActorDetected : &ASoldierAIController::OnActorUndetected;
	GetWorldTimerManager().SetTimer(DetectionTimerHandle, this, TimerFunction, TimerTickRate, true);
	LastTimeSecondsTimer = GetWorld()->GetTimeSeconds();
}

ASLSoldier* ASoldierAIController::GetInstigatorSoldier() const { return Cast<ASLSoldier>(GetInstigator()); }

void ASoldierAIController::StopLogic()
{
	UBrainComponent* Brain = AAIController::GetBrainComponent();
	Brain->StopLogic("Stunned");
}

void ASoldierAIController::ResumeLogic()
{
	UBrainComponent* Brain = AAIController::GetBrainComponent();
	Brain->RestartLogic();
}

bool ASoldierAIController::IsStunned()
{
	ASLSoldier *InstigatorSoldier = Cast<ASLSoldier>(GetInstigator());
	if (InstigatorSoldier != nullptr)
	{
		bIsStunned = InstigatorSoldier->bIsStunned;
	}
	return bIsStunned;
}