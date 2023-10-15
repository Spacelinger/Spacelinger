// Without Copyright. Project SpaceLinger, Master en Creacion de Videojuegos (MCV) at Universitat Pompeu Fabra (UPF)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CableComponent.h"
#include "SpiderWeb.h"
#include "AbilitySystemInterface.h"
#include "Containers/CircularQueue.h"
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
class USpiderTrapsAttributeSet;
class IInteractInterface;
class UInteractingComponent;
class UBoxComponent;
class UInventoryComponent;
class ULifeComponent;
class UUIHUD;
class UMaterialBillboardComponent;

UENUM(BlueprintType)
enum SLSpiderAbility {
	PutSpiderWeb = 0,
	PutTrap,
	ThrowStunningWeb,
	Hook,
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
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
	UInputAction* PutSpiderWebAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAbilityAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SlowTimeAbility;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ChangeSpiderWebSizeAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MeleeAttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* PutTrapAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* HookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* ThrowStunningAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* ToggleQuestLogAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UInteractingComponent* InteractingComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* InteractCollisionComponentCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* InteractCollisionComponentBody;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* InventoryComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UMaterialBillboardComponent* HookTargetCrosshair = nullptr;

public:
	UFUNCTION()
	void OnDie(AActor* Killer);
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spacelinger", meta = (AllowPrivateAccess = "true"))
	bool bIsDead = false;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	float StaminaRecoveryBaseRate = 2.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	USpiderTrapsAttributeSet* SpiderTrapsAttributeSet = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SL_Options, meta = (AllowPrivateAccess = "true"))
	float MaxTraps = 3.0f;
	
	// Interact
	IInteractInterface* CurrentInteractable = nullptr;

private:

	TQueue<ASpiderWeb*> ActiveSpiderTraps;
	int8 ActiveSpiderTrapsCount = 0;

public:
	ASlime_A();
	virtual void Tick(float DeltaTime) override;
	virtual void Landed(const FHitResult& Hit) override;
	UPostProcessComponent* GetPostProcessComponent() const;
	void JumpToPosition();
	void HandleThrownSpiderWeb();

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void MeleeAttackTriggered();

	UFUNCTION(BlueprintCallable, Category = "Angles")
	FVector ReturnCenterScreen();

	UFUNCTION(BlueprintCallable, Category = "Angles")
	float GetVerticalAngleToCenterScreen();

	UFUNCTION(BlueprintCallable, Category = "Angles")
	float GetHorizontalAngleToCenterScreen();

	// Hide the crosshair when it doesn't need to be shown
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="HUD")
	void SetCrosshairVisibility(bool bVisible);

	UFUNCTION()
	void CutSpiderWeb();

	UFUNCTION()
		void AddNewTrap(ASpiderWeb* NewTrap);

	UFUNCTION()
		void RemoveActiveTrap(ASpiderWeb* Trap);

	// Life Comp
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		ULifeComponent* LifeComponent = nullptr;

	// HUD
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UUIHUD* HUD = nullptr;

	UPROPERTY(EditAnywhere, Category = HUD, BlueprintReadOnly)
		TSubclassOf<UUIHUD> HUDClass;

	FVector2D GetViewportCenter();
	FVector GetLookDirection(FVector2D ScreenLocation);
	FVector GetLookAtLocation(FVector2D ScreenLocation);
	FHitResult PerformLineTrace(FVector StartPosition, FVector EndPosition);
	void SpawnAndAttachSpiderWeb(FVector Location, FVector HitLocation, bool bAttached, bool bIsHock);


	UPROPERTY(EditAnywhere)
		UChildActorComponent* SpiderWebBallF;

