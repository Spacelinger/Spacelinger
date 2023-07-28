// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#include "Slime_A.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/PostProcessComponent.h"
#include "Components/MCV_AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/HealthAttributeSet.h"
#include "GAS/Attributes/StaminaAttributeSet.h"
#include "GAS/Attributes/SpiderTrapsAttributeSet.h"
#include "GAS/Effects/GE_StaminaRecovery.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h" 
#include "Engine/StaticMeshActor.h"
#include "Components/BoxComponent.h"
#include "Components/InteractingComponent.h"
#include "Components/InventoryComponent.h"

#include <Kismet/KismetMathLibrary.h>
#include <Soldier/SLSoldier.h>

#include "Actors/SpiderProjectile.h"


#include "Blueprint/UserWidget.h"
#include "UI/Game/SLCrosshair.h"



//////////////////////////////////////////////////////////////////////////
// ASlime

class ASLCrosshair;

ASlime_A::ASlime_A()
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create Post Process component
	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->bUnbound = true;	// set to unbound so PPFX take whole map/screen

	// Interact Component

	InteractingComponent = CreateDefaultSubobject<UInteractingComponent>(TEXT("Interacting Component"));
	
	// Interact Collider component
	InteractCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Interact Collision Component"));
	InteractCollisionComponent->SetupAttachment(CameraBoom);
	InteractCollisionComponent->ComponentTags.Add(FName(TEXT("Interact Volume")));

	// Inventory Component
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));

	// Create GAS' Ability System Component and attributes
	AbilitySystemComponent = CreateDefaultSubobject<UMCV_AbilitySystemComponent>(TEXT("AbilitySystem"));
	HealthAttributeSet = CreateDefaultSubobject<UHealthAttributeSet>(TEXT("HealthAttributeSet"));
	StaminaAttributeSet = CreateDefaultSubobject<UStaminaAttributeSet>(TEXT("StaminaAttributeSet"));
	SpiderTrapsAttributeSet = CreateDefaultSubobject<USpiderTrapsAttributeSet>(TEXT("SpiderTrapsAttributeSet"));
	StaminaAttributeSet->StaminaRecoveryBaseRate = StaminaRecoveryBaseRate;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	TraceParams.AddIgnoredActor(this);

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ASlime_A::OnCollisionEnter);


}

void ASlime_A::BeginPlay()
{
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	InputRotator = GetActorRotation();
	UpdateCameraRotation();

	// Init climbing stuff
	DefaultMaxStepHeight = GetCharacterMovement()->MaxStepHeight;
	DiagonalDirections.Reserve(8);
	DiagonalDirections.Add(FVector( .5,  .5,  .5));
	DiagonalDirections.Add(FVector(-.5,  .5,  .5));
	DiagonalDirections.Add(FVector( .5, -.5,  .5));
	DiagonalDirections.Add(FVector(-.5, -.5,  .5));
	DiagonalDirections.Add(FVector( .5,  .5, -.5));
	DiagonalDirections.Add(FVector(-.5,  .5, -.5));
	DiagonalDirections.Add(FVector( .5, -.5, -.5));
	DiagonalDirections.Add(FVector(-.5, -.5, -.5));
	for (int i = 0; i < DiagonalDirections.Num(); ++i) {
		DiagonalDirections[i] = DiagonalDirections[i].GetSafeNormal();
	}

	/*
	//GAS
	UAbilitySystemComponent* asc = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(this);

	if(ensureMsgf(AbilitySystemComponent, TEXT("Missing Ability System component for %s"), *GetOwner()->GetName())){
		asc->SetNumericAttributeBase(UStaminaAttributeSet::GetMaxStaminaAttribute(), static_cast<float>(MaxStamina));
		asc->SetNumericAttributeBase(UStaminaAttributeSet::GetStaminaAttribute(), static_cast<float>(MaxStamina));
		asc->SetNumericAttributeBase(UHealthAttributeSet::GetMaxHealthAttribute(), static_cast<float>(MaxHealth));
		asc->SetNumericAttributeBase(UHealthAttributeSet::GetHealthAttribute(), static_cast<float>(MaxHealth));
		asc->SetNumericAttributeBase(USpiderTrapsAttributeSet::GetMaxTrapsAttribute(), static_cast<float>(MaxTraps));
		asc->SetNumericAttributeBase(USpiderTrapsAttributeSet::GetAvailableTrapsAttribute(), static_cast<float>(MaxTraps));
		
		FGameplayEffectSpecHandle specHandle = asc->MakeOutgoingSpec(UGE_StaminaRecovery::StaticClass(), 1, asc->MakeEffectContext());
		specHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag("Attribute.Stamina.RecoveryValue"), .0f);	// Not really needed
		StaminaAttributeSet->StaminaRecoveryEffect = asc->ApplyGameplayEffectSpecToSelf(*specHandle.Data.Get());
	}
	*/
}

void ASlime_A::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bJumpToLocation)
	{
		HandleClimbingBehaviour();
		HandleAttachedBehaviour();
		HandleHangingBehaviour();
		ModifyDamping();
		UpdateRotationOverTime(DeltaTime);

	}
	else
	{
		HandleJumpToLocationBehaviour();
	}

	if (GetCharacterMovement()->IsFalling())
	{
		bIsInAir = true;
	}
	else {
		bIsInAir = false;
	}

}

void ASlime_A::OnCollisionEnter(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) {
	AStaticMeshActor* StaticMeshActor = Cast<AStaticMeshActor>(OtherActor);

	if (previousActorCollision != StaticMeshActor) {
		if (StaticMeshActor && bJumpToLocation) {
			StopJumpToPosition();
		}
		if (StaticMeshActor && isHanging) {
			CutSpiderWeb();
			GetCapsuleComponent()->SetNotifyRigidBodyCollision(false);

		}
		canTrace = true;
	}
	previousActorCollision = nullptr;

}

