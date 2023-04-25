// Without Copyright. Project SpaceLinger, Master en Creación de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

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


//////////////////////////////////////////////////////////////////////////
// ASlime

ASlime_A::ASlime_A()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(13.0f, 13.0f);



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

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASlime_A::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	FVector ActorLocation = GetActorLocation();


	if (bIsClimbing) {
		ActorLocation = GetActorLocation();
		ensure(TraceDistance > 0);

		// We won't store normals that are from the same wall.
		TMap<FVector, FHitResult> HitNormals;
		FCollisionQueryParams Params2;
		Params2.AddIgnoredActor(this);

		for (int i = 0; i < DiagonalDirections.Num(); ++i) {
			FHitResult CurrentHitResult;
			FVector EndRayLocation = ActorLocation + DiagonalDirections[i] * TraceDistance;
			if (bDrawDebugLines) DrawDebugLine(GetWorld(), ActorLocation, EndRayLocation, FColor::Red, false, 0.2f, 0, 1.0f);
			bool bHit2 = GetWorld()->LineTraceSingleByChannel(CurrentHitResult, ActorLocation, EndRayLocation, ECC_Visibility, Params2);
			if (bHit2) {
				HitNormals.Add(CurrentHitResult.Normal, CurrentHitResult);
			}
		}

		if (HitNormals.Num() > 0) {
			FVector AverageNormal = HitNormals.Array()[0].Key;

			// We'll do a weighted average, closer hits will determine more how the resulting normal looks like
			if (HitNormals.Num() > 1) {
				AverageNormal = FVector::ZeroVector;
				for (auto &Pair : HitNormals.Array()) {
					FHitResult HitResult = Pair.Value;
					AverageNormal += HitResult.Normal * (1 - HitResult.Distance/TraceDistance);
				}
				AverageNormal.Normalize();
			}

			previousNormal = AverageNormal;
			AlignToPlane(previousNormal);
		}
		else {
			StopClimbing();
		}

		if (IsFloor(previousNormal)) {
			StopClimbing();
		}
	}
	else {
		FVector ForwardDirection = GetActorForwardVector();
		FVector DownwardDirection = -GetActorUpVector();
		FVector DiagonalDirection = (ForwardDirection + DownwardDirection).GetSafeNormal();

		FVector RightDirection = GetActorRightVector();
		// Perform line trace from actor location to diagonal direction
		FHitResult HitResult1;
		FCollisionQueryParams Params1;
		Params1.AddIgnoredActor(this);
		bool bHit1 = GetWorld()->LineTraceSingleByChannel(HitResult1, ActorLocation, ActorLocation + DiagonalDirection * TraceDistance, ECC_Visibility, Params1);

		// If it's hit and not the floor
		if (bHit1 && !IsFloor(HitResult1.Normal)) {
			if (bDrawDebugLines) DrawDebugLine(GetWorld(), HitResult1.Location, HitResult1.Location + HitResult1.Normal * TraceDistance, FColor::Red, false, 0.2f, 0, 1.0f);
			StartClimbing();

			if (previousNormal != HitResult1.Normal) {
				previousNormal = HitResult1.Normal;
				AlignToPlane(HitResult1.Normal);
			}

			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		else {
			StopClimbing();
			if (bDrawDebugLines) DrawDebugLine(GetWorld(), ActorLocation, ActorLocation + DiagonalDirection * TraceDistance, FColor::Red, false, 0.2f, 0, 1.0f);
		}
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

		EnhancedInputComponent->BindAction(DebugAction, ETriggerEvent::Started, this, &ASlime_A::ToggleDrawDebugLines);
	}
}

void ASlime_A::StopMoving(const FInputActionValue& Value) {
	if (bIsClimbing) {
		GetCharacterMovement()->Velocity = FVector::ZeroVector;
	}
}

void ASlime_A::Move(const FInputActionValue& Value) {
	if (Controller == nullptr) { return; }

	FVector2D MovementVector = Value.Get<FVector2D>();

	FVector RightDirection;
	FVector ForwardDirection;
	// Perform line trace from actor location to diagonal direction
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);

	// Draw the line
	if (bIsClimbing) {
		RightDirection = GetActorRightVector();
		ForwardDirection = GetActorForwardVector();
	}
	else {
		// Add movement directly up if at the ceiling
		RightDirection   = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);
		ForwardDirection = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::X);
	}

	MovementDirection.Normalize();
	MovementDirection = ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X;
	AddMovementInput(MovementDirection);
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
		StopClimbing();
		return;
	}

	FVector ActorLocation = GetActorLocation();
	FVector ForwardDirection = GetActorForwardVector();
	FVector DownwardDirection = -GetActorUpVector();
	FVector DiagonalDirection = (ForwardDirection + DownwardDirection).GetSafeNormal();

	FVector RightDirection = GetActorRightVector();
	// Perform line trace from actor location to diagonal direction
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ActorLocation, ActorLocation + DiagonalDirection * TraceDistance, ECC_Visibility, Params);

	// If it's hit and not the floor
	if (bHit && !IsFloor(HitResult.Normal)) {
		if (bDrawDebugLines) DrawDebugLine(GetWorld(), HitResult.Location, HitResult.Location + HitResult.Normal * TraceDistance, FColor::Red, false, 0.2f, 0, 1.0f);
		StartClimbing();

		if (previousNormal != HitResult.Normal) {
			previousNormal = HitResult.Normal;
			AlignToPlane(HitResult.Normal);
		}

		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		GetCharacterMovement()->bOrientRotationToMovement = false;
	}
	else {
		StopClimbing();
		if (bDrawDebugLines) DrawDebugLine(GetWorld(), ActorLocation, ActorLocation + DiagonalDirection * TraceDistance, FColor::Red, false, 0.2f, 0, 1.0f);
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