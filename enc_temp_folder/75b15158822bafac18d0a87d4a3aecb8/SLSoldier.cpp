#include "SLSoldier.h"

#include "SoldierAIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Components/WidgetComponent.h"
#include "UI/Soldier/SLDetectionWidget.h"
#include "Soldier/SLSoldierPath.h"
#include "EngineUtils.h"
#include "Audio/SpacelingerAudioComponent.h"
#include "Components/InteractableComponent.h"
#include "UI/Interact/InteractWidget.h"

ASLSoldier::ASLSoldier() {
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	DetectionWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("DetectionWidget"));
	DetectionWidget->SetupAttachment(RootComponent);
	DetectionWidget->SetWidgetSpace(EWidgetSpace::Screen);

	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(FName(TEXT("Interactable Component")));
	
	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(FName(TEXT("Interact UI")));
	InteractWidget->SetupAttachment(RootComponent);
	InteractWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	InteractWidget->ComponentTags.Add(FName(TEXT("Interact UI")));
	InteractWidget->SetWidget(InteractPromptWidget);

	ConesOfVision.Add(FUSLAICone());
}

void ASLSoldier::OnConstruction(const FTransform& Transform) {
#if WITH_EDITOR // Labels only exist in the editor
	if (PatrolType == SLIdleType::PatrolGuided) {
		if (!PathActor) {
			FString NewName = this->GetActorLabel();
			NewName.Append("_Path");
			FName NewFName(*NewName);

			// Try to find if it already exists
			for (TActorIterator<ASLSoldierPath> It(GetWorld(), ASLSoldierPath::StaticClass()); It; ++It) {
				if ((*It)->GetActorLabel().Equals(NewName)) {
					PathActor = *It;
					break;
				}
			}

			// If it doesn't then let's create it
			if (!PathActor) {
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = this;
				PathActor = GetWorld()->SpawnActor<ASLSoldierPath>(ASLSoldierPath::StaticClass(), GetTransform(), SpawnParameters);
				PathActor->SetActorLabel(NewName);
			}

			PathActor->OwnerSoldier = this;
		}
	}
#endif

	if (bDrawDebugAI) {
		GetWorldTimerManager().SetTimer(DrawDebugTimerHandle, this, &ASLSoldier::DrawDebugCones, .01f, true);
	}
	else {
		GetWorldTimerManager().ClearTimer(DrawDebugTimerHandle);
	}
}

void ASLSoldier::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (AnimationState == SoldierAIState::AIMING) {
		FRotator SoldierRotation = GetActorRotation();
		FVector SrcVector = SoldierRotation         .Vector();
		FVector DstVector = RotatorToFaceWhileAiming.Vector();
		SrcVector.Z = 0; SrcVector.Normalize();
		DstVector.Z = 0; DstVector.Normalize();

		float DotProduct = FVector::DotProduct(SrcVector, DstVector);
		bHasRotatedLastFrameWhileAiming = (DotProduct < .99f);
		if (bHasRotatedLastFrameWhileAiming ) {
			FRotator InterpRotator = FMath::RInterpTo(SoldierRotation, RotatorToFaceWhileAiming, DeltaTime, RotationSpeedWhileAiming);
			SoldierRotation.Yaw = InterpRotator.Yaw;
			SetActorRotation(SoldierRotation);
		}
	}

	if (bDrawDebugAI) DrawDebugCones();
}

void ASLSoldier::BeginPlay() {
	Super::BeginPlay();

	GameInstance = GetGameInstance();
	AudioManager = GameInstance->GetSubsystem<USpacelingerAudioComponent>();

	InteractableComponent->OnInteractDelegate.AddDynamic(this, &ASLSoldier::ReceiveDamage);
	InteractPromptWidget->OwningActor = this;

	if (USLDetectionWidget *DetectionInterface = Cast<USLDetectionWidget>(DetectionWidget->GetWidget())) {
		DetectionInterface->OwningActor = this;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ERROR! Soldier's DetectionWidget is not USLDetectionInterface"));
	}

	if (OffscreenDetectionWidgetClass) {
		OffscreenDetectionWidget = Cast<USLDetectionWidget>(CreateWidget(GameInstance->GetPrimaryPlayerController(), OffscreenDetectionWidgetClass));
		if (OffscreenDetectionWidget) {
			OffscreenDetectionWidget->AddToViewport();
			OffscreenDetectionWidget->OwningActor = this;
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("ERROR! Soldier's OffscreenDetectionWidgetClass is not set"));
	}

	if (PatrolType == SLIdleType::PatrolGuided) {
		if (PathActor->bInitialPositionAsPath) {
			FVector CurrentLocation = GetActorLocation();
			WorldPatrolPoints.Add(CurrentLocation);
		}
		for (FVector PP : PathActor->PatrolPoints) {
			WorldPatrolPoints.Add(PathActor->GetActorLocation() + PP);
		}
	}
}

void ASLSoldier::MoveToCeiling() {
	Die(nullptr);

	/* Disable all collision on capsule */
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	SetActorEnableCollision(true);

	// Ragdoll
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetSimulatePhysics(true);
	MeshComp->WakeAllRigidBodies();
	MeshComp->bBlendPhysics = true;
	// Enable "Simulation Generates Hit Events"
	MeshComp->SetNotifyRigidBodyCollision(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetEnableGravity(false);
	
}

void ASLSoldier::ReceiveDamage(AActor *DamageDealer)
{
	if (bIsStunned)
	{
		Die(DamageDealer);
		return;
	}

	ASoldierAIController* ControllerReference = Cast<ASoldierAIController>(GetController());
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([&]() { InteractableComponent->bCanInteract = true; });

	if (!ControllerReference->bIsAlerted)
	{
		Die(DamageDealer);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, 5.f, false);
		InteractableComponent->bCanInteract = false;
	}


	// if it fails maybe explicitly set a cooldown timer for the spider to be able to attack again.
	// maybe even deal damage, like if the soldier stomps the spider.

}