void ASlime_A::SwitchAbility(const FInputActionValue& Value)
{
	if (isHanging)
	{
		if (!bSetInitialRelativeLocation) {
			bSetInitialRelativeLocation = true;
			initialRelativePosition = getRelativePositionPhysicsConstraint();
		}

		//ConstraintComp->SetConstraintReferencePosition(EConstraintFrame::Frame2, newRelativeLocation);
		FVector vectorDirection = GetCapsuleComponent()->GetComponentLocation() - spiderWebReference->ConstraintComp->GetComponentLocation();
		FVector newLocation = getVectorInConstraintCoordinates(vectorDirection * Value.GetMagnitude(), 20.0f, 1.0f);
		if (initialRelativePosition.Length() < 30 && Value.GetMagnitude() > 0) {
			return;
		}
		initialRelativePosition = initialRelativePosition + newLocation;
		spiderWebReference->ConstraintComp->SetConstraintReferencePosition(EConstraintFrame::Frame2, initialRelativePosition);
		spiderWebReference->ConstraintComp->UpdateConstraintFrames();
		FVector HangingForce = FVector(0.0f, 0.0f, 0.0f) * 3000.0f;
		GetCapsuleComponent()->AddForce(HangingForce);

		// Force an immediate update of component transforms

	}
	else {
		int ActionValue = Value.GetMagnitude();
		int CurrentAbility = SelectedSpiderAbility.GetValue();
		CurrentAbility = (ActionValue + CurrentAbility + SLSpiderAbility::COUNT) % SLSpiderAbility::COUNT;
		SelectedSpiderAbility = static_cast<SLSpiderAbility>(CurrentAbility);
	}

}

void ASlime_A::ModifyDamping() {
	if (isHanging) {


		// Get the positions of both agents
		FVector Position1 = GetMesh()->GetSocketLocation("SpiderWebPoint");
		FVector Position2 = spiderWebReference->ConstraintComp->GetComponentLocation();

		// Create a vector from Agent1 to Agent2
		FVector VectorFrom1To2 = Position2 - Position1;

		// Normalize the vector
		VectorFrom1To2.Normalize();
		VectorFrom1To2.Normalize();

		// To compare to vertical alignment, create a vertical vector. In Unreal, Z is usually up.
		FVector VerticalVector = FVector(0, 0, 1);

		// Calculate the dot product between the vertical vector and the vector from Agent1 to Agent2
		float DotProduct = FVector::DotProduct(VectorFrom1To2, VerticalVector);

		// Use the arccosine (acos) function to find the angle. Unreal Engine's math library uses radians, so convert it to degrees.
		float AngleInDegrees = FMath::Acos(DotProduct) * (180.0f / PI);

		// Map the clamped angle to the range of angular damping values (0 to 5)
		// If the range of AngleInDegrees is -90 to 90, then we first add 90 to the angle to get a range from 0 to 180
		// Then we divide by 180 to get a range from 0 to 1
		// Finally, we multiply by 5 to get a range from 0 to 5
		// Subtract the result from 5 to invert the mapping
		float MappedAngularDamping = 5.0f - ((AngleInDegrees + 90.0f) / 180.0f) * 5.0f;

		// Set the angular damping
		GetCapsuleComponent()->SetAngularDamping(MappedAngularDamping);

		

	}
}



void ASlime_A::keepClimbing()
{
	FVector ActorLocation = GetActorLocation();

	if (bIsClimbing && bCanClimb)
	{
		PerformClimbingBehaviour(ActorLocation);
	}
	else
	{
		PerformGroundBehaviour(ActorLocation);
	}
}

void ASlime_A::PerformClimbingBehaviour(FVector ActorLocation)
{
	ensure(TraceDistance > 0);

	TPair<TMap<FVector, FHitResult>, int32> Result = GenerateHitNormals(ActorLocation);
	TMap<FVector, FHitResult> HitNormals = Result.Key;
	int32 ImpactCount = Result.Value;

	FHitResult HitDirectionDiagonal = ExecuteDiagonalTrace(ActorLocation, TraceParams);

	if (HitNormals.Num() > 0)
	{
		// Convert the integer to FString (Unreal's string type)
		//FString Message = FString::Printf(TEXT("YourNumber: %d"), HitNormals.Num());

		// Display the message on the screen
		//FColor Color = FColor::Red; // Change the color as desired
		//float Duration = 0.1f; // Display time in seconds, change as needed
		//GEngine->AddOnScreenDebugMessage(-1, Duration, Color, Message);

		if (ImpactCount > 7)
		{
			bCanClimb = false;
			StopClimbing();
		}
		else
		{
			HandleNormalHits(HitNormals, ActorLocation, TraceParams);

			bool AllStairs = true; // Flag to track if all elements have the tag "Stairs"

			for (const auto& HitPair : HitNormals)
			{
				AActor* HitActor = HitPair.Value.GetActor();
				if (HitActor && !HitActor->ActorHasTag("Stairs"))
				{
					AllStairs = false;
					break;
				}
			}

			if (AllStairs)
			{
				StopClimbing();
			}
		}
	}
	else if (HitDirectionDiagonal.IsValidBlockingHit())
	{
		previousLocation = HitDirectionDiagonal.Location;
	}
	else
	{
		StopClimbing();
	}

	HandleFloorAndCeiling();
}



TPair<TMap<FVector, FHitResult>, int32> ASlime_A::GenerateHitNormals(FVector ActorLocation)
{
	TMap<FVector, FHitResult> HitNormals;
	int32 ImpactCount = 0;

	for (const FVector& DiagonalDirection : DiagonalDirections)
	{
		FHitResult CurrentHitResult;
		FVector EndRayLocation = ActorLocation + DiagonalDirection * TraceDistance;
		DrawDebugLinesIfNeeded(ActorLocation, EndRayLocation);

		bool bHit2 = GetWorld()->LineTraceSingleByChannel(CurrentHitResult, ActorLocation, EndRayLocation, ECC_Visibility, TraceParams);
		if (bHit2)
		{
			HitNormals.Add(CurrentHitResult.Normal, CurrentHitResult);
			ImpactCount++;
		}
	}

	return TPair<TMap<FVector, FHitResult>, int32>(HitNormals, ImpactCount);
}

FHitResult ASlime_A::ExecuteDiagonalTrace(FVector ActorLocation, FCollisionQueryParams& Params)
{
	FHitResult HitDirectionDiagonal;
	GetWorld()->LineTraceSingleByChannel(HitDirectionDiagonal, ActorLocation + MovementDirection * TraceDistance * 2, ActorLocation - GetActorUpVector() * TraceDistance, ECC_Visibility, Params);

	return HitDirectionDiagonal;
}


