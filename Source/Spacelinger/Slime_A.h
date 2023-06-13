// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CableComponent.h"
#include "SpiderWeb.h"
#include "AbilitySystemInterface.h"
#include "Slime_A.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UStaticMeshComponent;
class UPostProcessComponent;
class UMCV_AbilitySystemComponent;
class UStaminaAttributeSet;
class UHealthAttributeSet;

UENUM(BlueprintType)
enum SLSpiderAbility {
	PutSpiderWeb = 0,
	PutTrap,
	ThrowSpiderWeb,
	MeleeAttack,
	COUNT UMETA(Hidden),
};

UCLASS(config = Game)
class ASlime_A : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UPostProcessComponent* PostProcessComponent;

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
	UInputAction* throwAbilityAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlowTimeAbility;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwitchAbilityAction;
	

protected:
	// GAS
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	UMCV_AbilitySystemComponent* AbilitySystemComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	UHealthAttributeSet* HealthAttributeSet = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SL_Options, meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	UStaminaAttributeSet* StaminaAttributeSet = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SL_Options, meta = (AllowPrivateAccess = "true"))
	float MaxStamina = 100.0f;

public:
	ASlime_A();
	virtual void Tick(float DeltaTime) override;
	virtual void Landed(const FHitResult& Hit) override;
	UPostProcessComponent* GetPostProcessComponent() const;
	void JumpToPosition();



protected:
	// Input callbacks
	void Move(const FInputActionValue& Value);
	void StopMoving(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Climb(const FInputActionValue& Value);
	void ToggleDrawDebugLines(const FInputActionValue& Value);
	void ThrowAbility(const FInputActionValue& Value);
	
	void SwitchAbility(const FInputActionValue& Value);
	void StopJumpToPosition();

	void PutTrap();

	//Handlers
	void HandleClimbingBehaviour();
	void HandleAttachedBehaviour();
	void HandleHangingBehaviour();
	void HandleJumpToLocationBehaviour();
	void ThrowSpiderWeb();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//ThrowSpiderWeb
	FVector2D GetViewportCenter();
	FVector GetLookDirection(FVector2D ScreenLocation);
	FHitResult PerformLineTrace(FVector StartPosition, FVector EndPosition);
	void SpawnAndAttachSpiderWeb(FVector Location, FVector HitLocation, bool bAttached);
	void PutSpiderWebAbility();

	// Helpers
	void PerformClimbingBehaviour(FVector ActorLocation);
	TMap<FVector, FHitResult> GenerateHitNormals(FVector ActorLocation);
	FHitResult ExecuteDiagonalTrace(FVector ActorLocation, FCollisionQueryParams& Params);
	void HandleNormalHits(TMap<FVector, FHitResult>& HitNormals, FVector ActorLocation, FCollisionQueryParams& Params);
	FVector CalculateAverageNormal(TMap<FVector, FHitResult>& HitNormals);
	void HandleFloorAndCeiling();
	void PerformGroundBehaviour(FVector ActorLocation);
	bool ExecuteGroundTrace(FVector StartLocation, FVector EndRayLocation, FCollisionQueryParams& Params, FHitResult& HitResult);
	void DrawDebugLinesIfNeeded(FVector StartLocation, FVector EndLocation);
	void keepClimbing();
	void UpdateRotation(FVector planeNormal);
	void StartClimbing();
	void StopClimbing();
	void ResetBlendingFactor();

	void MeleeAttack();

	void AlignToPlane(FVector planeNormal);
	void CutSpiderWeb();
	FVector getVectorInConstraintCoordinates(FVector input, float Speed, float DeltaTime);
	FVector getRelativePositionPhysicsConstraint();

	void UpdateBaseCameraRotation(FVector CurrentNormal);
	void UpdateCameraRotation();
	double FloorThreshold = 0.9;
	FORCEINLINE bool IsFloor(FVector Normal) { return FVector::DotProduct(Normal, FVector::UpVector) >= FloorThreshold; }
	FORCEINLINE bool IsCeiling(FVector Normal) {
		return FVector::DotProduct(Normal, FVector::UpVector) <= -FloorThreshold;
	}

private:
	const float TraceDistance = 50.0f;
	float DefaultMaxStepHeight;
	TArray<FVector> DiagonalDirections;
	TArray<FVector> InitialDiagonalDirections;


public:
	UPROPERTY(Config)
	bool bDrawDebugLines = false;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;


public:
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FMatrix BaseCameraRotation = FMatrix::Identity;
	FRotator InputRotator = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SL_Options, meta = (AllowPrivateAccess = "true", UIMin = "0.0", UIMax = "90.0"))
	float MaxCameraPitch = 10.0f;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		bool bJumpToLocation = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		bool bHasTrownSpiderWeb = false;
	UPROPERTY(EditDefaultsOnly, Category = "Swinging")
		bool bInitialForceApplied = false;
	UPROPERTY(EditDefaultsOnly, Category = "Swinging")
		float distanceConstraints ;
	UPROPERTY(EditDefaultsOnly, Category = "Swinging")
		float bSetInitialRelativeLocation;
	UPROPERTY(EditDefaultsOnly, Category = "Swinging")
		FVector initialRelativePosition;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category = "CR_Aniamtion")
		bool bHasLanded = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CR_Aniamtion")
		float fBlendingFactor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
		TEnumAsByte<SLSpiderAbility> SelectedSpiderAbility = SLSpiderAbility::PutSpiderWeb;

	FCollisionQueryParams TraceParams;
	
	// ============== Slow Time Ability
	UFUNCTION()
		void OnCollisionEnter(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:

	void SlowTime(const FInputActionValue& Value);
	void SlowTimeEnd(const FInputActionValue& Value);
	void SlowTimeFunc(float DeltaTime);
};
