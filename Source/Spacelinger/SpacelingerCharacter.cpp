// Copyright Epic Games, Inc. All Rights Reserved.

#include "SpacelingerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Actors/Weapons/SLProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
// ASpacelingerCharacter

ASpacelingerCharacter::ASpacelingerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
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

void ASpacelingerCharacter::BeginPlay()
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
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASpacelingerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASpacelingerCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASpacelingerCharacter::Look);

		EnhancedInputComponent->BindAction(SwitchAbilityAction, ETriggerEvent::Started, this, &ASpacelingerCharacter::SwitchAbility);
	}
}

void ASpacelingerCharacter::Move(const FInputActionValue& Value)
{
	
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
	
}

void ASpacelingerCharacter::Look(const FInputActionValue& Value)
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
		if (Rotation.Pitch < 180 && Rotation.Pitch > MaxCameraPitch)     { Rotation.Pitch = MaxCameraPitch; }
		if (Rotation.Pitch > 180 && Rotation.Pitch < 360-MinCameraPitch) { Rotation.Pitch = 360-MinCameraPitch; }
		Controller->SetControlRotation(Rotation);
	}
}

void ASpacelingerCharacter::SwitchAbility(const FInputActionValue& Value)
{
	int ActionValue = Value.GetMagnitude();
	int CurrentAbility = SelectedHumanoidAbility.GetValue();
	CurrentAbility = (ActionValue + CurrentAbility + SLHumanoidAbility::COUNT) % SLHumanoidAbility::COUNT;
	SelectedHumanoidAbility = static_cast<SLHumanoidAbility>(CurrentAbility);
}

void ASpacelingerCharacter::ThrowAbility(FTransform SpawnTransform) {
	FActorSpawnParameters SpawnParameters;
	UClass* ClassToSpawn = (SelectedHumanoidAbility == SLHumanoidAbility::StickyPuddle) ? StickyPuddleProjectileClass : CorrosiveSpitClass;
	ASLProjectile* ActorSpawned = GetWorld()->SpawnActor<ASLProjectile>(ClassToSpawn, SpawnTransform, SpawnParameters);
	if (ActorSpawned) {
		UProjectileMovementComponent* MC = ActorSpawned->MovementComponent;
		float ProjectileSpeed = 1000.0f; // TODO! Obtain from data table
		MC->MaxSpeed = ProjectileSpeed;
		MC->Velocity = ThrowableDirection * ProjectileSpeed;
	}
}

void ASpacelingerCharacter::DrawThrowTrajectory() {
	ensure(Controller);
	FRotator PlayerRotation = GetControlRotation();
	FVector PlayerForward = PlayerRotation.Vector();
	FVector PlayerUp      = FRotationMatrix(PlayerRotation).GetScaledAxis(EAxis::Z);

	float AmountUp = 0.5f; // TODO! Obtain from Data table
	ThrowableDirection = PlayerUp*AmountUp + PlayerForward*(1-AmountUp);
	ThrowableDirection.Normalize();

	float ProjectileSpeed = 1000.0f; // TODO! Obtain from data table
	float ProjectileRadius = 25.0f;  // TODO!
	FVector StartLocation  = FVector(0.0f, 40.0f, 60.0f); // TODO!

	FPredictProjectilePathParams PredictParams = {};
	PredictParams.StartLocation       = GetTransform().GetLocation() + StartLocation;
	PredictParams.LaunchVelocity      = ThrowableDirection*ProjectileSpeed;
	PredictParams.bTraceWithCollision = true;
	PredictParams.MaxSimTime          = 3.0f; // TODO! A constant
	PredictParams.bTraceWithChannel   = true;
	PredictParams.TraceChannel        = ECollisionChannel::ECC_WorldDynamic;
	PredictParams.ActorsToIgnore.Add(this);
	PredictParams.SimFrequency        = 25.0f; // TODO! A constant
	PredictParams.DrawDebugTime       = 1.0f;  // TODO! A constant

	FPredictProjectilePathResult PredictResult;
	bool IsHit = UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult);
	TArray<FPredictProjectilePathPointData> PathPoints = PredictResult.PathData;
	FVector HitPoint = PredictResult.LastTraceDestination.Location;


	FColor Color       = FColor::Cyan; // TODO! A constant
	float Thickness    = 2.0f;         // TODO! A constant
	float SphereRadius = 20.0f;        // TODO! A constant

	for (int i = 0; i < PathPoints.Num() - 1; ++i)
	{
		FVector P0 = PathPoints[i]  .Location;
		FVector P1 = PathPoints[i+1].Location;
		DrawDebugLine(GetWorld(), P0, P1, Color, false, -1, 0, Thickness);
	}
	if (IsHit) {
		FVector P0 = PathPoints[PathPoints.Num()-1].Location;
		DrawDebugLine(GetWorld(), P0, HitPoint, Color, false, -1, 0, Thickness);
		DrawDebugSphere(GetWorld(), HitPoint, SphereRadius, 12, Color, false, -1.0f, 0U, Thickness);
	}
}