void ASlime_A::HandleNormalHits(TMap<FVector, FHitResult>& HitNormals, FVector ActorLocation, FCollisionQueryParams& Params)
{
	if (GetCapsuleComponent()->IsSimulatingPhysics())
		GetCapsuleComponent()->SetSimulatePhysics(false);

	FVector AverageNormal = CalculateAverageNormal(HitNormals);

	if (previousNormal != AverageNormal) {
		UpdateBaseCameraRotation(AverageNormal);
	}
	previousNormal = AverageNormal;

	FHitResult HitDirection;
	bool bHit4 = GetWorld()->LineTraceSingleByChannel(HitDirection, ActorLocation, ActorLocation + MovementDirection * TraceDistance * 2 - GetActorUpVector() * TraceDistance, ECC_Visibility, Params);

	if (bHit4)
	{
		previousLocation = HitDirection.Location;
	}
}

FVector ASlime_A::CalculateAverageNormal(TMap<FVector, FHitResult>& HitNormals)
{
	FVector AverageNormal = HitNormals.Array()[0].Key;

	if (HitNormals.Num() > 1)
	{
		AverageNormal = FVector::ZeroVector;
		for (const auto& Pair : HitNormals.Array())
		{
			FHitResult HitResult = Pair.Value;
			AverageNormal += HitResult.Normal * (1 - HitResult.Distance / TraceDistance);
		}
		AverageNormal.Normalize();
	}

	return AverageNormal;
}

void ASlime_A::HandleFloorAndCeiling()
{
	if (IsFloor(previousNormal))
	{
		StopClimbing();
	}

	if (IsCeiling(previousNormal))
	{
		AtCeiling = true;
	}
	else
	{
		AtCeiling = false;
	}
}

void ASlime_A::PerformGroundBehaviour(FVector ActorLocation)
{
	bool bHitSurface = false;
	bool bAllAreStairs = true;
	FVector SurfaceNormal;

	TPair<TMap<FVector, FHitResult>, int32> Result = GenerateHitNormals(ActorLocation);
	TMap<FVector, FHitResult> HitNormals = Result.Key;
	int32 ImpactCount = Result.Value;

	if (HitNormals.Num() > 0) {
		FVector selectedNormal;
		for (auto& It : HitNormals) {
			selectedNormal = It.Value.Normal;
			if (!IsFloor(It.Value.Normal)) {
				selectedNormal = It.Value.Normal;
				break;
			}
		}
		for (auto& It : HitNormals) {
			if (It.Value.GetActor() != nullptr){
				AActor* actor = It.Value.GetActor();
				if (!actor->ActorHasTag("Stair")) {
					bAllAreStairs = false;
					break;
				}
				
			}
		}


		if (GetCapsuleComponent()->IsSimulatingPhysics())
		{
			GetCapsuleComponent()->SetSimulatePhysics(false);
			UpdateRotation(selectedNormal);
		}

		if (IsFloor(selectedNormal))
		{
			UpdateBaseCameraRotation(selectedNormal);
			previousNormal = selectedNormal;
			AlignToPlane(selectedNormal);
		}

		else if (!IsFloor(selectedNormal))
		{
			if (ImpactCount <= 6) {
				bCanClimb = true;
				bHitSurface = true;
				SurfaceNormal = selectedNormal;
			}

		}
	}


	if (bHitSurface && !bAllAreStairs)
	{
		StartClimbing();

		if (previousNormal != SurfaceNormal)
		{
			UpdateBaseCameraRotation(SurfaceNormal);
			previousNormal = SurfaceNormal;
			AlignToPlane(SurfaceNormal);
		}

		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else
	{
		StopClimbing();
	}
}




void ASlime_A::UpdateRotationOverTime(float DeltaTime) {
	if (bIsRotating) {
		FRotator CurrentRotation = GetActorRotation();
		float RotationSpeed = 10.0f;  // Adjust this value as needed

		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, RotationSpeed);
		SetActorRotation(NewRotation);

		// If the rotation is close enough to the target rotation, stop rotating
		if ((NewRotation - TargetRotation).IsNearlyZero()) {
			bIsRotating = false;
		}
	}
}


bool ASlime_A::ExecuteGroundTrace(FVector StartLocation, FVector EndRayLocation, FCollisionQueryParams& Params, FHitResult& HitResult)
{
	return GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndRayLocation, ECC_Visibility, Params);
}

void ASlime_A::DrawDebugLinesIfNeeded(FVector StartLocation, FVector EndLocation) {
	if (bDrawDebugLines)
		DrawDebugLine(GetWorld(), StartLocation + GetActorUpVector(), EndLocation, FColor::Red, false, 0.2f, 0, 1.0f);
}




void ASlime_A::HandleClimbingBehaviour()
{
	if (!bJumpToLocation && !isHanging && canTrace)
	{
		keepClimbing();
	}

	if (bIsClimbing && !isHanging)
	{
		UpdateRotation(previousNormal);
	}
}


void ASlime_A::HandleAttachedBehaviour()
{
	if (attached)
	{
		spiderWebReference->CableComponent->EndLocation = GetMesh()->GetSocketLocation("SpiderWebPoint") - spiderWebReference->CableComponent->GetComponentLocation();
		spiderWebReference->CableComponent->CableLength = spiderWebReference->CableComponent->EndLocation.Length() * 0.80f;
	}
}

void ASlime_A::HandleHangingBehaviour()
{
	if (isHanging)
	{
		FVector Direction = GetCapsuleComponent()->GetComponentLocation() - spiderWebReference->ConstraintComp->GetComponentLocation();
		FRotator NewRotation = Direction.Rotation();
		spiderWebReference->ConstraintComp->SetWorldRotation(NewRotation);
	}
}

void ASlime_A::HandleJumpToLocationBehaviour()
{
	FVector TargetLocation = spiderWebReference->CableComponent->GetComponentLocation();
	FVector CurrentLocation = GetMesh()->GetSocketLocation("Mouth");
	FVector Direction = TargetLocation - CurrentLocation;
	Direction.Normalize();

	// Apply force to move the character
	FVector Force = Direction * 30000.0f;
	GetCapsuleComponent()->AddForce(Force);

	// Rotate the character towards the target location
	FRotator Target_Rotation = FRotationMatrix::MakeFromX(Direction).Rotator();
	SetActorRotation(Target_Rotation);

}