protected:
	// Input callbacks
	void Move(const FInputActionValue& Value);
	void StopMoving(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Climb(const FInputActionValue& Value);
	void ThrowAbility(const FInputActionValue& Value);
	void AimAbility(const FInputActionValue& Value);
	void StopAimingAbility(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void ChangeSpiderWebSize(const FInputActionValue& Value);
	void StopJumpToPosition();
	void PutTrap();

	//Handlers
	void HandleClimbingBehaviour();
	void HandleAttachedBehaviour();
	void HandleHangingBehaviour();
	void HandleJumpToLocationBehaviour();
	void ThrowSpiderWeb(bool bisHook);
	void HandleHook();
	void AimHook();
	void ToggleAimHook();
	void ThrowStunningWeb();
	void AimStunningWeb();

	//Mode
	void setHookMode();
	void setStunningMode();
	void setTrapMode();


	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//ThrowSpiderWeb
	
	void SpawnStunningWeb(FVector Location, FVector HitLocation);
	FVector ReturnCenterScreenWorld();

	void PutSpiderWebAbility();

	// Helpers
	void PerformClimbingBehaviour(FVector ActorLocation);
	TArray<FHitResult> GenerateHitNormals(FVector ActorLocation);
	FHitResult ExecuteDiagonalTrace(FVector ActorLocation, FCollisionQueryParams& Params);
	void HandleNormalHits(TArray<FHitResult>& HitResults, FVector ActorLocation, FCollisionQueryParams& Params);
	FVector CalculateAverageNormal(TArray<FHitResult>& HitResults);
	void HandleFloorAndCeiling();
	void PerformGroundBehaviour(FVector ActorLocation);
	bool ExecuteGroundTrace(FVector StartLocation, FVector EndRayLocation, FCollisionQueryParams& Params, FHitResult& HitResult);
	void DrawDebugLinesIfNeeded(FVector StartLocation, FVector EndLocation);
	void keepClimbing();
	void UpdateRotation(FVector planeNormal);
	void StartClimbing();
	void StopClimbing();
	void ResetAttack();
	void ModifyDamping();
	void MeleeAttack();


	void AlignToPlane(FVector planeNormal);
	// void CutSpiderWeb();		MOVED TO PUBLIC -> NEED TO BE ACCESSED BY GAS
	void CutThrownSpiderWeb();
	FVector getVectorInConstraintCoordinates(FVector input, float Speed, float DeltaTime);
	FVector getRelativePositionPhysicsConstraint();

	void UpdateBaseCameraRotation(FVector CurrentNormal);
	void UpdateCameraRotation();
	void UpdateRotationOverTime(float DeltaTime);
	double FloorThreshold = 0.9;
	FORCEINLINE bool IsFloor(FVector Normal) {
		const float epsilon = 0.01f;  // Modify as needed
		return fabs(Normal.Z - 1.0f) < epsilon;
	}
	FORCEINLINE bool IsCeiling(FVector Normal) {
		return FVector::DotProduct(Normal, FVector::UpVector) <= -FloorThreshold;
	}

private:
	const float TraceDistance = 70.0f;
	float DefaultMaxStepHeight;
	TArray<FVector> DiagonalDirections;
	TArray<FVector> InitialDiagonalDirections;
	const float StunningWebStunDuration = 10.0f;
	bool bIsAimingHook = false;
	FTimerHandle AttackResetTimerHandle;


	APlayerController* GetPlayerController();

public:
	UPROPERTY(Config)
	bool bDrawDebugLines = false;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger|Camera", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface *TranslucentMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger|Camera", meta = (AllowPrivateAccess = "true"))
	UMaterialInterface *OpaqueMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spacelinger|Camera", meta = (AllowPrivateAccess = "true"))
	float MaterialCameraThreshold = 60.0f;
	bool bIsMaterialOpaque = true;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
		FRotator TargetRotation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
		bool bIsRotating;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Climbing")
		bool bCanClimb = true;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SpiderWeb")
		float SpiderWebTimeToLive = 120.f; // In seconds
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
		bool bIsAttacking;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swinging")
		float angleAlign;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Swinging")
		bool bhangingAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
		float StateChangeCooldown = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
		float LastStateChangeTime = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Air")
		bool bIsInAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hook")
		float HookLineTraceDistance = 1000.0f;
	
	AActor* previousActorCollision;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Abilities")
		TEnumAsByte<SLSpiderAbility> SelectedSpiderAbility = SLSpiderAbility::PutSpiderWeb;

	FCollisionQueryParams TraceParams;
	
	UFUNCTION()
		void OnCollisionEnter(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// ============== Slow Time Ability
protected:

	void SlowTime(const FInputActionValue& Value);
	void SlowTimeEnd(const FInputActionValue& Value);
	void SlowTimeFunc(float DeltaTime);

public:
	void SetStaminaRecoveryValue(float Value);
	void ResetStaminaRecoveryValue() { SetStaminaRecoveryValue(StaminaRecoveryBaseRate); }

	// ============== Quest Log
protected:

	UFUNCTION(BlueprintImplementableEvent)
		void ToggleQuestLogBP(bool bShowQuestLog);

	void ShowQuestLog() { ToggleQuestLogBP(true); };
	void HideQuestLog() { ToggleQuestLogBP(false); };
};
