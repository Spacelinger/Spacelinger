// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Soldier/SoldierAIController.h"

#include "NavigationSystem.h"
#include "BrainComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Spider/Slime_A.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


ASoldierAIController::ASoldierAIController() {
	AISenseConfigSight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AISenseConfigSight"));
	AISenseConfigSight->DetectionByAffiliation.bDetectEnemies    = false;
	AISenseConfigSight->DetectionByAffiliation.bDetectNeutrals   = true;
	AISenseConfigSight->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfigSight->SightRadius     = 800;
	AISenseConfigSight->LoseSightRadius = 1000;
	AISenseConfigSight->AutoSuccessRangeFromLastSeenLocation = 50.0f;

	// Initialize patrol parameters
	// CurrentPatrolPoint = 0;
}

void ASoldierAIController::BeginPlay() {
	Super::BeginPlay();

	if (ASLSoldier *InstigatorSoldier = GetInstigatorSoldier()) {
		if (UCharacterMovementComponent *InstigatorComponent = InstigatorSoldier->GetCharacterMovement()) {
			RunningSpeed = InstigatorComponent->MaxWalkSpeed;
		}

		InitialTransform = InstigatorSoldier->GetTransform();
	}

	SetIsAlerted(false);
}

void ASoldierAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (IsStunned()) return;
	ASLSoldier *SoldierCharacter = Cast<ASLSoldier>(GetInstigator());
	if (!SoldierCharacter || SoldierCharacter->bIsDead) return;

	/*SecondsTillNextTick -= DeltaTime;
	if (SecondsTillNextTick > 0) return;
	SecondsTillNextTick = SecondsPerTick;*/

	bPlayerInSight = IsPlayerInSight();
	if (bPlayerInSight) {
		//UE_LOG(LogTemp, Display, TEXT("Player detected!!"));

		DetectedActor = GetPlayerCharacter();
		DetectedLocation = DetectedActor.Get()->GetActorLocation();
		if (bIsAlerted) {
			CurrentAwareness = 1.0f;
		}
		else {
			float Distance = DetectedActor.Get()->GetDistanceTo(GetInstigator()); // TODO: We could get it from the IsPlayerInSight()
			if (Distance <= DistanceInstantDetection) {
				// Immediately detect player, arbitrary number to indicate detection
				CurrentAwareness = 999.0f;
			}
			else {
				// Calc new awareness
				float DistanceNormalized = (Distance - DistanceInstantDetection)/(AISenseConfigSight->LoseSightRadius - DistanceInstantDetection);
				if (DistanceNormalized >= 1.0f) { DistanceNormalized = 1.0f; } // Just in case we got away but it didn't quite undetected us yet.

				CurrentAwareness += DeltaTime * (1-DistanceNormalized) * DetectionSpeed;
			}

			if (CurrentAwareness >= 1.0f) {
				SetIsAlerted(true);
			}
		}
	}
	else {
		//UE_LOG(LogTemp, Display, TEXT("Player undetected!!"));
		DetectedActor.Reset();

		if (CurrentAwareness > 0.0f) {
			CurrentAwareness -= DeltaTime * UndetectionSpeed;
			if (CurrentAwareness <= 0.0f) {
				SetIsAlerted(false);
			}
		}
	}
}

bool ASoldierAIController::IsPlayerInSight() {
	ASlime_A *PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter) return false;
	ASLSoldier *SoldierCharacter = Cast<ASLSoldier>(GetInstigator());
	if (!SoldierCharacter) return false;

	FVector SoldierPosition = SoldierCharacter->GetActorLocation();
	FVector PlayerPosition  = PlayerCharacter ->GetActorLocation();
	float ZDistance = FMath::Abs(SoldierPosition.Z - PlayerPosition.Z);
	FVector SoldierPositionNoZ = FVector (SoldierPosition.X, SoldierPosition.Y, 0);
	FVector  PlayerPositionNoZ = FVector ( PlayerPosition.X,  PlayerPosition.Y, 0);
	FVector SoldierToPlayer = PlayerPositionNoZ - SoldierPositionNoZ;
	float DistanceSqr = SoldierToPlayer.SquaredLength();
	FVector SoldierForward = SoldierCharacter->GetActorForwardVector();
	SoldierForward .Normalize();
	SoldierToPlayer.Normalize();
	float DotResult = FVector::DotProduct(SoldierForward, SoldierToPlayer);

	for (FUSLAICone Cone : SoldierCharacter->ConesOfVision) {
		// Check if player is close enough
		float MaxRadiusSqr = Cone.MaxSightRadius * Cone.MaxSightRadius;
		float MinRadiusSqr = Cone.MinSightRadius * Cone.MinSightRadius;
		if (DistanceSqr > MaxRadiusSqr) continue;
		if (DistanceSqr < MinRadiusSqr) continue;

		// Check if player is at the right height
		if (ZDistance > Cone.SightHeight) continue;

		// Check if player is in front
		float CosPeripherialVision = FMath::Cos(FMath::DegreesToRadians(Cone.PeripherialVision/2));
		if (DotResult <= CosPeripherialVision) continue;

		// Raycast to check if we are looking at the player
		FHitResult HitResult;
		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(this);
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, SoldierPosition, PlayerPosition, ECC_Camera, TraceParams);
		if (bHit && HitResult.GetActor() == PlayerCharacter) return true;
	}

	return false;
}

ASlime_A* ASoldierAIController::GetPlayerCharacter() {
	if (!PlayerCharacterRef) {
		PlayerCharacterRef = Cast<ASlime_A>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	}
	return PlayerCharacterRef;
}

bool ASoldierAIController::CanPatrol() const
{
	ASLSoldier *InstigatorSoldier = GetInstigatorSoldier();
	return (InstigatorSoldier && InstigatorSoldier->PatrolType != SLIdleType::Standing);
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