void ASlime_A::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	canTrace = true;
	bHasLanded = true;

}

void ASlime_A::UpdateRotation(FVector planeNormal)
{
	if (!MovementDirection.IsZero())
	{
		FVector UpVector = planeNormal;
		FVector DesiredForwardVector = MovementDirection.GetSafeNormal();
		FVector DesiredRightVector = FVector::CrossProduct(UpVector, DesiredForwardVector);
		FVector NewForwardVector = FVector::CrossProduct(DesiredRightVector, UpVector);

		// Ensure the UpVector is perpendicular to the NewForwardVector
		FVector NewUpVector = FVector::CrossProduct(NewForwardVector, DesiredRightVector).GetSafeNormal();
		FVector NewRightVector = FVector::CrossProduct(NewUpVector, NewForwardVector);

		FMatrix NewRotationMatrix = FMatrix(NewForwardVector, NewRightVector, NewUpVector, FVector::ZeroVector);
		TargetRotation = NewRotationMatrix.Rotator();
		bIsRotating = true;
	}
}

void ASlime_A::AlignToPlane(FVector planeNormal)
{
	// https://forums.unrealengine.com/t/tilting-character-to-surface-lost-in-vector-math-help-please/306736/3
	FVector currentRightVect = GetActorRightVector();
	FVector newForward = FVector::CrossProduct(currentRightVect, planeNormal);
	FVector newRight   = FVector::CrossProduct(planeNormal, newForward);

	FMatrix RotMatrix(newForward, newRight, planeNormal, FVector::ZeroVector);
	TargetRotation = RotMatrix.Rotator();
	bIsRotating = true;
}

void ASlime_A::StartClimbing() {
	bIsClimbing = true;
	bCanClimb = true;
	GetCharacterMovement()->MaxStepHeight = 0;
}
void ASlime_A::StopClimbing() {
	if (!bIsClimbing) { return; }
	bIsClimbing = false;
	GetCharacterMovement()->MaxStepHeight = DefaultMaxStepHeight;

	AtCeiling = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	GetCharacterMovement()->bOrientRotationToMovement = true;

	UpdateBaseCameraRotation(FVector::UpVector);

	


	AlignToPlane(FVector(0, 0, 1));
}

void ASlime_A::Climb(const FInputActionValue& Value)
{

	SelectedSpiderAbility = SLSpiderAbility::ThrowSpiderWeb;

	if (isHanging) {
		CutSpiderWeb();
	}
	else if (attachedAtCeiling && AtCeiling) {
		FVector CharacterPosition = GetActorLocation();
		FVector TargetPosition = spiderWebReference->ConstraintComp->GetComponentLocation();

		// Calculate the direction from the character to the target point.
		FVector TargetDirection = (TargetPosition - CharacterPosition).GetSafeNormal();

		// Get the current forward vector of the character.
		FVector CharacterForward = GetActorForwardVector();

		// Calculate the desired rotation.
		FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(CharacterForward, TargetDirection);

		// Get the second component of the physics constraint.
		TWeakObjectPtr<UPrimitiveComponent> Component2 = spiderWebReference->ConstraintComp->OverrideComponent2;

		// Check if the component is valid before applying the rotation.
		if (Component2.IsValid())
		{
			Component2->SetWorldRotation(DesiredRotation);
		}
		bhangingAnimation = true;
		isHanging = true;
		bIsRotating = false;
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		GetCapsuleComponent()->SetSimulatePhysics(true);

		spiderWebReference->ConstraintComp->SetWorldLocation(spiderWebReference->StartLocationCable->GetComponentLocation());

		spiderWebReference->ConstraintComp->SetConstrainedComponents(
			spiderWebReference->StartLocationCable, NAME_None,
			GetCapsuleComponent(), NAME_None
		);

		// Set the linear motion types to 'limited'
		spiderWebReference->ConstraintComp->ConstraintInstance.SetLinearLimits(ELinearConstraintMotion::LCM_Locked, ELinearConstraintMotion::LCM_Locked, ELinearConstraintMotion::LCM_Locked, 0.0f);
		distanceConstraints = FVector::Dist(spiderWebReference->GetActorLocation(), GetCapsuleComponent()->GetComponentLocation());


		


		// Enable the Angular Drive

	}
	else {
		if (bIsClimbing) {
			canTrace = false;
			StopClimbing();
		}
	}
}

void ASlime_A::UpdateBaseCameraRotation(FVector CurrentNormal) {
	FVector NewForward = FVector::CrossProduct(FVector::RightVector, CurrentNormal);
	FVector NewRight   = FVector::CrossProduct(CurrentNormal, NewForward);
	NewForward.Normalize();
	NewRight.Normalize();

	BaseCameraRotation = FMatrix(NewForward, NewRight, CurrentNormal, FVector::ZeroVector);
	UpdateCameraRotation();
}
void ASlime_A::UpdateCameraRotation() {
	//CameraBoom->SetRelativeRotation(BaseCameraRotation.Rotator().Quaternion() * InputRotator.Quaternion());
	CameraBoom->SetRelativeRotation(InputRotator.Quaternion());
}

FHitResult ASlime_A::PerformLineTrace(FVector StartPosition, FVector EndPosition)
{
	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, StartPosition, EndPosition, ECC_Visibility);
	return HitResult;
}

FVector ASlime_A::ReturnCenterScreen() {
	FVector2D ScreenLocation = GetViewportCenter();
	FVector LookDirection = GetLookDirection(ScreenLocation);
	return LookDirection;
}

FVector ASlime_A::ReturnCenterScreenWorld() {
	FVector2D ScreenLocation = GetViewportCenter();
	FVector LookDirection = GetLookAtLocation(ScreenLocation);
	return LookDirection;
}

