// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CableComponent.h"
#include "SpiderWeb.h"
#include "Slime_A.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UStaticMeshComponent;
class UMCV_AbilitySystemComponent;
class UStaminaAttributeSet;

UCLASS(config = Game)
class ASlime_A : public ACharacter
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
	UInputAction* ClimbAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DebugAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* putSpiderWebAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* increaseSpiderWebAction;

protected:
	// GAS
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	UMCV_AbilitySystemComponent* AbilitySystemComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	UStaminaAttributeSet* StaminaAttributeSet = nullptr;

public:
	ASlime_A();
	virtual void Tick(float DeltaTime) override;
	virtual void Landed(const FHitResult& Hit) override;

protected:
	// Input callbacks
	void Move(const FInputActionValue& Value);
	void StopMoving(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Climb(const FInputActionValue& Value);
	void ToggleDrawDebugLines(const FInputActionValue& Value);
	void PutSpiderWeb(const FInputActionValue& Value);
	void ModifySpiderWeb(const FInputActionValue& Value);
	void LockSpiderWeb(const FInputActionValue& Value);


	// Helpers
	void keepClimbing();
	void UpdateRotation(FVector planeNormal);
	void StartClimbing();
	void StopClimbing();
	void AlignToPlane(FVector planeNormal);
	double FloorThreshold = 0.9;
	FORCEINLINE bool IsFloor(FVector Normal) { return FVector::DotProduct(Normal, FVector::UpVector) >= FloorThreshold; }
	FORCEINLINE bool IsCeiling(FVector Normal) {
		return FVector::DotProduct(Normal, FVector::UpVector) <= -FloorThreshold;
	}

private:
	const float TraceDistance = 50.0f;
	float DefaultMaxStepHeight;
	TArray<FVector> DiagonalDirections;

	bool bDrawDebugLines = true;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;


public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

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
	bool canTrace = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
	FVector previousNormal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
	FVector previousLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
	float upOffset = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
	ASpiderWeb* spiderWebReference;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
	bool attached = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
	bool attachedAtCeiling = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		bool isHanging = false;
	UPROPERTY(EditDefaultsOnly, Category = "Swinging")
		bool bInitialForceApplied = false;
	
	// ============== Slow Time Ability
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlowTimeAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SL_Options, meta = (AllowPrivateAccess = "true", UIMin = "0.1", UIMax = "1.0"))
	float SlowTimeDilation = .2f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SL_Options, meta = (AllowPrivateAccess = "true", UIMin = "0.001", UIMax = "1.0"))
	float PlayerSlowTimeCompensation = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SL_Options, meta = (AllowPrivateAccess = "true", UIMin = "0.01", UIMax = "0.5"))
	float SlowTimeFadeInRate = 0.5;	// Fade in time for the slow time

protected:
	
	//FTimerHandle SlowTimeTimerHandle;
	float CurrentSlowTimeDilation = 1;
	bool bIsTimeSlowing = false;
	float SlowStep = 0;

	void SlowTime(const FInputActionValue& Value);
	void SlowTimeEnd(const FInputActionValue& Value);
	void SlowTimeFunc(float DeltaTime);
};