void ASLSoldier::Stun(float StunDuration, FVector ThrowLocation)
{
	bIsStunned = true;
	GetCharacterMovement()->DisableMovement();
	// Get the controller of the character (SoldierAIController) --- CHANGE THIS!!!
	ASoldierAIController* ControllerReference = Cast<ASoldierAIController>(GetController());
	ControllerReference->StopLogic();
	ControllerReference->SetIsAlerted(true);
	ControllerReference->DetectedLocation = ThrowLocation;
	
	GetWorldTimerManager().SetTimer(UnstunTimerHandle, this, &ASLSoldier::Unstun, StunDuration, false);
}

void ASLSoldier::Unstun() 
{
	if (bIsDead) return;

	bIsStunned = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	ASoldierAIController* ControllerReference = Cast<ASoldierAIController>(GetController());
	ControllerReference->ResumeLogic();
}

float ASLSoldier::GetRemainingTimeToUnstunAsPercentage() {
	float Elapsed   = GetWorldTimerManager().GetTimerElapsed(UnstunTimerHandle);
	float Remaining = GetWorldTimerManager().GetTimerRemaining(UnstunTimerHandle);
	return Remaining / (Elapsed+Remaining);
}

bool ASLSoldier::IsStunned()
{
	return bIsStunned;
}


void ASLSoldier::Die(AActor *Killer)
{
	bIsDead = true;
	DetectionWidget->Deactivate();
	OffscreenDetectionWidget->RemoveFromParent();

	GetController()->UnPossess();

	// Disable collision and physics-based movement for the soldier's components
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	SoldierHasDied(Killer);
	AudioManager->SoldierDeathAudioReaction();
}

bool ASLSoldier::IsDead()
{
	return bIsDead;	
}

USpacelingerAudioComponent * ASLSoldier::GetAudioManager()
{
	if (AudioManager)
	{
		return AudioManager;
	}
	return nullptr;
}


FVector ASLSoldier::GetNextPatrolPoint() {
	int i = CurrentPatrolPointIndex;
	CurrentPatrolPointIndex = (CurrentPatrolPointIndex+1) % WorldPatrolPoints.Num();
	return WorldPatrolPoints[i];
}

void ASLSoldier::DrawDebugCones() {
	const bool PL = false; // bPersistentLines 
	const float LT = .1f;  // LifeTime
	const uint8 DP = 0;    //DepthPriority

	const float Thickness = 2.f;

	FVector Direction = GetActorForwardVector();
	FVector Origin = GetActorLocation();
	for (FUSLAICone Cone : ConesOfVision) {
		float AngleWidth = FMath::DegreesToRadians(Cone.PeripherialVision/2);
		int32 Segments = (int32)(Cone.PeripherialVision/10);
		FColor Color = Cone.DebugColor;

		// Positions stuff
		FVector SrcPosR = Origin + Direction.RotateAngleAxis( Cone.PeripherialVision/2, FVector::UpVector)*Cone.MinSightRadius;
		FVector SrcPosL = Origin + Direction.RotateAngleAxis(-Cone.PeripherialVision/2, FVector::UpVector)*Cone.MinSightRadius;
		FVector DstPosR = Origin + Direction.RotateAngleAxis( Cone.PeripherialVision/2, FVector::UpVector)*Cone.MaxSightRadius;
		FVector DstPosL = Origin + Direction.RotateAngleAxis(-Cone.PeripherialVision/2, FVector::UpVector)*Cone.MaxSightRadius;

		float ZOffsetArray[] = { 0.0f, Cone.SightHeight, -Cone.SightHeight };
		for (float ZOffset : ZOffsetArray) {
			FVector Origin2  = FVector(Origin.X,  Origin.Y,  Origin.Z  + ZOffset);
			FVector SrcPosR2 = FVector(SrcPosR.X, SrcPosR.Y, SrcPosR.Z + ZOffset);
			FVector SrcPosL2 = FVector(SrcPosL.X, SrcPosL.Y, SrcPosL.Z + ZOffset);
			FVector DstPosR2 = FVector(DstPosR.X, DstPosR.Y, DstPosR.Z + ZOffset);
			FVector DstPosL2 = FVector(DstPosL.X, DstPosL.Y, DstPosL.Z + ZOffset);
			DrawDebugCircleArc(GetWorld(), Origin2, Cone.MaxSightRadius, Direction, AngleWidth, Segments, Color, PL, LT, DP, Thickness);
			DrawDebugCircleArc(GetWorld(), Origin2, Cone.MinSightRadius, Direction, AngleWidth, Segments, Color, PL, LT, DP, Thickness);
			DrawDebugLine(GetWorld(), SrcPosR2, DstPosR2,  Color, PL, LT, DP, Thickness);
			DrawDebugLine(GetWorld(), SrcPosL2, DstPosL2,  Color, PL, LT, DP, Thickness);
		}

		FVector ConeVertex[] = { SrcPosR, SrcPosL, DstPosR, DstPosL };
		for (FVector Point : ConeVertex) {
			FVector PointT = FVector(Point.X, Point.Y, Point.Z + Cone.SightHeight);
			FVector PointB = FVector(Point.X, Point.Y, Point.Z - Cone.SightHeight);
			DrawDebugLine(GetWorld(), PointT, PointB, Color, PL, LT, DP, Thickness);
		}
	}

}