void ASlime_A::ThrowSpiderWeb(bool bisHook)
{
	if (!bHasTrownSpiderWeb || bisHook==false )
	{
		if (spiderWebReference != nullptr) {
			CutSpiderWeb();
		}
		bHasTrownSpiderWeb = true;
		FVector2D ScreenLocation = GetViewportCenter();
		FVector LookDirection = GetLookDirection(ScreenLocation);
		FVector StartPosition = GetMesh()->GetSocketLocation("Mouth");
		float LineTraceDistance = 1000.0f;
		FVector EndPosition = StartPosition + (LookDirection * LineTraceDistance);
		FHitResult HitResult = PerformLineTrace(StartPosition, EndPosition);
		

		// Perform ray trace in the direction of the negative Actor Up Vector.
		FVector StartPositionRayTrace = GetActorLocation(); // Start at the actor's location.
		FVector EndPositionRayTrace = StartPositionRayTrace - (GetActorUpVector() * LineTraceDistance); // Move in the direction of the negative Actor Up Vector.

		// Perform the ray trace.
		FHitResult RayTraceHitResult;
		GetWorld()->LineTraceSingleByChannel(RayTraceHitResult, StartPositionRayTrace, EndPositionRayTrace, ECC_Visibility);

		// If the ray trace hits a static mesh, store the actor in the global variable.
		if (RayTraceHitResult.bBlockingHit && RayTraceHitResult.GetActor() != nullptr && RayTraceHitResult.GetActor()->IsA(AStaticMeshActor::StaticClass()))
		{
			previousActorCollision = RayTraceHitResult.GetActor();
		}

		if (HitResult.bBlockingHit)
		{
			SpawnAndAttachSpiderWeb(StartPosition, HitResult.Location, true, bisHook);
		}
		else
		{
			SpawnAndAttachSpiderWeb(StartPosition, EndPosition, false, bisHook);
		}
	}
}

void ASlime_A::CutThrownSpiderWeb() {
	if (SelectedSpiderAbility == SLSpiderAbility::ThrowSpiderWeb) {
		CutSpiderWeb();
	}
}




void ASlime_A::CutSpiderWeb()
{
	GetCapsuleComponent()->SetAngularDamping(0.0f);
	bHasTrownSpiderWeb = false;
	FVector spiderPoint = GetMesh()->GetSocketLocation("SpiderWebPoint");
	if (isHanging)
	{
		isHanging = false;
		bhangingAnimation = false;
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		spiderWebReference->ResetConstraint(); // Function to encapsulate resetting constraint 
	}
	else if (spiderWebReference)
	{
		// Perform the raycast
		FHitResult HitResult;
		FVector StartLocation = GetActorLocation();
		FVector EndLocation = StartLocation - GetActorUpVector() * 30.0f; // Adjust RaycastDistance to your preference
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams);

		if (bHit)
		{
			spiderWebReference->CableComponent->bAttachEnd = true;
			spiderWebReference->CableComponent->EndLocation = spiderWebReference->CableComponent->GetComponentTransform().InverseTransformPosition(HitResult.Location);

		}
		else
		{
			spiderWebReference->CableComponent->bAttachEnd = false;
		}

		// Draw debug line to visualize the raycast in the editor (optional)
		if (bHit)
		{
			DrawDebugLine(GetWorld(), StartLocation, HitResult.Location, FColor::Green, false, 5.0f, 0, 1.0f);
		}
		else
		{
			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 5.0f, 0, 1.0f);
		}
	}
	attached = false;
	attachedAtCeiling = false;
	spiderWebReference = nullptr;
	bSetInitialRelativeLocation = false;
}

FVector2D ASlime_A::GetViewportCenter()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	int32 ViewportSizeX, ViewportSizeY;
	PlayerController->GetViewportSize(ViewportSizeX, ViewportSizeY);
	return FVector2D(ViewportSizeX / 2, ViewportSizeY / 2);
}

FVector ASlime_A::GetLookDirection(FVector2D ScreenLocation)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector LookDirection;
	FVector WorldLocation;
	PlayerController->DeprojectScreenPositionToWorld(ScreenLocation.X, ScreenLocation.Y, WorldLocation, LookDirection);
	return LookDirection;
}

FVector ASlime_A::GetLookAtLocation(FVector2D ScreenLocation)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	FVector LookDirection;
	FVector WorldLocation;
	PlayerController->DeprojectScreenPositionToWorld(ScreenLocation.X, ScreenLocation.Y, WorldLocation, LookDirection);

	// Calculate the look at location using the camera's location and the look direction
	FVector LookAtLocation = PlayerController->PlayerCameraManager->GetCameraLocation() + (LookDirection * 1000); // 1000 is a placeholder distance, adjust it to your needs.

	return LookAtLocation;
}

float ASlime_A::GetHorizontalAngleToCenterScreen()
{
	FVector CharacterLocation = GetActorLocation();
	FVector CenterScreenLocation = ReturnCenterScreenWorld();

	// Calculate the direction vector from the character to the center of the screen in world space
	FVector CameraLookAtWorldDir = CenterScreenLocation - CharacterLocation;
	CameraLookAtWorldDir.Normalize();

	// Get the character's forward vector
	FVector CharacterForwardVector = GetActorForwardVector();
	CharacterForwardVector.Normalize();

	// Calculate the dot product between the direction vector and the character's forward vector
	float DotProduct = FVector::DotProduct(CameraLookAtWorldDir, CharacterForwardVector);

	// Calculate the vertical angle in radians using the dot product
	float HorizontaAngleRad = FMath::Acos(FMath::Clamp(DotProduct, -1.0f, 1.0f));

	// Convert the angle to degrees
	float HorizontalAngleDeg = FMath::RadiansToDegrees(HorizontaAngleRad);

	// Get the cross product to determine the direction of the angle
	FVector CrossProduct = FVector::CrossProduct(CharacterForwardVector, CameraLookAtWorldDir);

	// If the dot product between the cross product and the character's up vector is negative, the angle should be negative
	if (FVector::DotProduct(CrossProduct, GetActorUpVector()) < 0)
	{
		HorizontalAngleDeg = -HorizontalAngleDeg;
	}


	HorizontalAngleDeg = FMath::Clamp(HorizontalAngleDeg, -70.0f, 70.0f);


	FString AngleString = FString::Printf(TEXT("Horizontal Angle: %f degrees"), HorizontalAngleDeg);

	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, AngleString);

	return HorizontalAngleDeg;
}



