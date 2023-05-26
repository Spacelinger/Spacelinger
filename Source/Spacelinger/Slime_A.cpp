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
#include "DrawDebugHelpers.h" // Include this header file for the DrawDebugLine function
#include "Components/MCV_AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/StaminaAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"


//////////////////////////////////////////////////////////////////////////
// ASlime

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

	// Create GAS' Ability System Component
	AbilitySystemComponent = CreateDefaultSubobject<UMCV_AbilitySystemComponent>(TEXT("AbilitySystem"));
	StaminaAttributeSet = CreateDefaultSubobject<UStaminaAttributeSet>(TEXT("StaminaAttributeSet"));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASlime_A::keepClimbing()
{
	FVector ActorLocation = GetActorLocation();

	if (bIsClimbing)
	{
		ensure(TraceDistance > 0);

		TMap<FVector, FHitResult> HitNormals;
		FCollisionQueryParams Params2;
		Params2.AddIgnoredActor(this);

		for (const FVector& DiagonalDirection : DiagonalDirections)
		{
			FHitResult CurrentHitResult;
			FVector EndRayLocation = ActorLocation + DiagonalDirection * TraceDistance;
			if (bDrawDebugLines) DrawDebugLine(GetWorld(), ActorLocation + GetActorUpVector(), EndRayLocation, FColor::Red, false, 0.2f, 0, 1.0f);

			bool bHit2 = GetWorld()->LineTraceSingleByChannel(CurrentHitResult, ActorLocation, EndRayLocation, ECC_Visibility, Params2);
			if (bHit2)
			{
				HitNormals.Add(CurrentHitResult.Normal, CurrentHitResult);
			}
		}

		FHitResult HitDirectionDiagonal;
		FCollisionQueryParams Params3;
		Params3.AddIgnoredActor(this);
		bool bHit3 = GetWorld()->LineTraceSingleByChannel(HitDirectionDiagonal, ActorLocation + MovementDirection * TraceDistance * 2, ActorLocation - GetActorUpVector() * TraceDistance, ECC_Visibility, Params3);

		if (HitNormals.Num() > 0)
		{
			if (GetCapsuleComponent()->IsSimulatingPhysics())
				GetCapsuleComponent()->SetSimulatePhysics(false);

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

			previousNormal = AverageNormal;

			FHitResult HitDirection;
			FCollisionQueryParams Params4;
			Params4.AddIgnoredActor(this);
			bool bHit4 = GetWorld()->LineTraceSingleByChannel(HitDirection, ActorLocation, ActorLocation + MovementDirection * TraceDistance * 2 - GetActorUpVector() * TraceDistance, ECC_Visibility, Params3);

			if (bHit4)
			{
				previousLocation = HitDirection.Location;
			}
		}
		else if (bHit3)
		{
			previousLocation = HitDirectionDiagonal.Location;
		}
		else {
			StopClimbing();
		}

		if (IsFloor(previousNormal))
		{
			StopClimbing();
		}

		if (IsCeiling(previousNormal)) {
			AtCeiling = true;
		}
		else {
			AtCeiling = false;
		}
	}
	else
	{

		/*
		// Get the actor's location and forward vector
		ActorLocation = GetActorLocation();
		FVector ActorForwardVector = GetActorForwardVector();

		// Calculate the end point of the ray
		FVector RayEnd = ActorLocation + ActorForwardVector* TraceDistance;  // Change 1000.0f to the desired length of the ray

		// Get the height of the character's step
		float StepMaxHeight = GetCharacterMovement()->MaxStepHeight/4.f;

		// Offset the end point of the ray by the step max height
		RayEnd.Z += StepMaxHeight;

		// Setup the trace parameters
		FCollisionQueryParams Params;
		Params.bTraceComplex = true; // Enable tracing against complex collision geometry
		Params.bReturnPhysicalMaterial = true; // Return information about the physical material of the hit object

		// Perform the line trace
		FHitResult HitResult;
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ActorLocation, RayEnd, ECC_Visibility, Params);
		*/

		FCollisionQueryParams Params;
		bool bHitSurface = false;
		FVector SurfaceNormal;

		for (const FVector& DiagonalDirection : DiagonalDirections)
		{
			FHitResult HitResult;
			FVector UpwardsDirection = FVector::UpVector * GetCharacterMovement()->MaxStepHeight;
			FVector EndRayLocation = ActorLocation + DiagonalDirection * TraceDistance;
			Params.AddIgnoredActor(this);
			float bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ActorLocation, EndRayLocation, ECC_Visibility, Params);
			if (bHit && GetCapsuleComponent()->IsSimulatingPhysics()) {
				GetCapsuleComponent()->SetSimulatePhysics(false);
				UpdateRotation(HitResult.Normal);
			}
			if (bHit && !IsFloor(HitResult.Normal))
			{
				bHitSurface = true;
				SurfaceNormal = HitResult.Normal;
				if (bDrawDebugLines) DrawDebugLine(GetWorld(), HitResult.Location, HitResult.Location + HitResult.Normal * TraceDistance, FColor::Red, false, 0.2f, 0, 1.0f);
				break;
			}
		}

		if (bHitSurface)
		{
			StartClimbing();

			if (previousNormal != SurfaceNormal) {
				previousNormal = SurfaceNormal;
				AlignToPlane(SurfaceNormal);
			}

			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		else {
			StopClimbing();
		}
	}
}

