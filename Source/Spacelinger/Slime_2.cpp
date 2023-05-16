// Copyright Epic Games, Inc. All Rights Reserved.

#include "Slime_2.h"
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

ASlime_2::ASlime_2()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(13.0f, 13.0f);

	// Set up the sphere component
	SphereComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'")).Object);
	SphereComponent->SetWorldScale3D(FVector(0.25f, 0.25f, 0.25f) * GetCapsuleComponent()->GetUnscaledCapsuleRadius() / 13.0f);
	SphereComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));


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

void ASlime_2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsClimbing) {
		FVector ActorLocation = GetActorLocation();
		FVector ForwardDirection = GetActorForwardVector() *1.3f;
		FVector DownwardDirection = -GetActorUpVector();

		FVector DiagonalDirection = (ForwardDirection + DownwardDirection).GetSafeNormal();
		FVector BackwardDirection = -ForwardDirection;
		FVector DiagonalBackwardDirection = (BackwardDirection + DownwardDirection).GetSafeNormal();
		FVector RightDirection = GetActorRightVector()* 1.3f;
		FVector DiagonalRightDirection = (RightDirection + DownwardDirection).GetSafeNormal();
		FVector DiagonalLeftDirection = (-RightDirection + DownwardDirection).GetSafeNormal();
		float TraceDistance = 50;
		FVector DiagonalDirections[4] = {
			DiagonalDirection,
			DiagonalBackwardDirection,
			DiagonalRightDirection,
			DiagonalLeftDirection
		};

		TArray<FVector> HitNormals;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		for (int i = 0; i < 4; ++i) {
			FHitResult CurrentHitResult;
			DrawDebugLine(GetWorld(), ActorLocation, ActorLocation + DiagonalDirections[i] * TraceDistance, FColor::Red, false, 0.2f, 0, 1.0f);
			bool bHit = GetWorld()->LineTraceSingleByChannel(CurrentHitResult, ActorLocation, ActorLocation + DiagonalDirections[i] * TraceDistance, ECC_Visibility, Params);
			if (bHit) {
				HitNormals.Add(CurrentHitResult.Normal);
			}
		}

		if (HitNormals.Num() > 0) {
			FVector AverageNormal = FVector::ZeroVector;
			for (const FVector& Normal : HitNormals) {
				AverageNormal += Normal;
			}
			AverageNormal /= HitNormals.Num();
			previousNormal = AverageNormal;
			AlignToPlane(previousNormal);
			

			bIsClimbing = true;
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		else {
			StopClimbing();
		}
		if (previousNormal.Z == 1) {
			StopClimbing();
		}
	}
	
}



void ASlime_2::AlignToPlane(FVector planeNormal)
{
	// Get the forward and right vectors of the actor
	FVector forwardVector = GetActorForwardVector();
	FVector rightVector = GetActorRightVector();

	// Calculate the dot products between the forward/right vectors and the plane's normal vector
	float forwardDot = FVector::DotProduct(forwardVector, planeNormal);
	float rightDot = FVector::DotProduct(rightVector, planeNormal);

	// Determine which method to use based on the dot products
	if (FMath::Abs(forwardDot) < FMath::Abs(rightDot))
	{
		// Use MakeFromXZ to align to the plane's normal vector
		FRotator newRotation = FRotationMatrix::MakeFromXZ(forwardVector, planeNormal).Rotator();
		SetActorRotation(newRotation);
	}
	else
	{
		// Use MakeFromZY to align to the plane's normal vector
		FRotator newRotation = FRotationMatrix::MakeFromYZ(rightVector, planeNormal).Rotator();
		SetActorRotation(newRotation);
	}
}


void ASlime_2::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Use the SphereComponent
	SphereComponent->SetVisibility(true);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASlime_2::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlime_2::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ASlime_2::StopMoving);

		//Climbing
		EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Started, this, &ASlime_2::Climb);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlime_2::Look);

	}

}

void ASlime_2::StopMoving(const FInputActionValue& Value)
{
	GetCharacterMovement()->Velocity = FVector::ZeroVector;

}

void ASlime_2::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{

		float TraceDistance = 20;
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
			

			// find out which way is forward
			
			// Add movement directly up if at the ceiling
			RightDirection = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);
			ForwardDirection = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::X);
			
		}

		MovementDirection.Normalize();
		MovementDirection = ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X;
		AddMovementInput(MovementDirection);
	}
}


void ASlime_2::StopClimbing()
{
	if (bIsClimbing)
	{
		bIsClimbing = false;
		AtCeiling = false;
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		GetCharacterMovement()->bOrientRotationToMovement = true;

		// Set actor rotation to controller's rotation
		if (Controller)
		{
			SetActorRotation(Controller->GetControlRotation());
		}
	}
}


#include <Kismet/KismetMathLibrary.h>
void ASlime_2::Climb(const FInputActionValue& Value)
{
	if (bIsClimbing) {
		StopClimbing();
		
	}
	else {
		FVector ActorLocation = GetActorLocation();
		FVector ForwardDirection = GetActorForwardVector();
		FVector DownwardDirection = -GetActorUpVector();
		FVector DiagonalDirection = (ForwardDirection + DownwardDirection).GetSafeNormal();

		float TraceDistance = 20;
		FVector RightDirection = GetActorRightVector();
		// Perform line trace from actor location to diagonal direction
		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, ActorLocation, ActorLocation + DiagonalDirection * TraceDistance, ECC_Visibility, Params);
		// Draw the line
		if (bHit) {
			DrawDebugLine(GetWorld(), HitResult.Location, HitResult.Location + HitResult.Normal * TraceDistance, FColor::Red, false, 0.2f, 0, 1.0f);

			bIsClimbing = true;

			if (previousNormal != HitResult.Normal) {
				previousNormal = HitResult.Normal;
				AlignToPlane(HitResult.Normal);
			}

			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			GetCharacterMovement()->bOrientRotationToMovement = false;

		}
		else {
			bIsClimbing = false;
		}
	}
}


void ASlime_2::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);

		// NOTE(Sergi): Clamp camera value
		FRotator Rotation = Controller->GetControlRotation();
		if (Rotation.Pitch < 180 && Rotation.Pitch > MaxCameraPitch) { Rotation.Pitch = MaxCameraPitch; }
		if (Rotation.Pitch > 180 && Rotation.Pitch < 360 - MinCameraPitch) { Rotation.Pitch = 360 - MinCameraPitch; }
		Controller->SetControlRotation(Rotation);
	}
}
