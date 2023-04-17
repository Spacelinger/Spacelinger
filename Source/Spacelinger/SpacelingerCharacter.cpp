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
#include "Actors/Weapons/SLWeapon_DT.h"
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

	ParabollicStartPosition = CreateDefaultSubobject<USceneComponent>(TEXT("ParabollicStartPosition"));
	ParabollicStartPosition->SetupAttachment(RootComponent);

	AimCameraPosition= CreateDefaultSubobject<USceneComponent>(TEXT("AimCameraPosition"));
	AimCameraPosition->SetupAttachment(RootComponent);
}

void ASpacelingerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

// Equivalent of Blueprint's Construction Script
void ASpacelingerCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	DefaultTargetArmLength = CameraBoom->TargetArmLength;
	DefaultCameraLocation = CameraBoom->GetRelativeLocation();
	AimCameraLocation = AimCameraPosition->GetRelativeLocation();
}

//////////////////////////////////////////////////////////////////////////
// Input

void ASpacelingerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASpacelingerCharacter::Move);

		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASpacelingerCharacter::Look);

		EnhancedInputComponent->BindAction(ShootAbilityAction, ETriggerEvent::Started,   this, &ASpacelingerCharacter::ShootAbility_Started);
		EnhancedInputComponent->BindAction(ShootAbilityAction, ETriggerEvent::Triggered, this, &ASpacelingerCharacter::ShootAbility_Triggered);
		EnhancedInputComponent->BindAction(ShootAbilityAction, ETriggerEvent::Completed, this, &ASpacelingerCharacter::ShootAbility_Completed);
		EnhancedInputComponent->BindAction(SwitchAbilityAction, ETriggerEvent::Started, this, &ASpacelingerCharacter::SwitchAbility);
	}
}

void ASpacelingerCharacter::Move(const FInputActionValue& Value)
{
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

void ASpacelingerCharacter::ShootAbility_Started(const FInputActionValue& Value) {
	bIsAimingAbility = true;
	LockCameraAiming();
}
void ASpacelingerCharacter::ShootAbility_Triggered(const FInputActionValue& Value) {
	if (bIsAimingAbility) {
		DrawThrowTrajectory();
	}
}
void ASpacelingerCharacter::ShootAbility_Completed(const FInputActionValue& Value) {
	if (bIsAimingAbility) {
		bIsAimingAbility = false;
		if (!bIsAiming) { // If player is holding aim button we won't reset camera
			UnlockCameraAiming();
		}

		FSLWeapon_DT* WeaponDT = GetAbilityRow(SelectedHumanoidAbility);

		UClass* ClassToSpawn = WeaponDT->ProjectileToSpawn.Get();
		FTransform SpawnTransform = ParabollicStartPosition->GetComponentTransform();
		FActorSpawnParameters SpawnParameters;
		ASLProjectile* ActorSpawned = GetWorld()->SpawnActor<ASLProjectile>(ClassToSpawn, SpawnTransform, SpawnParameters);
		if (ActorSpawned) {
			UProjectileMovementComponent* MC = ActorSpawned->MovementComponent;
			MC->MaxSpeed = WeaponDT->Speed;
			MC->Velocity = ThrowableDirection * WeaponDT->Speed;
		}
	}
}

void ASpacelingerCharacter::SwitchAbility(const FInputActionValue& Value)
{
	int ActionValue = Value.GetMagnitude();
	int CurrentAbility = SelectedHumanoidAbility.GetValue();
	CurrentAbility = (ActionValue + CurrentAbility + SLHumanoidAbility::COUNT) % SLHumanoidAbility::COUNT;
	SelectedHumanoidAbility = static_cast<SLHumanoidAbility>(CurrentAbility);
	bIsAimingAbility = false;
}

void ASpacelingerCharacter::DrawThrowTrajectory() {
	ensure(Controller);
	FSLWeapon_DT* WeaponDT = GetAbilityRow(SelectedHumanoidAbility);
	ensure (WeaponDT);

	FRotator PlayerRotation = GetControlRotation();
	FVector PlayerForward = PlayerRotation.Vector();
	FVector PlayerUp      = FRotationMatrix(PlayerRotation).GetScaledAxis(EAxis::Z);
	FVector PlayerRight   = FRotationMatrix(PlayerRotation).GetScaledAxis(EAxis::Y);

	FVector2D AimingOffset = WeaponDT->AimingOffset;
	FVector DirRight = PlayerRight*AimingOffset.X + PlayerForward*(1-AimingOffset.X);
	FVector DirUp    = PlayerUp   *AimingOffset.Y + PlayerForward*(1-AimingOffset.Y);
	ThrowableDirection = DirUp + DirRight;
	ThrowableDirection.Normalize();

	float ProjectileSpeed  = WeaponDT->Speed;
	float ProjectileRadius = 25.0f;
	//FVector StartLocation  = GetTransform().GetLocation() + FVector(0.0f, 40.0f, 60.0f);
	FVector StartLocation = ParabollicStartPosition->GetComponentLocation();

	FPredictProjectilePathParams PredictParams = {};
	PredictParams.StartLocation       = StartLocation;
	PredictParams.LaunchVelocity      = ThrowableDirection*ProjectileSpeed;
	PredictParams.bTraceWithCollision = true;
	PredictParams.MaxSimTime          = 3.0f;
	PredictParams.bTraceWithChannel   = true;
	PredictParams.TraceChannel        = ECollisionChannel::ECC_WorldDynamic;
	PredictParams.SimFrequency        = 25.0f;
	PredictParams.DrawDebugTime       = 1.0f;
	PredictParams.ActorsToIgnore.Add(this);

	FPredictProjectilePathResult PredictResult;
	bool IsHit = UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult);

	// Draw line
	TArray<FPredictProjectilePathPointData> PathPoints = PredictResult.PathData;
	FVector HitPoint = PredictResult.LastTraceDestination.Location;
	FColor Color       = FColor::Cyan;
	float Thickness    = 2.0f;
	float SphereRadius = 20.0f;

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

FSLWeapon_DT* ASpacelingerCharacter::GetAbilityRow(SLHumanoidAbility Ability) {
	ensure(AbilitiesDataTable);

	FName AbilityName;
	switch(Ability) {
		case StickyPuddle:  { AbilityName = TEXT("StickyPuddle");  } break;
		case CorrosiveSpit: { AbilityName = TEXT("CorrosiveSpit"); } break;
		case COUNT:
		default:
			ensure(false);
	}

	FString Context;
	return AbilitiesDataTable->FindRow<FSLWeapon_DT>(AbilityName, Context); // If null, FindRow() should warn us
}

void ASpacelingerCharacter::LockCameraAiming() {
	CameraBoom->TargetArmLength = AimTargetArmLength;
	CameraBoom->SetRelativeLocation(AimCameraLocation);
	bUseControllerRotationYaw = true;
}
void ASpacelingerCharacter::UnlockCameraAiming() {
	CameraBoom->TargetArmLength = DefaultTargetArmLength;
	CameraBoom->SetRelativeLocation(DefaultCameraLocation);
	bUseControllerRotationYaw = false;
}