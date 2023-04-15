// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SpacelingerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;

UENUM()
enum SLHumanoidAbility {
	StickyPuddle = 0,
	CorrosiveSpit,
	COUNT UMETA(Hidden),
};

UCLASS(config=Game)
class ASpacelingerCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwitchAbilityAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability, meta = (AllowPrivateAccess = "true"))
	UClass* StickyPuddleProjectileClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability, meta = (AllowPrivateAccess = "true"))
	UClass* CorrosiveSpitClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ability, meta = (AllowPrivateAccess = "true"))
	USceneComponent* ParabollicStartPosition;

public:
	ASpacelingerCharacter();

protected:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);

	void SwitchAbility(const FInputActionValue& Value);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay();

public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SL_Options, meta = (AllowPrivateAccess = "true", UIMin = "0.0", UIMax = "90.0"))
	float MaxCameraPitch = 40.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SL_Options, meta = (AllowPrivateAccess = "true", UIMin = "0.0", UIMax = "90.0"))
	float MinCameraPitch = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ability)
	TEnumAsByte<SLHumanoidAbility> SelectedHumanoidAbility = SLHumanoidAbility::StickyPuddle;
	UFUNCTION(BlueprintCallable)
	void ThrowAbility(FTransform SpawnTransform);
	UFUNCTION(BlueprintCallable)
	void DrawThrowTrajectory();

private:
	// Set while drawing the trajectory, used when the player uses an ability
	FVector ThrowableDirection = FVector::Zero();
};