float ASlime_A::GetVerticalAngleToCenterScreen()
{
	FVector CharacterLocation = GetActorLocation();
	FVector CenterScreenLocation = ReturnCenterScreenWorld();

	// Calculate the direction vector from the character to the center of the screen in world space
	FVector CameraLookAtWorldDir = CenterScreenLocation - CharacterLocation;
	CameraLookAtWorldDir.Normalize();

	// Get the character's up vector
	FVector CharacterUpVector = GetActorUpVector();
	CharacterUpVector.Normalize();

	// Calculate the dot product between the direction vector and the character's up vector
	float DotProduct = FVector::DotProduct(CameraLookAtWorldDir, CharacterUpVector);

	// Calculate the vertical angle in radians using the dot product
	float VerticalAngleRad = FMath::Acos(FMath::Clamp(DotProduct, -1.0f, 1.0f));

	// Convert the angle to degrees
	float VerticalAngleDeg = FMath::RadiansToDegrees(VerticalAngleRad);

	// Get the cross product to determine the direction of the angle
	FVector CrossProduct = FVector::CrossProduct(CharacterUpVector, CameraLookAtWorldDir);

	// If the dot product between the cross product and the character's forward vector is negative, the angle should be negative
	if (FVector::DotProduct(CrossProduct, GetActorUpVector()) < 0)
	{
		VerticalAngleDeg = -VerticalAngleDeg;
	}

	VerticalAngleDeg -= 90;
	VerticalAngleDeg *= -1;

	VerticalAngleDeg = FMath::Clamp(VerticalAngleDeg, -70.0f, 70.0f);


	FString AngleString = FString::Printf(TEXT("Vertical Angle: %f degrees"), VerticalAngleDeg);



	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, AngleString);

	return VerticalAngleDeg;
}

void ASlime_A::HandleThrownSpiderWeb() {

	if (spiderWebReference != nullptr) {
		FVector CharacterPosition = GetActorLocation();
		FVector TargetPosition = spiderWebReference->ConstraintComp->GetComponentLocation();

		// Calculate the direction from the character to the target point.
		FVector TargetDirection = (TargetPosition - CharacterPosition).GetSafeNormal();

		// Get the current forward vector of the character.
		FVector CharacterForward = GetActorForwardVector();

		// Calculate the desired rotation.
		FRotator DesiredRotation = UKismetMathLibrary::FindLookAtRotation(CharacterForward, TargetDirection);

		// Get the second component of the physics constraint.
		TWeakObjectPtr<UPrimitiveComponent> Component2 = spiderWebReference->ConstraintComp->OverrideComponent2;

		// Check if the component is valid before applying the rotation.
		if (Component2.IsValid())
		{
			Component2->SetWorldRotation(DesiredRotation);
		}

		isHanging = true;
		bIsRotating = false;
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		GetCapsuleComponent()->SetSimulatePhysics(true);
		GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);

		spiderWebReference->ConstraintComp->SetWorldLocation(spiderWebReference->StartLocationCable->GetComponentLocation());

		spiderWebReference->ConstraintComp->SetConstrainedComponents(
			spiderWebReference->StartLocationCable, NAME_None,
			GetCapsuleComponent(), NAME_None
		);

		// Set the linear motion types to 'limited'
		spiderWebReference->ConstraintComp->ConstraintInstance.SetLinearLimits(ELinearConstraintMotion::LCM_Locked, ELinearConstraintMotion::LCM_Locked, ELinearConstraintMotion::LCM_Locked, 0.0f);
		distanceConstraints = FVector::Dist(spiderWebReference->GetActorLocation(), GetCapsuleComponent()->GetComponentLocation());

	}
	
}

void ASlime_A::SpawnAndAttachSpiderWeb(FVector Location, FVector HitLocation, bool bAttached, bool bIsHook)
{
	spiderWebReference = GetWorld()->SpawnActor<ASpiderWeb>(ASpiderWeb::StaticClass(), Location, FRotator::ZeroRotator);
	spiderWebReference->CableComponent->bAttachEnd = true;
	spiderWebReference->CableComponent->EndLocation = FVector(0, 0, 0);
	spiderWebReference->CableComponent->SetAttachEndToComponent(GetMesh(), "Mouth");
	spiderWebReference->setFuturePosition(HitLocation, this, bAttached, bIsHook);
}

void ASlime_A::SpawnStunningWeb(FVector Location, FVector HitLocation)
{
	spiderWebReference = GetWorld()->SpawnActor<ASpiderWeb>(ASpiderWeb::StaticClass(), Location, FRotator::ZeroRotator);
	spiderWebReference->CableComponent->bAttachEnd = true;
	spiderWebReference->CableComponent->EndLocation = FVector(0, 0, 0);
	spiderWebReference->CableComponent->SetAttachEndToComponent(GetMesh(), "Mouth");
}

void ASlime_A::PutTrap()
{
	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FGameplayTag::RequestGameplayTag(TEXT("Input.PutTrap.Started")), Payload);
	
	/*	Implemented in GAS -> GA_PutTrap
	FVector spiderPoint = GetMesh()->GetSocketLocation("SpiderWebPoint");

	FHitResult Hit;
	FHitResult Hit2;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, spiderPoint, spiderPoint + GetActorUpVector() * -TraceDistance, ECC_Visibility, Params);
	bool bHit2 = GetWorld()->LineTraceSingleByChannel(Hit2, spiderPoint, spiderPoint + FVector::UpVector * -3000.0f, ECC_Visibility, Params);
	DrawDebugLine(GetWorld(), spiderPoint + GetActorUpVector(), spiderPoint + FVector::UpVector * -3000.0f, FColor::Red, false, 0.2f, 0, 1.0f);

	if (bHit)
	{
		FVector cablePosition = Hit.Location;
		ASpiderWeb* spiderWebTrap = GetWorld()->SpawnActor<ASpiderWeb>(ASpiderWeb::StaticClass(), cablePosition, FRotator::ZeroRotator);
		spiderWebTrap->CableComponent->bAttachEnd = true; // Attach the end of the cable to the spider web
		if (bHit2)
			spiderWebTrap->CableComponent->EndLocation = -(spiderWebTrap->CableComponent->GetComponentLocation() - Hit2.Location);
		else
			spiderWebTrap->CableComponent->EndLocation = spiderWebTrap->CableComponent->GetComponentLocation() - (spiderPoint + FVector::UpVector * 3000.0f);
		spiderWebTrap->SetTrap();
	}
	*/
}