void ASlime_A::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!isHanging && canTrace) {
		keepClimbing();
	}

	if (bIsClimbing && !isHanging) {
		UpdateRotation(previousNormal);
	}
	
	
	if (attached) {
		spiderWebReference->CableComponent->EndLocation = GetMesh()->GetSocketLocation("SpiderWebPoint") - spiderWebReference->CableComponent->GetComponentLocation();
		spiderWebReference->CableComponent->CableLength = spiderWebReference->CableComponent->EndLocation.Length() * 0.80f;
	}

	if (isHanging) {
		FVector Direction = GetCapsuleComponent()->GetComponentLocation() - spiderWebReference->ConstraintComp->GetComponentLocation();
		FRotator NewRotation = Direction.Rotation();
		spiderWebReference->ConstraintComp->SetWorldRotation(NewRotation);
	}
}

void ASlime_A::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	canTrace = true;

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
		FRotator NewRotation = NewRotationMatrix.Rotator();
		SetActorRotation(NewRotation);
	}
}

void ASlime_A::AlignToPlane(FVector planeNormal)
{
	// NOTE(Sergi): Wizardry I found online
	//https://forums.unrealengine.com/t/tilting-character-to-surface-lost-in-vector-math-help-please/306736/3
	FVector currentRightVect = GetActorRightVector();
	FVector newForward = FVector::CrossProduct(currentRightVect, planeNormal);
	FVector newRight   = FVector::CrossProduct(planeNormal, newForward);

	FTransform newTransform(newForward, newRight, planeNormal, GetActorLocation());
	SetActorRotation(newTransform.Rotator());
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

	//GAS
	UAbilitySystemComponent* asc = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(this);
	
	if(ensureMsgf(AbilitySystemComponent, TEXT("Missing Ability System component for %s"), *GetOwner()->GetName())){
		asc->SetNumericAttributeBase(UStaminaAttributeSet::GetMaxStaminaAttribute(), static_cast<float>(MaxStamina));
		asc->SetNumericAttributeBase(UStaminaAttributeSet::GetStaminaAttribute(), static_cast<float>(MaxStamina));
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASlime_A::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlime_A::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ASlime_A::StopMoving);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlime_A::Look);

		EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Started, this, &ASlime_A::Climb);

		EnhancedInputComponent->BindAction(putSpiderWebAction, ETriggerEvent::Started, this, &ASlime_A::PutSpiderWeb);

		EnhancedInputComponent->BindAction(increaseSpiderWebAction, ETriggerEvent::Started,   this, &ASlime_A::ModifySpiderWeb);
		EnhancedInputComponent->BindAction(increaseSpiderWebAction, ETriggerEvent::Completed, this, &ASlime_A::LockSpiderWeb);

		EnhancedInputComponent->BindAction(DebugAction, ETriggerEvent::Started, this, &ASlime_A::ToggleDrawDebugLines);

		// Slow Time Ability
		EnhancedInputComponent->BindAction(SlowTimeAction, ETriggerEvent::Started,   this, &ASlime_A::SlowTime);
		EnhancedInputComponent->BindAction(SlowTimeAction, ETriggerEvent::Completed, this, &ASlime_A::SlowTimeEnd);
	}
}

