// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Slime.generated.h"


UCLASS(config = Game)
class ASlime : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ClimbAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	/** Sphere component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* SphereComponent;

	

public:
	ASlime();
	virtual void Tick(float DeltaTime) override;


protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	void StopMoving(const FInputActionValue& Value);
	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for climbing input */
	void Climb(const FInputActionValue& Value);

	/** Called for climbing input */
	void StopClimbing();

	


	


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay() override;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SL_Options, meta = (AllowPrivateAccess = "true", UIMin = "0.0", UIMax = "90.0"))
		float MaxCameraPitch = 40.0f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SL_Options, meta = (AllowPrivateAccess = "true", UIMin = "0.0", UIMax = "90.0"))
		float MinCameraPitch = 60.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
		bool bIsClimbing = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
		bool AtCeiling = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
		TArray<FHitResult> OutHits;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
		FVector MovementDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
		bool hasMoved = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
	FVector previousNormal;




};