FVector ASlime_A::getVectorInConstraintCoordinates(FVector input, float Speed, float DeltaTime) {

	// Get the world space location of the physics constraint and the bone
	FVector WorldBoneLocation = GetCapsuleComponent()->GetComponentLocation();

	// Convert the constraint location to the local space of the bone
	FTransform boneTransform = GetCapsuleComponent()->GetComponentTransform();

	// Normalize the input vector
	FVector NormalizedInput = input.GetSafeNormal();

	// Apply speed and DeltaTime to the normalized input vector
	FVector Velocity = NormalizedInput * Speed * DeltaTime;

	// Update input with velocity
	FVector inputAdapted = boneTransform.InverseTransformPosition(WorldBoneLocation + Velocity);

	return inputAdapted;
}

FVector ASlime_A::getRelativePositionPhysicsConstraint() {

	// Get the world space location of the physics constraint and the bone
	FVector WorldConstraintLocation = spiderWebReference->ConstraintComp->GetComponentLocation();
	FVector WorldBoneLocation = GetCapsuleComponent()->GetComponentLocation();

	// Convert the constraint location to the local space of the bone
	FTransform boneTransform = GetCapsuleComponent()->GetComponentTransform();

	FVector LocalConstraintLocation = boneTransform.InverseTransformPosition(WorldConstraintLocation);

	return LocalConstraintLocation;
}

void ASlime_A::JumpToPosition() {
	bJumpToLocation = true;
	GetCapsuleComponent()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetEnableGravity(false);
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);

}

void ASlime_A::StopJumpToPosition() {
	bJumpToLocation = false;
	bHasTrownSpiderWeb = false;
	GetCapsuleComponent()->SetSimulatePhysics(false);
	GetCapsuleComponent()->SetEnableGravity(true);
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(false);
	spiderWebReference->CableComponent->SetAttachEndToComponent(nullptr, NAME_None);
	spiderWebReference->CableComponent->bAttachEnd = false; // Attach the end of the cable to the spider web
	spiderWebReference = nullptr;
}




void ASlime_A::PutSpiderWebAbility() {

	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FGameplayTag::RequestGameplayTag(TEXT("Input.PutSpiderWeb.Started")), Payload);

	/*	Implemented in GAS -> GA_PutSpiderWeb
	if (bJumpToLocation)
		return;

	FVector spiderPoint = GetMesh()->GetSocketLocation("SpiderWebPoint");

	if (spiderWebReference != nullptr)
	{
		CutSpiderWeb();
	}
	else
	{
		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, spiderPoint, spiderPoint + GetActorUpVector() * -TraceDistance, ECC_Visibility, Params);

		if (bHit)
		{
			FVector cablePosition = Hit.Location;
			spiderWebReference = GetWorld()->SpawnActor<ASpiderWeb>(ASpiderWeb::StaticClass(), cablePosition, FRotator::ZeroRotator);
			spiderWebReference->CableComponent->EndLocation = spiderPoint - spiderWebReference->CableComponent->GetComponentLocation();
			spiderWebReference->CableComponent->bAttachEnd = true; // Attach the end of the cable to the spider web
			attached = true;
			attachedAtCeiling = IsCeiling(previousNormal);
		}
	}
	*/
}

void ASlime_A::ThrowAbility(const FInputActionValue& Value)
{
	switch (SelectedSpiderAbility)
	{
	case SLSpiderAbility::PutSpiderWeb: PutSpiderWebAbility(); break;
	case SLSpiderAbility::Hook: ThrowSpiderWeb(true); break;
	case SLSpiderAbility::ThrowSpiderWeb: ThrowSpiderWeb(false); break;
	case SLSpiderAbility::PutTrap: PutTrap(); break;
	case SLSpiderAbility::MeleeAttack: MeleeAttack(); break;
	case SLSpiderAbility::ThrowStunningWeb: ThrowStunningWeb(); break;

	default:
		break;
	}
}

void ASlime_A::AimAbility(const FInputActionValue& value)
{
	switch (SelectedSpiderAbility)
	{
		case SLSpiderAbility::ThrowStunningWeb: AimStunningWeb(); break;

		default:
			break;
	}
}

void ASlime_A::StopAimingAbility(const FInputActionValue& value)
{
	SetCrosshairVisibility(false);
}

void ASlime_A::MeleeAttack() {
	if (fBlendingFactor == 0) {

		fBlendingFactor = 1.0f;
	}
	
}

void ASlime_A::MeleeAttackTriggered()
{
	// Get the bone indices of the specific bones you want to cast rays from
	FVector LFootLocation = GetMesh()->GetBoneLocation("l_foot");
	FVector RFootLocation = GetMesh()->GetBoneLocation("r_foot");

	// Calculate the start and end locations as the middle points between the bones
	FVector TraceStart = (LFootLocation + RFootLocation) * 0.5f;
	float TraceRadius = 10.0f; // Adjust the radius as needed

	// Perform the sphere trace using SweepSingleByChannel
	FCollisionShape TraceShape = FCollisionShape::MakeSphere(TraceRadius);
	FCollisionQueryParams TraceParameters;
	TraceParameters.AddIgnoredActor(this);
	FHitResult HitResult;
	bool bHit = GetWorld()->SweepSingleByChannel(HitResult, TraceStart, TraceStart, FQuat::Identity, ECC_Visibility, TraceShape, TraceParameters);

	// DrawDebugSphere for visualization
	//DrawDebugSphere(GetWorld(), TraceStart, TraceRadius, 12, FColor::Red, false, 1.0f, 0, 1.0f);

	if (bHit)
	{
		AActor* OtherActor = HitResult.GetActor();
		if (OtherActor && OtherActor->IsA<ASLSoldier>())
		{
			ASLSoldier* Soldier = Cast<ASLSoldier>(OtherActor);
			if (Soldier)
			{
				Soldier->ReceiveDamage();
			}
		}
		
	}
}


