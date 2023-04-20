// Copyright Epic Games, Inc. All Rights Reserved.

#include "Slime.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// ASlime

ASlime::ASlime()
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
	CameraBoom->TargetArmLength = 0.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = false; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = true; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void ASlime::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Check if the character is climbing
	if (bIsClimbing)
	{
		
		// Create a sphere multi-cast by channel with start and end the actor location
		const float SphereRadius = 15.0f;
		const FVector SphereTraceStart = GetActorLocation();
		const FVector SphereTraceEnd = GetActorLocation();
		OutHits.Empty();
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this); // Ignore self

		const bool bHit = GetWorld()->SweepMultiByChannel(
			OutHits,
			SphereTraceStart,
			SphereTraceEnd,
			FQuat::Identity,
			ECC_WorldStatic,
			FCollisionShape::MakeSphere(SphereRadius),
			CollisionParams
		);

		if (!bHit)
		{
			StopClimbing();
			return;
		}

		// Find the first hit that is not the character itself and has a normal in the opposite direction to the movement direction
		if (hasMoved) {
			FHitResult NewHit;
			if (OutHits.Num() == 1) {
				NewHit = OutHits[0];
			}
			else {
				for (const FHitResult& OutHit : OutHits)
				{
					FVector normal = OutHit.Normal;
					normal.Normalize();
					if (normal != previousNormal) {
						if (FVector::DotProduct(normal, MovementDirection) <= 0 && normal.Z == 0) // Check if the normal is opposite to the movement direction and not pointing upwards
						{
							hasMoved = false;
							NewHit = OutHit;
							break;
							AtCeiling = false;
																	
						}
					}
				}
			}

			// Calculate the end point by adding the ActorUpVector multiplied by 15
			FVector ActorLocation = GetActorLocation();
			FVector ActorUpVector = GetActorUpVector();
			FVector EndPoint = ActorLocation + (ActorUpVector * 15);

			// Set up the parameters for the line trace
			FCollisionQueryParams TraceParams;
			TraceParams.AddIgnoredActor(this);


			// Perform the line trace and get the hit result
			FHitResult CeilingHitResult;
			bool bHitCeiling = GetWorld()->LineTraceSingleByChannel(CeilingHitResult, ActorLocation, EndPoint, ECC_Visibility, TraceParams);

			// Check if the line trace hit anything
			if (bHitCeiling)
			{
				AtCeiling = true;
			}
			else {
				AtCeiling = false;
			}
	
			previousNormal = NewHit.Normal;
			previousNormal.Normalize();
		}


					
		const FRotator NewRotation = FRotationMatrix::MakeFromX(previousNormal).Rotator();
		SetActorRotation(FRotator(0.0f, NewRotation.Yaw + 180.0f, 0.0f));
		
	}
}



void ASlime::BeginPlay()
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

void ASlime::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlime::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ASlime::StopMoving);

		//Climbing
		EnhancedInputComponent->BindAction(ClimbAction, ETriggerEvent::Started, this, &ASlime::Climb);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlime::Look);

	}

}

void ASlime::StopMoving(const FInputActionValue& Value)
{
	GetCharacterMovement()->Velocity = FVector::ZeroVector;

}

void ASlime::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward and right vectors
		FVector ForwardDirection;
		FVector RightDirection;
		if (AtCeiling)
		{
			// Add movement directly up if at the ceiling
			RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		}
		else if (bIsClimbing)
		{
			// Add movement directly up if climbing
			ForwardDirection = GetActorUpVector();
			RightDirection = GetActorRightVector();
		}
		else
		{
			// Add movement directly up if at the ceiling
			RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		}

		// add movement 
		MovementDirection = ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X;
		MovementDirection.Normalize();

		// Check if the movement direction is obstructed
		FHitResult HitResult;
		const FVector StartLocation = GetActorLocation();
		// Calculate the end location taking into account the size of the capsule
		const FVector EndLocation = StartLocation + MovementDirection * GetCapsuleComponent()->GetScaledCapsuleRadius();

		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this); // Ignore self
		const bool bHitDirection = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Pawn, CollisionParams);

		bool bIsChangingSurface = false;
		if (bHitDirection)
		{
			FVector HitNormal = HitResult.Normal;
			HitNormal.Normalize();
			// Check if the character is changing surfaces
			if (bIsClimbing && HitNormal.Z > 0.1f)
			{
				bIsChangingSurface = true;
			}
		}

		if (!bHitDirection || bIsChangingSurface)
		{
			FString Message = FString::Printf(TEXT("The vector is: X=%f Y=%f Z=%f"), MovementDirection.X, MovementDirection.Y, MovementDirection.Z);

			// Add the message to the screen
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, Message);
			AddMovementInput(ForwardDirection, MovementVector.Y);
			AddMovementInput(RightDirection, MovementVector.X);
		}
	}
}



void ASlime::StopClimbing()
{
	if (bIsClimbing)
	{
		bIsClimbing = false;
		AtCeiling = false;
		GetCharacterMovement()->SetMovementMode(MOVE_Falling);
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

void ASlime::Climb(const FInputActionValue& Value)
{

	if (bIsClimbing)
	{
		StopClimbing();
	}
	else
	{
		// Create a sphere trace by channel with start and end the actor location
		const float SphereRadius = 15.f;
		const FVector SphereTraceStart = GetActorLocation();
		const FVector SphereTraceEnd = GetActorLocation();
		FHitResult OutHit;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this); // Ignore self

		const bool bHit = GetWorld()->SweepSingleByChannel(
			OutHit,
			SphereTraceStart,
			SphereTraceEnd,
			FQuat::Identity,
			ECC_WorldStatic,
			FCollisionShape::MakeSphere(SphereRadius),
			CollisionParams
		);

		if (bHit)
		{
			const FVector HitNormal = OutHit.Normal;
			previousNormal = HitNormal;
			const FRotator NewRotation = FRotationMatrix::MakeFromX(HitNormal).Rotator();
			SetActorRotation(FRotator(0.0f, NewRotation.Yaw + 180.0f, 0.0f));

			// Set climbing mode and movement settings
			bIsClimbing = true;
			GetCharacterMovement()->Velocity = FVector::ZeroVector;
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}
		else if (bIsClimbing)
		{
			// Stop climbing and reset movement settings
			bIsClimbing = false;
			GetCharacterMovement()->SetMovementMode(MOVE_Falling);
			GetCharacterMovement()->bOrientRotationToMovement = true;
		}
	}
}

void ASlime::Look(const FInputActionValue& Value)
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