void ASlime_A::LockSpiderWeb(const FInputActionValue& Value) {
	//Enable physics simulation
    //GetCapsuleComponent()->SetSimulatePhysics(true);

	// Reactivate the constraint
	//spiderWebReference->ConstraintComp->Activate();
}

void ASlime_A::ModifySpiderWeb(const FInputActionValue& Value)
{
	if (AtCeiling && attachedAtCeiling && spiderWebReference != nullptr)
	{
		if (!isHanging) {
			isHanging = true;
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			GetCapsuleComponent()->SetSimulatePhysics(true);

			spiderWebReference->ConstraintComp->SetWorldLocation(spiderWebReference->StartLocationCable->GetComponentLocation());

			spiderWebReference->ConstraintComp->SetConstrainedComponents(
				spiderWebReference->StartLocationCable, NAME_None,
				GetCapsuleComponent(), NAME_None
			);

			// Set the linear motion types to 'limited'
			spiderWebReference->ConstraintComp->SetLinearXLimit(ELinearConstraintMotion::LCM_Limited, 0.0f);
			spiderWebReference->ConstraintComp->SetLinearYLimit(ELinearConstraintMotion::LCM_Limited, 0.0f);
			spiderWebReference->ConstraintComp->SetLinearZLimit(ELinearConstraintMotion::LCM_Limited, 0.0f);

		}
		else {
			
			float LinearLimit = spiderWebReference->ConstraintComp->ConstraintInstance.GetLinearLimit();
			float newLimit = LinearLimit + Value.GetMagnitude() * 2000.0f * GetWorld()->GetDeltaSeconds();
			newLimit = FMath::Max(0, newLimit);
			spiderWebReference->ConstraintComp->ConstraintInstance.SetLinearLimits(LCM_Limited, LCM_Limited, LCM_Limited, newLimit);
			spiderWebReference->ConstraintComp->ConstraintInstance.UpdateLinearLimit();
		}
	}
}

void ASlime_A::PutSpiderWeb(const FInputActionValue& Value)
{
	FVector spiderPoint = GetMesh()->GetSocketLocation("SpiderWebPoint");

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, spiderPoint, spiderPoint + GetActorUpVector() * -TraceDistance, ECC_Visibility, Params);

	if (spiderWebReference != nullptr)
	{
		if (isHanging) {
			isHanging = false;
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);

			spiderWebReference->ConstraintComp->SetLinearXLimit(ELinearConstraintMotion::LCM_Free, 0.0f);
			spiderWebReference->ConstraintComp->SetLinearYLimit(ELinearConstraintMotion::LCM_Free, 0.0f);
			spiderWebReference->ConstraintComp->SetLinearZLimit(ELinearConstraintMotion::LCM_Free, 0.0f);
	
			spiderWebReference->ConstraintComp->SetConstrainedComponents(
				nullptr, NAME_None,
				nullptr, NAME_None
			);
			spiderWebReference->CableComponent->bAttachEnd = false;
			spiderWebReference->ConstraintComp = nullptr;
		}
		else {
			if (bHit)
			{
				spiderWebReference->CableComponent->EndLocation = Hit.Location - spiderWebReference->CableComponent->GetComponentLocation();
				spiderWebReference->CableComponent->bAttachEnd = true; // Attach the end of the cable to the spider web
			}
			else
			{
				// Detach the cable from the spider web
				spiderWebReference->CableComponent->bAttachEnd = false;
			}
		}
		attached = false;
		attachedAtCeiling = false;
		spiderWebReference = nullptr;
	}
	else
	{
		if (bHit)
		{
			FVector cablePosition = Hit.Location;
			spiderWebReference = GetWorld()->SpawnActor<ASpiderWeb>(ASpiderWeb::StaticClass(), cablePosition, FRotator::ZeroRotator);
			spiderWebReference->CableComponent->EndLocation = spiderPoint - spiderWebReference->CableComponent->GetComponentLocation();
			spiderWebReference->CableComponent->bAttachEnd = true; // Attach the end of the cable to the spider web
			attached = true;

			
			if (IsCeiling(previousNormal))
				attachedAtCeiling = true; // Since it's attached to the spider, it's not attached to the ceiling
			else
				attachedAtCeiling = false;
			// Connect the existing physics constraint to the spider and the end of the cable component
		}
	}
}


