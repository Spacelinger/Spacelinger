// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)


#include "Soldier/SoldierAIController.h"

#include "NavigationSystem.h"
#include "BrainComponent.h"
#include "Spider/Slime_A.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"


ASoldierAIController::ASoldierAIController() { }

void ASoldierAIController::BeginPlay() {
	Super::BeginPlay();

	if (ASLSoldier *InstigatorSoldier = GetInstigatorSoldier()) {
		InitialTransform = InstigatorSoldier->GetTransform();

		MaxSightRadius = 0.0f;
		for (FUSLAICone Cone : InstigatorSoldier->ConesOfVision) {
			if (Cone.MaxSightRadius > MaxSightRadius) MaxSightRadius = Cone.MaxSightRadius;
		}
	}

	SetIsAlerted(false);
}

void ASoldierAIController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (IsStunned()) return;

	ASLSoldier *SoldierCharacter = GetInstigatorSoldier();
	if (!SoldierCharacter || SoldierCharacter->bIsDead) return;

	ASlime_A *PlayerCharacter = GetPlayerCharacter();
	if (!PlayerCharacter || PlayerCharacter->bIsDead) return;

	DetectedLocation = PlayerCharacter->GetActorLocation();
	bPlayerInSight = IsPlayerInSight();
	if (bPlayerInSight) {
		//UE_LOG(LogTemp, Display, TEXT("Player detected!!"));
		bSearchLastLocation = true;

		DetectedActor = PlayerCharacter;
		if (bIsAlerted) {
			CurrentAwareness = 1.0f;
		}
		else {
			float Distance = FVector::DistXY(SoldierCharacter->GetActorLocation(), PlayerCharacter->GetActorLocation());
			if (Distance <= DistanceInstantDetection) {
				// Immediately detect player, arbitrary number to indicate detection
				CurrentAwareness = 999.0f;
			}
			else {
				// Calc new awareness
				float DistanceNormalized = (Distance-DistanceInstantDetection) / (MaxSightRadius-DistanceInstantDetection);
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

		if (CurrentAwareness > 0.0f && (!bIsAlerted || !bSearchLastLocation)) {
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

	// If the Soldier is alerted we don't care about the cones
	if (bIsAlerted) return IsPlayerInSightRaycast(SoldierPosition, PlayerPosition);

	FVector SoldierPositionNoZ = FVector (SoldierPosition.X, SoldierPosition.Y, 0);
	FVector  PlayerPositionNoZ = FVector ( PlayerPosition.X,  PlayerPosition.Y, 0);
	FVector SoldierToPlayer = PlayerPositionNoZ - SoldierPositionNoZ;
	float DistanceSqr = SoldierToPlayer.SquaredLength();

	// Check if the player is too far away to avoid executing more operations
	if (DistanceSqr > MaxSightRadius*MaxSightRadius) return false;

	float ZDistance = FMath::Abs(SoldierPosition.Z - PlayerPosition.Z);
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

		// At this point we know the player is inside the cone, so we raycast to check if it can be seen and return
		return IsPlayerInSightRaycast(SoldierPosition, PlayerPosition);
	}

	return false;
}

bool ASoldierAIController::IsPlayerInSightRaycast(FVector SoldierPosition, FVector PlayerPosition) {
	ASlime_A *PlayerCharacter = GetPlayerCharacter();
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, SoldierPosition, PlayerPosition, SL_ECC_SoldierAI, TraceParams);
	return (bHit && HitResult.GetActor() == PlayerCharacter);
}


ASlime_A* ASoldierAIController::GetPlayerCharacter() {
	if (!PlayerCharacterRef) {
		PlayerCharacterRef = Cast<ASlime_A>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	}
	return PlayerCharacterRef;
}
ASLSoldier* ASoldierAIController::GetInstigatorSoldier() {
	if (!SoldierCharacterRef) {
		SoldierCharacterRef = Cast<ASLSoldier>(GetInstigator());
	}
	return SoldierCharacterRef;
}


bool ASoldierAIController::CanPatrol() const {
	return (SoldierCharacterRef && SoldierCharacterRef->PatrolType != SLIdleType::Standing);
}

void ASoldierAIController::SetIsAlerted(bool NewState) {
	bIsAlerted = NewState;
	CurrentAwareness = bIsAlerted ? 1.0f : 0.0f;
	AimTimeRemaining = AimTime;
	
	if (ASLSoldier *InstigatorSoldier = GetInstigatorSoldier()) {
		InstigatorSoldier->AnimationState = NewState ? SoldierAIState::ALERTED : SoldierAIState::IDLE;
		if (UCharacterMovementComponent *InstigatorComponent = InstigatorSoldier->GetCharacterMovement()) {
			InstigatorComponent->MaxWalkSpeed = bIsAlerted ? RunningSpeed : WalkingSpeed;
		}
	}
}

void ASoldierAIController::StopLogic() {
	if (UBrainComponent* Brain = AAIController::GetBrainComponent()) {
		Brain->StopLogic("Stunned");
	}
}

void ASoldierAIController::ResumeLogic() {
	if (UBrainComponent* Brain = AAIController::GetBrainComponent()) {
		Brain->RestartLogic();
	}
}

bool ASoldierAIController::IsStunned() {
	ASLSoldier *InstigatorSoldier = Cast<ASLSoldier>(GetInstigator());
	if (InstigatorSoldier != nullptr)
	{
		bIsStunned = InstigatorSoldier->bIsStunned;
	}
	return bIsStunned;
}