void ASlime_A::ResetBlendingFactor() {
	fBlendingFactor = 0.0f;
}



void ASlime_A::StopMoving(const FInputActionValue& Value) {
	if (bIsClimbing) {
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}
}

void ASlime_A::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	FVector2D MovementVector = Value.Get<FVector2D>();
	if (MovementVector.IsNearlyZero()) return; // No movement input, early return

	FRotator Rotation = FollowCamera->GetComponentRotation();
	FRotator YawRotation(0, Rotation.Yaw, 0);
	FVector ForwardDirection = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
	FVector RightDirection = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);

	MovementDirection = ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X;
	MovementDirection.Normalize();

	if (isHanging)
	{
		FVector HangingForce = MovementDirection * 15000.0f;
		GetCapsuleComponent()->AddForce(HangingForce);
	}
	else
	{
		if (bIsClimbing)
		{
			FVector ClimbingDirection;
			if (FMath::Abs(previousNormal.Z) > 0.9f) // Ceiling or floor
			{
				RightDirection = -FVector::CrossProduct(previousNormal, FVector::RightVector).GetSafeNormal();
			}
			else // Wall
			{
				RightDirection = FVector::CrossProduct(previousNormal, FVector::UpVector).GetSafeNormal();
			}

			ClimbingDirection = (previousLocation - GetActorLocation()).GetSafeNormal();
			MovementDirection = FVector::VectorPlaneProject(MovementDirection, previousNormal);
			AddMovementInput(ClimbingDirection);

		}
		else
		{
			AddMovementInput(MovementDirection);
		}
	}
}




void ASlime_A::Look(const FInputActionValue& Value) {
	if (Controller == nullptr) { return; }

	FVector2D LookAxisVector = Value.Get<FVector2D>();
	InputRotator.Yaw   += LookAxisVector.X;
	InputRotator.Pitch -= LookAxisVector.Y;
	if (InputRotator.Pitch >  MaxCameraPitch) { InputRotator.Pitch =  MaxCameraPitch; }
	if (InputRotator.Pitch < -MinCameraPitch) { InputRotator.Pitch = -MinCameraPitch; }

	UpdateCameraRotation();
}

void ASlime_A::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlime_A::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ASlime_A::StopMoving);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlime_A::Look);

		EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Started, this, &ASlime_A::Climb);

		EnhancedInputComponent->BindAction(throwAbilityAction, ETriggerEvent::Started, this, &ASlime_A::ThrowAbility);
		EnhancedInputComponent->BindAction(throwAbilityAction, ETriggerEvent::Completed, this, &ASlime_A::CutThrownSpiderWeb);

		EnhancedInputComponent->BindAction(AimAbilityAction, ETriggerEvent::Started, this, &ASlime_A::AimAbility);
		EnhancedInputComponent->BindAction(AimAbilityAction, ETriggerEvent::Completed, this, &ASlime_A::StopAimingAbility);

		EnhancedInputComponent->BindAction(DebugAction, ETriggerEvent::Started, this, &ASlime_A::ToggleDrawDebugLines);

		EnhancedInputComponent->BindAction(SwitchAbilityAction, ETriggerEvent::Started, this, &ASlime_A::SwitchAbility);

		// Slow Time Ability
		EnhancedInputComponent->BindAction(SlowTimeAbility, ETriggerEvent::Started, this, &ASlime_A::SlowTime);
		EnhancedInputComponent->BindAction(SlowTimeAbility, ETriggerEvent::Completed, this, &ASlime_A::SlowTimeEnd);

		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ASlime_A::Interact);

		EnhancedInputComponent->BindAction(MeleeAttackAction, ETriggerEvent::Started, this, &ASlime_A::MeleeAttack);

	}
}

void ASlime_A::ToggleDrawDebugLines(const FInputActionValue& Value) {
	bDrawDebugLines = !bDrawDebugLines;
}

UAbilitySystemComponent* ASlime_A::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UPostProcessComponent* ASlime_A::GetPostProcessComponent() const 
{
	return PostProcessComponent;
}

void ASlime_A::Interact(const FInputActionValue& Value)
{
	InteractingComponent->TryToInteract();
}

// ============== Slow Time Ability
void ASlime_A::SlowTime(const FInputActionValue& Value)
{	
	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FGameplayTag::RequestGameplayTag(TEXT("Input.SlowTime.Started")), Payload);
}

void ASlime_A::SlowTimeEnd(const FInputActionValue& Value) 
{
	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FGameplayTag::RequestGameplayTag(TEXT("Input.SlowTime.Completed")), Payload);
}

// Stunning Web Ability
void ASlime_A::AimStunningWeb()
{
	SetCrosshairVisibility(true);
}

#include "DrawDebugHelpers.h"

void ASlime_A::ThrowStunningWeb()
{
	if (spiderWebReference != nullptr) {
		CutSpiderWeb();
	}

	bHasTrownSpiderWeb = true;
	FVector2D ScreenLocation = GetViewportCenter();
	FVector LookDirection = GetLookDirection(ScreenLocation);
	FVector StartPosition = GetMesh()->GetSocketLocation("Mouth");

	// Spawn the projectile
	if (ASpiderProjectile* Projectile = GetWorld()->SpawnActor<ASpiderProjectile>(ASpiderProjectile::StaticClass(), StartPosition, FRotator::ZeroRotator))
	{
		// Ignore ASpider
		Projectile->SphereCollider->IgnoreActorWhenMoving(this, true);
		// Make ASpider ignore the Projectile
		GetCapsuleComponent()->IgnoreActorWhenMoving(Projectile, true);
		GetMesh()->IgnoreActorWhenMoving(Projectile, true);

		// Set projectile velocity
		if (Projectile->ProjectileMovementComponent != nullptr) {
			Projectile->ProjectileMovementComponent->InitialSpeed = 2000.0f;
			Projectile->ProjectileMovementComponent->MaxSpeed =  2000.0f;
			Projectile->ProjectileMovementComponent->Velocity = LookDirection * 1000.0f;
		}


		
	}
}







	
void ASlime_A::SetStaminaRecoveryValue(float Value)
{
	StaminaAttributeSet->SetStaminaRecoveryValue(Value);
}