void ASlime_A::StopMoving(const FInputActionValue& Value) {
	if (bIsClimbing) {
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}
}

void ASlime_A::Move(const FInputActionValue& Value)
{
	if (!Controller)
	{
		return;
	}
	if (!canTrace)
		return;

	FVector2D MovementVector = Value.Get<FVector2D>();

	FVector RightDirection;
	FVector ForwardDirection;
	FRotator Rotation = FollowCamera->GetComponentRotation();
	FRotator YawRotation(0, Rotation.Yaw, 0);
	FVector ClimbingDirection;

	if (bIsClimbing)
	{
		if (FMath::Abs(previousNormal.Z) > 0.9f) // Ceiling or floor
		{
			RightDirection = -FVector::CrossProduct(previousNormal, FVector::RightVector).GetSafeNormal();
		}
		else // Wall
		{
			FVector WorldUpVector = FVector::UpVector;
			RightDirection = FVector::CrossProduct(previousNormal, WorldUpVector).GetSafeNormal();
		}

		ClimbingDirection = (previousLocation - GetActorLocation()).GetSafeNormal();
	}

	ForwardDirection = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
	RightDirection = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);

	MovementDirection.Normalize();
	MovementDirection = ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X;

	if (isHanging)
	{
		// When hanging, apply a force instead of the usual movement.
		// This is just an example, you'll want to adjust this based on your specific needs.
		FVector HangingForce = MovementDirection * 3000.0f;
		GetCapsuleComponent()->AddForce(HangingForce);
	}
	else {
		if (bIsClimbing && !previousNormal.IsNearlyZero())
		{
			MovementDirection = FVector::VectorPlaneProject(MovementDirection, previousNormal);
		}

		if (bIsClimbing)
		{
			AddMovementInput(ClimbingDirection);
		}
		else
		{
			AddMovementInput(MovementDirection);
		}
	}
}

void ASlime_A::StartClimbing() {
	bIsClimbing = true;
	GetCharacterMovement()->MaxStepHeight = 0;
}
void ASlime_A::StopClimbing() {
	if (!bIsClimbing) { return; }
	bIsClimbing = false;
	GetCharacterMovement()->MaxStepHeight = DefaultMaxStepHeight;

	AtCeiling = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// Set actor rotation to controller's rotation
	if (Controller) {
		FRotator ControllerRotator = Controller->GetControlRotation();
		FRotator NewRotator(0, ControllerRotator.Yaw, 0);
		SetActorRotation(NewRotator);
	}
}

#include <Kismet/KismetMathLibrary.h>
void ASlime_A::Climb(const FInputActionValue& Value)
{
	if (bIsClimbing) {
		canTrace = false;
		StopClimbing();
	}
}

void ASlime_A::Look(const FInputActionValue& Value) {
	if (Controller == nullptr) { return; }

	FVector2D LookAxisVector = Value.Get<FVector2D>();
	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ASlime_A::ToggleDrawDebugLines(const FInputActionValue& Value) {
	bDrawDebugLines = !bDrawDebugLines;
}

// ============== Slow Time Ability
void ASlime_A::SlowTime(const FInputActionValue& Value) {
	
	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FGameplayTag::RequestGameplayTag(TEXT("Input.SlowTime.Started")), Payload);
}

void ASlime_A::SlowTimeEnd(const FInputActionValue& Value) {

	FGameplayEventData Payload;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FGameplayTag::RequestGameplayTag(TEXT("Input.SlowTime.Completed")), Payload);
}
